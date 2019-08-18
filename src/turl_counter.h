#ifndef _H_COUNTER_
#define _H_COUNTER_
#include <condition_variable>
#include <unordered_map>
#include <mutex>
#include "turl_map.h"
#include "turl_define.h"
namespace turl {
    class URLCounter {
        public:
            URLCounter(std::shared_ptr<url_map> map, const int id);
           ~URLCounter();
            void count(const char* buf, const int64_t len, const int64_t start, const int64_t end);
            bool finish() { return done_;}
        private:
            std::shared_ptr<url_map> map_;
            bool done_;
            const int id_;
            std::shared_ptr<std::condition_variable> cv_;
    };
} //namespace turl
#endif