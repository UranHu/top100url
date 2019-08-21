#include <gflags/gflags.h>
#include <iostream>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <atomic>
#include <unistd.h>
#include "turl_reader.h" 
#include "turl_counter.h"
#include "turl_map.h"

using namespace turl;

std::atomic<int> counted;
std::atomic<int> sorted;
std::condition_variable cv;
std::mutex mu;

void read_file(char* buf, url_reader &r, std::shared_ptr<url_map> map, std::unordered_map<std::string, int32_t> &candidates) {
    while(!r.finish()) {
        while (r.start() && sorted.load(std::memory_order_acquire) != FLAGS_counter_num) { usleep(100); }
        if (r.start()) {
            map->stat(candidates);
        }
        counted.store(0, std::memory_order_release);
        sorted.store(0, std::memory_order_release);
        {
        std::unique_lock<std::mutex> guard(mu);
        r.read_file((void *)buf);
        cv.notify_all();
        }
    }
    while (sorted.load(std::memory_order_acquire) != FLAGS_counter_num) { usleep(100); }
    map->stat(candidates);
    counted.store(0, std::memory_order_release);
    sorted.store(0, std::memory_order_release);
}
void count(url_reader &r, url_counter &counter, std::shared_ptr<url_map> map) {
    while (!r.finish()) 
    {
        {
        std::unique_lock<std::mutex> guard(mu);
        cv.wait(guard);
        }
        counter.count();
        counted.fetch_add(1, std::memory_order_acq_rel);
        while (counted.load(std::memory_order_acquire) != FLAGS_counter_num) { usleep(1000); }
        map->sort(counter.id());
        sorted.fetch_add(1, std::memory_order_acq_rel);
    }
}

int main(int argc, char* argv[]) {
    
    gflags::ParseCommandLineFlags(&argc, &argv, true); 

    char *buf = (char*)malloc((FLAGS_block_size + 2048) * sizeof(char));
    std::unordered_map<std::string, int32_t> candidates;
    std::shared_ptr<url_map> map = std::make_shared<url_map>();

    url_reader r(FLAGS_url_file); 
    std::vector<std::thread> counter_threads;
    std::vector<url_counter> counters;

    int64_t interval_size = FLAGS_block_size / FLAGS_counter_num;
    // Since all counters' counting interval is fixed, 
    // some counters should not work at the last block.
    int tail = r.file_size() % FLAGS_block_size;
    int r1 = r.file_size() / FLAGS_block_size;
    int tail_counter_id = (tail / interval_size) + 1;
    for (int i = 0; i < FLAGS_counter_num; i++) {
        int64_t start = i * interval_size;
        int64_t end = (i == FLAGS_counter_num - 1)? FLAGS_block_size: (i + 1) * interval_size - 1;
        url_counter c(map, buf, start, end, i, (i < tail_counter_id)? r1+1:r1);
        LOG("INFO: Construct counter%d, start %ld, end %ld\n", i, start, end);
        counters.emplace_back(c);
    }

    for(int i = 0; i < FLAGS_counter_num; i++) {
        counter_threads.emplace_back(std::move(std::thread(count, std::ref(r), std::ref(counters[i]), map)));
    }

    std::thread t(read_file, buf, std::ref(r), map, std::ref(candidates));
    t.join();
    for (int i = 0; i < FLAGS_counter_num; i++) {
        counter_threads[i].join();
    }
    
    std::cout << candidates.size() << std::endl; 
    free((void*)buf);
    return 0;
}
