#pragma once

#include <atomic>
#include <thread>

#include "../../exchange/gateway/gateway.hpp"

namespace mercury::threading
{

//------------------------------------------------------------------------------
// Owns the exchange processing thread.
//
// Orders will eventually be submitted from the UI thread while the matching
// engine executes on this dedicated thread.
//------------------------------------------------------------------------------
class EngineThread
{
public:

    EngineThread(
        mercury::exchange::Gateway& gateway);

    EngineThread(const EngineThread&) = delete;

    EngineThread& operator=(const EngineThread&) = delete;

    ~EngineThread();

public:

    void start();

    void stop();

    [[nodiscard]]
    bool running() const noexcept;

private:

    void run();

private:

    mercury::exchange::Gateway& gateway_;

    std::thread thread_;

    std::atomic_bool running_{false};
};

} // namespace mercury::threading