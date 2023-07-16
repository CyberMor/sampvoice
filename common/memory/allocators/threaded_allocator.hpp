/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <system/types.hpp>
#include <execution/spinlock.hpp>
#include <memory/types.hpp>

template <class Allocator>
struct alignas(CACHE_LINE) ThreadedAllocator {

    ThreadedAllocator() noexcept = default;
    ~ThreadedAllocator() noexcept = default;

public:

    template <class... Arguments>
    ThreadedAllocator(Arguments&&... arguments) noexcept
        : _allocator { std::forward<Arguments>(arguments)... }
    {}

public:

    bool Valid() const noexcept
    {
        return _allocator.Valid();
    }

    bool Invalid() const noexcept
    {
        return _allocator.Invalid();
    }

public:

    template <class... Arguments>
    bool Initialize(Arguments&&... arguments) noexcept
    {
        return _allocator.Initialize(std::forward<Arguments>(arguments)...);
    }

    void Deinitialize() noexcept
    {
        _allocator.Deinitialize();
    }

public:

    ptr_t Allocate(const size_t bytes) noexcept
    {
        _lock.Lock();
        const ptr_t pointer = _allocator.Allocate(bytes);
        _lock.Unlock();

        return pointer;
    }

    void Deallocate(const ptr_t pointer) noexcept
    {
        _lock.Lock();
        _allocator.Deallocate(pointer);
        _lock.Unlock();
    }

public:

    void Clear() noexcept
    {
        _allocator.Clear();
    }

private:

    mutable Spinlock _lock;
    Allocator _allocator;

};
