#pragma once

#include "../exchange/gateway/gateway.hpp"
#include "../infrastructure/threading/engine_thread.hpp"

namespace mercury::app
{

//------------------------------------------------------------------------------
// Owns the lifetime of the Mercury Exchange application.
//
// The application initializes the graphical interface, starts the engine
// thread and coordinates shutdown.
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

    mercury::threading::EngineThread engine_thread_;
};

} // namespace mercury::app