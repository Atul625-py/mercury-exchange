#pragma once

#include "../events/market_snapshot.hpp"
#include "../events/order_result.hpp"
#include "../orderbook/order_book.hpp"

namespace mercury::exchange
{

//------------------------------------------------------------------------------
// Coordinates order execution.
//
// The matching engine owns the OrderBook and is responsible for assigning
// globally unique trade identifiers before forwarding orders for execution.
//------------------------------------------------------------------------------
class MatchingEngine
{
public:

    MatchingEngine() = default;

    MatchingEngine(const MatchingEngine&) = delete;

    MatchingEngine& operator=(const MatchingEngine&) = delete;

    ~MatchingEngine() = default;

public:

    [[nodiscard]]
    OrderResult submit(
        const mercury::core::Order& order);

    [[nodiscard]]
    bool cancel_order(
        mercury::core::OrderId order_id);

    [[nodiscard]]
    MarketSnapshot snapshot() const;

private:

    OrderBook order_book_;

    mercury::core::TradeId next_trade_id_{1};
};

}