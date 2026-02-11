#pragma once

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

namespace bench {

inline void busy_work(size_t iters) {
    volatile std::size_t sink = 0;
    for (size_t i = 0; i < iters; ++i) {
        sink += i;
    }
}

inline double percentile(std::vector<double>& values, double pct) {
    if (values.empty()) return 0.0;
    std::sort(values.begin(), values.end());
    size_t idx = static_cast<size_t>(pct * (values.size() - 1));
    return values[idx];
}

inline void report(const std::string& label, std::vector<double> lat_us) {
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

inline std::vector<double> compute_latencies(
    const std::vector<std::chrono::high_resolution_clock::time_point>& starts,
    const std::vector<std::chrono::high_resolution_clock::time_point>& ends) {
    std::vector<double> lat_us;
    lat_us.reserve(starts.size());
    for (size_t i = 0; i < starts.size(); ++i) {
        std::chrono::duration<double, std::micro> dur = ends[i] - starts[i];
        lat_us.push_back(dur.count());
    }
    return lat_us;
}

}  // namespace bench
