/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include <Windows.h>

#include <system/types.hpp>
#include <memory/structures/array.hpp>
#include <memory/structures/pool.hpp>
#include <other/utils.hpp>

#include <game/common.h>

#include <util/addresses.hpp>
#include <util/game_utils.hpp>
#include <util/imgui_utils.hpp>
#include <util/logger.hpp>
#include <util/raknet.hpp>
#include <util/render.hpp>
#include <util/samp.hpp>

#include "storage.hpp"
#include "localization.hpp"
#include "plugin_config.hpp"
#include "micro_icon.hpp"
#include "plugin_menu.hpp"
#include "speaker_list.hpp"
#include "black_list.hpp"
#include "listener.hpp"
#include "speaker.hpp"

#include "control.hpp"
#include "voice.hpp"

#include "source.hpp"
#include "stream.hpp"
#include "effect.hpp"

// Plugin
// ----------------------------------------------------------------

static bool gGameStatus = false;

// ----------------------------------------------------------------

static Clock gSourcesClock;

static Array<Source, kMaxPlayers> gSources;

static Pool<Stream, kMaxStreams> gStreams;
static Pool<Effect, kMaxEffects> gEffects;

// ----------------------------------------------------------------

static Array<ubyte_t, Bits<udword_t>> gKeys = Zero<ubyte_t>;

// ----------------------------------------------------------------

static udword_t gActiveChannels = 0;
static udword_t gPlayedChannels = 0;
static udword_t gHoldedChannels = 0;

// Main Loop
// ----------------------------------------------------------------

static void OnGamePause() noexcept
{
    gHoldedChannels = 0;

    Speaker::Instance().StopRecording();
    MicroIcon::Instance().SetPassiveIcon();

    VoiceService::Instance().SetChannels(0);

    if (VoiceService::Instance().IsInitialized())
        VoiceService::Instance().Close();

    for (size_t player = 0; player != kMaxPlayers; ++player)
    {
        gSources[player].Reset();
    }

    for (size_t stream = 0; stream != kMaxStreams; ++stream)
    {
        if (gStreams.Acquire<0>(stream))
        {
            gStreams[stream].Reset();
        }
    }
}

static void OnGameResume(const Time moment = Clock::Now()) noexcept
{
    gSourcesClock.Restart(moment);

    if (VoiceService::Instance().IsInitialized())
        VoiceService::Instance().Open();
}

// ----------------------------------------------------------------

static void MainLoop(const Time moment = Clock::Now()) noexcept
{
    if (Samp::Instance().IsLoaded())
    {
        if (gGameStatus == true && game_utils::is_game_active() == false)
        {
            Logger::Instance().LogToFile("[sv:dbg:plugin] : game paused");
            OnGamePause();
            gGameStatus = false;
        }
        if (gGameStatus == false && game_utils::is_game_active() == true)
        {
            Logger::Instance().LogToFile("[sv:dbg:plugin] : game resumed");
            OnGameResume(moment);
            gGameStatus = true;
        }
        if (gGameStatus == true)
        {
            // Keys Processing...
            // ----------------------------------------------------------------

            gHoldedChannels = 0;

            if (!Samp::Instance().IsChatActive())
            {
                for (size_t i = 0; i != gKeys.Size(); ++i)
                {
                    if (gKeys[i] != 0 && game_utils::is_key_pressed(gKeys[i]))
                        utils::bitset::set(gHoldedChannels, i);
                }
            }

            // ----------------------------------------------------------------

            const udword_t old_channels = VoiceService::Instance().GetChannels();
            VoiceService::Instance().SetChannels((gHoldedChannels | gPlayedChannels) & gActiveChannels);
            const udword_t new_channels = VoiceService::Instance().GetChannels();

            if (old_channels != new_channels)
            {
                if (old_channels == 0) // Play
                {
                    MicroIcon::Instance().SwitchToActiveIcon();
                    Speaker::Instance().StartRecording();
                }
                if (new_channels == 0) // Stop
                {
                    MicroIcon::Instance().SwitchToPassiveIcon();
                    Speaker::Instance().StopRecording();
                }
            }

            // ----------------------------------------------------------------

            for (size_t stream = 0; stream != kMaxStreams; ++stream)
            {
                if (gStreams.Acquire<0>(stream))
                {
                    gStreams[stream].UpdatePosition();
                }
            }

            Listener::Instance().Tick();

            if (VoiceService::Instance().IsOpened())
            {
                VoiceService::Instance().Tick(moment);

                for (ubyte_t buffer[kVoicePacketLimit];;)
                {
                    const size_t length = VoiceService::Instance().ReceivePacket(buffer);
                    if (length == 0 || length == -1) break;

                    const auto header = reinterpret_cast<const IncomingVoiceHeader*>(buffer);

                    if (!BlackList::Instance().IsPlayerBlocked(header->source))
                    {
                        auto iterator = header->streams;
                        while (*iterator != None<uword_t>) ++iterator;
                        ++iterator;

                        gSources[header->source].PushPacket(header->packet, header->streams,
                            { reinterpret_cast<cadr_t>(iterator), static_cast<opus_int32>
                            (buffer + length - reinterpret_cast<cadr_t>(iterator)) });
                    }
                }
            }

            if (gSourcesClock.Expired(kInterval, moment))
            {
                const Time delta = gSourcesClock.Restart(moment);

                for (size_t player = 0; player != kMaxPlayers; ++player)
                {
                    gSources[player].Tick(delta);
                }
            }
        }
    }
}

