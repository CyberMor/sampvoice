/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>
#include <cstdint>

#include "DynamicStream.h"

class DynamicLocalStreamAtPlayer : public DynamicStream {

    DynamicLocalStreamAtPlayer() = delete;
    DynamicLocalStreamAtPlayer(const DynamicLocalStreamAtPlayer&) = delete;
    DynamicLocalStreamAtPlayer(DynamicLocalStreamAtPlayer&&) = delete;
    DynamicLocalStreamAtPlayer& operator=(const DynamicLocalStreamAtPlayer&) = delete;
    DynamicLocalStreamAtPlayer& operator=(DynamicLocalStreamAtPlayer&&) = delete;

public:

    explicit DynamicLocalStreamAtPlayer(float distance, uint32_t maxPlayers,
                                        uint16_t playerId, uint32_t color,
                                        const std::string& name);

    ~DynamicLocalStreamAtPlayer() noexcept = default;

public:

    void Tick() override;

};
