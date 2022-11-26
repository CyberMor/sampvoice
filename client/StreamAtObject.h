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

struct StreamAtObject : public LocalStream {

    StreamAtObject() = delete;
    ~StreamAtObject() noexcept = default;
    StreamAtObject(const StreamAtObject&) = delete;
    StreamAtObject(StreamAtObject&&) = delete;
    StreamAtObject& operator=(const StreamAtObject&) = delete;
    StreamAtObject& operator=(StreamAtObject&&) = delete;

public:

    StreamAtObject(D3DCOLOR color, std::string name, float distance, WORD object_id) noexcept;

public:

    virtual void Tick() noexcept override;

private:

    virtual void OnChannelCreate(const Channel& channel) noexcept override;

private:

    WORD _object_id;

};
