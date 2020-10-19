#pragma once

#include <memory>

#include <Windows.h>

#include "Channel.h"

class Parameter {

    Parameter() = delete;
    Parameter(const Parameter&) = delete;
    Parameter(Parameter&&) = delete;
    Parameter& operator=(const Parameter&) = delete;
    Parameter& operator=(Parameter&&) = delete;

protected:

    explicit Parameter(DWORD parameter) noexcept;

public:

    virtual ~Parameter() noexcept = default;

public:

    virtual void Apply(const Channel& channel) const noexcept = 0;

protected:

    const DWORD parameter;

};

using ParameterPtr = std::unique_ptr<Parameter>;
