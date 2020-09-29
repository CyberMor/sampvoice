/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>
#include <cstdint>

#include <ysf/utils/cvector.h>

#include "PointStream.h"

class StaticLocalStreamAtPoint : public PointStream {

    StaticLocalStreamAtPoint() = delete;
    StaticLocalStreamAtPoint(const StaticLocalStreamAtPoint&) = delete;
    StaticLocalStreamAtPoint(StaticLocalStreamAtPoint&&) = delete;
    StaticLocalStreamAtPoint& operator=(const StaticLocalStreamAtPoint&) = delete;
    StaticLocalStreamAtPoint& operator=(StaticLocalStreamAtPoint&&) = delete;

public:

    explicit StaticLocalStreamAtPoint(float distance, const CVector& position,
                                      uint32_t color, const std::string& name);

    ~StaticLocalStreamAtPoint() noexcept = default;

};
