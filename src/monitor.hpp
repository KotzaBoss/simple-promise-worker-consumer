#pragma once

#include <functional>
#include <mutex>
#include <concepts>

// Inspired by Herb Sutter: https://stackoverflow.com/questions/60522330/how-does-herb-sutters-monitor-class-work
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
		const auto _ = std::lock_guard{m};
		return std::invoke(fn, obj);
	}

	// For when we know our Object has its own safety
	auto unsafe_invoke(std::invocable<That> auto fn) const -> auto {
		return std::invoke(fn, obj);
	}
};

