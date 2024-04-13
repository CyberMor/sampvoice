/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/mman.h>
#endif

#include <system/types.hpp>

template <size_t Size>
struct UnprotectScope {

    static_assert(Size != 0);

public:

    UnprotectScope() noexcept = default;
    ~UnprotectScope() noexcept { Disable(); }

    UnprotectScope(const UnprotectScope&) = delete;
    UnprotectScope(UnprotectScope&& object) noexcept
        : _address { object._address }
        , _enabled { object._enabled }
#ifdef _WIN32
        , _protect { object._protect }
#endif
    {
        object._address = nullptr;
        object._enabled = false;
    }

    UnprotectScope& operator=(const UnprotectScope&) = delete;
    UnprotectScope& operator=(UnprotectScope&& object) noexcept
    {
        if (&object != this)
        {
            Disable();

            _address = object._address;
            _enabled = object._enabled;
#ifdef _WIN32
            _protect = object._protect;
#endif

            object._address = nullptr;
            object._enabled = false;
        }

        return *this;
    }

public:

    UnprotectScope(const ptr_t address, const bool enabled = true) noexcept
        : _address { address }
    {
        if (_address != nullptr && enabled == true && Enable() == false)
            _address  = nullptr;
    }

public:

    bool Valid() const noexcept
    {
        return _address != nullptr;
    }

    bool Invalid() const noexcept
    {
        return _address == nullptr;
    }

public:

    bool Initialize(const ptr_t address, const bool enabled = true) noexcept
    {
        Disable();

        _address = address;
#ifdef _WIN32
        _protect = PAGE_EXECUTE_READWRITE;
#endif

        if (_address != nullptr && enabled == true && Enable() == false)
            _address  = nullptr;

        return _address != nullptr;
    }

    void Deinitialize() noexcept
    {
        Disable();

        _address = nullptr;
    }

public:

    bool Enable() noexcept
    {
        if (_enabled == false)
        {
            assert(_address != nullptr);
#ifdef _WIN32
            assert(_protect == PAGE_EXECUTE_READWRITE);

            if (VirtualProtect(_address, Size, _protect, &_protect) != 0)
                _enabled = true;
#else
            if (const long pagesize = sysconf(_SC_PAGESIZE); pagesize > 0)
            {
                const adr_t begin = reinterpret_cast<adr_t>
                    (reinterpret_cast<uptrint_t>(_address) & ~(static_cast<uptrint_t>(pagesize) - 1));
                const adr_t end = static_cast<adr_t>(_address) + Size;

                if (mprotect(begin, end - begin, PROT_READ | PROT_WRITE | PROT_EXEC) == 0)
                    _enabled = true;
            }
#endif
        }

        return _enabled == true;
    }

    bool Disable() noexcept
    {
        if (_enabled == true)
        {
            assert(_address != nullptr);
#ifdef _WIN32
            if (VirtualProtect(_address, Size, _protect, &_protect) != 0)
                _enabled = false;
#endif
        }

#ifdef _WIN32
        return _enabled == false;
#else
        return true;
#endif
    }

public:

    ptr_t Address() const noexcept
    {
        return _address;
    }

    static constexpr size_t Length() noexcept
    {
        return Size;
    }

private:

    ptr_t _address = nullptr;
    bool  _enabled = false;
#ifdef _WIN32
    DWORD _protect = PAGE_EXECUTE_READWRITE;
#endif

};

template <size_t Size>
static inline bool UnprotectMemory(const ptr_t address) noexcept
{
    static_assert(Size != 0);

    assert(address != nullptr);

#ifdef _WIN32
    if (DWORD protect; VirtualProtect(address, Size, PAGE_EXECUTE_READWRITE, &protect) != 0)
        return true;
#else
    if (const long pagesize = sysconf(_SC_PAGESIZE); pagesize > 0)
    {
        const adr_t begin = reinterpret_cast<adr_t>
            (reinterpret_cast<uptrint_t>(address) & ~(static_cast<uptrint_t>(pagesize) - 1));
        const adr_t end = static_cast<adr_t>(address) + Size;

        if (mprotect(begin, end - begin, PROT_READ | PROT_WRITE | PROT_EXEC) == 0)
            return true;
    }
#endif

    return false;
}
