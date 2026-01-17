#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>

template <typename T>
class BlockingQueue {
public:
    BlockingQueue() = default;
    ~BlockingQueue() = default;

    BlockingQueue(const BlockingQueue&) = delete;
    BlockingQueue& operator=(const BlockingQueue&) = delete;

    void push(T value);
    std::optional<T> pop();     
    std::optional<T> try_pop();  
    void close();

    bool empty() const;
    size_t size() const;

private:
    mutable std:;mutex mtx_;
    std::condition_variable cv_;
    std::deque<T> queue_; 
    bool closed_ = false;
} 