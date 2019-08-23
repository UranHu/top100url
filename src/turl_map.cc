#include <algorithm>
#include "turl_map.h"

namespace turl {

url_heap::url_heap(std::vector< std::unordered_map<std::string, int32_t> > &times):
            times_(times),
            poped(0),
            topk_t_(0),
            heap_size_(0) {
}

void url_heap::build_heap() {
    for (int i = 0; i < times_.size(); i++) {
        for (auto it = times_[i].begin(); it != times_[i].end(); it++) {
            heap_.emplace_back(it);
        }
    }
    heap_size_ = heap_.size();

    for (int i = heap_size_ / 2 - 1; i >= 0; i--) {
        int l = (i * 2 + 1);
        int r = i * 2 + 2;
        if (r < heap_size_ && heap_[r]->second > heap_[i]->second) {
            swap(i, r);
        }
        if (l < heap_size_ && heap_[l]->second > heap_[i]->second) {
            swap(i, l);
        }
    }
}

void url_heap::heapify(int i) {
    if (i >= heap_size_)
        return;
    int l = (i * 2 + 1);
    int r = i * 2 + 2;
    if (l < heap_size_ && r < heap_size_) {
    int max_child = (heap_[l]->second > heap_[r]->second)? l: r;
        if (heap_[max_child]->second > heap_[i]->second) {
            swap(i, max_child);
            heapify(max_child);
        }
    } else if (l < heap_size_) {
        if (heap_[l]->second > heap_[i]->second) {
            swap(i, l);
        }  
    } 
}

int url_heap::pop(std::pair<std::string, int32_t> &url_p) {
    url_p.first = "";
    url_p.second = 0;
    if (heap_size_== 0) {
        return -1;
    }
    auto it = heap_[0];
    if (poped > FLAGS_top_k) {
        return -1;
    }
    url_p = *it;
    if (heap_size_ > 1) {
        swap(0, heap_size_-1);
    }
    heap_size_--;
    poped++;
    heapify(0);
    return 1;
}

void url_heap::swap(int i, int j) {
    auto tmp = heap_[i];
    heap_[i] = heap_[j];
    heap_[j] = tmp;
}

url_map::url_map() {
    // maps[FLAGS_hash_shardings] is used to store an aggregated result.
    maps.resize(FLAGS_hash_shardings + 1);
    for (int i = 0; i < FLAGS_hash_shardings; i++) {
        std::shared_ptr< std::mutex > p = std::make_shared<std::mutex>();
        mu.emplace_back(p);
    }
}


void url_map::stat() {
    LOG("INFO: Stat starts.\n");
    url_heap heap(maps);
    heap.build_heap();
    std::unordered_map<std::string, int32_t> temp_topk;
    std::pair<std::string, int32_t> url;
    // pick 100 most often emerged urls from url_heap
    while (heap.pop(url) == 1) {
        temp_topk[url.first] = url.second;
    }
    
    maps[FLAGS_hash_shardings].swap(temp_topk);

    LOG("INFO: Stat is finished.\n");
    
    // clear old records
    for (int i = 0; i < FLAGS_hash_shardings; ++i) {
        std::unordered_map<std::string, int32_t> tmp_map;
        {
        std::lock_guard<std::mutex> l(*(mu[i]));
        std::swap(tmp_map, maps[i]);
        }
    }
}

void url_map::insert_url(const int idx, const std::string url) {
    {
        std::lock_guard<std::mutex> l(*(mu[idx]));
        if (maps[idx].find(url) == maps[idx].end()) {
            maps[idx][url] = 1;
        } else {
            maps[idx][url] += 1;
        }
    }
}

} //namespace turl
