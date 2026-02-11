#include <cassert>
#include <chrono>
#include <thread>

#include "tp/thread_pool.hpp"

/// Validates futures returned from submit and rejection after shutdown.
int main() {
    tp::ThreadPool pool(2);

    auto fut_opt = pool.submit([](int a, int b) { return a + b; }, 3, 4);
    assert(fut_opt.has_value());
    auto value = fut_opt->get();
    assert(value == 7);

    pool.shutdown();
    auto after_shutdown = pool.submit([] { return 42; });
    assert(!after_shutdown.has_value());

    return 0;
}
