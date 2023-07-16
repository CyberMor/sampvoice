/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <system/types.hpp>
#include <memory/structures/bitset.hpp>
#include <memory/block.hpp>

#include "main.hpp"

// Target
// ----------------------------------------------------------------

constexpr uword_t kVehicle = 1;
constexpr uword_t kPlayer  = 2;
constexpr uword_t kObject  = 3;

// ----------------------------------------------------------------

constexpr uword_t GetTargetType(const uword_t target) noexcept
{
    return target >> 14;
}

constexpr uword_t GetTargetIndex(const uword_t target) noexcept
{
    return target & static_cast<uword_t>(0x3FFF);
}

// ----------------------------------------------------------------

constexpr uword_t MakeTarget(const uword_t type, const uword_t index) noexcept
{
    return (type << 14) | (index & static_cast<uword_t>(0x3FFF));
}

// Stream
// ----------------------------------------------------------------

struct Stream {

    Stream() = delete;
    ~Stream() noexcept = default;
    Stream(const Stream&) = delete;
    Stream(Stream&&) = delete;
    Stream& operator=(const Stream&) = delete;
    Stream& operator=(Stream&&) = delete;

public:

    Stream(const fdword_t distance) noexcept
        : distance { distance }
    {}

public:

    bool transiter = true;

    Bitset<kMaxPlayers> listeners;

public:

    fdword_t volume = 1;
    fdword_t panning = 0;

    fdword_t distance;
    CVector  position;

    uword_t target = Zero<uword_t>;
    uword_t effect = None<uword_t>;

    Block<char> icon;

};
