/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>

#include <d3d9.h>

#include "LocalStream.h"
#include "Channel.h"

struct StreamAtPlayer : public LocalStream {

    StreamAtPlayer() = delete;
    ~StreamAtPlayer() noexcept = default;
    StreamAtPlayer(const StreamAtPlayer&) = delete;
    StreamAtPlayer(StreamAtPlayer&&) = delete;
    StreamAtPlayer& operator=(const StreamAtPlayer&) = delete;
    StreamAtPlayer& operator=(StreamAtPlayer&&) = delete;

public:

    StreamAtPlayer(D3DCOLOR color, std::string name, float distance, WORD player_id) noexcept;

public:

    virtual void Tick() noexcept override;

private:

    virtual void OnChannelCreate(const Channel& channel) noexcept override;

private:

    WORD _player_id;

};
