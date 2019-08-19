#include <gflags/gflags.h>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <atomic>
#include "reader.h" 
#include "turl_counter.h"
#include "turl_map.h"
namespace turl{

std::atomic<int> counted;
std::condition_variable cv;
std::mutex mu;

void read_file(char* buf, Reader &r, std::shared_ptr<url_map> map, std::unordered_map<std::shared_ptr<URL>, uint32_t> &candidates) {
    while(!r.finish()) {
        while (r.start() && counted.load(std::memory_order_acquire) != FLAGS_counter_num) {}
        if (r.start()) {
            map->stat(candidates);
        }
        {
        std::unique_lock<std::mutex> guard(mu);
        counted.store(0, std::memory_order_release);
        r.read_file((void **)&buf);
        }
        cv.notify_all();
    }
    map->stat(candidates);
}
void count(Reader &r, URLCounter &counter) {
    while (!r.finish()) {
        std::unique_lock<std::mutex> guard(mu);
        cv.wait(guard);
        map->sort(counter.id());
        counter.count();
        counted.fetch_add(1, std::memory_order_acq_rel);
        if (r.finish()) { break; }
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
        counters.emplace_back(std::move(std::thread(count, r, c)));
    }
    std::thread t(read_file, buf, r, map, candidates);
    
}
} // namespace turl
