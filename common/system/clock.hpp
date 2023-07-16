/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <chrono>

#include "time.hpp"

struct Clock {

    Clock(const Time moment = Now()) noexcept
        : _start_time { moment }
    {}

    ~Clock() noexcept = default;
    Clock(const Clock&) noexcept = default;
    Clock(Clock&&) noexcept = default;
    Clock& operator=(const Clock&) noexcept = default;
    Clock& operator=(Clock&&) noexcept = default;

public:

    static Time Now() noexcept
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>
              (std::chrono::system_clock::now().time_since_epoch()).count();
    }

public:

    Time StartTime() const noexcept
    {
        return _start_time;
    }

public:

    Time Elapsed(const Time moment = Now()) const noexcept
    {
        return moment - _start_time;
    }

    Time Restart(const Time moment = Now()) noexcept
    {
        const Time elapsed = Elapsed(moment);
        _start_time = moment;
        return elapsed;
    }

public:

    bool Expired(const Time timeout, const Time moment = Now()) const noexcept
    {
        return Elapsed(moment) >= timeout;
    }

    bool ExpiredAndRestart(const Time timeout, const Time moment = Now()) noexcept
    {
        const bool expired = Expired(timeout, moment);
        if (expired) _start_time = moment;
        return expired;
    }

private:

    Time _start_time;

};
