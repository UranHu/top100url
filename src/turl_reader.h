#ifndef _H_READER_
#define _H_READER_
#include <stdint.h>
#include <memory>
#include <vector>
#include "turl_define.h"
#include "turl_map.h"

namespace turl{
    class url_reader {
    public:
        url_reader(const std::string fp);
        ~url_reader();
        void init(const std::string fp);
        void read_file(void* buf);
        void next_block() {}
        bool finish() { return pos_ > file_size_; }
        bool start() { return pos_ != 0; }
        int64_t file_size() { return file_size_; }
    private:
        std::string file_path_;
        int fd_;
        int64_t file_size_;
        int64_t pos_;
        int64_t map_len;
        std::shared_ptr<url_map> map_;
    };
} //namespace turl
#endif