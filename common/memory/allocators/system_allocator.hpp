/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cstdlib>

#include <system/types.hpp>

struct SystemAllocator {

    SystemAllocator() noexcept = default;
    ~SystemAllocator() noexcept = default;
    SystemAllocator(const SystemAllocator&) noexcept = default;
    SystemAllocator(SystemAllocator&&) noexcept = default;
    SystemAllocator& operator=(const SystemAllocator&) noexcept = default;
    SystemAllocator& operator=(SystemAllocator&&) noexcept = default;

public:

    bool Valid() const noexcept
    {
        return true;
    }

    bool Invalid() const noexcept
    {
        return false;
    }

public:

    bool Initialize() noexcept
    {
        return true;
    }

    void Deinitialize() noexcept
    {}

public:

    ptr_t Allocate(const size_t bytes) noexcept
    {
        return std::malloc(bytes);
    }

    void Deallocate(const ptr_t pointer) noexcept
    {
        std::free(pointer);
    }

public:

    void Clear() noexcept
    {}

};
