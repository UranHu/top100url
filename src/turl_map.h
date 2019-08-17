#ifndef _H_URLMAP_
#define _H_URLMAP_

#include "turl_define"
#include <mutex>
#include <unordered_map>

namespace turl {
    class url_map {
        public:
            url_map();
            ~url_map();
            void clear(const int idx);
            void insert_url(const int idx, std::shared_ptr<URL> url);
        private:
            std::unordered_map<std::shared_ptr<URL>, uint32_t> maps[HASH_SHARDING]; 
            std::vector<std::shared_ptr<URL>> times[HASH_SHARDING];
            std::mutex mu[HASH_SHARDING];
    };
} //namespace turl

#endif