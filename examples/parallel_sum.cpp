#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

#include "tp/thread_pool.hpp"

int main() {
    const size_t size = 1'000'000;
    const size_t workers = 4;
    const size_t chunks = workers * 2;
    const size_t chunk_size = (size + chunks - 1) / chunks;

    std::vector<int> data(size);
    std::iota(data.begin(), data.end(), 1);

    tp::ThreadPool pool(workers);
    std::vector<std::future<long long>> futures;
    futures.reserve(chunks);

    for (size_t c = 0; c < chunks; ++c) {
        size_t start = c * chunk_size;
        size_t end = std::min(start + chunk_size, size);
        if (start >= end) break;

        auto fut_opt = pool.submit([start, end, &data]() {
            long long local = 0;
            for (size_t i = start; i < end; ++i) {
                local += data[i];
            }
            return local;
        });

        if (!fut_opt.has_value()) {
            std::cerr << "submit failed for chunk " << c << "\n";
            return 1;
        }
        futures.push_back(std::move(*fut_opt));
    }

    long long total = 0;
    for (auto& fut : futures) {
        total += fut.get();
    }

    pool.shutdown();

    std::cout << "parallel sum = " << total << "\n";
    return 0;
}
