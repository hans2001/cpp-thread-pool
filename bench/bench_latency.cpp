#include <algorithm>
#include <chrono>
#include <cstddef>
#include <future>
#include <iostream>
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

double percentile(std::vector<double>& values, double pct) {
    if (values.empty()) return 0.0;
    std::sort(values.begin(), values.end());
    size_t idx = static_cast<size_t>(pct * (values.size() - 1));
    return values[idx];
}

void report(const std::string& label, std::vector<double> lat_us) {
    double sum = 0.0;
    for (double v : lat_us) sum += v;
    double avg = lat_us.empty() ? 0.0 : (sum / lat_us.size());
    double p50 = percentile(lat_us, 0.50);
    double p95 = percentile(lat_us, 0.95);
    double p99 = percentile(lat_us, 0.99);

    std::cout << label << " avg_us=" << avg
              << " p50_us=" << p50
              << " p95_us=" << p95
              << " p99_us=" << p99 << "\n";
}

std::vector<double> run_pool(const BenchConfig& cfg) {
    tp::ThreadPool pool(cfg.workers);
    std::vector<std::future<void>> futures;
    std::vector<std::chrono::high_resolution_clock::time_point> starts;
    futures.reserve(cfg.tasks);
    starts.reserve(cfg.tasks);

    for (size_t i = 0; i < cfg.tasks; ++i) {
        starts.push_back(std::chrono::high_resolution_clock::now());
        auto fut_opt = pool.submit([iters = cfg.work_iters]() { busy_work(iters); });
        if (fut_opt.has_value()) {
            futures.push_back(std::move(*fut_opt));
        }
    }

    std::vector<double> lat_us;
    lat_us.reserve(futures.size());
    for (size_t i = 0; i < futures.size(); ++i) {
        futures[i].get();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::micro> dur = end - starts[i];
        lat_us.push_back(dur.count());
    }

    pool.shutdown();
    return lat_us;
}

std::vector<double> run_async(const BenchConfig& cfg) {
    std::vector<std::future<void>> futures;
    std::vector<std::chrono::high_resolution_clock::time_point> starts;
    futures.reserve(cfg.tasks);
    starts.reserve(cfg.tasks);

    for (size_t i = 0; i < cfg.tasks; ++i) {
        starts.push_back(std::chrono::high_resolution_clock::now());
        futures.push_back(std::async(std::launch::async, [iters = cfg.work_iters]() {
            busy_work(iters);
        }));
    }

    std::vector<double> lat_us;
    lat_us.reserve(futures.size());
    for (size_t i = 0; i < futures.size(); ++i) {
        futures[i].get();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::micro> dur = end - starts[i];
        lat_us.push_back(dur.count());
    }

    return lat_us;
}

}  // namespace

int main(int argc, char** argv) {
    auto cfg = parse_args(argc, argv);

    std::cout << "Latency benchmark\n";
    std::cout << "tasks=" << cfg.tasks << " work_iters=" << cfg.work_iters
              << " workers=" << cfg.workers << "\n";

    auto pool_lat = run_pool(cfg);
    auto async_lat = run_async(cfg);

    report("thread_pool", std::move(pool_lat));
    report("std_async", std::move(async_lat));

    return 0;
}
