#pragma once

#include "../matching/matching_engine.hpp"
#include "../risk/risk_engine.hpp"

namespace mercury::exchange
{

//------------------------------------------------------------------------------
// Entry point into the exchange.
//
// Every incoming order passes through the Gateway before reaching the matching
// engine. The Gateway performs lightweight pre-trade risk validation and
// forwards accepted orders for execution.
//------------------------------------------------------------------------------
class Gateway
{
public:

    Gateway(
        mercury::core::Quantity max_order_quantity,
        mercury::core::Price max_order_value);

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

    MatchingEngine matching_engine_;

    RiskEngine risk_engine_;
};

} // namespace mercury::exchange