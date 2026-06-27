#pragma once

#include "../exchange/gateway/gateway.hpp"
#include "../infrastructure/logging/trade_logger.hpp"
#include "../ui/dashboard.hpp"

namespace mercury::app
{

class Application
{
public:

    Application();

    ~Application();

public:

    void run();

private:

    void initialize();

    void shutdown();

    void launch_threads();

private:

    exchange::Gateway gateway_;

    infrastructure::TradeLogger trade_logger_;

    ui::Dashboard dashboard_;
};

}