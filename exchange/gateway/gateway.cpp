#include "gateway.hpp"

namespace mercury::exchange
{

Gateway::Gateway(
    mercury::core::Quantity max_order_quantity,
    mercury::core::Price max_order_value)
    :
    matching_engine_{},
    risk_engine_{
        max_order_quantity,
        max_order_value
    }
{
}

OrderResult Gateway::submit(
    const mercury::core::Order& order)
{
    if (!risk_engine_.validate(order))
    {
        OrderResult result;

        result.accepted = false;

        result.status =
            mercury::core::OrderStatus::Rejected;

        result.reject_reason =
            mercury::core::RejectReason::OrderTooLarge;

        return result;
    }

    return matching_engine_.submit(order);
}

bool Gateway::cancel_order(
    mercury::core::OrderId order_id)
{
    return matching_engine_.cancel_order(
        order_id);
}

MarketSnapshot Gateway::snapshot() const
{
    return matching_engine_.snapshot();
}

}