// Source Callbacks
// ----------------------------------------------------------------

static void OnPlayStream(const uword_t player, const uword_t stream, const udword_t packet) noexcept
{
    // Logger::Instance().LogToChat(0xFF00FF00, "PlayStream : player(%hu), stream(%hu), packet(%u)", player, stream, packet);

    if (gStreams.Acquire<0>(stream))
    {
        SpeakerList::Instance().Play(player, stream, gStreams[stream].target, gStreams[stream].icon);
        gStreams[stream].Play(stream, player, packet);
    }
}

static void OnStopStream(const uword_t player, const uword_t stream) noexcept
{
    // Logger::Instance().LogToChat(0xFFFF0000, "StopStream : player(%hu), stream(%hu)", player, stream);

    if (gStreams.Acquire<0>(stream))
    {
        SpeakerList::Instance().Stop(player, stream);
        gStreams[stream].Stop(player);
    }
}

// Control Server (Network)
// ----------------------------------------------------------------

static IPv4Address gServerAddress;

static void OnControlConnect(const cstr_t host, const uword_t port) noexcept
{
    gServerAddress = IPv4Address::FromString(host, port);

    if (!BlackList::Instance().Initialize())
        Logger::Instance().LogToFile("[sv:err:plugin] : failed to initialize blacklist");
    if (!BlackList::Instance().Load(Storage::Instance().GetBlacklistFile(gServerAddress)))
        Logger::Instance().LogToFile("[sv:inf:plugin] : failed to load blacklist");
}

static void OnControlHandshake(ConnectData& connect_data) noexcept
{
    connect_data.version = kCurrentVersion;
    connect_data.micro = Speaker::Instance().HasMicro();
}

