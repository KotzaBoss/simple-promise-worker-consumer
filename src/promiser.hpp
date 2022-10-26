#pragma once

#include <future>

#include "monitor.hpp"

/** The `set_...` methods are guaranteed to be threadsafe whereas the get_future is unspecified:
 * https://stackoverflow.com/questions/42534367/is-stdpromiset-thread-safe */
template<typename T>
struct Promiser final : Monitor<std::promise<T>> {
	using Promise = std::promise<T>;
	using Future = std::future<T>;
	using PromisedType = T;

public:
	auto get_future() -> auto {
		return this->invoke([](auto& p) { return p.get_future(); });
	}

	auto reset() -> void {
		this->assign(Promise{});
	}

	auto set_value(PromisedType&& v) -> void {
		this->invoke_unsafe([v = std::move(v)](auto& p) { p.set_value(v); });
	}

	auto set_exception(auto&& e) -> void {
		this->invoke_unsafe([e = std::move(e)](auto& p) { p.set_exception(std::make_exception_ptr(e)); });
	}
};

