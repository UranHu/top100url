#ifndef _H_READER_
#define _H_READER_
#include <condition_variable>
#include <memory>
#include <mutex>
#include <vector>
#include "turl_define.h"
#include "turl_map.h"
#include <stdint.h>

namespace turl{
    class Reader {
    public:
        Reader(const std::string fp, std::shared_ptr<std::condition_variable> condition_var);
        ~Reader();
        void read_file(void** buf);
        bool finish() { return pos_ > file_size_; }
        bool start() { return pos_ == 0; }
    private:
        std::string file_path_;
        int fd_;
        int64_t file_size_;
        int64_t pos_;
        int64_t map_len;
        std::shared_ptr<std::mutex> mu_;
        std::shared_ptr<std::condition_variable> cv_;
        std::shared_ptr<url_map> map_;
    };
} //namespace turl
#endif