static bool OnControlPacket(const ubyte_t packet, const cptr_t data) noexcept
{
    switch (packet)
    {
        case ControlPackets::ClientInitialize:
        {
            const auto& content = *static_cast<const ClientInitialize*>(data);

            // Voice Server Initializing
            // ----------------------------------------------------------------

            IPv4Address voice_address = IPv4Address::FromNumber(content.voice_host, content.voice_port);

            if (voice_address.IsAny()) voice_address.SetHost<false>(gServerAddress.GetHost<false>());

            if (char buffer[IPv4Address::HostLengthLimit + 1]; voice_address.PrintHost(buffer))
                Logger::Instance().LogToFile("[sv:dbg:ClientInitialize] : host(%s), port(%hu), key(0x%X), id(%hu)",
                    buffer, content.voice_port, content.voice_key, content.voice_id);

            VoiceService::Instance().Initiailize(voice_address, content.voice_id);
            VoiceService::Instance().SetKey(content.voice_key);

            if (!VoiceService::Instance().Open())
                Logger::Instance().LogToFile("[sv:err:plugin] : failed to open voice client");

            // Speaker Initializing
            // ----------------------------------------------------------------

            Speaker::Instance().Buffer = VoiceService::Instance().Buffer();
            Speaker::Instance().OnFrame = std::bind(&VoiceService::SendPacket,
                &VoiceService::Instance(), std::placeholders::_1);

            if (!Speaker::Instance().Initialize())
                Logger::Instance().LogToFile("[sv:err:plugin] : failed to initialize speaker module");

            // Sources Initializing
            // ----------------------------------------------------------------

            for (size_t player = 0; player != kMaxPlayers; ++player)
            {
                gSources[player].OnPlayStream = std::bind(&OnPlayStream,
                    static_cast<uword_t>(player), std::placeholders::_1, std::placeholders::_2);
                gSources[player].OnStopStream = std::bind(&OnStopStream,
                    static_cast<uword_t>(player), std::placeholders::_1);

                if (!gSources[player].Initialize())
                    Logger::Instance().LogToFile("[sv:err:plugin] : failed to initialize source #%u", player);
            }

            // ----------------------------------------------------------------

            return true;
        }
        case ControlPackets::SpeakerActiveChannels:
        {
            const auto& content = *static_cast<const SpeakerActiveChannels*>(data);

            Logger::Instance().LogToFile("[sv:dbg:SpeakerActiveChannels] : channels(0x%X)",
                content.channels);

            gActiveChannels = content.channels;

            return true;
        }
        case ControlPackets::SpeakerPlayedChannels:
        {
            const auto& content = *static_cast<const SpeakerPlayedChannels*>(data);

            Logger::Instance().LogToFile("[sv:dbg:SpeakerPlayedChannels] : channels(0x%X)",
                content.channels);

            gPlayedChannels = content.channels;

            return true;
        }
        case ControlPackets::SpeakerSetKey:
        {
            const auto& content = *static_cast<const SpeakerSetKey*>(data);

            Logger::Instance().LogToFile("[sv:dbg:SpeakerSetKey] : channels(0x%X), key(0x%hhX)",
                content.channels, content.key);

            BitsetForEach(channel, content.channels)
                gKeys[channel] = content.key;

            return true;
        }
        case ControlPackets::StreamCreate:
        {
            const auto& content = *static_cast<const StreamCreate*>(data);

            Logger::Instance().LogToFile("[sv:dbg:StreamCreate] : stream(%hu), distance(%.2f)",
                content.stream, content.distance);

            if (!gStreams.EmplaceAt(content.stream, true, content.distance))
                Logger::Instance().LogToFile("[sv:err:StreamCreate] : failed to create stream");

            return true;
        }
        case ControlPackets::StreamSetVolume:
        {
            const auto& content = *static_cast<const StreamSetVolume*>(data);

            Logger::Instance().LogToFile("[sv:dbg:StreamSetVolume] : stream(%hu), volume(%.2f)",
                content.stream, content.volume);

            if (gStreams.Acquire<0>(content.stream))
            {
                gStreams[content.stream].SetVolume(content.volume);
            }

            return true;
        }
        case ControlPackets::StreamSetPanning:
        {
            const auto& content = *static_cast<const StreamSetPanning*>(data);

            Logger::Instance().LogToFile("[sv:dbg:StreamSetPanning] : stream(%hu), panning(%.2f)",
                content.stream, content.panning);

            if (gStreams.Acquire<0>(content.stream))
            {
                gStreams[content.stream].SetPanning(content.panning);
            }

            return true;
        }
        case ControlPackets::StreamSetDistance:
        {
            const auto& content = *static_cast<const StreamSetDistance*>(data);

            Logger::Instance().LogToFile("[sv:dbg:StreamSetDistance] : stream(%hu), distance(%.2f)",
                content.stream, content.distance);

            if (gStreams.Acquire<0>(content.stream))
            {
                gStreams[content.stream].SetDistance(content.distance);
            }

            return true;
        }
        case ControlPackets::StreamSetPosition:
        {
            const auto& content = *static_cast<const StreamSetPosition*>(data);

            Logger::Instance().LogToFile("[sv:dbg:StreamSetPosition] : stream(%hu), x(%.2f), y(%.2f), z(%.2f)",
                content.stream, content.x, content.y, content.z);

            if (gStreams.Acquire<0>(content.stream))
            {
                gStreams[content.stream].SetPosition({ content.x, content.y, content.z });
            }

            return true;
        }
        case ControlPackets::StreamSetTarget:
        {
            const auto& content = *static_cast<const StreamSetTarget*>(data);

            Logger::Instance().LogToFile("[sv:dbg:StreamSetTarget] : stream(%hu), target(0x%hX)",
                content.stream, content.target);

            if (gStreams.Acquire<0>(content.stream))
            {
                SpeakerList::Instance().UpdateTargetForStream(content.stream,
                    (gStreams[content.stream].target = content.target));
            }

            return true;
        }
        case ControlPackets::StreamSetEffect:
        {
            const auto& content = *static_cast<const StreamSetEffect*>(data);

            Logger::Instance().LogToFile("[sv:dbg:StreamSetEffect] : stream(%hu), effect(%hu)",
                content.stream, content.effect);

            if (gStreams.Acquire<0>(content.stream))
            {
                gStreams[content.stream].SetEffect(content.effect);
            }

            return true;
        }
        case ControlPackets::StreamSetIcon:
        {
            const auto& content = *static_cast<const StreamSetIcon*>(data);

            Logger::Instance().LogToFile("[sv:dbg:StreamSetIcon] : stream(%hu), icon(%s)",
                content.stream, content.icon);

            if (gStreams.Acquire<0>(content.stream))
            {
                SpeakerList::Instance().UpdateIconForStream(content.stream,
                    (gStreams[content.stream].icon = content.icon));
            }

            return true;
        }
        case ControlPackets::StreamDelete:
        {
            const auto& content = *static_cast<const StreamDelete*>(data);

            Logger::Instance().LogToFile("[sv:dbg:StreamDelete] : stream(%hu)",
                content.stream);

            gStreams.Remove(content.stream, false);

            return true;
        }
        case ControlPackets::EffectCreate:
        {
            const auto& content = *static_cast<const EffectCreate*>(data);

            Logger::Instance().LogToFile("[sv:dbg:EffectCreate] : effect(%hu)",
                content.effect);

            if (!gEffects.EmplaceAt(content.effect, true))
                Logger::Instance().LogToFile("[sv:err:EffectCreate] : failed to create effect");

            return true;
        }
        case ControlPackets::EffectAppendFilter:
        {
            const auto& content = *static_cast<const EffectAppendFilter*>(data);

            Block<const ubyte_t> parameters;

            switch (content.type)
            {
                case Filter::Chorus:
                {
                    Logger::Instance().LogToFile("[sv:dbg:EffectAppendFilter:Chorus] : effect(%hu), type(%hhu), priority(%hd), "
                        "wetdrymix(%.2f), depth(%.2f), feedback(%.2f), frequency(%.2f), waveform(%u), delay(%.2f), phase(%u)",
                        content.effect, content.type, content.priority,
                        reinterpret_cast<const ChorusParameters*>(content.parameters)->wetdrymix,
                        reinterpret_cast<const ChorusParameters*>(content.parameters)->depth,
                        reinterpret_cast<const ChorusParameters*>(content.parameters)->feedback,
                        reinterpret_cast<const ChorusParameters*>(content.parameters)->frequency,
                        reinterpret_cast<const ChorusParameters*>(content.parameters)->waveform,
                        reinterpret_cast<const ChorusParameters*>(content.parameters)->delay,
                        reinterpret_cast<const ChorusParameters*>(content.parameters)->phase);

                    parameters = Block<const ubyte_t>::FromData<true>({ content.parameters, sizeof(ChorusParameters) });

                    break;
                }
                case Filter::Compressor:
                {
                    Logger::Instance().LogToFile("[sv:dbg:EffectAppendFilter:Compressor] : effect(%hu), type(%hhu), priority(%hd), "
                        "gain(%.2f), attack(%.2f), release(%.2f), threshold(%.2f), ratio(%.2f), predelay(%.2f)",
                        content.effect, content.type, content.priority,
                        reinterpret_cast<const CompressorParameters*>(content.parameters)->gain,
                        reinterpret_cast<const CompressorParameters*>(content.parameters)->attack,
                        reinterpret_cast<const CompressorParameters*>(content.parameters)->release,
                        reinterpret_cast<const CompressorParameters*>(content.parameters)->threshold,
                        reinterpret_cast<const CompressorParameters*>(content.parameters)->ratio,
                        reinterpret_cast<const CompressorParameters*>(content.parameters)->predelay);

                    parameters = Block<const ubyte_t>::FromData<true>({ content.parameters, sizeof(CompressorParameters) });

                    break;
                }
                case Filter::Distortion:
                {
                    Logger::Instance().LogToFile("[sv:dbg:EffectAppendFilter:Distortion] : effect(%hu), type(%hhu), priority(%hd), "
                        "gain(%.2f), edge(%.2f), posteqcenterfrequency(%.2f), posteqbandwidth(%.2f), prelowpasscutoff(%.2f)",
                        content.effect, content.type, content.priority,
                        reinterpret_cast<const DistortionParameters*>(content.parameters)->gain,
                        reinterpret_cast<const DistortionParameters*>(content.parameters)->edge,
                        reinterpret_cast<const DistortionParameters*>(content.parameters)->posteqcenterfrequency,
                        reinterpret_cast<const DistortionParameters*>(content.parameters)->posteqbandwidth,
                        reinterpret_cast<const DistortionParameters*>(content.parameters)->prelowpasscutoff);

                    parameters = Block<const ubyte_t>::FromData<true>({ content.parameters, sizeof(DistortionParameters) });

                    break;
                }
                case Filter::Echo:
                {
                    Logger::Instance().LogToFile("[sv:dbg:EffectAppendFilter:Echo] : effect(%hu), type(%hhu), priority(%hd), "
                        "wetdrymix(%.2f), feedback(%.2f), leftdelay(%.2f), rightdelay(%.2f), pandelay(%hhu)",
                        content.effect, content.type, content.priority,
                        reinterpret_cast<const EchoParameters*>(content.parameters)->wetdrymix,
                        reinterpret_cast<const EchoParameters*>(content.parameters)->feedback,
                        reinterpret_cast<const EchoParameters*>(content.parameters)->leftdelay,
                        reinterpret_cast<const EchoParameters*>(content.parameters)->rightdelay,
                        reinterpret_cast<const EchoParameters*>(content.parameters)->pandelay);

                    parameters = Block<const ubyte_t>::FromData<true>({ content.parameters, sizeof(EchoParameters) });

                    break;
                }
                case Filter::Flanger:
                {
                    Logger::Instance().LogToFile("[sv:dbg:EffectAppendFilter:Flanger] : effect(%hu), type(%hhu), priority(%hd), "
                        "wetdrymix(%.2f), depth(%.2f), feedback(%.2f), frequency(%.2f), waveform(%u), delay(%.2f), phase(%u)",
                        content.effect, content.type, content.priority,
                        reinterpret_cast<const FlangerParameters*>(content.parameters)->wetdrymix,
                        reinterpret_cast<const FlangerParameters*>(content.parameters)->depth,
                        reinterpret_cast<const FlangerParameters*>(content.parameters)->feedback,
                        reinterpret_cast<const FlangerParameters*>(content.parameters)->frequency,
                        reinterpret_cast<const FlangerParameters*>(content.parameters)->waveform,
                        reinterpret_cast<const FlangerParameters*>(content.parameters)->delay,
                        reinterpret_cast<const FlangerParameters*>(content.parameters)->phase);

                    parameters = Block<const ubyte_t>::FromData<true>({ content.parameters, sizeof(FlangerParameters) });

                    break;
                }
                case Filter::Gargle:
                {
                    Logger::Instance().LogToFile("[sv:dbg:EffectAppendFilter:Gargle] : effect(%hu), type(%hhu), priority(%hd), "
                        "ratehz(%u), waveshape(%u)",
                        content.effect, content.type, content.priority,
                        reinterpret_cast<const GargleParameters*>(content.parameters)->ratehz,
                        reinterpret_cast<const GargleParameters*>(content.parameters)->waveshape);

                    parameters = Block<const ubyte_t>::FromData<true>({ content.parameters, sizeof(GargleParameters) });

                    break;
                }
                case Filter::I3dl2reverb:
                {
                    Logger::Instance().LogToFile("[sv:dbg:EffectAppendFilter:I3dl2reverb] : effect(%hu), type(%hhu), priority(%hd), "
                        "room(%d), roomhf(%d), roomrollofffactor(%.2f), decaytime(%.2f), decayhfratio(%u), reflections(%d), "
                        "reflectionsdelay(%.2f), reverb(%d), reverbdelay(%.2f), diffusion(%.2f), density(%.2f), hfreference(%.2f)",
                        content.effect, content.type, content.priority,
                        reinterpret_cast<const I3dl2reverbParameters*>(content.parameters)->room,
                        reinterpret_cast<const I3dl2reverbParameters*>(content.parameters)->roomhf,
                        reinterpret_cast<const I3dl2reverbParameters*>(content.parameters)->roomrollofffactor,
                        reinterpret_cast<const I3dl2reverbParameters*>(content.parameters)->decaytime,
                        reinterpret_cast<const I3dl2reverbParameters*>(content.parameters)->decayhfratio,
                        reinterpret_cast<const I3dl2reverbParameters*>(content.parameters)->reflections,
                        reinterpret_cast<const I3dl2reverbParameters*>(content.parameters)->reflectionsdelay,
                        reinterpret_cast<const I3dl2reverbParameters*>(content.parameters)->reverb,
                        reinterpret_cast<const I3dl2reverbParameters*>(content.parameters)->reverbdelay,
                        reinterpret_cast<const I3dl2reverbParameters*>(content.parameters)->diffusion,
                        reinterpret_cast<const I3dl2reverbParameters*>(content.parameters)->density,
                        reinterpret_cast<const I3dl2reverbParameters*>(content.parameters)->hfreference);

                    parameters = Block<const ubyte_t>::FromData<true>({ content.parameters, sizeof(I3dl2reverbParameters) });

                    break;
                }
                case Filter::Parameq:
                {
                    Logger::Instance().LogToFile("[sv:dbg:EffectAppendFilter:Parameq] : effect(%hu), type(%hhu), priority(%hd), "
                        "center(%.2f), bandwidth(%.2f), gain(%.2f)",
                        content.effect, content.type, content.priority,
                        reinterpret_cast<const ParameqParameters*>(content.parameters)->center,
                        reinterpret_cast<const ParameqParameters*>(content.parameters)->bandwidth,
                        reinterpret_cast<const ParameqParameters*>(content.parameters)->gain);

                    parameters = Block<const ubyte_t>::FromData<true>({ content.parameters, sizeof(ParameqParameters) });

                    break;
                }
                case Filter::Reverb:
                {
                    Logger::Instance().LogToFile("[sv:dbg:EffectAppendFilter:Reverb] : effect(%hu), type(%hhu), priority(%hd), "
                        "ingain(%.2f), reverbmix(%.2f), reverbtime(%.2f), highfreqrtratio(%.2f)",
                        content.effect, content.type, content.priority,
                        reinterpret_cast<const ReverbParameters*>(content.parameters)->ingain,
                        reinterpret_cast<const ReverbParameters*>(content.parameters)->reverbmix,
                        reinterpret_cast<const ReverbParameters*>(content.parameters)->reverbtime,
                        reinterpret_cast<const ReverbParameters*>(content.parameters)->highfreqrtratio);

                    parameters = Block<const ubyte_t>::FromData<true>({ content.parameters, sizeof(ReverbParameters) });

                    break;
                }
            }

            if (gEffects.Acquire<0>(content.effect))
            {
                if (const auto filter = gEffects[content.effect].AppendFilter(content.type,
                    content.priority, std::move(parameters)); filter != nullptr)
                {
                    for (size_t stream = 0; stream != kMaxStreams; ++stream)
                    {
                        if (gStreams.Acquire<0>(stream))
                        {
                            if (gStreams[stream].effect == content.effect)
                            {
                                gStreams[stream].ForEach([=](const size_t channel, const uword_t) noexcept -> void
                                {
                                    filter->Attach(ChannelPool::Instance()[channel].Handle());
                                });
                            }
                        }
                    }
                }
            }

            return true;
        }
        case ControlPackets::EffectRemoveFilter:
        {
            const auto& content = *static_cast<const EffectRemoveFilter*>(data);

            Logger::Instance().LogToFile("[sv:dbg:EffectRemoveFilter] : effect(%hu), type(%hhu), priority(%hd)",
                content.effect, content.type, content.priority);

            if (gEffects.Acquire<0>(content.effect))
            {
                gEffects[content.effect].RemoveFilter(content.type, content.priority);
            }

            return true;
        }
        case ControlPackets::EffectDelete:
        {
            const auto& content = *static_cast<const EffectDelete*>(data);

            Logger::Instance().LogToFile("[sv:dbg:EffectDelete] : effect(%hu)",
                content.effect);

            gEffects.Remove(content.effect, false);

            return true;
        }
    }

    return false;
}

