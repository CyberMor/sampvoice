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
#include "DynamicStream.h"

class DynamicLocalStreamAtPoint : public PointStream, public DynamicStream {

    DynamicLocalStreamAtPoint() = delete;
    DynamicLocalStreamAtPoint(const DynamicLocalStreamAtPoint&) = delete;
    DynamicLocalStreamAtPoint(DynamicLocalStreamAtPoint&&) = delete;
    DynamicLocalStreamAtPoint& operator=(const DynamicLocalStreamAtPoint&) = delete;
    DynamicLocalStreamAtPoint& operator=(DynamicLocalStreamAtPoint&&) = delete;

public:

    explicit DynamicLocalStreamAtPoint(float distance, uint32_t maxPlayers,
                                       const CVector& position, uint32_t color,
                                       const std::string& name);

    ~DynamicLocalStreamAtPoint() noexcept = default;

public:

    void Tick() override;

};
