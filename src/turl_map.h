#ifndef _H_URLMAP_
#define _H_URLMAP_

#include "turl_define.h"
#include <mutex>
#include <unordered_map>
#include <vector>
#include <memory>

namespace turl {
class url_map {
    public:
        url_map();
        ~url_map();
        void stat(std::unordered_map<std::shared_ptr<URL>, uint32_t> &candidates);
        void insert_url(const int idx, const std::string url);
        void sort(const int idx);
    private:
        std::vector< std::unordered_map<std::string, uint32_t> > maps;
        std::vector< std::vector< std::shared_ptr<URL> > > times;
        std::vector<std::mutex> mu;
};
} //namespace turl

#endif