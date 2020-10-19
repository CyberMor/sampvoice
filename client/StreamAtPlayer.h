/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>
#include <string>

#include <d3d9.h>

#include "LocalStream.h"
#include "Channel.h"

class StreamAtPlayer : public LocalStream {

    StreamAtPlayer() = delete;
    StreamAtPlayer(const StreamAtPlayer&) = delete;
    StreamAtPlayer(StreamAtPlayer&&) = delete;
    StreamAtPlayer& operator=(const StreamAtPlayer&) = delete;
    StreamAtPlayer& operator=(StreamAtPlayer&&) = delete;

public:

    explicit StreamAtPlayer(D3DCOLOR color, std::string name,
                            float distance, WORD playerId) noexcept;

    ~StreamAtPlayer() noexcept = default;

public:

    void Tick() noexcept override;

private:

    void OnChannelCreate(const Channel& channel) noexcept override;

private:

    const WORD playerId;

};

using StreamAtPlayerPtr = std::unique_ptr<StreamAtPlayer>;
#define MakeStreamAtPlayer std::make_unique<StreamAtPlayer>
