#ifndef _H_WORKER_
#define _H_WORKER_
#include "turl_buf.h"
#include "turl_define.h"
#include <condition_variable>
#include <memory>

namespace turl {
class url_worker {
public:
  url_worker(url_buf &buf, int id);
  ~url_worker() = default;
  virtual void work() = 0;
  bool worked() { return buf_.worked(); }
  bool get_task();
  int id() { return id_; }

protected:
  url_buf &buf_;
  int64_t start_;
  int64_t end_;
  int id_;
};
} // namespace turl
#endif