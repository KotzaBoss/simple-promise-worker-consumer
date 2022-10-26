#include <iostream>
#include <vector>
#include <ranges>
#include <algorithm>
namespace rs = std::ranges;

#include "counter.hpp"

auto is_sequential(const std::vector<size_t>& work) -> bool {
	return rs::all_of(work,
		[start = work.front()](const auto& x) mutable { return x == start++; }
	);
}

auto main() -> int {
	auto counter = Counter();

	auto consumer = std::jthread([&counter]() {
		auto consumed_work = std::vector<size_t>();

		while (true) {
			try {
				const auto work = counter.future_work().get();
				std::cerr << work << '\n';
				consumed_work.push_back(work);
				continue;
			}
			catch (const std::future_error& e) {
				std::cerr << e.what() << '\n';
				assert(e.code() == std::future_errc::future_already_retrieved);
				std::cerr << "Consumer is too fast for the WorkSource: " << e.what() << '\n';
				continue;
			}
			catch (const WorkerDone& e) {
				std::cerr << e.what() << "\n\n"
					<< "Values consumed:\n" ;
				rs::for_each(consumed_work, [](const auto& x) { std::cerr << x << ' '; });
				std::cerr << '\n'
					<< "Consumed values are" << (is_sequential(consumed_work) ? " " : " NOT ") << "sequential\n";
				return;
			}
			catch (const WorkerError& e) {	// TODO: Should be CounterError somehow
				std::cerr << e.what() << '\n';
				continue;
			}
		}
	});

	counter.start();

	std::this_thread::sleep_for(3s);

	counter.stop();
}
