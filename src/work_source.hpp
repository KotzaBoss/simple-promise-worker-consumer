#pragma once

#include <cassert>
#include <thread>
#include <optional>
#include <string>
#include<concepts>

#include "promiser.hpp"

struct WorkerError : std::runtime_error {
	WorkerError(const std::string& what)
		: runtime_error(what)
	{}
	WorkerError(const std::runtime_error& other)
		: runtime_error(other)
	{}
};

struct WorkerDone : WorkerError {
	WorkerDone()
		: WorkerError("Done")
	{}
};

template<typename Product>
struct Worker : Promiser<Product> {
	using Future = Promiser<Product>::Future;

private:
	std::jthread loop;

protected:
	virtual auto setup() -> void = 0;
	/** Must throw a derivative of WorkerError to communicate error through set_exception */
	virtual auto work() -> Product = 0;

public:
	Worker() = default;

	~Worker() {
		assert(loop.get_stop_token().stop_requested());
	}

	auto start() {
		loop = std::jthread([this](auto stoken) {
			setup();
			while (not stoken.stop_requested()) {
				try {
					this->set_value(work());
				}
				catch(const WorkerError& e) {	// TODO: how to propagate the type of the exception thrown in work()?
					this->set_exception(e);
				}
				this->reset();
			}
			this->set_exception(WorkerDone());
		});
	}

	auto stop() {
		loop.request_stop();
	}

	auto future_work() -> Future {
		return this->get_future();
	}
};

