#ifndef _H_DEFINE_
#define _H_DEFINE_
#include <stdint.h>
#include <gflags/gflags.h>
#include <stdlib.h>

namespace turl {
#define MAX_URL_LEN 256

DECLARE_string(conf_path);
DECLARE_string(url_file);
DECLARE_int32(top_k);
DECLARE_int32(sfile_num);
DECLARE_int64(block_size);
// hash_shardings should equal worker_num.
DECLARE_int64(worker_num);
DECLARE_int64(hash_shardings);

#define LOG(format, ...) \
fprintf(stderr, format, ##__VA_ARGS__) 

} //namespace turl
#endif