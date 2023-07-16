/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>

#include <system/types.hpp>
#include <system/time.hpp>

#include "header.hpp"

struct Poll {

    struct Event : private pollfd {

        Event(const SOCKET handle = INVALID_SOCKET, const sword_t filter = 0) noexcept
            : pollfd { handle, filter, 0 }
        {}

        ~Event() noexcept = default;
        Event(const Event&) noexcept = default;
        Event(Event&&) noexcept = default;
        Event& operator=(const Event&) noexcept = default;
        Event& operator=(Event&&) noexcept = default;

    public:

        SOCKET Handle() const noexcept
        {
            return fd;
        }

        sword_t Filter() const noexcept
        {
            return events;
        }

    public:

        sword_t Events() const noexcept
        {
            return revents;
        }

    };

public:

    Poll() = delete;
    ~Poll() = delete;
    Poll(const Poll&) = delete;
    Poll(Poll&&) = delete;
    Poll& operator=(const Poll&) = delete;
    Poll& operator=(Poll&&) = delete;

public:

    static constexpr Time InfiniteTimeout = -1;
    static constexpr Time  InstantTimeout =  0;

public:

    static int Wait(Event* const events, const nfds_t count, const Time timeout) noexcept
    {
        return PollSockets(reinterpret_cast<pollfd*>(events), count, timeout.Milliseconds());
    }

    template <nfds_t Count>
    static int Wait(Event(&events)[Count], const Time timeout) noexcept
    {
        return Wait(events, Count, timeout);
    }

    static int Wait(Event& event, const Time timeout) noexcept
    {
        return Wait(&event, 1, timeout);
    }

public:

    static constexpr int WaitError = SOCKET_ERROR;
    static constexpr int WaitEmpty = 0;

};
