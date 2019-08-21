#include "turl_reader.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
namespace turl {

url_reader::url_reader(const std::string fp):
        fd_(0) {
    init(fp);
}

url_reader::~url_reader() {
    if (close(fd_) == -1) {
        LOG("FATAL: Close file failed.\n");
    }
}

void url_reader::init(const std::string fp) {
    if (fd_ > 0) {
        if (close(fd_) == -1) {
            LOG("FATAL: Close file failed.\n");
        }
    }

    file_path_ = fp;
    pos_ = 0; 
    struct stat statbuf;
    if (stat(file_path_.c_str(), &statbuf) != 0) {
        LOG("FATAL: Get file size failed.\n");
    }
    file_size_ = statbuf.st_size; 
    map_len = (FLAGS_block_size < file_size_)? FLAGS_block_size + 2048 : file_size_ + 2048;
    fd_ = open(file_path_.c_str(), O_RDONLY);
    if (fd_ == -1) {
        LOG("FATAL: Open file failed\n");
        exit(EXIT_FAILURE);
    }
}

void url_reader::read_file(void *buf) { 
        LOG("INFO: Read length %ld.\n", map_len);

        if (read(fd_, buf, map_len) == -1) {
            LOG("FATAL: Read file failed, pos: %ld, length: %ld.\n", pos_, map_len);
        }
        pos_ += map_len + 1; 
        LOG("INFO: Current reader pos >> %ld <<\n", pos_);
        map_len = (FLAGS_block_size < file_size_ - pos_ + 1)? FLAGS_block_size : file_size_ - pos_ + 1;
        map_len += 2048;
}
} //namespace turl
