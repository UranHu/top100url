#include "reader.h"
#include <sys/mman.h>
#include <fcntl.h>
namespace turl {
    Reader::Reader(const std::string fp, std::shared_ptr<std::condition_variable> cv):
            file_path_(fp),
            fd_(-1),
            pos_(0),
            cv_(cv) {
        fd_ = open(file_path_.c_str(), O_RDONLY);
        if (fd_ == -1) {
            LOG("Open file failed");
            exit(EXIT_FAILURE);
        } 
        phase = 1;
    }

    Reader::~Reader() {
    }

    void Reader::read_file(void **buf) { 
        while (true) {
            std::unique_lock<std::mutex> guard(*mu_);
            if (pos_ > file_size_) {
                if (phase == 1) {
                    phase++;
                    pos_ = 0;

                } else if (phase ==2) {
                    break;
                }
            }
            if (*buf == nullptr) {
            } else if (munmap((*buf), DATA_BLOCK) != 0) {
                LOG("munmap failed, fd:%d, pos:%ld, phase:%d.", fd_, pos_, phase);
            } 
            (*buf) = mmap(NULL, DATA_BLOCK, PROT_READ, MAP_PRIVATE, fd_, pos_);
            if ((*buf) == MAP_FAILED) {
                LOG("mmap failed, fd:%d, pos:%ld, phase:%d.", fd_, pos_, phase);
            }
            pos_ += DATA_BLOCK;
            cv_->wait(guard, []() {
                for (int i = 0; i < WORKER_NUM; ++i) {
                    if (!counters_[i].finish()) 
                        return false;
                } 
                return true;
            });
        }
    }

    static Reader* reader; 
    static Reader* get_reader() { return reader;}
} //namespace turl