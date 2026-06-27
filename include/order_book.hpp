#pragma once

#include "types.hpp"
#include <map>
#include <unordered_map>
#include <vector>

class OrderBook {
public:
    // A single node inside our price-level FIFO queue
    struct BookOrder {
        OrderID order_id;
        Price price;
        Quantity quantity;
        Side side;
        uint64_t timestamp;
        
        // Doubly linked list pointers for O(1) removals on cancellations
        BookOrder* next{nullptr};
        BookOrder* prev{nullptr};
    };

    // Represents a single price tier (e.g., all buy orders at $100.50)
    struct LimitLevel {
        Price price;
        Quantity total_volume{0};
        uint32_t order_count{0};
        BookOrder* head{nullptr};
        BookOrder* tail{nullptr};
    };

public:
    OrderBook() = default;
    ~OrderBook();

    // Disable copying to protect our raw pointer allocations and prevent performance dips
    OrderBook(const OrderBook&) = delete;
    OrderBook& operator=(const OrderBook&) = delete;

    // Core Matching Engine API
    std::vector<Trade> limit_order(OrderID id, Side side, Price price, Quantity quantity);
    void cancel_order(OrderID id);

    // Getters for our upcoming UI panel
    Price get_best_bid() const;
    Price get_best_ask() const;

private:
    // Bids are sorted highest-to-lowest (std::greater), Asks lowest-to-highest (std::less)
    std::map<Price, LimitLevel, std::greater<Price>> bid_levels_;
    std::map<Price, LimitLevel, std::less<Price>> ask_levels_;

    // Fast lookup map tracking where an order lives for O(1) cancellations
    std::unordered_map<OrderID, BookOrder*> order_registry_;

    // Private helper methods to manipulate our internal queues
    void append_to_level(LimitLevel& level, BookOrder* order);
    void remove_from_level(BookOrder* order);
};