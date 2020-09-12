// fifo_queue.hh
/// A blocking FIFO queue
#pragma once

#include <queue>

#include "blocking_base.hh"

/// Generic FIFO queue. Provides operations for adding work item to queue
/// and retrieving from it.
template<typename T>
class fifo_queue : public blocking_base {
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
