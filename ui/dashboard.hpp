#pragma once

#include "../market/market_publisher.hpp"

namespace mercury::ui
{

class Dashboard
{
public:

    Dashboard();

    ~Dashboard();

public:

    void initialize();

    void render(
        const mercury::market::MarketPublisher& publisher,
        std::uint64_t orders_processed,
        std::uint64_t trades_executed);

    [[nodiscard]]
    bool should_close() const;

    void shutdown();

private:

};

}