/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>
#include <cstring>
#include <utility>

#include <system/types.hpp>

#include "unprotect_scope.hpp"

template <size_t Size>
struct Patch {

    static_assert(Size != 0);

public:

    Patch() noexcept = default;
    ~Patch() noexcept { Disable(); }

    Patch(const Patch&) = delete;
    Patch(Patch&& object) noexcept
        : _enabled { object._enabled }
        , _upscope { std::move(object._upscope) }
    {
        std::memcpy(_patch_bytes, object._patch_bytes, Size);
        std::memcpy(_orig_bytes, object._orig_bytes, Size);

        object._enabled = false;
    }

    Patch& operator=(const Patch&) = delete;
    Patch& operator=(Patch&& object) noexcept
    {
        if (&object != this)
        {
            Disable();

            _enabled = object._enabled;
            _upscope = std::move(object._upscope);

            std::memcpy(_patch_bytes, object._patch_bytes, Size);
            std::memcpy(_orig_bytes, object._orig_bytes, Size);

            object._enabled = false;
        }

        return *this;
    }

public:

    Patch(const ptr_t address, const cptr_t patch, const bool enabled = true) noexcept
        : _upscope { patch != nullptr ? address : nullptr, false }
    {
        if (_upscope.Valid())
        {
            std::memcpy(_patch_bytes, patch, Size);
            std::memcpy(_orig_bytes, address, Size);

            if (enabled == true && Enable() == false)
                _upscope.Deinitialize();
        }
    }

public:

    bool Valid() const noexcept
    {
        return _upscope.Valid();
    }

    bool Invalid() const noexcept
    {
        return _upscope.Invalid();
    }

public:

    bool Initialize(const ptr_t address, const cptr_t patch, const bool enabled = true) noexcept
    {
        Disable();

        if (_upscope.Initialize(patch != nullptr ? address : nullptr, false))
        {
            std::memcpy(_patch_bytes, patch, Size);
            std::memcpy(_orig_bytes, address, Size);

            if (enabled == true && Enable() == false)
                _upscope.Deinitialize();
        }

        return _upscope.Valid();
    }

    void Deinitialize() noexcept
    {
        Disable();

        _upscope.Deinitialize();
    }

public:

    bool Enable() noexcept
    {
        if (_enabled == false)
        {
            if (_upscope.Enable())
            {
                std::memcpy(_upscope.Address(), _patch_bytes, Size);
#ifndef NDEBUG
                const bool disabled =
#endif
                    _upscope.Disable();
#ifndef NDEBUG
                assert(disabled);
#endif
                _enabled = true;
            }
        }

        return _enabled == true;
    }

    bool Disable() noexcept
    {
        if (_enabled == true)
        {
            if (_upscope.Enable())
            {
                std::memcpy(_upscope.Address(), _orig_bytes, Size);
#ifndef NDEBUG
                const bool disabled =
#endif
                    _upscope.Disable();
#ifndef NDEBUG
                assert(disabled);
#endif
                _enabled = false;
            }
        }

        return _enabled == false;
    }

public:

    ptr_t Address() const noexcept
    {
        return _upscope.Address();
    }

    static constexpr size_t Length() noexcept
    {
        return Size;
    }

private:

    bool _enabled = false;

    alignas (alignof(std::max_align_t)) ubyte_t _patch_bytes[Size];
    alignas (alignof(std::max_align_t)) ubyte_t  _orig_bytes[Size];

    UnprotectScope<Size> _upscope;

};

template <size_t Size>
static inline void FillWithNops(const ptr_t address) noexcept
{
    static_assert(Size != 0);

    assert(address != nullptr);

    const UnprotectScope<Size> upscope { address };
    std::memset(address, 0x90, Size);
}
