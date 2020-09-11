#pragma once

#include <queue>

template<typename T>
class fifo_queue {
public:
	fifo_queue() = default;
	fifo_queue(const fifo_queue&) = delete;
	fifo_queue& operator= (const fifo_queue&) = delete;

	void push(T&& item);
	T pop();
private:
	std::queue<T> queue_;
};

