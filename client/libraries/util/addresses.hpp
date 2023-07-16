/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>

#include <system/types.hpp>

#include <samp/commonSA.h>

struct Addresses {

    Addresses(const Addresses&) = delete;
    Addresses(Addresses&&) = delete;
    Addresses& operator=(const Addresses&) = delete;
    Addresses& operator=(Addresses&&) = delete;

private:

    Addresses() noexcept = default;
    ~Addresses() noexcept = default;

public:

    static Addresses& Instance() noexcept
    {
        static Addresses instance;
        return instance;
    }

public:

    void Initialize(const ptr_t base) noexcept
    {
        _base = static_cast<adr_t>(base);
    }

    void Deinitialize() noexcept
    {
        _base = nullptr;
    }

public:

    ptr_t Base() const noexcept
    {
        assert(_base != nullptr);
        return _base;
    }

public:

    ptr_t RakClientInitialization() const noexcept
    {
        assert(_base != nullptr);
#if defined(SAMP_R1)
        return _base + 0x2401D3;
#elif defined(SAMP_R3)
        return _base + 0xB658;
#endif
    }

    ptr_t BassInitialization() const noexcept
    {
        assert(_base != nullptr);
#if defined(SAMP_R1)
        return _base + 0x628DF;
#elif defined(SAMP_R3)
        return _base + 0x65D2F;
#endif
    }

    ptr_t BassConfiguration() const noexcept
    {
        assert(_base != nullptr);
#if defined(SAMP_R1)
        return _base + 0x6290F;
#elif defined(SAMP_R3)
        return _base + 0x65D5F;
#endif
    }

    ptr_t SampInitialization() const noexcept
    {
        assert(_base != nullptr);
#if defined(SAMP_R1)
        return _base + 0x2565E2;
#elif defined(SAMP_R3)
        return _base + 0xC57E2;
#endif
    }

    ptr_t SampDestruction() const noexcept
    {
        assert(_base != nullptr);
#if defined(SAMP_R1)
        return _base + 0x9380;
#elif defined(SAMP_R3);
        return _base + 0x9510;
#endif
    }

    ptr_t OpenChatFunction() const noexcept
    {
        assert(_base != nullptr);
#if defined(SAMP_R1)
        return _base + 0x657E0;
#elif defined(SAMP_R3)
        return _base + 0x68D10;
#endif
    }

    ptr_t SwitchModeFunction() const noexcept
    {
        assert(_base != nullptr);
#if defined(SAMP_R1)
        return _base + 0x5D7B0;
#elif defined(SAMP_R3)
        return _base + 0x60B50;
#endif
    }

    ptr_t OpenScoreboardFunction() const noexcept
    {
        assert(_base != nullptr);
#if defined(SAMP_R1)
        return _base + 0x6AD30;
#elif defined(SAMP_R3)
        return _base + 0x6EC80;
#endif
    }

    ptr_t CreatePlayerInPoolFunction() const noexcept
    {
        assert(_base != nullptr);
#if defined(SAMP_R1)
        return _base + 0x10D50;
#elif defined(SAMP_R3)
        return _base + 0x13E80;
#endif
    }

    ptr_t DeletePlayerFromPoolFunction() const noexcept
    {
        assert(_base != nullptr);
#if defined(SAMP_R1)
        return _base + 0x10B90;
#elif defined(SAMP_R3)
        return _base + 0x13CB0;
#endif
    }

    ptr_t SpawnLocalPlayerFunction() const noexcept
    {
        assert(_base != nullptr);
#if defined(SAMP_R1)
        return _base + 0x3AD0;
#elif defined(SAMP_R3)
        return _base + 0x3AD0;
#endif
    }

    ptr_t DrawLabelFunction() const noexcept
    {
        assert(_base != nullptr);
#if defined(SAMP_R1)
        return _base + 0x686C0;
#elif defined(SAMP_R3)
        return _base + 0x6C630;
#endif
    }

private:

    adr_t _base = nullptr;

};
