#pragma once

#include <functional>
#include <mutex>
#include <concepts>

#define LOCK_GUARD(m) const auto _ = std::lock_guard{m}

template<typename T>
struct Monitor {
	using Object = T;
	using This = T*;
	using That = T&;

protected:
	Object obj;
	mutable std::mutex m;

public:
	template<typename... Args>
	Monitor(Args&&... args)
		: obj(std::forward<Args>(args)...)
	{}

// Making the api protected, forces making subclasses to expose access the underlying object.
// This should be used if the design choice is to have more "OOP" style architecture.
// Making it public allows for composibility allowing for a more "Composition" based architecture.
// If the latter is chosen, the functions can change to operators `=` and `()` respectively.
//
// IMO, keeping it private should be the default use because it allows for more self documenting classes.
// You dont have to look around to understand what a "Promiser" is, just the inherited Monitor class.
protected:

	auto invoke(std::invocable<That> auto fn) -> auto {
		LOCK_GUARD(m);
		return fn(obj);
	}

	auto invoke_unsafe(std::invocable<That> auto fn) -> auto {
		return fn(obj);
	}

	auto assign(Object&& o) -> void {
		LOCK_GUARD(m);
		obj = std::move(o);
	}

};

#undef LOCK_GUARD

