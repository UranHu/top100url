#include <gflags/gflags.h>
#include <iostream>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <atomic>
#include "reader.h" 
#include "turl_counter.h"
#include "turl_map.h"
namespace turl{

std::atomic<int> counted;
std::atomic<int> sorted;
std::condition_variable cv;
std::mutex mu;

void read_file(char* buf, Reader &r, std::shared_ptr<url_map> map, std::unordered_map<std::string, uint32_t> &candidates) {
    while(!r.finish()) {
        while (r.start() && sorted.load(std::memory_order_acquire) != FLAGS_counter_num) {}
        if (r.start()) {
            map->stat(candidates);
        }
        counted.store(0, std::memory_order_release);
        sorted.store(0, std::memory_order_release);
        {
        std::unique_lock<std::mutex> guard(mu);
        r.read_file((void **)&buf);
        }
        cv.notify_all();
    }
    map->stat(candidates);
    counted.store(0, std::memory_order_release);
    sorted.store(0, std::memory_order_release);
}
void count(Reader &r, URLCounter &counter, std::shared_ptr<url_map> map) {
    while (!r.finish()) 
    {
        std::unique_lock<std::mutex> guard(mu);
        cv.wait(guard);
        counter.count();
        counted.fetch_add(1, std::memory_order_acq_rel);
        while (counted.load(std::memory_order_acquire) != FLAGS_counter_num) {}
        map->sort(counter.id());
        sorted.fetch_add(1, std::memory_order_acq_rel);
    }
}
int main(int argc, char** argv) {
    gflags::ParseCommandLineFlags(&argc, &argv, true); 

    std::shared_ptr<std::condition_variable> cv_ptr = std::make_shared<std::condition_variable>();
    char *buf;
    std::unordered_map<std::string, uint32_t> candidates;
    std::shared_ptr<url_map> map = std::make_shared<url_map>();
    counted.store(0, std::memory_order_release);

    Reader r(FLAGS_url_file, cv_ptr); 
    std::vector<std::thread> counters;
    int64_t interval_size = FLAGS_block_size / FLAGS_counter_num;
    for (int i = 0; i < FLAGS_counter_num; i++) {
        int64_t start = i * interval_size;
        int64_t end = (i == FLAGS_counter_num - 1)? FLAGS_block_size: (i + 1) * interval_size - 1;
        URLCounter c(map, buf, start, end, i, cv_ptr);
        counters.emplace_back(std::move(std::thread(count, r, c, map)));
    }
    std::thread t(read_file, buf, r, map, candidates);
    std::cout << candidates.size() << std::endl; 
}
} // namespace turl
