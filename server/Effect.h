#pragma once

#include <cstdint>
#include <unordered_set>
#include <unordered_map>

#include "ControlPacket.h"
#include "Header.h"

struct ChorusParameters
{
    float wetdrymix;
    float depth;
    float feedback;
    float frequency;
    uint32_t waveform;
    float delay;
    uint32_t phase;
};

struct CompressorParameters
{
    float gain;
    float attack;
    float release;
    float threshold;
    float ratio;
    float predelay;
};

struct DistortionParameters
{
    float gain;
    float edge;
    float posteqcenterfrequency;
    float posteqbandwidth;
    float prelowpasscutoff;
};

struct EchoParameters
{
    float wetdrymix;
    float feedback;
    float leftdelay;
    float rightdelay;
    bool pandelay;
};

struct FlangerParameters
{
    float wetdrymix;
    float depth;
    float feedback;
    float frequency;
    uint32_t waveform;
    float delay;
    uint32_t phase;
};

struct GargleParameters
{
    uint32_t ratehz;
    uint32_t waveshape;
};

struct I3dl2reverbParameters
{
    int room;
    int roomhf;
    float roomrollofffactor;
    float decaytime;
    float decayhfratio;
    int reflections;
    float reflectionsdelay;
    int reverb;
    float reverbdelay;
    float diffusion;
    float density;
    float hfreference;
};

struct ParameqParameters
{
    float center;
    float bandwidth;
    float gain;
};

struct ReverbParameters
{
    float ingain;
    float reverbmix;
    float reverbtime;
    float highfreqrtratio;
};

class Effect {

    Effect() = delete;
    Effect(const Effect&) = delete;
    Effect(Effect&&) = delete;
    Effect& operator=(const Effect&) = delete;
    Effect& operator=(Effect&&) = delete;

public:

    template<class ParametersType>
    explicit Effect(const uint32_t number, const int priority, const ParametersType& parameters)
    {
        PackWrap(this->packetCreateEffect, SV::ControlPacketType::createEffect, sizeof(SV::CreateEffectPacket) + sizeof(parameters));

        PackGetStruct(&*this->packetCreateEffect, SV::CreateEffectPacket)->effect = reinterpret_cast<uint32_t>(this);
        std::memcpy(PackGetStruct(&*this->packetCreateEffect, SV::CreateEffectPacket)->params, &parameters, sizeof(parameters));
        PackGetStruct(&*this->packetCreateEffect, SV::CreateEffectPacket)->priority = priority;
        PackGetStruct(&*this->packetCreateEffect, SV::CreateEffectPacket)->number = number;

        PackWrap(this->packetDeleteEffect, SV::ControlPacketType::deleteEffect, sizeof(SV::DeleteEffectPacket));

        PackGetStruct(&*this->packetDeleteEffect, SV::DeleteEffectPacket)->effect = reinterpret_cast<uint32_t>(this);
    }

    virtual ~Effect();

public:

    void AttachStream(class Stream* stream);
    void DetachStream(class Stream* stream);

private:

    void PlayerCallback(class Stream* stream, uint16_t player);
    void DeleteCallback(class Stream* stream);

private:

    std::unordered_set<class Stream*> attachedStreams;

    std::unordered_map<class Stream*, std::size_t> streamPlayerCallbacks;
    std::unordered_map<class Stream*, std::size_t> streamDeleteCallbacks;

    ControlPacketContainerPtr packetCreateEffect { nullptr };
    ControlPacketContainerPtr packetDeleteEffect { nullptr };

};
