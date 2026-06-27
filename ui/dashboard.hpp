#pragma once

#include "../exchange/gateway/gateway.hpp"

namespace mercury::ui
{

//------------------------------------------------------------------------------
// Renders the Mercury Exchange dashboard.
//
// The dashboard is responsible only for visualization and user interaction.
// All business logic remains inside the exchange core.
//------------------------------------------------------------------------------
class Dashboard
{
public:

    explicit Dashboard(
        mercury::exchange::Gateway& gateway);

    Dashboard(const Dashboard&) = delete;

    Dashboard& operator=(const Dashboard&) = delete;

    ~Dashboard() = default;

public:

    void render();

private:

    void render_order_entry();

    void render_order_book();

    void render_recent_trades();

    void render_statistics();

private:

    mercury::exchange::Gateway& gateway_;
};

} // namespace mercury::ui