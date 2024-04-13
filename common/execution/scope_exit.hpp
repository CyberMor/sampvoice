/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <functional>

struct ScopeExit {

    using Callback = std::function<void()>;

public:

    ScopeExit() noexcept = default;
    ~ScopeExit() noexcept
    {
        if (_callback != nullptr)
            _callback();
    }

    ScopeExit(const ScopeExit&) = delete;
    ScopeExit(ScopeExit&& object) noexcept
        : _callback { std::move(object._callback) }
    {
        object._callback = nullptr;
    }

    ScopeExit& operator=(const ScopeExit&) = delete;
    ScopeExit& operator=(ScopeExit&& object) noexcept
    {
        if (&object != this)
        {
            if (_callback != nullptr)
                _callback();

            _callback = std::move(object._callback);

            object._callback = nullptr;
        }

        return *this;
    }

public:

    ScopeExit(Callback&& callback) noexcept
        : _callback { std::move(callback) }
    {}

public:

    void Release() noexcept
    {
        _callback = nullptr;
    }

private:

    Callback _callback;

};
