#include "turl_worker.h"

namespace turl {
url_worker::url_worker(url_buf &buf, int id)
    : buf_(buf), start_(0), end_(0), id_(id) {}

bool url_worker::get_task() {
  std::pair<int64_t, int64_t> p;
  bool get = false;
  if (get = buf_.pop_task(p)) {
    start_ = p.first;
    end_ = p.second;
  }
  return get;
}

} // namespace turl