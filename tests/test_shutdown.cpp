#include <atomic>
#include <cassert>
#include <chrono>
#include <thread>
#include <vector>

#include "tp/thread_pool.hpp"

/// Validates shutdown drains queued tasks and joins workers.
int main() {
    tp::ThreadPool pool(4);

    std::atomic<int> counter{0};
    std::vector<std::future<void>> futures;
    futures.reserve(8);

    for (int i = 0; i < 8; ++i) {
        auto fut_opt = pool.submit([&counter]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            counter.fetch_add(1, std::memory_order_relaxed);
        });
        assert(fut_opt.has_value());
        futures.push_back(std::move(*fut_opt));
    }

    pool.shutdown();

    for (auto& fut : futures) {
        fut.get();
    }

    assert(counter.load(std::memory_order_relaxed) == 8);
    return 0;
}
