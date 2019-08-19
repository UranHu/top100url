#include "turl_map.h"
#include <algorithm>

namespace turl {
bool url_cmp(std::shared_ptr<URL> &a, std::shared_ptr<URL> &b) {
    return a->t_ < b->t_;
}

url_heap::url_heap(std::vector< std::vector< std::shared_ptr<URL> > > &times):times_(times) {
    heap_.resize(FLAGS_hash_shardings);
    cursors.resize(FLAGS_hash_shardings);
}

url_heap::~url_heap() { }

void url_heap::build_heap() {
    for (int i = 0; i < times_.size(); i++) {
        if (!times_[i].empty()) {
            heap_node node(times_[i][0], i);
            heap_.emplace_back(node);
            cursors_.emplace_back(1);
        } else {
            cursors_.emplace_back(0);
        }
    }
    heap_size_ = heap_.size();

    for (int i = heap_size_ / 2; i >= 0; i--) {
        int l = (i * 2 + 1);
        int r = i * 2 + 2;
        int max_child = (heap_[l].url->t_ > heap_[r].url->t_)? l: r;
        if (heap_[max_child].url->t_ > heap_[i].url->t_) {
            swap(i, max_child);
        }
    }
}

void url_heap::heapify(int i) {
    int l = (i * 2 + 1);
    int r = i * 2 + 2;
    int max_child = (heap_[l].url->t_ > heap_[r].url->t_)? l: r;
    if (heap_[max_child].url->t_ > heap_[i].url->t_) {
        swap(i, max_child);
        heapify(max_child);
    }
}

int url_heap::pop(std::string &s) {
    s = "";
    if (heap_size_== 0) {
        return -1;
    }
    auto it = heap_.begin();
    s = it->url->url_;
    std::size_t vid = it->vid;
    if (times_[vid].size() > cursors_[vid]) {
        heap_node node(times_[vid][cursors_[vid]], vid);
        (*it) = node;
        cursors_[vid]++;
    } else {
        swap(0, heap_size_);
        heap_size_--;
    }
    heapify(0);
}

void url_heap::swap(int i, int j) {
    heap_node tmp = heap_[i];
    heap_[i] = heap_[j];
    heap_[j] = tmp;
}

url_map::url_map() {
    maps.resize(FLAGS_hash_shardings);
    times.resize(FLAGS_hash_shardings);
    mu.resize(FLAGS_hash_shardings);
}
url_map::~url_map() {
}

void url_map::stat(std::unordered_map<std::string, uint32_t> &candidates) {
    url_heap heap(times);
    heap.build_heap();
    int num = 0;

    // clear old records
    for (int i = 0; i < maps.size(); ++i) {
        std::unordered_map<std::string, uint32_t> tmp_map;
        std::vector<std::shared_ptr<URL>> tmp_v;
        {
        std::lock_guard<std::mutex> l(mu[i]);
        std::swap(tmp_map, maps[i]);
        times[i].swap(tmp_v);
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
    if (times[idx].empty())
        return;
    std::sort(times[idx].begin(), times[idx].end(), url_cmp);
    std::size_t limit = (times[idx].size() > FLAGS_top_k)? FLAGS_top_k: times[idx].size();
    std::shared_ptr<URL> topk_time = times[idx][limit - 1];
    auto it = std::upper_bound(times[idx].begin(), times[idx].end(), topk_time, url_cmp);
    times[idx].erase(it, times[idx].end());
}

} //namespace turl