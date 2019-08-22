#include <functional>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "turl_split.h"

namespace turl {
    url_split::url_split(char* buf, const int64_t start, const int64_t end, const int id, int max_r, std::vector<int64_t> &fd_v): 
                buf_(buf),
                start_(start),
                end_(end),
                id_(id),
                max_round(max_r),
                round(0),
                fd(fd_v) {}
    url_split::~url_split() {}
    
    void split_url(const char* ch, int64_t &p) {
        p = 0;
        while ((*(ch + p) != '\n') && (*(ch + p) != EOF)) { p++; } 
    }

    void url_split::split() {
        LOG("INFO: Split%d starts counting, current round %d, max round %d\n", id_, round + 1, max_round);
        if (round >= max_round)
            return;

        int64_t pos = start_;
        if ((start_ == 0) || (*(buf_ + start_ - 1) == '\n')) {
        } else {
            int64_t p;
            split_url(buf_ + pos, p);
            pos += p + 1;
        }
        std::hash<std::string> str_hash;
        while(pos <= end_) {
            int64_t p;
            split_url(buf_ + pos, p);
            std::string url(buf_ + pos, p + 1);
            if (p > MAX_URL_LEN) {
                LOG("ERROR: get a invaild url, maybe input file is not compliant. >> %s << counter_id %d, round %d, pos %ld length %ld\n", url.c_str(), id_, round, pos, p);
            } else {
                int16_t hash_id = str_hash(url) % FLAGS_sfile_num; 
                if (write(fd[hash_id], url.c_str(), url.size()) != url.size()) {
                    perror("write failed");
                    LOG("ERROR: Partical write or write failed, url >> %s << file_id %d.\n", url.c_str(), hash_id);
                }
            }
            pos += p + 1;
            if ((*(buf_ + pos - 1)) == EOF) {
                break;
            }
        }
        LOG("INFO: Split%d finishs round %d.\n", id_, round + 1);
        round++;
    }       
} //namespace turl