/**
 * Copyright (c) 2020 Kimmo Surakka
 *
 * Licensed under the MIT license. See file COPYING in the
 * project root for details.
 **/
#pragma once

#include <functional>
#include <mutex>
#include <condition_variable>

/// Base class handles blocking & closing
class blocking_base {
public:
	// Make noncopyable
	blocking_base() = default;
	blocking_base(const blocking_base&) = delete;
	blocking_base& operator=(const blocking_base&) = delete;

	/// Close the queue. In a closed queue, pushing fails
	/// and popping success only if there are work items in the
	/// queue (i.e., pop() on a closed queue won't block)
	void close();

	/// It the queue closed?
	bool is_closed() const { return closed_; }
protected:
	typedef std::unique_lock<std::mutex> lock_t;

	/// Get a lock for this queue's mutex
	lock_t get_lock() {
		return lock_t(mutex_);
	}

	/// Wait until the given condition returns true or the
	/// queue is closed.
	lock_t wait_until(std::function<bool()> condition);

	/// Notify a listener about new work item.
	void notify() {
		condition_var_.notify_one();
	}

private:
	std::mutex mutex_;
	std::condition_variable condition_var_;
	bool closed_ = false;
};
