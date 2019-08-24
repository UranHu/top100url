#include "turl_split.h"
#include <fcntl.h>
#include <functional>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace turl {
url_split::url_split(url_buf &buf, int id, std::vector<int64_t> &fd_v)
    : url_worker(buf, id), fd(fd_v) {}

void split_url(const char *ch, int64_t &p) {
  p = 0;
  while ((*(ch + p) != '\n') && (*(ch + p) != EOF)) {
    p++;
  }
}

void url_split::work() {
  LOG("INFO: Split%d starts.\n", id_);
  if (start_ == end_) {
    LOG("INFO: Splite%d has no job to do, exit.\n", id_);
    return;
  }

  int64_t pos = start_;
  if ((start_ == 0) || (*(buf_.buf() + start_ - 1) == '\n')) {
  } else {
    int64_t p;
    split_url(buf_.buf() + pos, p);
    pos += p + 1;
  }
  std::hash<std::string> str_hash;
  while (pos < end_) {
    int64_t p;
    split_url(buf_.buf() + pos, p);
    std::string url(buf_.buf() + pos, p + 1);
    if (p > MAX_URL_LEN) {
      LOG("ERROR: get a invaild url, maybe input file is not compliant. >> %s "
          "<< splite id %d pos %ld length %ld.\n",
          url.c_str(), id_, pos, p);
    } else {
      int16_t hash_id = str_hash(url) % FLAGS_sfile_num;
      if (write(fd[hash_id], url.c_str(), url.size()) != url.size()) {
        perror("write failed");
        LOG("ERROR: Partical write or write failed, url >> %s << file_id %d.\n",
            url.c_str(), hash_id);
      }
    }
    pos += p + 1;
    if ((*(buf_.buf() + pos - 1)) == EOF) {
      break;
    }
  }
  LOG("INFO: Split%d finishs.\n", id_);
}
} // namespace turl