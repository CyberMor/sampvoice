/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>
#include <cstdint>

#include "LocalStream.h"

class StaticLocalStreamAtPlayer : public LocalStream {

    StaticLocalStreamAtPlayer() = delete;
    StaticLocalStreamAtPlayer(const StaticLocalStreamAtPlayer&) = delete;
    StaticLocalStreamAtPlayer(StaticLocalStreamAtPlayer&&) = delete;
    StaticLocalStreamAtPlayer& operator=(const StaticLocalStreamAtPlayer&) = delete;
    StaticLocalStreamAtPlayer& operator=(StaticLocalStreamAtPlayer&&) = delete;

public:

    explicit StaticLocalStreamAtPlayer(float distance, uint16_t playerId,
                                       uint32_t color, const std::string& name);

    ~StaticLocalStreamAtPlayer() noexcept = default;

};
