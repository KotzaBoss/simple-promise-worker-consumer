#pragma once

#include <chrono>
using namespace std::chrono_literals;

#include "worker.hpp"

struct CounterError : WorkerError {
	CounterError()
		: WorkerError("Counter is busted")
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
			throw CounterError();
		else
			return counter++;	// Like above trivial only here
	}
};

