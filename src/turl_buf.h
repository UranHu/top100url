#ifndef _H_TURLBUF_
#define _H_TURLBUF_

#include "turl_define.h"
#include <atomic>
#include <deque>
#include <utility>
#include <stdint.h>
#include <mutex>

namespace turl {
class url_buf {
public:
  url_buf(const int64_t max_size);
  ~url_buf();
  char *buf() { return buf_; }
  int64_t read() { return read_; }
  bool worked() {
    LOG("WORKED %d\n", worked_.load(std::memory_order_acquire));
    return worked_.fetch_add(1, std::memory_order_acq_rel) == FLAGS_worker_num - 1;
  };
  
  int64_t has_read(const int64_t read) {
    std::unique_lock<std::mutex> guard(mu);
    read_ = read;
    worked_.store(0, std::memory_order_release);
    int64_t intervel_l = read_ / FLAGS_worker_num;
    for (int i = 0; i < FLAGS_worker_num; i++) {
    int64_t start = intervel_l * i;
    int64_t end = (i == FLAGS_worker_num - 1) ? read_ : (i + 1) * intervel_l - 1;
      tasks.emplace_back(std::make_pair(start, end));
    }
  }

  bool pop_task(std::pair<int64_t, int64_t> &p) {
    p.first = 0;
    p.second = 0;
    {
    std::unique_lock<std::mutex> guard(mu);
    if (tasks.empty()) {
      LOG("empty tasks list.\n");
      return false;
    }
    p.first = tasks.front().first;
    p.second = tasks.front().second;
    tasks.pop_front();
    LOG("%ld\n", tasks.size());
    }
    return true;
  }

private:
  char *buf_;
  int64_t max_size_;
  int64_t read_;
  std::deque< std::pair<int64_t, int64_t> > tasks;
  std::atomic_int worked_;
  std::mutex mu;
};
} // namespace turl
#endif