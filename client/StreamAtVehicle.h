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

struct StreamAtVehicle : public LocalStream {

    StreamAtVehicle() = delete;
    ~StreamAtVehicle() noexcept = default;
    StreamAtVehicle(const StreamAtVehicle&) = delete;
    StreamAtVehicle(StreamAtVehicle&&) = delete;
    StreamAtVehicle& operator=(const StreamAtVehicle&) = delete;
    StreamAtVehicle& operator=(StreamAtVehicle&&) = delete;

public:

    StreamAtVehicle(D3DCOLOR color, std::string name, float distance, WORD vehicle_id) noexcept;

public:

    virtual void Tick() noexcept override;

private:

    virtual void OnChannelCreate(const Channel& channel) noexcept override;

private:

    WORD _vehicle_id;

};
