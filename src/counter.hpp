#pragma once

#include <chrono>
#include <string>
using namespace std::literals;

#include "worker.hpp"

struct MaybeBustedCounterError : WorkerError {
	MaybeBustedCounterError(const size_t current_counter)
		: WorkerError("Counter is busted at "s + std::to_string(current_counter))
	{}
};

using MaybeBustedCounterDone = WorkerDone;

struct MaybeBustedCounter : Worker<size_t> {
	using Worker = Worker<size_t>;

private:
	size_t counter = 0;

protected:
	auto work() -> size_t override {
		using namespace std::chrono;

		std::this_thread::sleep_for(milliseconds(std::rand() % 50 + 10));
		if (std::rand() % 10 > 8)
			throw MaybeBustedCounterError(counter);
		else
			return counter++;
	}

public:
	auto future_value() -> Worker::Future {
		return future_work();
	}
};

