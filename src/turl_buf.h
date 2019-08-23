#ifndef _H_TURLBUF_
#define _H_TURLBUF_

#include <stdint.h>
#include "turl_define.h"

namespace turl {
class url_buf {
public:
    url_buf(const int64_t max_size): max_size_(max_size), read_(0) {
        buf_ = (char*)malloc((max_size_) * sizeof(char));
    }
    ~url_buf() { free(buf_); };
    char* buf() { return buf_; }
    int64_t read() {return read_; }
    int64_t has_read(const int64_t read) { read_ = read; }
private:
    char *buf_;
    int64_t max_size_;
    int64_t read_;
};
} //namespace turl
#endif