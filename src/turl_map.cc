#include "turl_map.h"
#include <algorithm>

namespace turl {
bool url_cmp(std::shared_ptr<URL> &a, std::shared_ptr<URL> &b) {
    return a->t_ < b->t_;
}

url_map::url_map() {
    maps.resize(FLAGS_hash_shardings);
    times.resize(FLAGS_hash_shardings);
    mu.resize(FLAGS_hash_shardings);
}
url_map::~url_map() {
}

void url_map::stat(std::unordered_map<std::shared_ptr<URL>, uint32_t> &candidates) {
    // pick top_k url in single block
        
    // add top_k url to candidates table

    // clear old records
    for (int i = 0; i < maps.size(); ++i) {
        std::unordered_map<std::string, uint32_t> tmp_map;
        std::vector<std::shared_ptr<URL>> tmp_v;
        {
        std::lock_guard<std::mutex> l(mu[i]);
        std::swap(tmp_map, maps[i]);
        times[i].swap(tmp_v);
        }
    }
}
void url_map::insert_url(const int idx, std::string url) {
    {
        std::lock_guard<std::mutex> l(mu[idx]);
        if (maps[idx].find(url) != maps[idx].end()) {
            times[idx][maps[idx][url]].add();
        } else {
            maps[idx][url] = times[idx].size();
            std::shared_ptr<URL> url_ptr = std::make_shared<URL>(url, 1);
            times[idx].emplace_back(url_ptr);
        }
    }
}

void url_map::sort(const int idx) {
    // only be called after all urls inserted
    std::sort(times[idx].begin(), times[idx].end(), url_cmp);
    std::shared_ptr<URL> topk_time = times[idx][FLAGS_top_k - 1];
    auto it = std::upper_bound(times[idx].begin(), times[idx].end(), topk_time, url_cmp);
    times[idx].erase(it, times[idx].end());
}

} //namespace turl