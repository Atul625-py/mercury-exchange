#pragma once

#include <functional>
#include <map>
#include <unordered_map>

#include "../../core/config/constants.hpp"
#include "../../core/types/order.hpp"
#include "../../core/types/trade.hpp"

#include "../../infrastructure/memory/object_pool.hpp"

#include "../events/market_snapshot.hpp"
#include "../events/order_result.hpp"

#include "book_order.hpp"
#include "limit_level.hpp"

namespace mercury::exchange
{

class OrderBook
{
public:

    using BidLevels = std::map<
        mercury::core::Price,
        LimitLevel,
        std::greater<>
    >;

    using AskLevels = std::map<
        mercury::core::Price,
        LimitLevel,
        std::less<>
    >;

public:

    OrderBook() = default;

    OrderBook(const OrderBook&) = delete;

    OrderBook& operator=(const OrderBook&) = delete;

    ~OrderBook() = default;

public:

    //--------------------------------------------------------------------------
    // Executes an incoming order using strict price-time priority.
    //
    // Any remaining quantity is inserted into the order book as resting
    // liquidity.
    //--------------------------------------------------------------------------
    [[nodiscard]]
    OrderResult execute(
        const mercury::core::Order& order,
        mercury::core::TradeId& next_trade_id);

    //--------------------------------------------------------------------------
    // Removes a resting order from the order book.
    //--------------------------------------------------------------------------
    [[nodiscard]]
    bool cancel_order(
        mercury::core::OrderId order_id);

    //--------------------------------------------------------------------------
    // Returns the current visible market depth.
    //--------------------------------------------------------------------------
    [[nodiscard]]
    MarketSnapshot snapshot() const;

    [[nodiscard]]
    mercury::core::Price top_bid() const noexcept;

    [[nodiscard]]
    mercury::core::Price top_ask() const noexcept;

private:

    //--------------------------------------------------------------------------
    // Inserts a resting order into its corresponding price level while
    // preserving FIFO ordering.
    //--------------------------------------------------------------------------
    void insert_order(
        BookOrder* order);

    //--------------------------------------------------------------------------
    // Removes an order from its price level and updates linked-list state.
    //--------------------------------------------------------------------------
    void detach_order(
        BookOrder* order);

private:

    mercury::core::ObjectPool<
        BookOrder,
        mercury::core::DefaultOrderPoolSize
    > order_pool_;

    BidLevels bids_;

    AskLevels asks_;

    std::unordered_map<
        mercury::core::OrderId,
        BookOrder*
    > order_registry_;
};

} // namespace mercury::exchange