#pragma once

#include <functional>
#include <mutex>
#include <concepts>

#define LOCK_GUARD(m) const auto _ = std::lock_guard{m}

template<typename T>
struct Monitor {
	using Object = T;
	using That = T&;

protected:
	Object obj;
	std::mutex m;

public:
	template<typename... Args>
	Monitor(Args&&... args)
		: obj(std::forward<Args>(args)...)
	{}

public:
	auto operator() (std::invocable<That> auto fn) -> auto {
		LOCK_GUARD(m);
		return fn(obj);
	}

	// For when we know our Object has its own safety
	auto unsafe_invoke(std::invocable<That> auto fn) -> auto {
		return fn(obj);
	}

	auto operator= (Object&& o) -> Monitor& {
		LOCK_GUARD(m);
		obj = std::move(o);
		return *this;
	}
};

#undef LOCK_GUARD

