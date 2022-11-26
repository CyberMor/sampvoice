/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>

#include <d3d9.h>

#include "Stream.h"

struct GlobalStream : public Stream {

    GlobalStream() = delete;
    ~GlobalStream() noexcept = default;
    GlobalStream(const GlobalStream&) = delete;
    GlobalStream(GlobalStream&&) = delete;
    GlobalStream& operator=(const GlobalStream&) = delete;
    GlobalStream& operator=(GlobalStream&&) = delete;

public:

    GlobalStream(D3DCOLOR color, std::string name) noexcept;

};
