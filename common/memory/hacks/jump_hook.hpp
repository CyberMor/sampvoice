/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <system/types.hpp>

#include "patch.hpp"

struct JumpHook {

    JumpHook() noexcept = default;
    ~JumpHook() noexcept = default;
    JumpHook(const JumpHook&) = delete;
    JumpHook(JumpHook&&) noexcept = default;
    JumpHook& operator=(const JumpHook&) = delete;
    JumpHook& operator=(JumpHook&&) noexcept = default;

private:

#pragma pack(push, 1)

    struct JumpInstruction {

        JumpInstruction() = delete;
        ~JumpInstruction() noexcept = default;
        JumpInstruction(const JumpInstruction&) noexcept = default;
        JumpInstruction(JumpInstruction&&) noexcept = default;
        JumpInstruction& operator=(const JumpInstruction&) noexcept = default;
        JumpInstruction& operator=(JumpInstruction&&) noexcept = default;

    public:

        JumpInstruction(const sptrint_t offset) noexcept
            : _offset { offset }
        {}

    public:

        sptrint_t Offset() const noexcept
        {
            return _offset;
        }

    private:

          ubyte_t _opcode = 0xE9;
        sptrint_t _offset;

    };

#pragma pack(pop)

public:

    JumpHook(const ptr_t inject, const cptr_t hook, const bool enabled = true) noexcept
        : _patch { hook != nullptr ? inject : nullptr, &JumpInstruction(reinterpret_cast<sptrint_t>(hook) -
            (reinterpret_cast<sptrint_t>(inject) + static_cast<sptrint_t>(sizeof(JumpInstruction)))), enabled }
    {}

public:

    bool Valid() const noexcept
    {
        return _patch.Valid();
    }

    bool Invalid() const noexcept
    {
        return _patch.Invalid();
    }

public:

    bool Initialize(const ptr_t inject, const cptr_t hook, const bool enabled = true) noexcept
    {
        return _patch.Initialize(hook != nullptr ? inject : nullptr, &JumpInstruction(reinterpret_cast<sptrint_t>(hook) -
            (reinterpret_cast<sptrint_t>(inject) + static_cast<sptrint_t>(sizeof(JumpInstruction)))), enabled);
    }

    void Deinitialize() noexcept
    {
        _patch.Deinitialize();
    }

public:

    bool Enable() noexcept
    {
        return _patch.Enable();
    }

    bool Disable() noexcept
    {
        return _patch.Disable();
    }

public:

    ptr_t Address() const noexcept
    {
        return _patch.Address();
    }

    static constexpr size_t Length() noexcept
    {
        return sizeof(JumpInstruction);
    }

private:

    Patch<sizeof(JumpInstruction)> _patch;

};
