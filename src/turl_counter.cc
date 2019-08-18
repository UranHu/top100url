#include <functional>
#include "turl_counter.h"

namespace turl {
    URLCounter::URLCounter(std::shared_ptr<url_map> map, const int id): 
                map_(map), 
                done_(false),
                id_(id) {}
    URLCounter::~URLCounter() {}
    
    void split(const char* ch, int64_t &p) {
        p = 0;
        while (*(ch + p) != '\n') { p++; }
    }

    void URLCounter::count(const char* buf, const int64_t len, const int64_t start, const int64_t end) {
        int64_t pos = start;
        if (*(buf + start - 1) == '\n') {
        } else {
            int64_t p;
            split(buf + pos, p);
            pos += p + 1;
        }
        std::hash<std::string> str_hash;
        while(pos <= end) {
            int64_t p;
            split(buf + pos, p);
            std::string url(buf + pos, p);
            int hash_id = str_hash(url) % FLAGS_hash_shardings; 
            map_->insert_url(hash_id, url);
            pos += p + 1;
        }
        done_ = true;
        cv_->notify_one();
    }       
} //namespace turl