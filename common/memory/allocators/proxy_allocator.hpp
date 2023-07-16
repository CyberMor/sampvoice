/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>
#include <memory>

#include <system/types.hpp>

template <class Allocator>
struct ProxyAllocator {

    ProxyAllocator() noexcept = default;
    ~ProxyAllocator() noexcept = default;

public:

    ProxyAllocator(std::shared_ptr<Allocator> allocator) noexcept
        : _allocator { std::move(allocator) }
    {}

public:

    bool Valid() const noexcept
    {
        return _allocator != nullptr && _allocator->Valid();
    }

    bool Invalid() const noexcept
    {
        return _allocator == nullptr || _allocator->Invalid();
    }

public:

    bool Initialize(std::shared_ptr<Allocator> allocator) noexcept
    {
        return (_allocator = std::move(allocator)) != nullptr;
    }

    void Deinitialize() noexcept
    {
        _allocator = nullptr;
    }

public:

    ptr_t Allocate(const size_t bytes) noexcept
    {
        assert(_allocator != nullptr);

        return _allocator->Allocate(bytes);
    }

    void Deallocate(const ptr_t pointer) noexcept
    {
        assert(_allocator != nullptr);

        _allocator->Deallocate(pointer);
    }

public:

    void Clear() noexcept
    {
        assert(_allocator != nullptr);

        _allocator->Clear();
    }

private:

    std::shared_ptr<Allocator> _allocator;

};