static void OnControlDisconnect() noexcept
{
    if (!BlackList::Instance().Save(Storage::Instance().GetBlacklistFile(gServerAddress)))
    {
        Logger::Instance().LogToFile("[sv:err:plugin] : failed to save blacklist");
    }

    VoiceService::Instance().Deinitialize();
    Speaker::Instance().Deinitialize();
}

// Windows Handler
// ----------------------------------------------------------------

static LONG gOriginalWindowProcedure = NULL;
static HWND gOriginalWindowHandle = NULL;

static LRESULT CALLBACK WindowProcedure(const HWND window, const UINT message,
    const WPARAM wparam, const LPARAM lparam) noexcept
{
    if (PluginMenu::Instance().WindowProcedure(window, message, wparam, lparam) != 0)
        return 1;

    return CallWindowProc(reinterpret_cast<WNDPROC>(gOriginalWindowProcedure),
        window, message, wparam, lparam);
}

// Draw Radar Callback
// ----------------------------------------------------------------

static CallHook gDrawRadarHook;

static void DrawRadarHook() noexcept
{
    static_cast<void(*)()>(gDrawRadarHook.Target())();

    SpeakerList::Instance().Render();
    MicroIcon::Instance().Render();
}

// Game Callbacks
// ----------------------------------------------------------------

