#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include "order_book.hpp"
#include "lock_free_queue.hpp"

struct QueueEvent
{
    OrderID order_id;
    Side side;
    Price price;
    Quantity quantity;
    uint64_t enqueue_time;
    bool is_poison_pill{false};
};

int main()
{
    std::cout << "========================================\n";
    std::cout << "  Mercury Pool-Optimized Benchmark     \n";
    std::cout << "========================================\n";

    LockFreeQueue<QueueEvent, 4096> engine_queue;
    OrderBook book;
    std::atomic<bool> engine_running{true};
    
    const uint64_t total_orders = 50000;
    
    // Pre-allocate metric samples buffer upfront to keep the critical loop tight
    std::vector<uint64_t> latency_samples(total_orders * 2, 0);
    std::atomic<size_t> sample_count{0};

    std::thread engine_thread([&]()
    {
        QueueEvent event;
        OrderBook::TradeArray execution_trades; // Stack-allocated thread reuse
        
        while (engine_running)
        {
            if (engine_queue.dequeue(event))
            {
                if (event.is_poison_pill) break;

                // Call optimized zero-allocation matching loop
                book.limit_order(event.order_id, event.side, event.price, event.quantity, execution_trades);
                
                uint64_t dequeue_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
                if (dequeue_time > event.enqueue_time)
                {
                    size_t idx = sample_count.fetch_add(1, std::memory_order_relaxed);
                    if (idx < latency_samples.size())
                    {
                        latency_samples[idx] = dequeue_time - event.enqueue_time;
                    }
                }
            }
        }
    });

    std::cout << "[Gateway] Streaming " << total_orders << " orders into the exchange...\n";

    for (uint64_t i = 1; i <= total_orders; ++i)
    {
        QueueEvent sell_order{
            .order_id = i, .side = Side::SELL, .price = 10000, .quantity = 5,
            .enqueue_time = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count())
        };
        while (!engine_queue.enqueue(sell_order)) {}
    }

    for (uint64_t i = total_orders + 1; i <= total_orders * 2; ++i)
    {
        QueueEvent buy_order{
            .order_id = i, .side = Side::BUY, .price = 10000, .quantity = 5,
            .enqueue_time = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count())
        };
        while (!engine_queue.enqueue(buy_order)) {}
    }

    QueueEvent poison_pill{.is_poison_pill = true};
    while (!engine_queue.enqueue(poison_pill)) {}

    if (engine_thread.joinable()) engine_thread.join();

    std::cout << "[Gateway] Benchmark complete. Analyzing performance profile...\n\n";

    size_t valid_samples = sample_count.load();
    if (valid_samples > 0)
    {
        latency_samples.resize(valid_samples);
        std::sort(latency_samples.begin(), latency_samples.end());

        double p50 = latency_samples[static_cast<size_t>(valid_samples * 0.50)];
        double p90 = latency_samples[static_cast<size_t>(valid_samples * 0.90)];
        double p99 = latency_samples[static_cast<size_t>(valid_samples * 0.99)];

        std::cout << "--- OPTIMIZED LATENCY PROFILE ---\n";
        std::cout << "Total Orders Processed : " << valid_samples << "\n";
        std::cout << "P50 (Median Latency)   : " << (p50 / 1000.0) << " microseconds\n";
        std::cout << "P90 Latency            : " << (p90 / 1000.0) << " microseconds\n";
        std::cout << "P99 (Tail Latency)     : " << (p99 / 1000.0) << " microseconds\n";
        std::cout << "---------------------------------\n";
    }

    return 0;
}