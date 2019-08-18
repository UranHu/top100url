#include "reader.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
namespace turl {

Reader::Reader(const std::string fp, std::shared_ptr<std::condition_variable> cv):
        file_path_(fp),
        fd_(-1),
        pos_(0),
        map_len((FLAGS_block_size < file_size_)? FLAGS_block_size : file_size_),
        cv_(cv) {
    fd_ = open(file_path_.c_str(), O_RDONLY);
    if (fd_ == -1) {
        LOG("Open file failed\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < FLAGS_counter_num; ++i) {
        
    }
}

Reader::~Reader() {
    if (close(fd_) == -1) {
        LOG("Close file failed.\n");
    }
}

void Reader::read_file(void **buf) { 
    while (pos_ < file_size_) {
        std::unique_lock<std::mutex> guard(*mu_);
        // set all counters done_ false
        
        if (*buf == nullptr) {
        } else if (munmap((*buf), map_len) != 0) {
            LOG("munmap failed, fd:%d, pos:%ld.\n", fd_, pos_);
        }
        (*buf) = mmap(NULL, map_len, PROT_READ, MAP_PRIVATE, fd_, pos_);
        if ((*buf) == MAP_FAILED) {
            LOG("mmap failed, fd:%d, pos:%ld.\n", fd_, pos_);
        }
        pos_ += map_len; 
        map_len = (FLAGS_block_size < file_size_ - pos_ + 1)? FLAGS_block_size : file_size_ - pos_ + 1;
        cv_->wait(guard, [this]() {
            for (int i = 0; i < FLAGS_counter_num; ++i) {
                if (!counters_[i]->finish()) 
                    return false;
            } 
            return true;
        });
        map_->stat();
    }
}
} //namespace turl