static void OnGameLoad() noexcept
{
    // if (!SocketLibraryStartup())
    //     Logger::Instance().LogToFile("[sv:err:plugin] : failed to initialize socket library");

    if (!ChannelPool::Instance().Initialize())
        Logger::Instance().LogToFile("[sv:err:plugin] : failed to initialize channel pool");

    gDrawRadarHook.Initialize(reinterpret_cast<LPVOID>(0x58FC53), DrawRadarHook);

    game_utils::disable_anticheat(Addresses::Instance().Base());
}

static void OnGameExit() noexcept
{
    if (!PluginConfig::Instance().Save(Storage::Instance().GetConfigFile()))
        Logger::Instance().LogToFile("[sv:inf:plugin] : failed to save config");

    BlackList::Instance().Deinitialize();

    Listener::Instance().Deinitialize();
    ControlService::Instance().Deinitialize();

    Addresses::Instance().Deinitialize();

    Render::Instance().Deinitialize();
    Localization::Instance().Deinitialize();
    Logger::Instance().Deinitialize();

    Storage::Instance().Deinitialize();

    ChannelPool::Instance().Deinitialize();

    for (size_t player = 0; player != kMaxPlayers; ++player)
        gSources[player].Deinitialize();

    gStreams.Deinitialize();
    gEffects.Deinitialize();

    // SocketLibraryCleanup();
}

