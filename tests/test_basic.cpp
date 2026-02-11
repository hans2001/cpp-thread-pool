#include <cassert>
#include <chrono>
#include <optional>
#include <thread>

#include "tp/blocking_queue.hpp"

/// Validates BlockingQueue push/pop/try_pop and close wake behavior.
int main() {
    tp::BlockingQueue<int> queue;

    queue.push(1);
    auto value = queue.pop();
    assert(value.has_value());
    assert(*value == 1);

    auto empty_value = queue.try_pop();
    assert(!empty_value.has_value());

    std::optional<int> closed_value;
    std::thread waiter([&queue, &closed_value]() { closed_value = queue.pop(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    queue.close();
    waiter.join();

    assert(!closed_value.has_value());
    return 0;
}
