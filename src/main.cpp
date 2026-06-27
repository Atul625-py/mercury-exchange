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
    uint64_t enqueue_time;      // Track exactly when the gateway pushed it
    bool is_poison_pill{false};
};

int main()
{
    std::cout << "========================================\n";
    std::cout << "  Mercury Latency Telemetry Benchmark   \n";
    std::cout << "========================================\n";

    LockFreeQueue<QueueEvent, 4096> engine_queue;
    OrderBook book;

    std::atomic<bool> engine_running{true};
    
    // Dynamic vector to collect our latency samples (in nanoseconds)
    std::vector<uint64_t> latency_samples;
    latency_samples.reserve(100000); 

    // ---------------------------------------------------------
    // 1. ENGINE THREAD WITH LATENCY TRACKING
    // ---------------------------------------------------------
    std::thread engine_thread([&]()
    {
        QueueEvent event;
        while (engine_running)
        {
            if (engine_queue.dequeue(event))
            {
                if (event.is_poison_pill)
                {
                    break;
                }

                // Match order
                book.limit_order(event.order_id, event.side, event.price, event.quantity);
                
                // Capture current time instantly after processing
                uint64_t dequeue_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
                
                // Calculate total transit + execution time
                if (dequeue_time > event.enqueue_time)
                {
                    latency_samples.push_back(dequeue_time - event.enqueue_time);
                }
            }
        }
    });

    // ---------------------------------------------------------
    // 2. GATEWAY THREAD FLOODING WITH TIMESTAMPING
    // ---------------------------------------------------------
    const uint64_t total_orders = 50000;
    std::cout << "[Gateway] Streaming " << total_orders << " orders into the exchange...\n";

    // Feed Sell side
    for (uint64_t i = 1; i <= total_orders; ++i)
    {
        QueueEvent sell_order{
            .order_id = i,
            .side = Side::SELL,
            .price = 10000,
            .quantity = 5,
            .enqueue_time = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count())
        };
        while (!engine_queue.enqueue(sell_order)) {}
    }

    // Feed crossing Buy side
    for (uint64_t i = total_orders + 1; i <= total_orders * 2; ++i)
    {
        QueueEvent buy_order{
            .order_id = i,
            .side = Side::BUY,
            .price = 10000,
            .quantity = 5,
            .enqueue_time = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count())
        };
        while (!engine_queue.enqueue(buy_order)) {}
    }

    // Shut down engine thread cleanly
    QueueEvent poison_pill{.is_poison_pill = true};
    while (!engine_queue.enqueue(poison_pill)) {}

    if (engine_thread.joinable())
    {
        engine_thread.join();
    }

    std::cout << "[Gateway] Benchmark complete. Analyzing performance profile...\n\n";

    // ---------------------------------------------------------
    // 3. COMPUTE METRICS (P50, P90, P99)
    // ---------------------------------------------------------
    if (!latency_samples.empty())
    {
        std::sort(latency_samples.begin(), latency_samples.end());

        size_t size = latency_samples.size();
        double p50 = latency_samples[static_cast<size_t>(size * 0.50)];
        double p90 = latency_samples[static_cast<size_t>(size * 0.90)];
        double p99 = latency_samples[static_cast<size_t>(size * 0.99)];

        std::cout << "--- LATENCY PROFILE ---\n";
        std::cout << "Total Orders Processed : " << size << "\n";
        std::cout << "P50 (Median Latency)   : " << (p50 / 1000.0) << " microseconds\n";
        std::cout << "P90 Latency            : " << (p90 / 1000.0) << " microseconds\n";
        std::cout << "P99 (Tail Latency)     : " << (p99 / 1000.0) << " microseconds\n";
        std::cout << "-----------------------\n";
    }
    else
    {
        std::cout << "Error: No latency samples collected.\n";
    }

    return 0;
}