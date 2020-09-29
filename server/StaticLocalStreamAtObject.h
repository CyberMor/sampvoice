/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>
#include <cstdint>

#include "LocalStream.h"

class StaticLocalStreamAtObject : public LocalStream {

    StaticLocalStreamAtObject() = delete;
    StaticLocalStreamAtObject(const StaticLocalStreamAtObject&) = delete;
    StaticLocalStreamAtObject(StaticLocalStreamAtObject&&) = delete;
    StaticLocalStreamAtObject& operator=(const StaticLocalStreamAtObject&) = delete;
    StaticLocalStreamAtObject& operator=(StaticLocalStreamAtObject&&) = delete;

public:

    explicit StaticLocalStreamAtObject(float distance, uint16_t objectId,
                                       uint32_t color, const std::string& name);

    ~StaticLocalStreamAtObject() noexcept = default;

};
