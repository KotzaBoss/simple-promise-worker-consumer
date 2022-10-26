#pragma once

#include <cassert>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;
#include <string>

#include "promiser.hpp"

struct WorkDone : std::runtime_error {
	WorkDone()
		: runtime_error("WorkSource is DONE")
	{}
};

struct WorkSource : Promiser<size_t> {
	using Future = Promiser<size_t>::Future;

private:
	std::jthread loop;
public:
	WorkSource()
		: loop {std::jthread([this](auto stoken) {
				auto counter = 0ul;
				while (not stoken.stop_requested()) {
					using namespace std::chrono;

					// This sequence EMPIRICALLY produces fewer future_already_retrieved exceptions
					set_value(counter++);
					reset();
					std::this_thread::sleep_for(milliseconds((std::rand() % 250) + 50));
				}
			})
		}
	{}

	~WorkSource() {
		assert(loop.get_stop_token().stop_requested());
	}

	auto future_work() -> Future {
		return get_future();
	}

	auto stop() {
		set_exception(WorkDone());
		loop.request_stop();
	}
};

