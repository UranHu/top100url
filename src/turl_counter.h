#ifndef _H_COUNTER_
#define _H_COUNTER_
#include <condition_variable>
#include <mutex>
#include "turl_map.h"
#include "turl_define.h"
namespace turl {
    class URLCounter {
        public:
            URLCounter(std::shared_ptr<url_map> map, char* buf, const int64_t start, const int64_t end, const int id, std::shared_ptr<std::condition_variable> cv);
           ~URLCounter();
            void count();
            int id() const { return id_; }
        private:
            std::shared_ptr<url_map> map_;
            char* buf_;
            int64_t start_;
            int64_t end_;
            int id_;
            std::shared_ptr<std::condition_variable> cv_;
    };
} //namespace turl
#endif
