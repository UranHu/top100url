#include "turl_buf.h"

namespace turl {

url_buf::url_buf(const int64_t max_size) : max_size_(max_size), read_(0), worked_(0) {
  buf_ = (char *)malloc((max_size_) * sizeof(char));
}

url_buf::~url_buf() { free(buf_); }


} // namespace turl