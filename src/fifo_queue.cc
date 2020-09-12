#include "fifo_queue.hh"

blocking_base::lock_t
blocking_base::wait_until(std::function<bool()> cond) {
    auto lock  = get_lock();
    while (!cond()) {
        condition_var_.wait(lock);
    }
    return lock;
}
