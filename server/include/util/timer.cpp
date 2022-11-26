#include "timer.h"

void Timer::Tick() noexcept
{
    _timestamp.store(std::chrono::duration_cast<std::chrono::milliseconds>
                    (std::chrono::system_clock::now().time_since_epoch()).count(),
                     std::memory_order_relaxed);
}

Timer::time_t Timer::Get() noexcept
{
    return _timestamp.load(std::memory_order_relaxed);
}

std::atomic<Timer::time_t> Timer::_timestamp = 0;
