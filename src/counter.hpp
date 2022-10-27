#pragma once

#include <chrono>
#include <string>
using namespace std::literals;

#include "worker.hpp"

struct CounterError : WorkerError {
	CounterError(const size_t current_counter)
		: WorkerError("Counter is busted at "s + std::to_string(current_counter))
	{}
};

struct Counter : Worker<size_t> {
private:
	size_t counter = 0;
protected:
	auto work() -> size_t override {
		using namespace std::chrono;

		std::this_thread::sleep_for(milliseconds(std::rand() % 50 + 10));
		if (std::rand() % 10 > 8)
			throw CounterError(counter);
		else
			return counter++;	// Like above trivial only here
	}
};

