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
        cv_(cv) {
    struct stat statbuf;
    if (stat(file_path_.c_str(), &statbuf) != 0) {
        LOG("Get file size failed.\n");
    }
    file_size_ = statbuf.st_size; 
    map_len = (FLAGS_block_size < file_size_)? FLAGS_block_size : file_size_;
    fd_ = open(file_path_.c_str(), O_RDONLY);
    if (fd_ == -1) {
        LOG("Open file failed\n");
        exit(EXIT_FAILURE);
    }
}

Reader::~Reader() {
    if (close(fd_) == -1) {
        LOG("Close file failed.\n");
    }
}

void Reader::read_file(void **buf) { 
        if (*buf == nullptr) {
        } else if (munmap((*buf), map_len) != 0) {
            LOG("munmap failed, fd:%d, pos:%ld.\n", fd_, pos_);
        }
        (*buf) = mmap(NULL, map_len, PROT_READ, MAP_PRIVATE, fd_, pos_);
        if ((*buf) == MAP_FAILED) {
            LOG("mmap failed, fd:%d, pos:%ld.\n", fd_, pos_);
        }
        pos_ += map_len + 1; 
        map_len = (FLAGS_block_size < file_size_ - pos_ + 1)? FLAGS_block_size : file_size_ - pos_ + 1;
}
} //namespace turl