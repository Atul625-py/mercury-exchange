#include <iostream>
#include <thread>
#include <chrono>
#include "order_book.hpp"
#include "lock_free_queue.hpp"

// Define a safe payload package for our queue
struct QueueEvent
{
    OrderID order_id;
    Side side;
    Price price;
    Quantity quantity;
    bool is_poison_pill{false}; // Signal to cleanly shut down the background thread
};

int main()
{
    std::cout << "========================================\n";
    std::cout << "  Mercury Multithreaded Engine Run      \n";
    std::cout << "========================================\n";

    // Instantiating a 1024-capacity lock-free pipe
    LockFreeQueue<QueueEvent, 1024> engine_queue;
    OrderBook book;

    std::atomic<bool> engine_running{true};
    uint64_t total_trades_executed = 0;

    // ---------------------------------------------------------
    // 1. SPIN UP THE BACKGROUND MATCHING ENGINE THREAD
    // ---------------------------------------------------------
    std::thread engine_thread([&]()
    {
        QueueEvent event;
        while (engine_running)
        {
            // Spin-lock loop: constantly poll the queue for incoming data
            if (engine_queue.dequeue(event))
            {
                if (event.is_poison_pill)
                {
                    break;
                }

                auto trades = book.limit_order(event.order_id, event.side, event.price, event.quantity);
                total_trades_executed += trades.size();
            }
        }
    });

    // ---------------------------------------------------------
    // 2. MAIN THREAD ACTS AS THE ORDER GATEWAY
    // ---------------------------------------------------------
    std::cout << "[Gateway] Flooding engine with orders...\n";

    // Stack up 500 Ask orders at $100.00
    for (uint64_t i = 1; i <= 500; ++i)
    {
        QueueEvent sell_order{
            .order_id = i,
            .side = Side::SELL,
            .price = 10000,
            .quantity = 10
        };
        while (!engine_queue.enqueue(sell_order)) { /* Spin if queue is temporarily full */ }
    }

    // Stack up 500 crossing Buy orders at $100.00 to trigger immediate matches
    for (uint64_t i = 501; i <= 1000; ++i)
    {
        QueueEvent buy_order{
            .order_id = i,
            .side = Side::BUY,
            .price = 10000,
            .quantity = 10
        };
        while (!engine_queue.enqueue(buy_order)) { /* Spin if queue is temporarily full */ }
    }

    // Send a poison pill to cleanly stop our engine thread loop
    QueueEvent poison_pill{.is_poison_pill = true};
    while (!engine_queue.enqueue(poison_pill)) {}

    // Join the thread back to prevent application crashes
    if (engine_thread.joinable())
    {
        engine_thread.join();
    }

    std::cout << "[Gateway] Simulation Complete.\n";
    std::cout << "Total Trades Processed Asynchronously: " << total_trades_executed << "\n";

    return 0;
}