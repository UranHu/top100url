#include "turl_counter.h"
#include <functional>

namespace turl {
url_counter::url_counter(std::shared_ptr<url_map> map, url_buf &buf,
                         const int id)
    : url_worker(buf, id), map_(map) {}

void split(const char *ch, int64_t &p) {
  p = 0;
  while ((*(ch + p) != '\n') && (*(ch + p) != EOF)) {
    p++;
  }
}

void url_counter::work() {
  LOG("INFO: Counter%d starts counting.\n", id_);
  if (start_ == end_) {
    LOG("INFO: Counter%d has no job to do, exit.\n", id_);
    return;
  }

  int64_t pos = start_;
  if ((start_ == 0) || (*(buf_.buf() + start_ - 1) == '\n')) {
  } else {
    int64_t p;
    split(buf_.buf() + pos, p);
    pos += p + 1;
  }
  std::hash<std::string> str_hash;
  while (pos < end_) {
    int64_t p;
    split(buf_.buf() + pos, p);
    std::string url(buf_.buf() + pos, p);
    if (p > MAX_URL_LEN) {
      LOG("ERROR: get a invaild url, maybe input file is not compliant. >> %s "
          "<< counter_id %d, pos %ld length %ld\n",
          url.c_str(), id_, pos, p);
    } else {
      int hash_id = (str_hash(url) > 2) % FLAGS_hash_shardings;
      map_->insert_url(hash_id, url);
    }
    pos += p + 1;
    if ((*(buf_.buf() + pos - 1)) == EOF) {
      break;
    }
  }
  LOG("INFO: Counter%d finishs.\n", id_);
}
} // namespace turl