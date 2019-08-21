#include "turl_define.h"

namespace turl {

DEFINE_string(conf_path, "test.conf", "program configure file.");

DEFINE_string(url_file, "url_file", "input file path.");

DEFINE_int32(top_k, 100, "top k url.");

DEFINE_int32(sfile_num, 512, "split file num.");

DEFINE_int64(block_size, 256 * 1024 * 1024, "single reading block size");

DEFINE_int64(counter_num, 4, "number of counters");

DEFINE_int64(hash_shardings, 4, "number of hash shardings");

} //namespace turl