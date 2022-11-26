#pragma once

#include <memory>

#include <Windows.h>

#include "Parameter.h"
#include "Channel.h"

struct SetController : public Parameter {

    SetController() = delete;
    ~SetController() noexcept = default;
    SetController(const SetController&) = delete;
    SetController(SetController&&) = delete;
    SetController& operator=(const SetController&) = delete;
    SetController& operator=(SetController&&) = delete;

public:

    SetController(DWORD parameter, float value) noexcept;

public:

    virtual void Apply(const Channel& channel) noexcept override;

private:

    float _value;

};
