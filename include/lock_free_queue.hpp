#pragma once

#include <atomic>
#include <vector>
#include <optional>

template <typename T, size_t Capacity>
class LockFreeQueue
{
public:
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of 2 for fast bitmask indexing");

    LockFreeQueue() : head_(0), tail_(0)
    {
        ring_buffer_.resize(Capacity);
    }

    ~LockFreeQueue() = default;

    // Enqueue: The Producer (Gateway Thread) calls this to push an element
    bool enqueue(const T& item)
    {
        size_t current_tail = tail_.load(std::memory_order_relaxed);
        size_t current_head = head_.load(std::memory_order_acquire);

        // Check if the ring buffer is completely full
        if ((current_tail - current_head) == Capacity)
        {
            return false;
        }

        ring_buffer_[current_tail & (Capacity - 1)] = item;
        tail_.store(current_tail + 1, std::memory_order_release);
        return true;
    }

    // Dequeue: The Consumer (Matching Engine Thread) calls this to pull an element
    bool dequeue(T& item)
    {
        size_t current_head = head_.load(std::memory_order_relaxed);
        size_t current_tail = tail_.load(std::memory_order_acquire);

        // Check if the ring buffer is completely empty
        if (current_head == current_tail)
        {
            return false;
        }

        item = ring_buffer_[current_head & (Capacity - 1)];
        head_.store(current_head + 1, std::memory_order_release);
        return true;
    }

private:
    std::vector<T> ring_buffer_;
    
    // Align atomic indices to avoid false-sharing CPU cache conflicts
    alignas(64) std::atomic<size_t> head_;
    alignas(64) std::atomic<size_t> tail_;
};