#pragma once

/// A blocking FIFO queue

#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <system_error>

/// Base class handles blocking & closing
class fifo_queue_base {
public:
	// Make noncopyable
	fifo_queue_base() = default;
	fifo_queue_base(const fifo_queue_base&) = delete;
	fifo_queue_base& operator=(const fifo_queue_base&) = delete;

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

/// Generic FIFO queue. Provides operations for adding work item to queue
/// and retrieving from it.
template<typename T>
class fifo_queue : public fifo_queue_base {
public:
	/// The data item type.
	typedef T value_type;

	/// Put item to queue. Returns false if operation fails (queue is closed)
	inline bool operator<<(T&& item);

	/// Get next item from queue. Return false if queue is closed.
	inline bool operator>>(T& dest);
private:
	std::queue<T> queue_;
};

template <typename T>
inline
bool fifo_queue<T>::operator<<(T&& item) {
	auto lock = get_lock();
	if (is_closed()) {
		return false;
	}
	queue_.push(std::move(item));
	notify();
	return true;
}

template <typename T>
inline
bool fifo_queue<T>::operator>>(T& dest) {
	auto lock = wait_until([this](){return !queue_.empty();});
	if (queue_.empty()) {
		return false;
	}
	dest = std::move(queue_.front());
	queue_.pop();
	return true;
}
