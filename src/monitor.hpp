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
// IMO being protected should be the default because the inheritance should be just one level deep.
// For example: Monitor <- Promiser and thats it.
protected:

	auto invoke(std::invocable<That> auto fn) -> auto {
		LOCK_GUARD(m);
		return fn(obj);
	}

	// For when we know our Object has its own safety
	auto invoke_unsafe(std::invocable<That> auto fn) -> auto {
		return fn(obj);
	}

	auto assign(Object&& o) -> void {
		LOCK_GUARD(m);
		obj = std::move(o);
	}

};

#undef LOCK_GUARD

