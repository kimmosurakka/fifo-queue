/**
 * Copyright (c) 2020 Kimmo Surakka
 *
 * Licensed under the MIT license. See file COPYING in the
 * project root for details.
 **/

// Implementation for blocking_base

#include "fifo_queue.hh"

void blocking_base::close() {
    auto lock = get_lock();
    closed_ = true;
    condition_var_.notify_all();
}

blocking_base::lock_t
blocking_base::wait_until(std::function<bool()> cond) {
    auto lock = get_lock();
    while (!cond() && !is_closed() ) {
        condition_var_.wait(lock);
    }
    return lock;
}
