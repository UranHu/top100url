#ifndef _H_DEFINE_
#define _H_DEFINE_
#include <stdint.h>
#include <gflags/gflags.h>
#include <stdlib.h>

namespace turl {
DECLARE_string(conf_path);
DECLARE_string(url_file);
DECLARE_int32(top_k);
DECLARE_int32(sfile_num);
DECLARE_int64(block_size);
DECLARE_int64(counter_num);
DECLARE_int64(hash_shardings);

struct URL {
    std::string url_;
    uint32_t t_;
    void add() { t_++; }
    URL(std::string url, uint32_t t): url_(url), t_(t) {}
    URL(): url_(std::string()), t_(0) {}
};

#define LOG(format, ...) \
fprintf(stderr, format, ##__VA_ARGS__) 

} //namespace turl
#endif