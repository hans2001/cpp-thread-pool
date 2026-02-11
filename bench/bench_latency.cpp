#include <cassert>
#include <chrono>
#include <cstddef>
#include <future>
#include <iostream>
#include <string>
#include <vector>

#include "bench_utils.hpp"
#include "tp/thread_pool.hpp"

namespace {

struct BenchConfig {
    size_t tasks = 10000;
    size_t work_iters = 2000;
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

std::vector<double> run_pool(const BenchConfig& cfg) {
    tp::ThreadPool pool(cfg.workers);
    std::vector<std::future<void>> futures;
    std::vector<std::chrono::high_resolution_clock::time_point> starts;
    std::vector<std::chrono::high_resolution_clock::time_point> ends(cfg.tasks);
    futures.reserve(cfg.tasks);
    starts.reserve(cfg.tasks);

    for (size_t i = 0; i < cfg.tasks; ++i) {
        starts.push_back(std::chrono::high_resolution_clock::now());
        auto fut_opt = pool.submit([iters = cfg.work_iters, &ends, i]() {
            bench::busy_work(iters);
            ends[i] = std::chrono::high_resolution_clock::now();
        });
        assert(fut_opt.has_value());
        futures.push_back(std::move(*fut_opt));
    }

    for (auto& fut : futures) {
        fut.get();
    }
    pool.shutdown();
    return bench::compute_latencies(starts, ends);
}

std::vector<double> run_async(const BenchConfig& cfg) {
    std::vector<std::future<void>> futures;
    std::vector<std::chrono::high_resolution_clock::time_point> starts;
    std::vector<std::chrono::high_resolution_clock::time_point> ends(cfg.tasks);
    futures.reserve(cfg.tasks);
    starts.reserve(cfg.tasks);

    for (size_t i = 0; i < cfg.tasks; ++i) {
        starts.push_back(std::chrono::high_resolution_clock::now());
        futures.push_back(std::async(std::launch::async, [iters = cfg.work_iters, &ends, i]() {
            bench::busy_work(iters);
            ends[i] = std::chrono::high_resolution_clock::now();
        }));
    }

    for (auto& fut : futures) {
        fut.get();
    }
    return bench::compute_latencies(starts, ends);
}

}  // namespace

int main(int argc, char** argv) {
    auto cfg = parse_args(argc, argv);

    std::cout << "Latency benchmark\n";
    std::cout << "tasks=" << cfg.tasks << " work_iters=" << cfg.work_iters
              << " workers=" << cfg.workers << "\n";

    auto pool_lat = run_pool(cfg);
    auto async_lat = run_async(cfg);

    bench::report("thread_pool", std::move(pool_lat));
    bench::report("std_async", std::move(async_lat));

    return 0;
}
