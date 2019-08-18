#include <gflags/gflags.h>
#include <condition_variable>
#include <thread>
#include <mutex>
#include "reader.h" 

namespace turl{
std::condition_variable cv;
char *buf;
std::unordered_map<std::string, int> candidates;
std::vector< std::shared_ptr<URL> > candidates_time;
void init() {

}
int main(const int argc, const char** argv) {
    gflags::ParseCommandLineFlags(&argc, &argv, true); 
    init();
    std::shared_ptr<std::condition_variable> cv_ptr(cv);
    Reader r(FLAGS_url_file, cv_ptr); 
    std::thread t(r::readfile, *buf);
    std::vector<std::thread> counters;
    for (int i = 0; i < WORKER_NUM; i++) {
        URLCounter c();
        counters.emplace_back(std::move(std::thread(&c::count,
                                    )));
    }
    
    for (int i = 0; i < counters.size(); i++) {
        counters[i].join();
    }
}
} // namespace turl
