#pragma once

#include "../exchange/gateway/gateway.hpp"

namespace mercury::app
{

//------------------------------------------------------------------------------
// Coordinates the lifetime of the Mercury Exchange application.
//
// The Application owns the top-level components of the system and will later
// initialize the engine thread, dashboard, and networking layer. For now it
// serves as the entry point into the exchange core.
//------------------------------------------------------------------------------
class Application
{
public:

    Application();

    Application(const Application&) = delete;

    Application& operator=(const Application&) = delete;

    ~Application() = default;

public:

    void run();

private:

    mercury::exchange::Gateway gateway_;
};

} // namespace mercury::app