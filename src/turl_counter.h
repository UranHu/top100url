#ifndef _H_COUNTER_
#define _H_COUNTER_

#include "turl_map.h"
#include "turl_define.h"
namespace turl {
    class url_counter {
        public:
            url_counter(std::shared_ptr<url_map> map, char* buf, const int64_t start, const int64_t end, const int id, int max_r);
           ~url_counter();
            void count();
            int id() const { return id_; }
        private:
            std::shared_ptr<url_map> map_;
            char* buf_;
            int64_t start_;
            int64_t end_;
            int id_;
            int max_round;
            int round;
    };
} //namespace turl
#endif
