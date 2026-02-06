#include <iostream>

#include "tp/thread_pool.hpp"

int main() {
    tp::ThreadPool pool(2);

    auto fut_opt = pool.submit([] { return 21 * 2; });
    if (!fut_opt.has_value()) {
        std::cerr << "Submit failed\n";
        return 1;
    }

    std::cout << "Result: " << fut_opt->get() << "\n";
    auto stats = pool.stats();
    std::cout << "submitted=" << stats.submitted
              << " completed=" << stats.completed
              << " rejected=" << stats.rejected
              << " queued=" << stats.queued
              << " workers=" << stats.workers << "\n";
    pool.shutdown();
    return 0;
}
