#pragma once

#include "types.hpp"
#include <map>
#include <unordered_map>
#include <array>

class OrderBook 
{
public:
    struct BookOrder 
    {
        OrderID order_id;
        Price price;
        Quantity quantity;
        Side side;
        uint64_t timestamp;
        
        BookOrder* next{nullptr};
        BookOrder* prev{nullptr};
    };

    struct LimitLevel 
    {
        Price price;
        Quantity total_volume{0};
        uint32_t order_count{0};
        BookOrder* head{nullptr};
        BookOrder* tail{nullptr};
    };

    // Statically allocated array for generating trades out of the critical path
    static constexpr size_t MAX_TRADES_PER_ORDER = 128;
    using TradeArray = std::array<Trade, MAX_TRADES_PER_ORDER>;

public:
    OrderBook();
    ~OrderBook();

    OrderBook(const OrderBook&) = delete;
    OrderBook& operator=(const OrderBook&) = delete;

    // Execution profile: accepts pre-allocated array storage to guarantee 0 heap allocations
    size_t limit_order(OrderID id, Side side, Price price, Quantity quantity, TradeArray& trade_buffer);
    void cancel_order(OrderID id);

    Price get_best_bid() const;
    Price get_best_ask() const;

private:
    void append_to_level(LimitLevel& level, BookOrder* order);
    void remove_from_level(BookOrder* order);

    // Memory Pool Mechanics
    static constexpr size_t POOL_SIZE = 102400; // Capacity for 102,400 concurrent resting orders
    std::array<BookOrder, POOL_SIZE> order_pool_;
    std::array<size_t, POOL_SIZE> free_nodes_stack_;
    size_t pool_index_{POOL_SIZE};

    BookOrder* allocate_node();
    void deallocate_node(BookOrder* node);

    std::map<Price, LimitLevel, std::greater<Price>> bid_levels_;
    std::map<Price, LimitLevel, std::less<Price>> ask_levels_;
    std::unordered_map<OrderID, BookOrder*> order_registry_;
};