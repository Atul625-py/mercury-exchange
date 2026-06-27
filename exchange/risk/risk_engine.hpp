#pragma once

#include "../../core/types/order.hpp"
#include "../../core/types/numeric_types.hpp"

namespace mercury::exchange
{

//------------------------------------------------------------------------------
// Performs pre-trade validation.
//
// The RiskEngine is intentionally lightweight. It performs only deterministic,
// constant-time checks and contains no knowledge of matching logic.
//------------------------------------------------------------------------------
class RiskEngine
{
public:

    RiskEngine(
        mercury::core::Quantity max_order_quantity,
        mercury::core::Price max_order_value);

public:

    [[nodiscard]]
    bool validate(
        const mercury::core::Order& order) const noexcept;

private:

    mercury::core::Quantity max_order_quantity_;

    mercury::core::Price max_order_value_;
};

} // namespace mercury::exchange