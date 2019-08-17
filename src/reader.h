#ifndef _H_READER_
#define _H_READER_
#include <condition_variable>
#include <memory>
#include <mutex>
#include <vector>
#include "turl_define.h"
#include "turl_counter.h"

namespace turl{
    class Reader {
    public:
        Reader(const std::string fp, std::shared_ptr<std::condition_variable> condition_var);
        ~Reader();
        void read_file(void** buf);
    private:
        std::string file_path_;
        int fd_;
        int64_t file_size_;
        int64_t pos_;
        std::shared_ptr<std::mutex> mu_;
        std::shared_ptr<std::condition_variable> cv_;
        std::vector<URLCounter>& counters_;
    };
} //namespace turl
#endif