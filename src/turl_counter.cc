#include <functional>
#include "turl_counter.h"

namespace turl {
    url_counter::url_counter(std::shared_ptr<url_map> map, char* buf, const int id): 
                map_(map), 
                buf_(buf),
                id_(id),
                round(0) {}
    url_counter::~url_counter() {}
    
    void split(const char* ch, int64_t &p) {
        p = 0;
        while ((*(ch + p) != '\n') && (*(ch + p) != EOF)) { p++; } 
    }

    void url_counter::count() {
        LOG("INFO: Counter%d starts counting, current round %d, max round %d\n", id_, round + 1, max_round);
        if (round >= max_round || start_ == end_)
            return;

        int64_t pos = start_;
        if ((start_ == 0) || (*(buf_ + start_ - 1) == '\n')) {
        } else {
            int64_t p;
            split(buf_ + pos, p);
            pos += p + 1;
        }
        std::hash<std::string> str_hash;
        while(pos < end_) {
            int64_t p;
            split(buf_ + pos, p);
            std::string url(buf_ + pos, p);
            if (p > MAX_URL_LEN) {
                LOG("ERROR: get a invaild url, maybe input file is not compliant. >> %s << counter_id %d, round %d, pos %ld length %ld\n", url.c_str(), id_, round, pos, p);
            } else {
                int hash_id = (str_hash(url) > 2) % FLAGS_hash_shardings; 
                map_->insert_url(hash_id, url);
            }
            pos += p + 1;
            if ((*(buf_ + pos - 1)) == EOF) {
                break;
            }
        }
        LOG("INFO: Counter%d finishs round %d.\n", id_, round + 1);
        round++;
    }       
} //namespace turl