#include <iostream>
#include <vector>
#include <span>
#include <ranges>
#include <algorithm>
namespace rs = std::ranges;

#include "counter.hpp"

auto is_sequential(const std::span<size_t> work) noexcept -> bool {
	return rs::all_of(work,
		[start = work.front()](const auto& x) mutable { return x == start++; }
	);
}

auto main() -> int {
	auto counter = MaybeBustedCounter();

	auto consumer = std::jthread([&counter]() noexcept {
		auto consumed_work = std::vector<size_t>();
		consumed_work.reserve(70);

		while (true) {
			try {
				const auto work = counter.future_value().get();
				std::cerr << work << '\n';
				consumed_work.push_back(work);
				continue;
			}
			catch (const std::future_error& e) {
				assert(e.code() == std::future_errc::future_already_retrieved);
				std::cerr << "Consumer is too fast for the WorkSource: " << e.what() << '\n';
				continue;
			}
			catch (const MaybeBustedCounterError& e) {
				std::cerr << e.what() << '\n';
				continue;
			}
			catch (const MaybeBustedCounterDone& e) {
				std::cerr << e.what() << "\n\n"
					<< "Values consumed:\n" ;
				rs::for_each(consumed_work, [](const auto& x) { std::cerr << x << ' '; });
				std::cerr << '\n'
					<< "Consumed values are" << (is_sequential(consumed_work) ? " " : " NOT ") << "sequential\n";
				return;
			}
		}
	});

	counter.start();

	std::this_thread::sleep_for(3s);

	counter.stop();
}
