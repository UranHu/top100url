#ifndef _H_URLMAP_
#define _H_URLMAP_

#include <mutex>
#include <unordered_map>
#include <vector>
#include <memory>
#include "turl_define.h"

namespace turl {
// a priority queue to merge k sorted vectors.
class url_heap {
    public:
        url_heap(std::vector< std::vector< std::shared_ptr<URL> > > &times);
        ~url_heap();
        void build_heap();
        void heapify(int i);
        int pop(std::shared_ptr<URL> &url);
    private:
        struct heap_node {
            std::shared_ptr<URL> url;
            std::size_t vid;
            heap_node(std::shared_ptr<URL> u, std::size_t id): url(u), vid(id) {}
        };
        void swap(int i, int j);
        std::vector<heap_node> heap_;
        std::vector< std::vector< std::shared_ptr<URL> > > &times_;
        std::vector<std::int32_t> cursors_;
        int poped;
        int topk_t_;
        int heap_size_;
};

class url_map {
    public:
        url_map();
        ~url_map();
        void stat();
        void insert_url(const int idx, const std::string url);
        void sort(const int idx);
        std::vector< std::shared_ptr<URL> >& top_k() { return times[FLAGS_counter_num]; }
    private:
        // multiple hashtables and vecotrs to support concurrency.
        // <std::string, int32_t> -> <url,  index of the corresponding URL in times>
        std::vector< std::unordered_map<std::string, int32_t> > maps;
        std::vector< std::vector< std::shared_ptr<URL> > > times;
        // using mutexes to protect hashtables.
        std::mutex mu[4];
};
} //namespace turl

#endif
