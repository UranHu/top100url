#ifndef _H_DEFINE_
#define _H_DEFINE_
#include <stdint.h>
#include <gflags/gflags.h>
#include <stdlib.h>

namespace turl {
DEFINE_string(conf_path, "test.conf", "program configure file.");
DEFINE_string(url_file, "url_file", "input file path.");
DEFINE_int32(top_k, 100, "top k url.");
DEFINE_int64(block_size, 256 * 1024 * 1024, "single reading block size");
DEFINE_int64(counter_num, 16, "number of counters");
DEFINE_int64(hash_shardings, 16, "number of hash shardings");

struct URL {
    std::string url_;
    uint32_t t_;
    void add() { t_++; }
    URL(std::string &url, uint32_t t): url_(url), t_(t) {}
    URL(): url_(std::string()), t_(0) {}
};

#define LOG(format, ...) \
fprintf(stderr, format, ##__VA_ARGS__) 
} //namespace turl
#endif