#include "engine_thread.hpp"

#include <chrono>

namespace mercury::threading
{

EngineThread::EngineThread(
    mercury::exchange::Gateway& gateway)
    :
    gateway_{gateway}
{
}

EngineThread::~EngineThread()
{
    stop();
}

void EngineThread::start()
{
    if (running_)
    {
        return;
    }

    running_ = true;

    thread_ =
        std::thread(
            &EngineThread::run,
            this);
}

void EngineThread::stop()
{
    if (!running_)
    {
        return;
    }

    running_ = false;

    if (thread_.joinable())
    {
        thread_.join();
    }
}

bool EngineThread::running() const noexcept
{
    return running_;
}

void EngineThread::run()
{
    while (running_)
    {
        [[maybe_unused]]
        const auto snapshot =
            gateway_.snapshot();
        std::this_thread::sleep_for(
            std::chrono::milliseconds(1));
    }
}

} // namespace mercury::threading