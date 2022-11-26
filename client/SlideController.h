#pragma once

#include <Windows.h>

#include <util/Timer.h>

#include "Parameter.h"
#include "Channel.h"

struct SlideController : public Parameter {

    SlideController() = delete;
    ~SlideController() noexcept = default;
    SlideController(const SlideController&) = delete;
    SlideController(SlideController&&) = delete;
    SlideController& operator=(const SlideController&) = delete;
    SlideController& operator=(SlideController&&) = delete;

public:

    SlideController(DWORD parameter, float start_value, float end_value, Timer::time_t time) noexcept;

public:

    virtual void Apply(const Channel& channel) noexcept override;

private:

    float         _ratio;
    Timer::time_t _end_time;
    float         _end_value;

};
