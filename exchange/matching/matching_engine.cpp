#include "matching_engine.hpp"

namespace mercury::exchange
{

OrderResult MatchingEngine::submit(
    const mercury::core::Order& order)
{
    return order_book_.execute(
        order,
        next_trade_id_);
}

bool MatchingEngine::cancel_order(
    mercury::core::OrderId order_id)
{
    return order_book_.cancel_order(
        order_id);
}

MarketSnapshot MatchingEngine::snapshot() const
{
    return order_book_.snapshot();
}

}