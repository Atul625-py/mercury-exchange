#pragma once

#include "../exchange/events/market_snapshot.hpp"

namespace mercury::market
{

class MarketPublisher
{
public:

    MarketPublisher() = default;

public:

    void publish(
        const mercury::exchange::MarketSnapshot& snapshot);

    [[nodiscard]]
    mercury::exchange::MarketSnapshot latest_snapshot() const;

private:

    mercury::exchange::MarketSnapshot snapshot_;
};

}