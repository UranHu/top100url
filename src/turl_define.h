#ifndef _H_DEFINE_
#define _H_DEFINE_
#include <stdint.h>
namespace turl {

const int64_t DATA_BLOCK = 256 * 1024 * 1024;
const int64_t WORKER_NUM = 10;
const int64_t HASH_SHARDING = 128;

int phase = 0;
#define LOG(format, ...) \

} //namespace turl
#endif