/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <system/types.hpp>
#include <memory/structures/list.hpp>
#include <memory/block.hpp>

// Parameters
// ----------------------------------------------

#pragma pack(push, 1)

struct ChorusParameters
{
    fdword_t wetdrymix;
    fdword_t depth;
    fdword_t feedback;
    fdword_t frequency;
    udword_t waveform;
    fdword_t delay;
    udword_t phase;
};

struct CompressorParameters
{
    fdword_t gain;
    fdword_t attack;
    fdword_t release;
    fdword_t threshold;
    fdword_t ratio;
    fdword_t predelay;
};

struct DistortionParameters
{
    fdword_t gain;
    fdword_t edge;
    fdword_t posteqcenterfrequency;
    fdword_t posteqbandwidth;
    fdword_t prelowpasscutoff;
};

struct EchoParameters
{
    fdword_t wetdrymix;
    fdword_t feedback;
    fdword_t leftdelay;
    fdword_t rightdelay;
     ubyte_t pandelay;
};

struct FlangerParameters
{
    fdword_t wetdrymix;
    fdword_t depth;
    fdword_t feedback;
    fdword_t frequency;
    udword_t waveform;
    fdword_t delay;
    udword_t phase;
};

struct GargleParameters
{
    udword_t ratehz;
    udword_t waveshape;
};

struct I3dl2reverbParameters
{
    sdword_t room;
    sdword_t roomhf;
    fdword_t roomrollofffactor;
    fdword_t decaytime;
    fdword_t decayhfratio;
    sdword_t reflections;
    fdword_t reflectionsdelay;
    sdword_t reverb;
    fdword_t reverbdelay;
    fdword_t diffusion;
    fdword_t density;
    fdword_t hfreference;
};

struct ParameqParameters
{
    fdword_t center;
    fdword_t bandwidth;
    fdword_t gain;
};

struct ReverbParameters
{
    fdword_t ingain;
    fdword_t reverbmix;
    fdword_t reverbtime;
    fdword_t highfreqrtratio;
};

#pragma pack(pop)

// Constants
// ----------------------------------------------

constexpr size_t kParametersLimit = std::max
({
    sizeof(ChorusParameters),
    sizeof(CompressorParameters),
    sizeof(DistortionParameters),
    sizeof(EchoParameters),
    sizeof(FlangerParameters),
    sizeof(GargleParameters),
    sizeof(I3dl2reverbParameters),
    sizeof(ParameqParameters),
    sizeof(ReverbParameters)
});

// Filter
// ----------------------------------------------

struct Filter {

    enum : ubyte_t
    {
        Chorus,
        Compressor,
        Distortion,
        Echo,
        Flanger,
        Gargle,
        I3dl2reverb,
        Parameq,
        Reverb,

        ENUM_END
    };

public:

    Filter() = delete;
    ~Filter() noexcept = default;
    Filter(const Filter&) = delete;
    Filter(Filter&&) = delete;
    Filter& operator=(const Filter&) = delete;
    Filter& operator=(Filter&&) = delete;

public:

    Filter(const ubyte_t type, const sword_t priority, DataBlock<ubyte_t>&& parameters) noexcept
        : _type       { type }
        , _priority   { priority }
        , _parameters { std::move(parameters) }
    {}

public:

    ubyte_t Type() const noexcept
    {
        return _type;
    }

    sword_t Priority() const noexcept
    {
        return _priority;
    }

    const DataView<ubyte_t>& Parameters() const noexcept
    {
        return _parameters.Area();
    }

private:

    const ubyte_t _type;
    const sword_t _priority;

    const DataBlock<ubyte_t> _parameters;

};

// Effect
// ----------------------------------------------

struct Effect {

    Effect() noexcept = default;
    ~Effect() noexcept = default;
    Effect(const Effect&) = delete;
    Effect(Effect&&) = delete;
    Effect& operator=(const Effect&) = delete;
    Effect& operator=(Effect&&) = delete;

public:

    bool AppendFilter(const ubyte_t type, const sword_t priority, DataBlock<ubyte_t>&& parameters) noexcept
    {
        const auto filter = filters.Construct(type, priority, std::move(parameters));
        return filter != nullptr && (filters.PushBack(filter), true);
    }

    void RemoveFilter(const ubyte_t type, const sword_t priority) noexcept
    {
        ListRemove(filter, filters, filter->Type() == type && filter->Priority() == priority);
    }

public:

    List<Filter> filters;

};
