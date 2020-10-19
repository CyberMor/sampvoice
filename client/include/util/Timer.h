#pragma once

#include <atomic>
#include <cstdint>

class Timer {

    Timer() = delete;
    ~Timer() = delete;
    Timer(const Timer&) = delete;
    Timer(Timer&&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer& operator=(Timer&&) = delete;

public:

    using time_t = std::int64_t;

public:

    static void Tick() noexcept;
    static time_t Get() noexcept;

private:

    static std::atomic<time_t> timestamp;

};
