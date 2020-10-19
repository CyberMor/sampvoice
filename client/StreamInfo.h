/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>

#include <d3d9.h>

enum class StreamType
{
    None,
    GlobalStream,
    LocalStreamAtPoint,
    LocalStreamAtVehicle,
    LocalStreamAtPlayer,
    LocalStreamAtObject
};

struct StreamInfo {

    StreamInfo() noexcept = default;
    StreamInfo(const StreamInfo&) = default;
    StreamInfo(StreamInfo&&) noexcept = default;
    StreamInfo& operator=(const StreamInfo&) = default;
    StreamInfo& operator=(StreamInfo&&) noexcept = default;

public:

    StreamInfo(StreamType type, D3DCOLOR color, std::string name) noexcept;

    ~StreamInfo() noexcept = default;

public:

    StreamType GetType() const noexcept;
    D3DCOLOR GetColor() const noexcept;
    const std::string& GetName() const noexcept;

private:

    StreamType type { StreamType::None };
    D3DCOLOR color { -1u };
    std::string name;

};
