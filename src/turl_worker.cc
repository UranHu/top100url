#include "turl_worker.h"

namespace turl {
url_worker::url_worker(url_buf &buf, int id)
    : buf_(buf), start_(0), end_(0), id_(id) {}

bool url_worker::get_task() {
    LOG("Worker %d try to get task.\n", id_);
    std::pair<int64_t, int64_t> p;
    bool get = false;
    if (get = buf_.pop_task(p)) {
        start_ = p.first;
        end_ = p.second;
        LOG("Worker %d get work, start at%ld, end at %ld.\n", id_, start_, end_);
    }
    return get; 
}

} // namespace turl