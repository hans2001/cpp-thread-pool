#include <atomic>
#include <cassert>
#include <future>
#include <vector>

#include "tp/thread_pool.hpp"

/// Stress test for repeated enqueue/drain cycles.
int main() {
    tp::ThreadPool pool(4);

    constexpr int rounds = 3;
    constexpr int tasks_per_round = 5000;
    std::atomic<int> counter{0};

    for (int r = 0; r < rounds; ++r) {
        std::vector<std::future<void>> futures;
        futures.reserve(tasks_per_round);
        for (int i = 0; i < tasks_per_round; ++i) {
            auto fut_opt = pool.submit([&counter]() {
                counter.fetch_add(1, std::memory_order_relaxed);
            });
            assert(fut_opt.has_value());
            futures.push_back(std::move(*fut_opt));
        }

        for (auto& fut : futures) {
            fut.get();
        }
    }

    pool.shutdown();
    assert(counter.load(std::memory_order_relaxed) == rounds * tasks_per_round);
    return 0;
}
