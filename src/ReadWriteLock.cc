#include <ReadWriteLock.h>

namespace turl
{
ReadWriteLock::write_lock() {
    mu.lock();
    write.store(true, std::memory_order_release);
}

ReadWriteLock::write_unlock() {
   write.store(false, std::memory_order_release);
   mu.unlock();
}

ReadWriteLock::lock() {
    if (read.load(std::memory_order_acquire) > 0) {

    }

}

ReadWriteLock::unlock() {
    if (read.fetch_sub(1, std::memory_order_acq_rel) == 1)
        mu.unlock();
}

} // namepace turl 
