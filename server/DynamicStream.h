/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cstdint>
#include <vector>
#include <set>

#include "LocalStream.h"

class DynamicStream : public virtual LocalStream {

    DynamicStream() = delete;
    DynamicStream(const DynamicStream&) = delete;
    DynamicStream(DynamicStream&&) = delete;
    DynamicStream& operator=(const DynamicStream&) = delete;
    DynamicStream& operator=(DynamicStream&&) = delete;

protected:

    explicit DynamicStream(float distance, uint32_t maxPlayers);

public:

    virtual ~DynamicStream() noexcept = default;

public:

    virtual void Tick() = 0;

    bool AttachListener(uint16_t playerId) noexcept override;
    bool DetachListener(uint16_t playerId) noexcept override;
    std::vector<uint16_t> DetachAllListeners() noexcept override;

protected:

    const uint32_t maxPlayers;

protected:

    struct PlayerSortInfo {

        PlayerSortInfo() = delete;
        PlayerSortInfo(const PlayerSortInfo&) noexcept = default;
        PlayerSortInfo(PlayerSortInfo&&) noexcept = default;
        PlayerSortInfo& operator=(const PlayerSortInfo&) = delete;
        PlayerSortInfo& operator=(PlayerSortInfo&&) = delete;

    public:

        explicit PlayerSortInfo(float distance, uint32_t playerId) noexcept
            : distance(distance), playerId(playerId) {}

        ~PlayerSortInfo() noexcept = default;

    public:

        bool operator<(const PlayerSortInfo& object) const noexcept
        {
            return this->distance < object.distance;
        }

    public:

        const float distance;
        const uint32_t playerId;

    };

    using PlayerSortList = std::multiset<PlayerSortInfo>;

};
