#include <functional>
#include "turl_counter.h"

namespace turl {
    URLCounter::URLCounter(std::shared_ptr<url_map> map, char* buf, const int64_t start, const int64_t end, const int id, std::shared_ptr<std::condition_variable> cv): 
                map_(map), 
                buf_(buf),
                start_(start),
                end_(end),
                id_(id),
                cv_(cv) {}
    URLCounter::~URLCounter() {}
    
    void split(const char* ch, int64_t &p) {
        p = 0;
        while ((*(ch + p) != '\n') && (*(ch + p) != EOF)) { p++; } 
    }

    void URLCounter::count() {
        int64_t pos = start_;
        if ((start_ == 0) || (*(buf_ + start_ - 1) == '\n')) {
        } else {
            int64_t p;
            split(buf_ + pos, p);
            pos += p + 1;
        }
        std::hash<std::string> str_hash;
        while(pos <= end_) {
            int64_t p;
            split(buf_ + pos, p);
            std::string url(buf_ + pos, p);
            int hash_id = str_hash(url) % FLAGS_hash_shardings; 
            map_->insert_url(hash_id, url);
            pos += p + 1;
            if ((*(buf_ + pos - 1)) == EOF) {
                break;
            }
        }
        map_->sort(id_);
    }       
} //namespace turl