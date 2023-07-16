/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <functional>

struct ScopeExit {

    ScopeExit() noexcept = default;
    ~ScopeExit() noexcept
    {
        if (_callback != nullptr)
            _callback();
    }

    ScopeExit(const ScopeExit&) = delete;
    ScopeExit(ScopeExit&&) noexcept = default;
    ScopeExit& operator=(const ScopeExit&) = delete;
    ScopeExit& operator=(ScopeExit&&) noexcept = default;

public:

    ScopeExit(std::function<void()>&& callback) noexcept
        : _callback { std::move(callback) }
    {}

public:

    void Release() noexcept
    {
        _callback = nullptr;
    }

private:

    std::function<void()> _callback;

};
