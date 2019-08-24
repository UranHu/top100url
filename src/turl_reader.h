#ifndef _H_READER_
#define _H_READER_
#include "turl_buf.h"
#include "turl_define.h"
#include "turl_map.h"
#include <atomic>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <stdint.h>
#include <vector>

namespace turl {
// url_reader is not thread-safe.
class url_reader {
public:
  url_reader(url_buf &b, std::shared_ptr<std::condition_variable> t_w,
             std::shared_ptr<std::condition_variable> f_w,
             std::shared_ptr<std::atomic_bool> stop);
  ~url_reader();
  void add_file(const std::string file);
  int read_file();
  bool finish() { return pos_ > file_size_; }
  int64_t file_size() { return file_size_; }

private:
  int fd_;
  int64_t file_size_;
  int64_t pos_;
  int64_t next_read_;
  url_buf &buf_;
  std::deque<std::string> read_list;
  std::shared_ptr<std::condition_variable> to_workers;
  std::shared_ptr<std::condition_variable> from_worker;
  std::shared_ptr<std::atomic_bool> stop_;
  std::mutex mu_;
  void init(const std::string &fp);
};
} // namespace turl
#endif