// Render Callbacks
// ----------------------------------------------------------------

static Block<ubyte_t> gPassiveMicroIconResource;
static Block<ubyte_t> gActiveMicroIconResource;
static Block<ubyte_t> gMutedMicroIconResource;
static Block<ubyte_t> gSpeakerIconResource;
static Block<ubyte_t> gBlurShaderResource;
static Block<ubyte_t> gLogoIconResource;

static void OnDeviceInitialize(IDirect3D9* const, IDirect3DDevice9* const device,
    const D3DPRESENT_PARAMETERS& parameters) noexcept
{
    assert(device != nullptr);

    gPassiveMicroIconResource = File::Content(Storage::Instance().GetResourceFile("micro_passive.png"));
    gActiveMicroIconResource = File::Content(Storage::Instance().GetResourceFile("micro_active.png"));
    gMutedMicroIconResource = File::Content(Storage::Instance().GetResourceFile("micro_muted.png"));
    gSpeakerIconResource = File::Content(Storage::Instance().GetResourceFile("speaker.png"));
    gBlurShaderResource = File::Content(Storage::Instance().GetResourceFile("gauss.hlsl"));
    gLogoIconResource = File::Content(Storage::Instance().GetResourceFile("logo.png"));

    if (gPassiveMicroIconResource.Valid() && gActiveMicroIconResource.Valid() && gMutedMicroIconResource.Valid())
        MicroIcon::Instance().Initialize(device, gPassiveMicroIconResource.Area(),
            gActiveMicroIconResource.Area(), gMutedMicroIconResource.Area());

    ImGuiUtils::Instance().Initialize(device);

    if (gSpeakerIconResource.Valid())
        SpeakerList::Instance().Initialize(device, gSpeakerIconResource.Area(),
            Storage::Instance().GetResourceFile("font.ttf"));

    if (gBlurShaderResource.Valid() && gLogoIconResource.Valid())
        PluginMenu::Instance().Initialize(device, gBlurShaderResource.Area(),
            gLogoIconResource.Area(), Storage::Instance().GetResourceFile("font.ttf"));

    gOriginalWindowHandle = parameters.hDeviceWindow;
    gOriginalWindowProcedure = GetWindowLong(gOriginalWindowHandle, GWL_WNDPROC);
    SetWindowLong(gOriginalWindowHandle, GWL_WNDPROC, reinterpret_cast<LONG>(&WindowProcedure));

    gGameStatus = game_utils::is_game_active();
}

