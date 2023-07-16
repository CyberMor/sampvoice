/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <functional>

#include <system/types.hpp>
#include <other/version.hpp>
#include <memory/block.hpp>

#include <util/raknet.hpp>

#include <ysf/structs.h>
#include <ysf/globals.h>

#include "effect.hpp"
#include "stream.hpp"

// Constants
// --------------------------------------------

constexpr ubyte_t kRaknetPacketId = 222;

// Types
// --------------------------------------------

struct ControlPackets
{
    enum : ubyte_t
    {
        ClientInitialize,

        SpeakerActiveChannels,
        SpeakerPlayedChannels,
        SpeakerSetKey,

        StreamCreate,
        StreamSetVolume,
        StreamSetPanning,
        StreamSetDistance,
        StreamSetPosition,
        StreamSetTarget,
        StreamSetEffect,
        StreamSetIcon,
        StreamDelete,

        EffectCreate,
        EffectAppendFilter,
        EffectRemoveFilter,
        EffectDelete,

        ENUM_END
    };
};

// Packets
// --------------------------------------------

#pragma pack(push, 1)

struct ControlConnectData
{
    Version version;
    ubyte_t micro;
};

#pragma pack(pop)

// --------------------------------------------

#pragma pack(push, 1)

struct ControlClientInitialize
{
    udword_t voice_key;
    udword_t voice_host;
     uword_t voice_port;
     uword_t voice_id;
};

struct ControlSpeakerActiveChannels
{
    udword_t channels;
};

struct ControlSpeakerPlayedChannels
{
    udword_t channels;
};

struct ControlSpeakerSetKey
{
    udword_t channels;
     ubyte_t key;
};

struct ControlStreamCreate
{
     uword_t stream;
    fdword_t distance;
};

struct ControlStreamSetVolume
{
     uword_t stream;
    fdword_t volume;
};

struct ControlStreamSetPanning
{
     uword_t stream;
    fdword_t panning;
};

struct ControlStreamSetDistance
{
     uword_t stream;
    fdword_t distance;
};

struct ControlStreamSetPosition
{
     uword_t stream;
    fdword_t x;
    fdword_t y;
    fdword_t z;
};

struct ControlStreamSetTarget
{
    uword_t stream;
    uword_t target;
};

struct ControlStreamSetEffect
{
    uword_t stream;
    uword_t effect;
};

struct ControlStreamSetIcon
{
    uword_t stream;
     char   icon[];
};

struct ControlStreamDelete
{
    uword_t stream;
};

struct ControlEffectCreate
{
    uword_t effect;
};

struct ControlEffectAppendFilter
{
    uword_t effect;
    ubyte_t type;
    sword_t priority;
    ubyte_t parameters[];
};

struct ControlEffectRemoveFilter
{
    uword_t effect;
    ubyte_t type;
    sword_t priority;
};

struct ControlEffectDelete
{
    uword_t effect;
};

#pragma pack(pop)

// --------------------------------------------

struct ControlService {

    ControlService(const ControlService&) = delete;
    ControlService(ControlService&&) = delete;
    ControlService& operator=(const ControlService&) = delete;
    ControlService& operator=(ControlService&&) = delete;

private:

    ControlService() noexcept = default;
    ~ControlService() noexcept = default;

public:

    static ControlService& Instance() noexcept
    {
        static ControlService instance;
        return instance;
    }

public:

    bool Initialize(const cptr_t base) noexcept
    {
        RakNet::Instance().OnConnect = OnRaknetConnect;

        return RakNet::Instance().Initialize(base);
    }

    void Deinitialize() noexcept
    {
        RakNet::Instance().Deinitialize();

        RakNet::Instance().OnConnect = nullptr;
    }

public:

    ptr_t BeginPacket(const ubyte_t packet) noexcept
    {
        _packet_buffer[0] = kRaknetPacketId;
        _packet_buffer[1] = packet;

        _packet_length = 2;

        return _packet_buffer + 2;
    }

