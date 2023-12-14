#ifndef IO_LOCK_HH
#define IO_LOCK_HH

#include <mutex>

namespace IOLock
{
    static std::mutex io_mtx;
    void io_lock() { io_mtx.lock(); }
    void io_unlock() { io_mtx.unlock(); }
}

#endif