static void OnPresent() noexcept
{
    MainLoop();

    MicroIcon::Instance().Update();

    PluginMenu::Instance().Update();
    PluginMenu::Instance().Render();
}

static void OnBeforeReset() noexcept
{
    PluginMenu::Instance().Deinitialize();
    SpeakerList::Instance().Deinitialize();
    ImGuiUtils::Instance().Deinitialize();
    MicroIcon::Instance().Deinitialize();
}

static void OnAfterReset(IDirect3DDevice9* const device,
    const D3DPRESENT_PARAMETERS& parameters) noexcept
{
    assert(device != nullptr);

    if (gPassiveMicroIconResource.Valid() && gActiveMicroIconResource.Valid() && gMutedMicroIconResource.Valid())
        MicroIcon::Instance().Initialize(device, gPassiveMicroIconResource.Area(),
            gActiveMicroIconResource.Area(), gMutedMicroIconResource.Area());

    ImGuiUtils::Instance().Initialize(device);

    if (gSpeakerIconResource.Valid())
        SpeakerList::Instance().Initialize(device, gSpeakerIconResource.Area(),
            Storage::Instance().GetResourceFile("font.ttf"));

    if (gBlurShaderResource.Valid() && gLogoIconResource.Valid())
        PluginMenu::Instance().Initialize(device, gBlurShaderResource.Area(),
            gLogoIconResource.Area(), Storage::Instance().GetResourceFile("font.ttf"));
}

static void OnDeviceDestroy() noexcept
{
    SetWindowLong(gOriginalWindowHandle, GWL_WNDPROC, gOriginalWindowProcedure);

    PluginMenu::Instance().Deinitialize();
    SpeakerList::Instance().Deinitialize();
    ImGuiUtils::Instance().Deinitialize();
    MicroIcon::Instance().Deinitialize();

    gPassiveMicroIconResource.Destroy();
    gActiveMicroIconResource.Destroy();
    gMutedMicroIconResource.Destroy();
    gSpeakerIconResource.Destroy();
    gBlurShaderResource.Destroy();
    gLogoIconResource.Destroy();
}

// Load Callbacks
// ----------------------------------------------------------------