    void EndPacket() noexcept
    {
        switch (_packet_buffer[1])
        {
            case ControlPackets::ClientInitialize:
            {
                _packet_length += sizeof(ControlClientInitialize);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<ControlClientInitialize*>(_packet_buffer + 2)->voice_key);
                    utils::bswap(&reinterpret_cast<ControlClientInitialize*>(_packet_buffer + 2)->voice_host);
                    utils::bswap(&reinterpret_cast<ControlClientInitialize*>(_packet_buffer + 2)->voice_port);
                    utils::bswap(&reinterpret_cast<ControlClientInitialize*>(_packet_buffer + 2)->voice_id);
                }

                break;
            }
            case ControlPackets::SpeakerActiveChannels:
            {
                _packet_length += sizeof(ControlSpeakerActiveChannels);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<ControlSpeakerActiveChannels*>(_packet_buffer + 2)->channels);
                }

                break;
            }
            case ControlPackets::SpeakerPlayedChannels:
            {
                _packet_length += sizeof(ControlSpeakerPlayedChannels);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<ControlSpeakerPlayedChannels*>(_packet_buffer + 2)->channels);
                }

                break;
            }
            case ControlPackets::SpeakerSetKey:
            {
                _packet_length += sizeof(ControlSpeakerSetKey);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<ControlSpeakerSetKey*>(_packet_buffer + 2)->channels);
                }

                break;
            }
            case ControlPackets::StreamCreate:
            {
                _packet_length += sizeof(ControlStreamCreate);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<ControlStreamCreate*>(_packet_buffer + 2)->stream);
                    utils::bswap(&reinterpret_cast<ControlStreamCreate*>(_packet_buffer + 2)->distance);
                }

                break;
            }
            case ControlPackets::StreamSetVolume:
            {
                _packet_length += sizeof(ControlStreamSetVolume);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<ControlStreamSetVolume*>(_packet_buffer + 2)->stream);
                    utils::bswap(&reinterpret_cast<ControlStreamSetVolume*>(_packet_buffer + 2)->volume);
                }

                break;
            }
            case ControlPackets::StreamSetPanning:
            {
                _packet_length += sizeof(ControlStreamSetPanning);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<ControlStreamSetPanning*>(_packet_buffer + 2)->stream);
                    utils::bswap(&reinterpret_cast<ControlStreamSetPanning*>(_packet_buffer + 2)->panning);
                }

                break;
            }
            case ControlPackets::StreamSetDistance:
            {
                _packet_length += sizeof(ControlStreamSetDistance);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<ControlStreamSetDistance*>(_packet_buffer + 2)->stream);
                    utils::bswap(&reinterpret_cast<ControlStreamSetDistance*>(_packet_buffer + 2)->distance);
                }

                break;
            }
            case ControlPackets::StreamSetPosition:
            {
                _packet_length += sizeof(ControlStreamSetPosition);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<ControlStreamSetPosition*>(_packet_buffer + 2)->stream);
                    utils::bswap(&reinterpret_cast<ControlStreamSetPosition*>(_packet_buffer + 2)->x);
                    utils::bswap(&reinterpret_cast<ControlStreamSetPosition*>(_packet_buffer + 2)->y);
                    utils::bswap(&reinterpret_cast<ControlStreamSetPosition*>(_packet_buffer + 2)->z);
                }

                break;
            }
            case ControlPackets::StreamSetTarget:
            {
                _packet_length += sizeof(ControlStreamSetTarget);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<ControlStreamSetTarget*>(_packet_buffer + 2)->stream);
                    utils::bswap(&reinterpret_cast<ControlStreamSetTarget*>(_packet_buffer + 2)->target);
                }

                break;
            }
            case ControlPackets::StreamSetEffect:
            {
                _packet_length += sizeof(ControlStreamSetEffect);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<ControlStreamSetEffect*>(_packet_buffer + 2)->stream);
                    utils::bswap(&reinterpret_cast<ControlStreamSetEffect*>(_packet_buffer + 2)->effect);
                }

                break;
            }
            case ControlPackets::StreamSetIcon:
            {
                _packet_length += sizeof(ControlStreamSetIcon) +
                    std::strlen(reinterpret_cast<const ControlStreamSetIcon*>(_packet_buffer + 2)->icon) + 1;

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<ControlStreamSetIcon*>(_packet_buffer + 2)->stream);
                }

                break;
            }
            case ControlPackets::StreamDelete:
            {
                _packet_length += sizeof(ControlStreamDelete);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<ControlStreamDelete*>(_packet_buffer + 2)->stream);
                }

                break;
            }
            case ControlPackets::EffectCreate:
            {
                _packet_length += sizeof(ControlEffectCreate);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<ControlEffectCreate*>(_packet_buffer + 2)->effect);
                }

                break;
            }
            case ControlPackets::EffectAppendFilter:
            {
                _packet_length += sizeof(ControlEffectAppendFilter);

                const ubyte_t effect_type = reinterpret_cast
                    <const ControlEffectAppendFilter*>(_packet_buffer + 2)->type;

                switch (effect_type)
                {
                    case Filter::Chorus:      _packet_length += sizeof(ChorusParameters);      break;
                    case Filter::Compressor:  _packet_length += sizeof(CompressorParameters);  break;
                    case Filter::Distortion:  _packet_length += sizeof(DistortionParameters);  break;
                    case Filter::Echo:        _packet_length += sizeof(EchoParameters);        break;
                    case Filter::Flanger:     _packet_length += sizeof(FlangerParameters);     break;
                    case Filter::Gargle:      _packet_length += sizeof(GargleParameters);      break;
                    case Filter::I3dl2reverb: _packet_length += sizeof(I3dl2reverbParameters); break;
                    case Filter::Parameq:     _packet_length += sizeof(ParameqParameters);     break;
                    case Filter::Reverb:      _packet_length += sizeof(ReverbParameters);      break;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<ControlEffectAppendFilter*>(_packet_buffer + 2)->effect);
                    utils::bswap(&reinterpret_cast<ControlEffectAppendFilter*>(_packet_buffer + 2)->priority);

                    const ptr_t parameters = reinterpret_cast<ControlEffectAppendFilter*>(_packet_buffer + 2)->parameters;

                    switch (effect_type)
                    {
                        case Filter::Chorus:
                        {
                            utils::bswap(&static_cast<ChorusParameters*>(parameters)->wetdrymix);
                            utils::bswap(&static_cast<ChorusParameters*>(parameters)->depth);
                            utils::bswap(&static_cast<ChorusParameters*>(parameters)->feedback);
                            utils::bswap(&static_cast<ChorusParameters*>(parameters)->frequency);
                            utils::bswap(&static_cast<ChorusParameters*>(parameters)->waveform);
                            utils::bswap(&static_cast<ChorusParameters*>(parameters)->delay);
                            utils::bswap(&static_cast<ChorusParameters*>(parameters)->phase);

                            break;
                        }
                        case Filter::Compressor:
                        {
                            utils::bswap(&static_cast<CompressorParameters*>(parameters)->gain);
                            utils::bswap(&static_cast<CompressorParameters*>(parameters)->attack);
                            utils::bswap(&static_cast<CompressorParameters*>(parameters)->release);
                            utils::bswap(&static_cast<CompressorParameters*>(parameters)->threshold);
                            utils::bswap(&static_cast<CompressorParameters*>(parameters)->ratio);
                            utils::bswap(&static_cast<CompressorParameters*>(parameters)->predelay);

                            break;
                        }
                        case Filter::Distortion:
                        {
                            utils::bswap(&static_cast<DistortionParameters*>(parameters)->gain);
                            utils::bswap(&static_cast<DistortionParameters*>(parameters)->edge);
                            utils::bswap(&static_cast<DistortionParameters*>(parameters)->posteqcenterfrequency);
                            utils::bswap(&static_cast<DistortionParameters*>(parameters)->posteqbandwidth);
                            utils::bswap(&static_cast<DistortionParameters*>(parameters)->prelowpasscutoff);

                            break;
                        }
                        case Filter::Echo:
                        {
                            utils::bswap(&static_cast<EchoParameters*>(parameters)->wetdrymix);
                            utils::bswap(&static_cast<EchoParameters*>(parameters)->feedback);
                            utils::bswap(&static_cast<EchoParameters*>(parameters)->leftdelay);
                            utils::bswap(&static_cast<EchoParameters*>(parameters)->rightdelay);

                            break;
                        }
                        case Filter::Flanger:
                        {
                            utils::bswap(&static_cast<FlangerParameters*>(parameters)->wetdrymix);
                            utils::bswap(&static_cast<FlangerParameters*>(parameters)->depth);
                            utils::bswap(&static_cast<FlangerParameters*>(parameters)->feedback);
                            utils::bswap(&static_cast<FlangerParameters*>(parameters)->frequency);
                            utils::bswap(&static_cast<FlangerParameters*>(parameters)->waveform);
                            utils::bswap(&static_cast<FlangerParameters*>(parameters)->delay);
                            utils::bswap(&static_cast<FlangerParameters*>(parameters)->phase);

                            break;
                        }
                        case Filter::Gargle:
                        {
                            utils::bswap(&static_cast<GargleParameters*>(parameters)->ratehz);
                            utils::bswap(&static_cast<GargleParameters*>(parameters)->waveshape);

                            break;
                        }
                        case Filter::I3dl2reverb:
                        {
                            utils::bswap(&static_cast<I3dl2reverbParameters*>(parameters)->room);
                            utils::bswap(&static_cast<I3dl2reverbParameters*>(parameters)->roomhf);
                            utils::bswap(&static_cast<I3dl2reverbParameters*>(parameters)->roomrollofffactor);
                            utils::bswap(&static_cast<I3dl2reverbParameters*>(parameters)->decaytime);
                            utils::bswap(&static_cast<I3dl2reverbParameters*>(parameters)->decayhfratio);
                            utils::bswap(&static_cast<I3dl2reverbParameters*>(parameters)->reflections);
                            utils::bswap(&static_cast<I3dl2reverbParameters*>(parameters)->reflectionsdelay);
                            utils::bswap(&static_cast<I3dl2reverbParameters*>(parameters)->reverb);
                            utils::bswap(&static_cast<I3dl2reverbParameters*>(parameters)->reverbdelay);
                            utils::bswap(&static_cast<I3dl2reverbParameters*>(parameters)->diffusion);
                            utils::bswap(&static_cast<I3dl2reverbParameters*>(parameters)->density);
                            utils::bswap(&static_cast<I3dl2reverbParameters*>(parameters)->hfreference);

                            break;
                        }
                        case Filter::Parameq:
                        {
                            utils::bswap(&static_cast<ParameqParameters*>(parameters)->center);
                            utils::bswap(&static_cast<ParameqParameters*>(parameters)->bandwidth);
                            utils::bswap(&static_cast<ParameqParameters*>(parameters)->gain);

                            break;
                        }
                        case Filter::Reverb:
                        {
                            utils::bswap(&static_cast<ReverbParameters*>(parameters)->ingain);
                            utils::bswap(&static_cast<ReverbParameters*>(parameters)->reverbmix);
                            utils::bswap(&static_cast<ReverbParameters*>(parameters)->reverbtime);
                            utils::bswap(&static_cast<ReverbParameters*>(parameters)->highfreqrtratio);

                            break;
                        }
                    }
                }

                break;
            }
            case ControlPackets::EffectRemoveFilter:
            {
                _packet_length += sizeof(ControlEffectRemoveFilter);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<ControlEffectRemoveFilter*>(_packet_buffer + 2)->effect);
                    utils::bswap(&reinterpret_cast<ControlEffectRemoveFilter*>(_packet_buffer + 2)->priority);
                }

                break;
            }
            case ControlPackets::EffectDelete:
            {
                _packet_length += sizeof(ControlEffectDelete);

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&reinterpret_cast<ControlEffectDelete*>(_packet_buffer + 2)->effect);
                }

                break;
            }
        }
    }

    bool SendPacket(const uword_t player) noexcept
    {
        return RakNet::Instance().SendPacket(player, _packet_buffer, _packet_length);
    }

private:

    static void OnRaknetConnect(const uword_t player, const ptr_t data, const ubyte_t size) noexcept
    {
        if (data != nullptr && size == sizeof(ControlConnectData))
        {
            if constexpr (HostEndian != NetEndian)
            {
                utils::bswap(&static_cast<ControlConnectData*>(data)->version);
            }

            Instance().OnConnect(player, *static_cast<const ControlConnectData*>(data));
        }
    }

private:

    ubyte_t _packet_buffer[1024];
    uword_t _packet_length = 0;

public:

    std::function<void(uword_t, const ControlConnectData&)> OnConnect;

    std::function<void(uword_t)>& OnDisconnect = RakNet::Instance().OnDisconnect;

};
