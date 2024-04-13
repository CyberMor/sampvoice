/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <atomic>
#include <thread>

struct Spinlock {

    Spinlock() noexcept = default;
    ~Spinlock() noexcept = default;
    Spinlock(const Spinlock&) = delete;
    Spinlock(Spinlock&&) = delete;
    Spinlock& operator=(const Spinlock&) = delete;
    Spinlock& operator=(Spinlock&&) = delete;

public:

    bool TryLock() noexcept
    {
        return _flag.test_and_set(std::memory_order_acquire) == false;
    }

    void Lock() noexcept
    {
        while (_flag.test_and_set(std::memory_order_acquire) == true)
            std::this_thread::yield();
    }

    void Unlock() noexcept
    {
        _flag.clear(std::memory_order_release);
    }

private:

    std::atomic_flag _flag = ATOMIC_FLAG_INIT;

};
