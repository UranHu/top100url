#ifndef _H_SPLIT_
#define _H_SPLIT_

#include <vector>
#include "turl_map.h"
#include "turl_define.h"

namespace turl {
    class url_split {
        public:
            url_split(char* buf, const int64_t start, const int64_t end, const int id, int max_r, std::vector<int64_t> &fd_v);
           ~url_split();
            void split();
            int id() const { return id_; }
        private:
            char* buf_;
            int64_t start_;
            int64_t end_;
            int id_;
            int max_round;
            int round;
            std::vector<int64_t> &fd;
    };
} //namespace turl
#endif
