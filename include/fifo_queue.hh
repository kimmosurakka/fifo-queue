#pragma once

#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

class blocking_base {
public:
	blocking_base() = default;
	blocking_base(const blocking_base&) = delete;
	blocking_base& operator=(const blocking_base&) = delete;
protected:
	typedef std::unique_lock<std::mutex> lock_t;
	lock_t get_lock() {
		return lock_t(mutex_);
	}

	lock_t wait_until(std::function<bool()> condition);

	void notify() {
		condition_var_.notify_one();
	}

private:
	std::mutex mutex_;
	std::condition_variable condition_var_;
};

template<typename T>
class fifo_queue : blocking_base {
public:
	inline void push(T&& item);
	inline T pop();
private:
	std::queue<T> queue_;
};

template <typename T>
inline
void fifo_queue<T>::push(T&& item) {
	auto lock = get_lock();
	queue_.push(std::move(item));
	notify();
}

template <typename T>
inline
T fifo_queue<T>::pop() {
	auto lock = wait_until([this](){return ! queue_.empty();});
	T first = queue_.front();
	queue_.pop();
	return first;
}