static bool OnSampLoad(const HMODULE module) noexcept
{
    Addresses::Instance().Initialize(module);

    Samp::Instance().OnLoad = OnGameLoad;
    Samp::Instance().OnExit = OnGameExit;

    if (!Samp::Instance().Initialize())
    {
        Addresses::Instance().Deinitialize();
        Render::Instance().Deinitialize();
        Localization::Instance().Deinitialize();
        Logger::Instance().Deinitialize();
        Storage::Instance().Deinitialize();
        return false;
    }

    ControlService::Instance().OnConnect    = OnControlConnect;
    ControlService::Instance().OnDisconnect = OnControlDisconnect;
    ControlService::Instance().OnHandshake  = OnControlHandshake;
    ControlService::Instance().OnPacket     = OnControlPacket;

    if (!ControlService::Instance().Initialize())
    {
        Samp::Instance().Deinitialize();
        Addresses::Instance().Deinitialize();
        Render::Instance().Deinitialize();
        Localization::Instance().Deinitialize();
        Logger::Instance().Deinitialize();
        Storage::Instance().Deinitialize();
        return false;
    }

    if (!Listener::Instance().Initialize())
    {
        ControlService::Instance().Deinitialize();
        Samp::Instance().Deinitialize();
        Addresses::Instance().Deinitialize();
        Render::Instance().Deinitialize();
        Localization::Instance().Deinitialize();
        Logger::Instance().Deinitialize();
        Storage::Instance().Deinitialize();
        return false;
    }

    return true;
}

static bool OnPluginLoad(const HMODULE module) noexcept
{
    if (!Storage::Instance().Initialize())
        return false;

    Logger::Instance().OnLogToChat = std::bind(&Samp::AddMessageToChat, &Samp::Instance(),
        std::placeholders::_1, std::placeholders::_2);

    const cstr_t log_file = Storage::Instance().GetLogFile();

    if (File::Size(log_file) > 10 * 1024 * 1024) // 10 MB
        File::Remove(log_file);

    if (!Logger::Instance().Initialize(log_file))
    {
        Storage::Instance().Deinitialize();
        return false;
    }

    if (!PluginConfig::Instance().Load(Storage::Instance().GetConfigFile()))
    {
        Logger::Instance().LogToFile("[sv:inf:plugin] : failed to load config (default config will be applied)");
    }

    if (!Localization::Instance().Initialize())
    {
        Logger::Instance().Deinitialize();
        Storage::Instance().Deinitialize();
        return false;
    }

    Render::Instance().OnDeviceInitialize = OnDeviceInitialize;
    Render::Instance().OnPresent          = OnPresent;
    Render::Instance().OnBeforeReset      = OnBeforeReset;
    Render::Instance().OnAfterReset       = OnAfterReset;
    Render::Instance().OnDeviceDestroy    = OnDeviceDestroy;

    if (!Render::Instance().Initialize())
    {
        Localization::Instance().Deinitialize();
        Logger::Instance().Deinitialize();
        Storage::Instance().Deinitialize();
        return false;
    }

    return true;
}

// Launcher
// ----------------------------------------------------------------

static char gLibraryName[64] = "samp.dll";

static DWORD WINAPI LibraryWaitingThread(const LPVOID) noexcept
{
    HMODULE samp_module;

    while ((samp_module = GetModuleHandle(gLibraryName)) == nullptr)
        utils::thread::sleep(100); // 100 milliseconds

    OnSampLoad(samp_module);

    return NULL;
}

BOOL APIENTRY DllMain(const HMODULE module, const DWORD reason, const LPVOID) noexcept
{
    if (reason != DLL_PROCESS_ATTACH)
        return TRUE;

    if (OnPluginLoad(module) == false)
        return FALSE;

    if (cstr_t cmd_line = GetCommandLine(); cmd_line != nullptr)
    {
        bool finded = false; while (*cmd_line != '\0')
        {
            while (*cmd_line == ' ')
                ++cmd_line;

            const cstr_t token_begin = cmd_line;

            while (*cmd_line != ' ' && *cmd_line != '\0')
                ++cmd_line;

            const cstr_t token_end = cmd_line;

            if (finded)
            {
                if (const size_t length = token_end - token_begin;
                    length != 0 && length < sizeof(gLibraryName))
                {
                    std::memcpy(gLibraryName, token_begin, length);
                    gLibraryName[length] = '\0';
                }

                break;
            }

            if (constexpr char kLibraryParameterName[] = "-mplib";
                token_end - token_begin == sizeof(kLibraryParameterName) - 1 &&
                std::memcmp(token_begin, kLibraryParameterName, sizeof(kLibraryParameterName) - 1) == 0)
            {
                finded = true;
            }
        }
    }

    const auto waiting_thread = CreateThread
        (NULL, 0, LibraryWaitingThread, NULL, NULL, NULL);
    return waiting_thread != NULL ? TRUE : FALSE;
}
