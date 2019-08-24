#include "turl_reader.h"
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
namespace turl {

url_reader::url_reader(url_buf &b, std::shared_ptr<std::condition_variable> t_w,
                       std::shared_ptr<std::condition_variable> f_w,
                       std::shared_ptr<std::atomic_bool> stop)
    : fd_(0), file_size_(0), pos_(0), next_read_(0), buf_(b), to_workers(t_w),
      from_worker(f_w), stop_(stop) {}

url_reader::~url_reader() {
  if (close(fd_) == -1) {
    LOG("FATAL: Close file failed.\n");
  }
}

void url_reader::init(const std::string &fp) {
  if (fd_ > 0) {
    if (close(fd_) == -1) {
      LOG("FATAL: Close file failed.\n");
    }
  }

  pos_ = 0;
  struct stat statbuf;
  if (stat(fp.c_str(), &statbuf) != 0) {
    LOG("FATAL: Get file size failed.\n");
    perror(fp.c_str());
  }
  file_size_ = statbuf.st_size;
  next_read_ = (FLAGS_block_size < file_size_) ? FLAGS_block_size + MAX_URL_LEN
                                               : file_size_ + MAX_URL_LEN;
  fd_ = open(fp.c_str(), O_RDONLY);
  if (fd_ == -1) {
    LOG("FATAL: Open file failed\n");
    perror(fp.c_str());
    exit(EXIT_FAILURE);
  }
}

void url_reader::add_file(const std::string file) {
  if (read_list.empty()) {
    stop_->store(false, std::memory_order_release);
  }
  read_list.emplace_back(file);
}

int url_reader::read_file() {
  if (read_list.empty()) {
    return -1;
  }
  std::string &file = read_list.front();
  init(file);
  LOG("%s\n", file.c_str());
  read_list.pop_front();
  if (file_size_ == 0) {
    if (read_list.empty()) {
        stop_->store(true, std::memory_order_release);
        to_workers->notify_all();
    }
    return 1;
  }
  while (pos_ < file_size_) {
    LOG("INFO: Read length %ld.\n", next_read_);
    if (read(fd_, buf_.buf(), next_read_) == -1) {
      LOG("FATAL: Read file failed, pos: %ld, length: %ld.\n", pos_,
          next_read_);
    }
    pos_ += next_read_ + 1;
    LOG("INFO: Current reader pos >> %ld <<\n", pos_);
    buf_.has_read(next_read_);
    next_read_ = (FLAGS_block_size < file_size_ - pos_ + 1)
                     ? FLAGS_block_size
                     : file_size_ - pos_ + 1;
    next_read_ += MAX_URL_LEN;
    to_workers->notify_all();
    {
    std::unique_lock<std::mutex> guard(mu_);
    from_worker->wait(guard);
    }
  }
  if (read_list.empty()) {
    stop_->store(true, std::memory_order_release);
    to_workers->notify_all();
  }
  return 1;
}
} // namespace turl
