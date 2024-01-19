/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <functional>

#include <other/version.hpp>

#include <util/logger.hpp>
#include <util/raknet.hpp>

#include "effect.hpp"

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

        UpdateKey,

        ENUM_END
    };
};

// Packets
// --------------------------------------------

#pragma pack(push, 1)

struct ConnectData
{
    Version version;
    ubyte_t micro;
};

#pragma pack(pop)

// --------------------------------------------

#pragma pack(push, 1)

struct ClientInitialize
{
    udword_t voice_key;
    udword_t voice_host;
     uword_t voice_port;
     uword_t voice_id;
};

struct SpeakerActiveChannels
{
    udword_t channels;
};

struct SpeakerPlayedChannels
{
    udword_t channels;
};

struct SpeakerSetKey
{
    udword_t channels;
     ubyte_t key;
};

struct StreamCreate
{
     uword_t stream;
    fdword_t distance;
};

struct StreamSetVolume
{
     uword_t stream;
    fdword_t volume;
};

struct StreamSetPanning
{
     uword_t stream;
    fdword_t panning;
};

struct StreamSetDistance
{
     uword_t stream;
    fdword_t distance;
};

struct StreamSetPosition
{
     uword_t stream;
    fdword_t x;
    fdword_t y;
    fdword_t z;
};

struct StreamSetTarget
{
    uword_t stream;
    uword_t target;
};

struct StreamSetEffect
{
    uword_t stream;
    uword_t effect;
};

struct StreamSetIcon
{
    uword_t stream;
     char   icon[];
};

struct StreamDelete
{
    uword_t stream;
};

struct EffectCreate
{
    uword_t effect;
};

struct EffectAppendFilter
{
    uword_t effect;
    ubyte_t type;
    sword_t priority;
    ubyte_t parameters[];
};

struct EffectRemoveFilter
{
    uword_t effect;
    ubyte_t type;
    sword_t priority;
};

struct EffectDelete
{
    uword_t effect;
};

struct UpdateKey
{
    udword_t key;
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

    bool Initialize() noexcept
    {
        RakNet::Instance().OnHandshake = OnRaknetHandshake;
        RakNet::Instance().OnPacket    = OnRaknetPacket;

        return RakNet::Instance().Initialize();
    }

    void Deinitialize() noexcept
    {
        RakNet::Instance().Deinitialize();

        RakNet::Instance().OnHandshake = nullptr;
        RakNet::Instance().OnPacket    = nullptr;
    }

private:

    static ubyte_t OnRaknetHandshake(const ptr_t buffer) noexcept
    {
        Instance().OnHandshake(*static_cast<ConnectData*>(buffer));

        if constexpr (HostEndian != NetEndian)
        {
            utils::bswap(&static_cast<ConnectData*>(buffer)->version);
        }

        return sizeof(ConnectData);
    }

    static bool OnRaknetPacket(const uword_t, const ptr_t data, const uint_t size) noexcept
    {
        if (size == 0) return false;

        if (static_cast<cadr_t>(data)[0] != kRaknetPacketId)
            return false;

        if (size == 1) return false;

        switch (static_cast<cadr_t>(data)[1])
        {
            case ControlPackets::ClientInitialize:
            {
                auto& content = *reinterpret_cast<ClientInitialize*>(static_cast<adr_t>(data) + 2);

                if (size != 2 + sizeof(content))
                {
                    Logger::Instance().LogToFile("[err:control:ClientInitialize] invalid packet size(%u)", size);
                    return false;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.voice_key);
                    utils::bswap(&content.voice_host);
                    utils::bswap(&content.voice_port);
                    utils::bswap(&content.voice_id);
                }

                break;
            }
            case ControlPackets::SpeakerActiveChannels:
            {
                auto& content = *reinterpret_cast<SpeakerActiveChannels*>(static_cast<adr_t>(data) + 2);

                if (size != 2 + sizeof(content))
                {
                    Logger::Instance().LogToFile("[err:control:SpeakerActiveChannels] invalid packet size(%u)", size);
                    return false;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.channels);
                }

                break;
            }
            case ControlPackets::SpeakerPlayedChannels:
            {
                auto& content = *reinterpret_cast<SpeakerPlayedChannels*>(static_cast<adr_t>(data) + 2);

                if (size != 2 + sizeof(content))
                {
                    Logger::Instance().LogToFile("[err:control:SpeakerPlayedChannels] invalid packet size(%u)", size);
                    return false;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.channels);
                }

