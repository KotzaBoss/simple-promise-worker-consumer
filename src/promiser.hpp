#pragma once

#include <future>

#include "monitor.hpp"

/** The `set_...` methods are guaranteed to be threadsafe whereas the get_future is unspecified:
 * https://stackoverflow.com/questions/42534367/is-stdpromiset-thread-safe */
template<typename T>
struct Promiser {
	using Promise = std::promise<T>;
	using Future = std::future<T>;
	using PromisedType = T;

private:
	Monitor<Promise> monitor;

public:
	/** Should be wrapped with a function to rename the context of the future. Eg. future_frame, future_client, ... */
	auto get_future() -> Future {
		return monitor.invoke([](auto& p) { return p.get_future(); });
	}

	auto reset() -> void {
		monitor.invoke([](auto& p) { p = Promise{}; });
	}

	auto set_value(PromisedType&& v) -> void {
		monitor.unsafe_invoke([v = std::move(v)](auto& p) { p.set_value(v); });
	}

	/** Needed for sane "exception polymorphism" when subclass errors are being set */
	auto set_exception(std::exception_ptr e) {
		monitor.unsafe_invoke([e = std::move(e)](auto& p) { p.set_exception(e); });
	}
};

