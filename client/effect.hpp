/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>
#include <vector>

#include <audio/bass.h>

#include <system/types.hpp>
#include <memory/structures/list.hpp>
#include <memory/view.hpp>

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
        Reverb
    };

public:

    Filter() = delete;
    ~Filter() noexcept
    {
        for (const auto& handle : _handles)
        {
            BASS_ChannelRemoveFX(handle.first, handle.second);
        }
    }

    Filter(const Filter&) = delete;
    Filter(Filter&&) = delete;
    Filter& operator=(const Filter&) = delete;
    Filter& operator=(Filter&&) = delete;

public:

    Filter(const ubyte_t type, const sword_t priority,
        Block<const ubyte_t>&& parameters) noexcept
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

    const View<const ubyte_t>& Parameters() const noexcept
    {
        return _parameters.Area();
    }

public:

    bool Attach(const HSTREAM channel) noexcept
    {
        const HFX handle = BASS_ChannelSetFX(channel, _type, _priority);
        if (handle == NULL) return false;

        if (BASS_FXSetParameters(handle, _parameters.Data()) == FALSE)
        {
            BASS_ChannelRemoveFX(channel, handle);
            return false;
        }

        _handles.emplace_back(channel, handle);

        return true;
    }

    void Detach(const HSTREAM channel) noexcept
    {
        for (auto iterator = _handles.begin(); iterator != _handles.end();)
        {
            if (iterator->first == channel)
            {
                BASS_ChannelRemoveFX(iterator->first, iterator->second);
                iterator = _handles.erase(iterator);
            }
            else ++iterator;
        }
    }

private:

    ubyte_t _type;
    sword_t _priority;

    Block<const ubyte_t> _parameters;

    std::vector<std::pair<HSTREAM, HFX>>
        _handles;

};

struct Effect {

    Effect() noexcept = default;
    ~Effect() noexcept = default;
    Effect(const Effect&) = delete;
    Effect(Effect&&) = delete;
    Effect& operator=(const Effect&) = delete;
    Effect& operator=(Effect&&) = delete;

public:

    Filter* AppendFilter(const ubyte_t type, const sword_t priority, Block<const ubyte_t>&& parameters) noexcept
    {
        const auto filter = _filters.Construct(type, priority, std::move(parameters));
        if (filter != nullptr) _filters.PushBack(filter);
        return filter;
    }

    void RemoveFilter(const ubyte_t type, const sword_t priority) noexcept
    {
        ListRemove(filter, _filters, filter->Type() == type && filter->Priority() == priority);
    }

public:

    bool Attach(const HSTREAM channel) noexcept
    {
        bool result = true;
        ForwardForEach(filter, _filters) result &= filter->Attach(channel);
        return result;
    }

    void Detach(const HSTREAM channel) noexcept
    {
        ForwardForEach(filter, _filters) filter->Detach(channel);
    }

private:

    List<Filter> _filters;

};
