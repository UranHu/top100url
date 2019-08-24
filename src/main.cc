#include "turl_buf.h"
#include "turl_counter.h"
#include "turl_map.h"
#include "turl_reader.h"
#include "turl_split.h"
#include <atomic>
#include <condition_variable>
#include <fcntl.h>
#include <gflags/gflags.h>
#include <iostream>
#include <memory.h>
#include <mutex>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

using namespace turl;

std::mutex mu;
std::condition_variable cv;
std::atomic<int> ready(0);

void work_thread(url_worker *worker,
                 std::shared_ptr<std::condition_variable> from_reader,
                 std::shared_ptr<std::condition_variable> to_reader,
                 std::shared_ptr<std::atomic_bool> stop) {
  bool the_last = false;
  while (!stop->load(std::memory_order_acquire)) {
    {
      std::unique_lock<std::mutex> garud(mu);
      if (the_last) {
        to_reader->notify_one();
        the_last = false;
      }
      from_reader->wait(garud);
    }
    if (stop->load(std::memory_order_acquire)) {
      break;
    }
    while (worker->get_task()) {
      worker->work();
      the_last = worker->worked();
    }
  }
}

void init(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if ((FLAGS_worker_num != FLAGS_hash_shardings) || (FLAGS_sfile_num > 4096) ||
      (FLAGS_block_size > 512 * 1024 * 1024)) {
    LOG("FATAL: Invalid args.\n");
    exit(1);
  }
}

int main(int argc, char *argv[]) {

  init(argc, argv);

  // Phase 1: divide large input file into small files.
  url_buf buf(FLAGS_block_size + MAX_URL_LEN);
  std::shared_ptr<std::condition_variable> to_works =
      std::make_shared<std::condition_variable>();

  std::shared_ptr<std::condition_variable> from_work =
      std::make_shared<std::condition_variable>();

  std::shared_ptr<std::atomic_bool> stop = std::make_shared<std::atomic_bool>();
  std::shared_ptr<url_map> map = std::make_shared<url_map>();

  url_reader r(buf, to_works, from_work, stop);

  r.add_file(FLAGS_url_file);

  std::vector<int64_t> fd;
  std::vector<std::string> sp_file_list;
  for (int i = 0; i < FLAGS_sfile_num; i++) {
    std::string splited_file =
        "temp/" + FLAGS_url_file + "_" + std::to_string(i);
    sp_file_list.emplace_back(splited_file);

    int filed = open(splited_file.c_str(), O_RDWR | O_CREAT, 0777);
    if (filed == -1) {
      LOG("FATAL: Creat split file %d failed\n", i);
      perror("Create file failed");
      exit(EXIT_FAILURE);
    }
    fd.emplace_back(filed);
  }

  std::vector<std::thread> split_threads;
  std::vector<url_worker *> workers;

  for (int i = 0; i < FLAGS_worker_num; i++) {
    url_worker *worker = new url_split(buf, i, fd);
    LOG("INFO: Construct spilit%d.\n", i);
    workers.emplace_back(worker);
  }

  for (int i = 0; i < FLAGS_worker_num; i++) {
    split_threads.emplace_back(std::move(
        std::thread(work_thread, workers[i], to_works, from_work, stop)));
  }

  r.read_file();

  for (int i = 0; i < FLAGS_worker_num; i++) {
    split_threads[i].join();
  }

  for (int i = 0; i < FLAGS_sfile_num; i++) {
    if (close(fd[i]) == -1) {
      LOG("FATAL: Close split file failed.\n");
      perror(sp_file_list[i].c_str());
      exit(EXIT_FAILURE);
    }
  }

  // Phase 2: read splited files and find 100 most often emerged url in a single
  // file.
  ready.store(0, std::memory_order_release);
  for (int i = 0; i < FLAGS_sfile_num; i++) {
    r.add_file(sp_file_list[i]);
  }
  sp_file_list.clear();

  std::vector<std::thread> counter_threads;
  for (int i = 0; i < FLAGS_worker_num; i++) {
    url_worker *worker = new url_counter(map, buf, i);
    LOG("INFO: Construct counter%d.\n", i);
    free(workers[i]);
    workers[i] = worker;
  }

  for (int i = 0; i < FLAGS_worker_num; i++) {
    counter_threads.emplace_back(std::move(
        std::thread(work_thread, workers[i], to_works, from_work, stop)));
  }

  while (r.read_file() == 1) {
    map->stat();
  }

  for (int i = 0; i < FLAGS_worker_num; i++) {
    counter_threads[i].join();
    free(workers[i]);
  }

  // Output ans.
  std::vector<std::pair<std::string, int32_t>> &topk = map->top_k();
  for (auto it = topk.begin(); it != topk.end(); it++) {
    std::cout << it->first << " " << it->second << std::endl;
  }

  return 0;
}
