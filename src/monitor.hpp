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
	mutable Object obj;
	mutable std::mutex m;

public:
	template<typename... Args>
	Monitor(Args&&... args)
		: obj(std::forward<Args>(args)...)
	{}

public:
	auto invoke(std::invocable<That> auto fn) const -> auto {
		LOCK_GUARD(m);
		return fn(obj);
	}

	// For when we know our Object has its own safety
	auto unsafe_invoke(std::invocable<That> auto fn) const -> auto {
		return fn(obj);
	}
};

#undef LOCK_GUARD

