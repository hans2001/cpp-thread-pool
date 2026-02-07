#include <chrono>
#include <cstddef>
#include <future>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "tp/thread_pool.hpp"

namespace {

void busy_work(size_t iters) {
    volatile std::size_t sink = 0;
    for (size_t i = 0; i < iters; ++i) {
        sink += i;
    }
}

struct BenchConfig {
    size_t tasks = 20000;
    size_t work_iters = 5000;
    size_t workers = std::thread::hardware_concurrency();
};

BenchConfig parse_args(int argc, char** argv) {
    BenchConfig cfg;
    if (argc > 1) cfg.tasks = static_cast<size_t>(std::stoull(argv[1]));
    if (argc > 2) cfg.work_iters = static_cast<size_t>(std::stoull(argv[2]));
    if (argc > 3) cfg.workers = static_cast<size_t>(std::stoull(argv[3]));
    if (cfg.workers == 0) cfg.workers = 1;
    return cfg;
}

double run_pool(const BenchConfig& cfg) {
    tp::ThreadPool pool(cfg.workers);
    std::vector<std::future<void>> futures;
    futures.reserve(cfg.tasks);

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < cfg.tasks; ++i) {
        auto fut_opt = pool.submit([iters = cfg.work_iters]() { busy_work(iters); });
        if (fut_opt.has_value()) {
            futures.push_back(std::move(*fut_opt));
        }
    }
    for (auto& fut : futures) {
        fut.get();
    }
    auto end = std::chrono::high_resolution_clock::now();
    pool.shutdown();

    std::chrono::duration<double> seconds = end - start;
    return seconds.count();
}

double run_async(const BenchConfig& cfg) {
    std::vector<std::future<void>> futures;
    futures.reserve(cfg.tasks);

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < cfg.tasks; ++i) {
        futures.push_back(std::async(std::launch::async, [iters = cfg.work_iters]() {
            busy_work(iters);
        }));
    }
    for (auto& fut : futures) {
        fut.get();
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> seconds = end - start;
    return seconds.count();
}

}  // namespace

int main(int argc, char** argv) {
    auto cfg = parse_args(argc, argv);

    std::cout << "Throughput benchmark\n";
    std::cout << "tasks=" << cfg.tasks << " work_iters=" << cfg.work_iters
              << " workers=" << cfg.workers << "\n";

    auto pool_sec = run_pool(cfg);
    auto async_sec = run_async(cfg);

    auto pool_tps = static_cast<double>(cfg.tasks) / pool_sec;
    auto async_tps = static_cast<double>(cfg.tasks) / async_sec;

    std::cout << "thread_pool_sec=" << pool_sec
              << " tasks_per_sec=" << pool_tps << "\n";
    std::cout << "std_async_sec=" << async_sec
              << " tasks_per_sec=" << async_tps << "\n";

    return 0;
}
