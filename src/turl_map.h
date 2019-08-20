#ifndef _H_URLMAP_
#define _H_URLMAP_

#include "turl_define.h"
#include <mutex>
#include <unordered_map>
#include <vector>
#include <memory>

namespace turl {

class url_heap {
    public:
        url_heap(std::vector< std::vector< std::shared_ptr<URL> > > &times);
        ~url_heap();
        void build_heap();
        void heapify(int i);
        int pop(std::string &s);
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
        void stat(std::unordered_map<std::string, uint32_t> &candidates);
        void insert_url(const int idx, const std::string url);
        void sort(const int idx);
    private:
        std::vector< std::unordered_map<std::string, uint32_t> > maps;
        std::vector< std::vector< std::shared_ptr<URL> > > times;
        std::vector<std::mutex> mu;
};
} //namespace turl

#endif