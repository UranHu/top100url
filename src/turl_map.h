#ifndef _H_URLMAP_
#define _H_URLMAP_

#include "turl_define.h"
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace turl {
// a heap to pick out top k most emerged url for now.
class url_heap {
public:
  url_heap();
  ~url_heap() = default;
  void heapify(int i);
  void insert(std::unordered_map<std::string, int32_t>::iterator it);
  std::vector<std::pair<std::string, int32_t>> &top_k() { return heap_; }

private:
  void swap(int i, int j);
  std::vector<std::pair<std::string, int32_t>> heap_;
};

class url_map {
public:
  url_map();
  ~url_map() = default;
  void stat();
  void insert_url(const int idx, const std::string url);
  std::vector<std::pair<std::string, int32_t>> &top_k() { return heap.top_k(); }

private:
  // multiple hashtables to support concurrency.
  // <std::string, int32_t> -> <url,  emerged times>
  url_heap heap;
  std::vector<std::unordered_map<std::string, int32_t>> maps;
  // using mutexes to protect hashtables.
  std::vector<std::shared_ptr<std::mutex>> mu;
};
} // namespace turl

#endif
