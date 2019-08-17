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
            URLCounter();
           ~URLCounter();
            bool finish() { return done_;}
        private:
            std::shared_ptr<url_map> map;
            bool done_;
    };
} //namespace turl
#endif