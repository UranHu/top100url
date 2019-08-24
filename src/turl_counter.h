#ifndef _H_COUNTER_
#define _H_COUNTER_

#include "turl_define.h"
#include "turl_map.h"
#include "turl_worker.h"

namespace turl {
class url_counter : public url_worker {
public:
  url_counter(std::shared_ptr<url_map> map, url_buf &buf, const int id);
  void work() override;

private:
  std::shared_ptr<url_map> map_;
};
} // namespace turl
#endif