                break;
            }
            case ControlPackets::SpeakerSetKey:
            {
                auto& content = *reinterpret_cast<SpeakerSetKey*>(static_cast<adr_t>(data) + 2);

                if (size != 2 + sizeof(content))
                {
                    Logger::Instance().LogToFile("[err:control:SpeakerSetKey] invalid packet size(%u)", size);
                    return false;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.channels);
                }

                break;
            }
            case ControlPackets::StreamCreate:
            {
                auto& content = *reinterpret_cast<StreamCreate*>(static_cast<adr_t>(data) + 2);

                if (size != 2 + sizeof(content))
                {
                    Logger::Instance().LogToFile("[err:control:StreamCreate] invalid packet size(%u)", size);
                    return false;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.stream);
                    utils::bswap(&content.distance);
                }

                break;
            }
            case ControlPackets::StreamSetVolume:
            {
                auto& content = *reinterpret_cast<StreamSetVolume*>(static_cast<adr_t>(data) + 2);

                if (size != 2 + sizeof(content))
                {
                    Logger::Instance().LogToFile("[err:control:StreamSetVolume] invalid packet size(%u)", size);
                    return false;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.stream);
                    utils::bswap(&content.volume);
                }

                break;
            }
            case ControlPackets::StreamSetPanning:
            {
                auto& content = *reinterpret_cast<StreamSetPanning*>(static_cast<adr_t>(data) + 2);

                if (size != 2 + sizeof(content))
                {
                    Logger::Instance().LogToFile("[err:control:StreamSetPanning] invalid packet size(%u)", size);
                    return false;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.stream);
                    utils::bswap(&content.panning);
                }

                break;
            }
            case ControlPackets::StreamSetDistance:
            {
                auto& content = *reinterpret_cast<StreamSetDistance*>(static_cast<adr_t>(data) + 2);

                if (size != 2 + sizeof(content))
                {
                    Logger::Instance().LogToFile("[err:control:StreamSetDistance] invalid packet size(%u)", size);
                    return false;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.stream);
                    utils::bswap(&content.distance);
                }

                break;
            }
            case ControlPackets::StreamSetPosition:
            {
                auto& content = *reinterpret_cast<StreamSetPosition*>(static_cast<adr_t>(data) + 2);

                if (size != 2 + sizeof(content))
                {
                    Logger::Instance().LogToFile("[err:control:StreamSetPosition] invalid packet size(%u)", size);
                    return false;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.stream);
                    utils::bswap(&content.x);
                    utils::bswap(&content.y);
                    utils::bswap(&content.z);
                }

                break;
            }
            case ControlPackets::StreamSetTarget:
            {
                auto& content = *reinterpret_cast<StreamSetTarget*>(static_cast<adr_t>(data) + 2);

                if (size != 2 + sizeof(content))
                {
                    Logger::Instance().LogToFile("[err:control:StreamSetTarget] invalid packet size(%u)", size);
                    return false;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.stream);
                    utils::bswap(&content.target);
                }

                break;
            }
            case ControlPackets::StreamSetEffect:
            {
                auto& content = *reinterpret_cast<StreamSetEffect*>(static_cast<adr_t>(data) + 2);

                if (size != 2 + sizeof(content))
                {
                    Logger::Instance().LogToFile("[err:control:StreamSetEffect] invalid packet size(%u)", size);
                    return false;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.stream);
                    utils::bswap(&content.effect);
                }

                break;
            }
            case ControlPackets::StreamSetIcon:
            {
                auto& content = *reinterpret_cast<StreamSetIcon*>(static_cast<adr_t>(data) + 2);

                if (size <= 2 + sizeof(content))
                {
                    Logger::Instance().LogToFile("[err:control:StreamSetIcon] invalid packet size(%u)", size);
                    return false;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.stream);
                }

                break;
            }
            case ControlPackets::StreamDelete:
            {
                auto& content = *reinterpret_cast<StreamDelete*>(static_cast<adr_t>(data) + 2);

                if (size != 2 + sizeof(content))
                {
                    Logger::Instance().LogToFile("[err:control:StreamDelete] invalid packet size(%u)", size);
                    return false;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.stream);
                }

                break;
            }
            case ControlPackets::EffectCreate:
            {
                auto& content = *reinterpret_cast<EffectCreate*>(static_cast<adr_t>(data) + 2);

                if (size != 2 + sizeof(content))
                {
                    Logger::Instance().LogToFile("[err:control:EffectCreate] invalid packet size(%u)", size);
                    return false;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.effect);
                }

                break;
            }
            case ControlPackets::EffectAppendFilter:
            {
                auto& content = *reinterpret_cast<EffectAppendFilter*>(static_cast<adr_t>(data) + 2);

                if (size < 2 + sizeof(content))
                {
                    Logger::Instance().LogToFile("[err:control:EffectAppendFilter] invalid packet size(%u)", size);
                    return false;
                }

                switch (content.type)
                {
                    case Filter::Chorus:
                    {
                        if (size != 2 + sizeof(content) + sizeof(ChorusParameters))
                        {
                            Logger::Instance().LogToFile("[err:control:EffectAppendFilter:Chorus] invalid packet size(%u)", size);
                            return false;
                        }

                        if constexpr (HostEndian != NetEndian)
                        {
                            utils::bswap(&reinterpret_cast<ChorusParameters*>(content.parameters)->wetdrymix);
                            utils::bswap(&reinterpret_cast<ChorusParameters*>(content.parameters)->depth);
                            utils::bswap(&reinterpret_cast<ChorusParameters*>(content.parameters)->feedback);
                            utils::bswap(&reinterpret_cast<ChorusParameters*>(content.parameters)->frequency);
                            utils::bswap(&reinterpret_cast<ChorusParameters*>(content.parameters)->waveform);
                            utils::bswap(&reinterpret_cast<ChorusParameters*>(content.parameters)->delay);
                            utils::bswap(&reinterpret_cast<ChorusParameters*>(content.parameters)->phase);
                        }

                        break;
                    }
                    case Filter::Compressor:
                    {
                        if (size != 2 + sizeof(content) + sizeof(CompressorParameters))
                        {
                            Logger::Instance().LogToFile("[err:control:EffectAppendFilter:Compressor] invalid packet size(%u)", size);
                            return false;
                        }

                        if constexpr (HostEndian != NetEndian)
                        {
                            utils::bswap(&reinterpret_cast<CompressorParameters*>(content.parameters)->gain);
                            utils::bswap(&reinterpret_cast<CompressorParameters*>(content.parameters)->attack);
                            utils::bswap(&reinterpret_cast<CompressorParameters*>(content.parameters)->release);
                            utils::bswap(&reinterpret_cast<CompressorParameters*>(content.parameters)->threshold);
                            utils::bswap(&reinterpret_cast<CompressorParameters*>(content.parameters)->ratio);
                            utils::bswap(&reinterpret_cast<CompressorParameters*>(content.parameters)->predelay);
                        }

                        break;
                    }
                    case Filter::Distortion:
                    {
                        if (size != 2 + sizeof(content) + sizeof(DistortionParameters))
                        {
                            Logger::Instance().LogToFile("[err:control:EffectAppendFilter:Distortion] invalid packet size(%u)", size);
                            return false;
                        }

                        if constexpr (HostEndian != NetEndian)
                        {
                            utils::bswap(&reinterpret_cast<DistortionParameters*>(content.parameters)->gain);
                            utils::bswap(&reinterpret_cast<DistortionParameters*>(content.parameters)->edge);
                            utils::bswap(&reinterpret_cast<DistortionParameters*>(content.parameters)->posteqcenterfrequency);
                            utils::bswap(&reinterpret_cast<DistortionParameters*>(content.parameters)->posteqbandwidth);
                            utils::bswap(&reinterpret_cast<DistortionParameters*>(content.parameters)->prelowpasscutoff);
                        }

                        break;
                    }
                    case Filter::Echo:
                    {
                        if (size != 2 + sizeof(content) + sizeof(EchoParameters))
                        {
                            Logger::Instance().LogToFile("[err:control:EffectAppendFilter:Echo] invalid packet size(%u)", size);
                            return false;
                        }

                        if constexpr (HostEndian != NetEndian)
                        {
                            utils::bswap(&reinterpret_cast<EchoParameters*>(content.parameters)->wetdrymix);
                            utils::bswap(&reinterpret_cast<EchoParameters*>(content.parameters)->feedback);
                            utils::bswap(&reinterpret_cast<EchoParameters*>(content.parameters)->leftdelay);
                            utils::bswap(&reinterpret_cast<EchoParameters*>(content.parameters)->rightdelay);
                        }

                        break;
                    }
                    case Filter::Flanger:
                    {
                        if (size != 2 + sizeof(content) + sizeof(FlangerParameters))
                        {
                            Logger::Instance().LogToFile("[err:control:EffectAppendFilter:Flanger] invalid packet size(%u)", size);
                            return false;
                        }

                        if constexpr (HostEndian != NetEndian)
                        {
                            utils::bswap(&reinterpret_cast<FlangerParameters*>(content.parameters)->wetdrymix);
                            utils::bswap(&reinterpret_cast<FlangerParameters*>(content.parameters)->depth);
                            utils::bswap(&reinterpret_cast<FlangerParameters*>(content.parameters)->feedback);
                            utils::bswap(&reinterpret_cast<FlangerParameters*>(content.parameters)->frequency);
                            utils::bswap(&reinterpret_cast<FlangerParameters*>(content.parameters)->waveform);
                            utils::bswap(&reinterpret_cast<FlangerParameters*>(content.parameters)->delay);
                            utils::bswap(&reinterpret_cast<FlangerParameters*>(content.parameters)->phase);
                        }

                        break;
                    }
                    case Filter::Gargle:
                    {
                        if (size != 2 + sizeof(content) + sizeof(GargleParameters))
                        {
                            Logger::Instance().LogToFile("[err:control:EffectAppendFilter:Gargle] invalid packet size(%u)", size);
                            return false;
                        }

                        if constexpr (HostEndian != NetEndian)
                        {
                            utils::bswap(&reinterpret_cast<GargleParameters*>(content.parameters)->ratehz);
                            utils::bswap(&reinterpret_cast<GargleParameters*>(content.parameters)->waveshape);
                        }

                        break;
                    }
                    case Filter::I3dl2reverb:
                    {
                        if (size != 2 + sizeof(content) + sizeof(I3dl2reverbParameters))
                        {
                            Logger::Instance().LogToFile("[err:control:EffectAppendFilter:I3dl2reverb] invalid packet size(%u)", size);
                            return false;
                        }

                        if constexpr (HostEndian != NetEndian)
                        {
                            utils::bswap(&reinterpret_cast<I3dl2reverbParameters*>(content.parameters)->room);
                            utils::bswap(&reinterpret_cast<I3dl2reverbParameters*>(content.parameters)->roomhf);
                            utils::bswap(&reinterpret_cast<I3dl2reverbParameters*>(content.parameters)->roomrollofffactor);
                            utils::bswap(&reinterpret_cast<I3dl2reverbParameters*>(content.parameters)->decaytime);
                            utils::bswap(&reinterpret_cast<I3dl2reverbParameters*>(content.parameters)->decayhfratio);
                            utils::bswap(&reinterpret_cast<I3dl2reverbParameters*>(content.parameters)->reflections);
                            utils::bswap(&reinterpret_cast<I3dl2reverbParameters*>(content.parameters)->reflectionsdelay);
                            utils::bswap(&reinterpret_cast<I3dl2reverbParameters*>(content.parameters)->reverb);
                            utils::bswap(&reinterpret_cast<I3dl2reverbParameters*>(content.parameters)->reverbdelay);
                            utils::bswap(&reinterpret_cast<I3dl2reverbParameters*>(content.parameters)->diffusion);
                            utils::bswap(&reinterpret_cast<I3dl2reverbParameters*>(content.parameters)->density);
                            utils::bswap(&reinterpret_cast<I3dl2reverbParameters*>(content.parameters)->hfreference);
                        }

                        break;
                    }
                    case Filter::Parameq:
                    {
                        if (size != 2 + sizeof(content) + sizeof(ParameqParameters))
                        {
                            Logger::Instance().LogToFile("[err:control:EffectAppendFilter:Parameq] invalid packet size(%u)", size);
                            return false;
                        }

                        if constexpr (HostEndian != NetEndian)
                        {
                            utils::bswap(&reinterpret_cast<ParameqParameters*>(content.parameters)->center);
                            utils::bswap(&reinterpret_cast<ParameqParameters*>(content.parameters)->bandwidth);
                            utils::bswap(&reinterpret_cast<ParameqParameters*>(content.parameters)->gain);
                        }

                        break;
                    }
                    case Filter::Reverb:
                    {
                        if (size != 2 + sizeof(content) + sizeof(ReverbParameters))
                        {
                            Logger::Instance().LogToFile("[err:control:EffectAppendFilter:Reverb] invalid packet size(%u)", size);
                            return false;
                        }

                        if constexpr (HostEndian != NetEndian)
                        {
                            utils::bswap(&reinterpret_cast<ReverbParameters*>(content.parameters)->ingain);
                            utils::bswap(&reinterpret_cast<ReverbParameters*>(content.parameters)->reverbmix);
                            utils::bswap(&reinterpret_cast<ReverbParameters*>(content.parameters)->reverbtime);
                            utils::bswap(&reinterpret_cast<ReverbParameters*>(content.parameters)->highfreqrtratio);
                        }

                        break;
                    }
                    default:
                    {
                        Logger::Instance().LogToFile("[err:control:EffectAppendFilter] unknown effect type(%hhu)", content.type);
                        return false;
                    }
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.effect);
                    utils::bswap(&content.priority);
                }

                break;
            }
            case ControlPackets::EffectRemoveFilter:
            {
                auto& content = *reinterpret_cast<EffectRemoveFilter*>(static_cast<adr_t>(data) + 2);

                if (size != 2 + sizeof(content))
                {
                    Logger::Instance().LogToFile("[err:control:EffectRemoveFilter] invalid packet size(%u)", size);
                    return false;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.effect);
                    utils::bswap(&content.priority);
                }

                break;
            }
            case ControlPackets::EffectDelete:
            {
                auto& content = *reinterpret_cast<EffectDelete*>(static_cast<adr_t>(data) + 2);

                if (size != 2 + sizeof(content))
                {
                    Logger::Instance().LogToFile("[err:control:EffectDelete] invalid packet size(%u)", size);
                    return false;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.effect);
                }

                break;
            }
            case ControlPackets::UpdateKey:
            {
                auto& content = *reinterpret_cast<UpdateKey*>(static_cast<adr_t>(data) + 2);

                if (size != 2 + sizeof(content))
                {
                    Logger::Instance().LogToFile("[err:control:UpdateKey] invalid packet size(%u)", size);
                    return false;
                }

                if constexpr (HostEndian != NetEndian)
                {
                    utils::bswap(&content.key);
                }

                break;
            }
            default:
            {
                Logger::Instance().LogToFile("[err:control:packet] unknown packet type(%hhu)", static_cast<cadr_t>(data)[1]);
                return false;
            }
        }

        Instance().OnPacket(static_cast<cadr_t>(data)[1], static_cast<cadr_t>(data) + 2);

        return true;
    }

public:

    std::function<void(cstr_t, uword_t)>& OnConnect    = RakNet::Instance().OnConnect;
    std::function<void()>&                OnDisconnect = RakNet::Instance().OnDisconnect;

public:

    std::function<void(ConnectData&)>    OnHandshake;
    std::function<void(ubyte_t, cptr_t)> OnPacket;

};
