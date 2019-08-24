#ifndef _H_SPLIT_
#define _H_SPLIT_

#include "turl_define.h"
#include "turl_map.h"
#include "turl_worker.h"
#include <vector>

namespace turl {
class url_split : public url_worker {
public:
  url_split(url_buf &buf, int id, std::vector<int64_t> &fd_v);
  void work() override;

private:
  std::vector<int64_t> &fd;
};
} // namespace turl
#endif
