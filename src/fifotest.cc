#include <iostream>
#include <thread>
#include <algorithm>

#include "fifo_queue.hh"

typedef fifo_queue<std::string> queue_t;

inline int mod13(int i) {
	return (i + 13) % 26;
}

inline char rot13(char c) {
	if (c >='a' && c <= 'z') {
		return 'a' + mod13(c - 'a');
	}
	if (c >='A' && c <= 'Z') {
		return 'A' + mod13(c - 'A');
	}
	return c;
}

std::string obfuscate(const std::string& str) {
	std::string res;
	std::transform(str.begin(), str.end(), std::back_inserter(res), rot13);
	return res;
}

void producer(queue_t& queue) {
	queue_t::value_type input;
	while(std::getline(std::cin, input)) {
		queue << std::move(input);
	}
	queue.close();
}

void consumer(queue_t& queue) {
	queue_t::value_type str;
	while(queue >> str) {
		std::cout << obfuscate(str) << std::endl;
	}
}

int main() {
	queue_t queue;
	auto consume_thread = std::thread(consumer, std::ref(queue));
	auto produce_thread = std::thread(producer, std::ref(queue));
	produce_thread.join();
	consume_thread.join();
}
