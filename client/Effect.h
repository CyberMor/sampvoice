#pragma once

#include <vector>
#include <map>

#include <audio/bass.h>

#include "Channel.h"

struct Effect {

    Effect() = delete;
    ~Effect() noexcept;
    Effect(const Effect&) = delete;
    Effect(Effect&&) = delete;
    Effect& operator=(const Effect&) = delete;
    Effect& operator=(Effect&&) = delete;

public:

    Effect(DWORD type, int priority, const void* param_data, DWORD param_size);

public:

    void Apply(const Channel& channel);

private:

    DWORD                  _type;
    int                    _priority;
    std::vector<BYTE>      _params;

    std::map<HSTREAM, HFX> _fx_handles;

};
