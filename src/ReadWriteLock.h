#ifndef _H_READWRITELOCK_
#define _H_READWRITELOCK_

#include <mutex>
#include <atomic>

namespace turl {
class ReadWriteLock {
    public:
        ReadWriteLock() = default;
        ReadWriteLock(ReadWriteLock& rw) = delete;
        ReadWriteLock& operator=(ReadWriteLock &rhs) = delete;
        void write_lock();
        void write_unlock();
        void lock();
        void unlock();
    private:
        std::atomic<int> read;
        std::atomic_bool write;
        std::mutex mu;
};
} //namespace turl

#endif