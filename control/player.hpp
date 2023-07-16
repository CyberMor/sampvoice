/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <system/types.hpp>
#include <memory/structures/array.hpp>
#include <memory/structures/bitset.hpp>
#include <other/version.hpp>

#include "main.hpp"

// Player
// ----------------------------------------------------------------

struct Player {

    Player() = delete;
    ~Player() noexcept = default;
    Player(const Player&) = delete;
    Player(Player&&) = delete;
    Player& operator=(const Player&) = delete;
    Player& operator=(Player&&) = delete;

public:

    Player(const Version version, const bool micro) noexcept
        : version { version }
        , micro   { micro }
    {}

public:

    const Version version;
    const bool    micro;

public:

    bool listener = true;

    udword_t active_channels = 0;
    udword_t played_channels = 0;

    Array<ubyte_t, Bits<udword_t>> keys = 0;
    Array<uword_t, Bits<udword_t>> listeners_count = 0;
    Array<Bitset<kMaxStreams>, Bits<udword_t>> listeners;

    Bitset<kMaxStreams> speakers;
    Bitset<kMaxEffects> effects;

};
