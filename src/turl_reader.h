#ifndef _H_READER_
#define _H_READER_
#include <stdint.h>
#include <memory>
#include <vector>
#include <deque>
#include "turl_define.h"
#include "turl_map.h"
#include "turl_buf.h"

namespace turl{
// url_reader is not thread-safe.
    class url_reader {
    public:
        url_reader(url_buf& b);
        ~url_reader();
        void init(const std::string fp);
        void add_file(const std::string file);
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
        int64_t next_read_;
        std::shared_ptr<url_map> map_;
        url_buf &buf_;  
        std::deque<std::string> read_list;   
    };
} //namespace turl
#endif