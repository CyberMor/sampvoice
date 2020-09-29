/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <atomic>
#include <cstdint>
#include <set>

#include "Stream.h"

struct PlayerInfo {

    PlayerInfo() = delete;
    PlayerInfo(const PlayerInfo&) = delete;
    PlayerInfo(PlayerInfo&&) = delete;
    PlayerInfo& operator=(const PlayerInfo&) = delete;
    PlayerInfo& operator=(PlayerInfo&&) = delete;

public:

    PlayerInfo(uint8_t pluginVersion, bool microStatus) noexcept
        : pluginVersion(pluginVersion), microStatus(microStatus) {}

    ~PlayerInfo() noexcept = default;

public:

    const uint8_t pluginVersion { NULL };
    const bool microStatus { false };

    std::atomic_bool muteStatus { false };
    std::atomic_bool recordStatus { false };
    std::set<Stream*> listenerStreams;
    std::set<Stream*> speakerStreams;
    std::set<uint8_t> keys;

};
