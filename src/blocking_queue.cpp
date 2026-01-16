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
    
        // 核心 API
        void push(T value);
        std::optional<T> pop();       // 阻塞
        std::optional<T> try_pop();   // 不阻塞
        void close();                 // 关闭队列
    
        // 调试/测试用
        bool empty() const;
        size_t size() const;
    private: 
} 