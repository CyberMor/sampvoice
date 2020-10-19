#pragma once

#include <memory>
#include <vector>
#include <map>

#include <audio/bass.h>

#include "Channel.h"

class Effect {

    Effect() = delete;
    Effect(const Effect&) = delete;
    Effect(Effect&&) = delete;
    Effect& operator=(const Effect&) = delete;
    Effect& operator=(Effect&&) = delete;

public:

    explicit Effect(DWORD type, int priority,
                    const void* paramPtr, DWORD paramSize);

    ~Effect() noexcept;

public:

    void Apply(const Channel& channel);

private:

    const DWORD type;
    const int priority;
    std::vector<BYTE> params;

    std::map<HSTREAM, HFX> fxHandles;

};

using EffectPtr = std::unique_ptr<Effect>;
#define MakeEffect std::make_unique<Effect>
