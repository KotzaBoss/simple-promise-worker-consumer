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
};

struct WorkerDone : WorkerError {
	WorkerDone()
		: WorkerError("Done")
	{}
};

/**	Unfortunately this-> is required to access the promiser's methods.
 *	It has to do with how C++ looks up "unqualified" names but i lost the sauce. */
template<typename Product>
struct Worker : Promiser<Product> {
	using Future = Promiser<Product>::Future;

private:
	std::jthread loop;

protected:
	/** Must throw an exception to communicate error through set_exception */
	virtual auto work() -> Product = 0;

public:
	~Worker() {
		assert(loop.get_stop_token().stop_requested());
	}

	/** Assumes the subclass has performed its setup in the constructor */
	auto start() {
		loop = std::jthread([this](auto stoken) {
			while (not stoken.stop_requested()) {
				try {
					this->set_value(work());
				}
				catch (...) {
					this->set_exception(std::current_exception());
				}
				this->reset();
			}
			this->set_exception(std::make_exception_ptr(WorkerDone()));
		});
	}

	auto stop() {
		loop.request_stop();
	}

	auto future_work() -> Future {
		return this->get_future();
	}
};

