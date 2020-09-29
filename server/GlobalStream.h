/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>
#include <cstdint>

#include "Stream.h"

class GlobalStream : public Stream {

    GlobalStream() = delete;
    GlobalStream(const GlobalStream&) = delete;
    GlobalStream(GlobalStream&&) = delete;
    GlobalStream& operator=(const GlobalStream&) = delete;
    GlobalStream& operator=(GlobalStream&&) = delete;

public:

    explicit GlobalStream(uint32_t color, const std::string& name);

    ~GlobalStream() noexcept = default;

};
