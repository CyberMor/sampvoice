#pragma once

#include <Windows.h>

#include "Channel.h"

struct Parameter {

    Parameter() = delete;
    virtual ~Parameter() noexcept = default;
    Parameter(const Parameter&) = delete;
    Parameter(Parameter&&) = delete;
    Parameter& operator=(const Parameter&) = delete;
    Parameter& operator=(Parameter&&) = delete;

protected:

    Parameter(DWORD parameter) noexcept;

public:

    virtual void Apply(const Channel& channel) noexcept = 0;

protected:

    DWORD _parameter;

};
