#include "risk_engine.hpp"

namespace mercury::exchange
{

RiskEngine::RiskEngine(
    mercury::core::Quantity max_order_quantity,
    mercury::core::Price max_order_value)
    :
    max_order_quantity_(max_order_quantity),
    max_order_value_(max_order_value)
{
}

bool RiskEngine::validate(
    const mercury::core::Order& order) const noexcept
{
    if (order.quantity == 0)
    {
        return false;
    }

    if (order.price == 0)
    {
        return false;
    }

    if (order.quantity > max_order_quantity_)
    {
        return false;
    }

    if ((order.price * order.quantity) > max_order_value_)
    {
        return false;
    }

    return true;
}

}