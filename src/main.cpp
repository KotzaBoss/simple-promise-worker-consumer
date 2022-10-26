#include <iostream>
#include <vector>
#include <ranges>
#include <algorithm>
namespace rs = std::ranges;

#include "work_source.hpp"

auto is_sequential(const std::vector<size_t>& work) -> bool {
	return rs::all_of(work,
		[start = work.front()](const auto& x) mutable { return x == start++; }
	);
}

auto main() -> int {
	auto ws = WorkSource();

	auto consumer = std::jthread([&ws](auto stoken) {
		auto consumed_work = std::vector<size_t>();

		while (not stoken.stop_requested()) {
			try {
				const auto work = ws.future_work().get();
				std::cerr << work << '\n';
				consumed_work.push_back(work);
			}
			catch (const std::future_error& e) {
				assert(e.code() == std::future_errc::future_already_retrieved);
				std::cerr << "Consumer is too fast for the WorkSource: " << e.what() << '\n';
			}
			catch (const WorkDone& e) {
				std::cerr << e.what() << "\n\n"
					<< "Values consumed:\n" ;
				rs::for_each(consumed_work, [](const auto& x) { std::cerr << x << ' '; });
				std::cerr << '\n'
					<< "Consumed values are" << (is_sequential(consumed_work) ? " " : " NOT ") << "sequential\n";
				return;
			}
		}
	});

	std::this_thread::sleep_for(3s);

	consumer.request_stop();
	ws.stop();
	return 0;
}
