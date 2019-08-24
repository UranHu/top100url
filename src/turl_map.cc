#include "turl_map.h"
#include <algorithm>

namespace turl {

url_heap::url_heap() {
  for (int i = 0; i < FLAGS_top_k; i++) {
    heap_.emplace_back(std::make_pair("", 0));
  }
}

void url_heap::insert(std::unordered_map<std::string, int32_t>::iterator it) {
  if (it->second > heap_[0].second) {
    heap_[0].first = it->first;
    heap_[0].second = it->second;
    heapify(0);
  }
}

void url_heap::heapify(int i) {
  if (i >= FLAGS_top_k)
    return;
  int l = (i * 2 + 1);
  int r = i * 2 + 2;
  if (l < FLAGS_top_k && r < FLAGS_top_k) {
    int min_child = (heap_[l].second < heap_[r].second) ? l : r;
    if (heap_[min_child].second < heap_[i].second) {
      swap(i, min_child);
      heapify(min_child);
    }
  } else if (l < FLAGS_top_k) {
    if (heap_[l].second < heap_[i].second) {
      swap(i, l);
    }
  }
}

void url_heap::swap(int i, int j) {
  auto tmp = heap_[i];
  heap_[i] = heap_[j];
  heap_[j] = tmp;
}

url_map::url_map() {
  maps.resize(FLAGS_hash_shardings);
  for (int i = 0; i < FLAGS_hash_shardings; i++) {
    std::shared_ptr<std::mutex> p = std::make_shared<std::mutex>();
    mu.emplace_back(p);
  }
}

void url_map::stat() {
  LOG("INFO: Stat starts.\n");
  // maintain a size fixed to Flags_top_k heap. father node is smaller than
  // child(ren).
  for (int i = 0; i < maps.size(); i++) {
    for (auto it = maps[i].begin(); it != maps[i].end(); it++) {
      heap.insert(it);
    }
  }
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

} // namespace turl
