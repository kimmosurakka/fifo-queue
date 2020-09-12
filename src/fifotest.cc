#include <iostream>
#include <thread>

#include "fifo_queue.hh"

typedef fifo_queue<std::string> queue_t;

std::string obfuscate(const std::string& str) {
	return std::string(str.rbegin(), str.rend());
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
