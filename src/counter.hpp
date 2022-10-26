#pragma once

#include <chrono>
using namespace std::chrono_literals;

#include "work_source.hpp"

struct CounterError : WorkerError {
	CounterError()
		: WorkerError("Counter is busted")
	{}
};

struct Counter : Worker<size_t> {
private:
	size_t counter;
protected:
	auto setup() -> void override {
		counter = 0;	// Trivial but irl theres more code like opening ports, starting streams etc
	}

	auto work() -> size_t override {
		using namespace std::chrono;

		std::this_thread::sleep_for(milliseconds(std::rand() % 50 + 10));
		if (std::rand() % 2 == 1)
			throw CounterError();
		else
			return counter++;	// Like above trivial only here
	}
};

