/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>

#include "types.hpp"

struct Time {

    constexpr Time() noexcept = default;
    ~Time() noexcept = default;
    constexpr Time(const Time&) noexcept = default;
    constexpr Time(Time&&) noexcept = default;
    constexpr Time& operator=(const Time&) noexcept = default;
    constexpr Time& operator=(Time&&) noexcept = default;

public:

    constexpr Time(const sqword_t milliseconds) noexcept
        : _milliseconds { milliseconds }
    {}

public:

    static constexpr Time Milliseconds(const sqword_t value) noexcept
    {
        return value;
    }

    template <class Number, typename = std::enable_if_t<std::is_arithmetic_v<Number>>>
    static constexpr Time Seconds(const Number value) noexcept
    {
        return static_cast<sqword_t>(static_cast<sqword_t>(1000) * value);
    }

    template <class Number, typename = std::enable_if_t<std::is_arithmetic_v<Number>>>
    static constexpr Time Minutes(const Number value) noexcept
    {
        return static_cast<sqword_t>(static_cast<sqword_t>(60 * 1000) * value);
    }

    template <class Number, typename = std::enable_if_t<std::is_arithmetic_v<Number>>>
    static constexpr Time Hours(const Number value) noexcept
    {
        return static_cast<sqword_t>(static_cast<sqword_t>(60 * 60 * 1000) * value);
    }

public:

    constexpr Time operator+(const Time time) const noexcept
    {
        return _milliseconds + time._milliseconds;
    }

    constexpr Time operator-(const Time time) const noexcept
    {
        return _milliseconds - time._milliseconds;
    }

    constexpr Time operator%(const Time time) const noexcept
    {
        return _milliseconds % time._milliseconds;
    }

    template <class Number, typename = std::enable_if_t<std::is_arithmetic_v<Number>>>
    constexpr Time operator*(const Number number) const noexcept
    {
        return static_cast<sqword_t>(_milliseconds * number);
    }

    template <class Number, typename = std::enable_if_t<std::is_arithmetic_v<Number>>>
    constexpr Time operator/(const Number number) const noexcept
    {
        assert(number != 0);

        return static_cast<sqword_t>(_milliseconds / number);
    }

public:

    constexpr Time& operator+=(const Time time) noexcept
    {
        _milliseconds += time._milliseconds;

        return *this;
    }

    constexpr Time& operator-=(const Time time) noexcept
    {
        _milliseconds -= time._milliseconds;

        return *this;
    }

    constexpr Time& operator%=(const Time time) noexcept
    {
        _milliseconds %= time._milliseconds;

        return *this;
    }

    template <class Number, typename = std::enable_if_t<std::is_arithmetic_v<Number>>>
    constexpr Time& operator*=(const Number number) noexcept
    {
        _milliseconds *= number;

        return *this;
    }

    template <class Number, typename = std::enable_if_t<std::is_arithmetic_v<Number>>>
    constexpr Time& operator/=(const Number number) noexcept
    {
        assert(number != 0);

        _milliseconds /= number;

        return *this;
    }

public:

    constexpr bool operator<(const Time time) const noexcept
    {
        return _milliseconds < time._milliseconds;
    }

    constexpr bool operator>(const Time time) const noexcept
    {
        return _milliseconds > time._milliseconds;
    }

    constexpr bool operator<=(const Time time) const noexcept
    {
        return _milliseconds <= time._milliseconds;
    }

    constexpr bool operator>=(const Time time) const noexcept
    {
        return _milliseconds >= time._milliseconds;
    }

    constexpr bool operator==(const Time time) const noexcept
    {
        return _milliseconds == time._milliseconds;
    }

    constexpr bool operator!=(const Time time) const noexcept
    {
        return _milliseconds != time._milliseconds;
    }

public:

    constexpr sqword_t Milliseconds() const noexcept
    {
        return _milliseconds;
    }

    template <class Number = fdword_t, typename = std::enable_if_t<std::is_arithmetic_v<Number>>>
    constexpr Number Seconds() const noexcept
    {
        if constexpr (std::is_floating_point_v<Number>)
             return static_cast<Number>(_milliseconds) / static_cast<sqword_t>(1000);
        else return static_cast<Number>(_milliseconds / static_cast<sqword_t>(1000));
    }

    template <class Number = fdword_t, typename = std::enable_if_t<std::is_arithmetic_v<Number>>>
    constexpr Number Minutes() const noexcept
    {
        if constexpr (std::is_floating_point_v<Number>)
             return static_cast<Number>(_milliseconds) / static_cast<sqword_t>(60 * 1000);
        else return static_cast<Number>(_milliseconds / static_cast<sqword_t>(60 * 1000));
    }

    template <class Number = fdword_t, typename = std::enable_if_t<std::is_arithmetic_v<Number>>>
    constexpr Number Hours() const noexcept
    {
        if constexpr (std::is_floating_point_v<Number>)
             return static_cast<Number>(_milliseconds) / static_cast<sqword_t>(60 * 60 * 1000);
        else return static_cast<Number>(_milliseconds / static_cast<sqword_t>(60 * 60 * 1000));
    }

private:

    sqword_t _milliseconds = {};

};
