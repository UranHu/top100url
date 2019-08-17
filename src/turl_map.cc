#include "turl_map.h"

namespace turl {
    url_map::url_map(): {}
    url_map::~url_map() {

    }

    void url_map::insert_url(const int idx, std::shared_ptr<URL> url) {
        {
            std::lock_guard(mu[idx]);
            if (maps[idx].find(url) != maps[idx].end()) {
                times[idx][maps[idx][url]]++;
            } else {
                maps[idx][url] = times[idx].size();
                times[idx].emplace_back(1);
            }
        }
    }

} //namespace turl