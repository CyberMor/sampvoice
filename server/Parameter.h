#pragma once

#include <cstdint>

#include <util/timer.h>

#include "ControlPacket.h"

class Parameter {

    Parameter() = delete;
    Parameter(const Parameter&) = delete;
    Parameter(Parameter&&) = delete;
    Parameter& operator=(const Parameter&) = delete;
    Parameter& operator=(Parameter&&) = delete;

public:

    explicit Parameter(class Stream* stream, uint8_t parameter, float initValue);

    ~Parameter() noexcept = default;

public:

    float Get() const noexcept;
    void Set(float value) noexcept;

    void SlideFromTo(float startValue, float endValue, uint32_t time) noexcept;
    void SlideTo(float endValue, uint32_t time) noexcept;
    void Slide(float deltaValue, uint32_t time) noexcept;

    void Update() noexcept;
    void ApplyForPlayer(uint16_t player) const;

private:

    bool sliding { false };
    float ratio { 0.f };
    Timer::time_t endTime { 0 };
    float endValue { 0.f };

    ControlPacketContainerPtr packetSetParameter { nullptr };
    ControlPacketContainerPtr packetSlideParameter { nullptr };

};
