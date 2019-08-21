#include <gflags/gflags.h>
#include <iostream>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <atomic>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "turl_reader.h" 
#include "turl_counter.h"
#include "turl_split.h"
#include "turl_map.h"

using namespace turl;

std::atomic<int> counted;
std::atomic<int> sorted;
std::atomic<int> splited;
std::atomic_bool producer_ready(false);
std::atomic<int> customers_ready(0);
std::condition_variable cv1;
std::condition_variable cv2;
std::mutex mu;

void read_file(char* buf, url_reader &r) {
    while(!r.finish()) {
        producer_ready.store(true, std::memory_order_release);
        while (customers_ready.load(std::memory_order_acquire) != FLAGS_counter_num) { usleep(100); }
        producer_ready.store(false, std::memory_order_release);
        while (r.start() && splited.load(std::memory_order_acquire) != FLAGS_counter_num) { usleep(100); }
        splited.store(0, std::memory_order_release);
        customers_ready.store(0, std::memory_order_release);
        {
        std::unique_lock<std::mutex> guard(mu);
        r.read_file((void *)buf);
        cv1.notify_all();
        }
    }
    splited.store(0, std::memory_order_release);
    customers_ready.store(0, std::memory_order_release);
}

void split(url_reader &r, url_split &sp) {
    while (!r.finish()) 
    {
        while (!producer_ready.load(std::memory_order_acquire)) { usleep(100); }
        {
        std::unique_lock<std::mutex> guard(mu);
        customers_ready.fetch_add(1, std::memory_order_acq_rel);
        cv1.wait(guard);
        }
        sp.split();
        splited.fetch_add(1, std::memory_order_acq_rel);
    }
}

void read_spilt_file(char* buf, url_reader &r, std::shared_ptr<url_map> map, std::vector<std::string> &file_list) {
    for (int i = 0; i < FLAGS_sfile_num; i++) {
        LOG("INFO: read split file%d.\n", i);
        std::string splited_file = file_list[i];
        r.init(splited_file);
        while(!r.finish()) {
            producer_ready.store(true, std::memory_order_release);
            while (customers_ready.load(std::memory_order_acquire) != FLAGS_counter_num) { usleep(100); }
            producer_ready.store(false, std::memory_order_release);
            counted.store(0, std::memory_order_release);
            customers_ready.store(0, std::memory_order_release);
            {
            std::unique_lock<std::mutex> guard(mu);
            r.read_file((void *)buf);
            }
            cv2.notify_all();
            LOG("INFO: Cv2 notify all\n");
            while (counted.load(std::memory_order_acquire) != FLAGS_counter_num) { usleep(100); }
        }
        while (sorted.load(std::memory_order_acquire) != FLAGS_counter_num) { usleep(100); }
        customers_ready.store(0, std::memory_order_release);
        map->stat();
        counted.store(0, std::memory_order_release);
        sorted.store(0, std::memory_order_release);
    }
}

void count(url_reader &r, url_counter &counter, std::shared_ptr<url_map> map) {
    for (int i = 0; i < FLAGS_sfile_num; i++) {
        while (!producer_ready.load(std::memory_order_acquire)) { usleep(100); }
        int id = counter.id();
        int max_round;
        if (r.file_size() <= FLAGS_block_size) {
            int64_t interval_size = r.file_size() / FLAGS_counter_num;
            max_round = 1;
            counter.start_at(id * interval_size);
            counter.end_at((id == FLAGS_counter_num - 1)? r.file_size(): (id + 1) * interval_size - 1);
            counter.set_max_round(1);
        } else {
            int64_t interval_size = FLAGS_block_size / FLAGS_counter_num;
            int r1 = r.file_size() / FLAGS_block_size;
            int tail_counter_id = (r.file_size() % FLAGS_block_size) / interval_size + 1;
            counter.start_at(id * interval_size);
            counter.end_at((id == FLAGS_counter_num - 1)? r.file_size(): (id + 1) * interval_size - 1);
            max_round = r1+1;
            counter.set_max_round((id < tail_counter_id)? r1+1: r1);
        }
        for (int j = 0; j < max_round; j++) {
            {
            std::unique_lock<std::mutex> guard(mu);
            customers_ready.fetch_add(1, std::memory_order_acq_rel);
            std::cout << customers_ready.load() << std::endl;
            cv2.wait(guard);
            }
            counter.count();
            counted.fetch_add(1, std::memory_order_acq_rel);
        }
        while (counted.load(std::memory_order_acquire) != FLAGS_counter_num) { usleep(100); }
        map->sort(counter.id());
        sorted.fetch_add(1, std::memory_order_acq_rel);
    }
}

int main(int argc, char* argv[]) {
    
    gflags::ParseCommandLineFlags(&argc, &argv, true); 

    char *buf = (char*)malloc((FLAGS_block_size + 2048) * sizeof(char));
    
    std::shared_ptr<url_map> map = std::make_shared<url_map>();

    url_reader r(FLAGS_url_file); 
    
    std::vector<int64_t> fd;
    std::vector<std::string> sp_file_list;
    for (int i = 0; i < FLAGS_sfile_num; i++) {
        std::string splited_file = "temp/" + FLAGS_url_file + "_" + std::to_string(i);
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
    std::vector<url_split> splits;

    int64_t interval_size = FLAGS_block_size / FLAGS_counter_num;
    int tail = r.file_size() % FLAGS_block_size;
    int r1 = r.file_size() / FLAGS_block_size;
    int tail_counter_id = (tail / interval_size) + 1;
    for (int i = 0; i < FLAGS_counter_num; i++) {
        int64_t start = i * interval_size;
        int64_t end = (i == FLAGS_counter_num - 1)? FLAGS_block_size: (i + 1) * interval_size - 1;
        url_split sp(buf, start, end, i, (i < tail_counter_id)? r1+1:r1, fd);
        LOG("INFO: Construct counter%d, start %ld, end %ld\n", i, start, end);
        splits.emplace_back(sp);
    }

    for(int i = 0; i < FLAGS_counter_num; i++) {
        split_threads.emplace_back(std::move(std::thread(split, std::ref(r), std::ref(splits[i]))));
    }

    std::thread t1(read_file, buf, std::ref(r));
    t1.join();
    for (int i = 0; i < FLAGS_counter_num; i++) {
        split_threads[i].join();
    }

    for (int i = 0; i < FLAGS_sfile_num; i++) {
            if (close(fd[i]) == -1) {
                LOG("FATAL: Close split file failed.\n");
                perror("Close file failed");
                exit(EXIT_FAILURE);
            }
    }

    LOG("Phase 1 finished.\n");

    std::thread t2(read_spilt_file, buf, std::ref(r), map, std::ref(sp_file_list));
    
    std::vector<std::thread> counter_threads;
    std::vector<url_counter> counters;
    for (int i = 0; i < FLAGS_counter_num; i++) {
        url_counter c(map, buf, i);
        LOG("INFO: Construct counter%d.\n", i);
        counters.emplace_back(c);
    }

    for(int i = 0; i < FLAGS_counter_num; i++) {
        counter_threads.emplace_back(std::move(std::thread(count, std::ref(r), std::ref(counters[i]), map)));
    }

    t2.join();
    for (int i = 0; i < FLAGS_counter_num; i++) {
        counter_threads[i].join();
    }
    
    std::vector< std::shared_ptr<URL> >& topk = map->top_k();

    for (int i = 0; i < topk.size(); i++) {
        std::cout << topk[i]->url_ << " " << topk[i]->t_ << std::endl;
    }
    
    free((void*)buf);

    return 0;
}
