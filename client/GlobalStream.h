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

#include "Stream.h"

class GlobalStream : public Stream {

    GlobalStream() = delete;
    GlobalStream(const GlobalStream&) = delete;
    GlobalStream(GlobalStream&&) = delete;
    GlobalStream& operator=(const GlobalStream&) = delete;
    GlobalStream& operator=(GlobalStream&&) = delete;

public:

    explicit GlobalStream(D3DCOLOR color, std::string name) noexcept;

    ~GlobalStream() noexcept = default;

};

using GlobalStreamPtr = std::unique_ptr<GlobalStream>;
#define MakeGlobalStream std::make_unique<GlobalStream>
