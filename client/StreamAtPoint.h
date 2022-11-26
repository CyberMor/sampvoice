/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>

#include <d3d9.h>

#include <game/CVector.h>

#include "LocalStream.h"
#include "Channel.h"

struct StreamAtPoint : public LocalStream {

    StreamAtPoint() = delete;
    ~StreamAtPoint() noexcept = default;
    StreamAtPoint(const StreamAtPoint&) = delete;
    StreamAtPoint(StreamAtPoint&&) = delete;
    StreamAtPoint& operator=(const StreamAtPoint&) = delete;
    StreamAtPoint& operator=(StreamAtPoint&&) = delete;

public:

    StreamAtPoint(D3DCOLOR color, std::string name, float distance, const CVector& position) noexcept;

public:

    void SetPosition(const CVector& position) noexcept;

private:

    virtual void OnChannelCreate(const Channel& channel) noexcept override;

private:

    CVector _position;

};
