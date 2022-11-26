#pragma once

#include <atomic>
#include <chrono>

struct Timer {

    Timer() = delete;
    ~Timer() = delete;
    Timer(const Timer&) = delete;
    Timer(Timer&&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer& operator=(Timer&&) = delete;

public:

    using time_t = std::chrono::milliseconds::rep;

public:

    static void Tick() noexcept;
    static time_t Get() noexcept;

private:

    static std::atomic<time_t> _timestamp;

};
