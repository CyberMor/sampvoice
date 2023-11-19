/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include <other/config.hpp>
#include <memory/structures/pool.hpp>

#include <pawn/amx/amx.h>
#include <pawn/plugincommon.h>
#include <util/logger.hpp>
#include <ysf/globals.h>

#include "command.hpp"
#include "control.hpp"

#include "pawn.hpp"
#include "main.hpp"

#include "player.hpp"
#include "stream.hpp"
#include "effect.hpp"

// Plugin Context
// ----------------------------------------------------------------

static Pool<Player, kMaxPlayers> gPlayers;
static Pool<Stream, kMaxStreams> gStreams;
static Pool<Effect, kMaxEffects> gEffects;

// ----------------------------------------------------------------

static IPv4Address gVoiceAddress;

// Pawn Interface
// ----------------------------------------------------------------

static Version OnGetVersion(const uword_t player) noexcept
{
    if (gPlayers.Acquire<0>(player) == false)
        return Zero<Version>;

    return gPlayers[player].version;
}

static bool OnHasMicro(const uword_t player) noexcept
{
    if (gPlayers.Acquire<0>(player) == false)
        return false;

    return gPlayers[player].micro;
}

static bool OnEnableListener(const uword_t player) noexcept
{
    if (gPlayers.Acquire<0>(player) == false)
        return false;

    if (gPlayers[player].listener == true)
        return false;

    // Sending Command To Server...
    // -------------------------------------------------

    const auto command = static_cast<CommandPlayerListener*>
        (CommandService::Instance().BeginCommand(CommandPackets::PlayerListener));

    command->player = player;
    command->status = true;

    CommandService::Instance().EndCommand();
    if (!CommandService::Instance().SendCommand())
        return false;

    // -------------------------------------------------

    gPlayers[player].listener = true;

    return true;
}

static bool OnDisableListener(const uword_t player) noexcept
{
    if (gPlayers.Acquire<0>(player) == false)
        return false;

    if (gPlayers[player].listener == false)
        return false;

    // Sending Command To Server...
    // -------------------------------------------------

    const auto command = static_cast<CommandPlayerListener*>
        (CommandService::Instance().BeginCommand(CommandPackets::PlayerListener));

    command->player = player;
    command->status = false;

    CommandService::Instance().EndCommand();
    if (!CommandService::Instance().SendCommand())
        return false;

    // -------------------------------------------------

    gPlayers[player].listener = false;

    return true;
}

static bool OnCheckListener(const uword_t player) noexcept
{
    if (gPlayers.Acquire<0>(player) == false)
        return false;

    return gPlayers[player].listener;
}

static bool OnEnableSpeaker(const uword_t player, const udword_t channels) noexcept
{
    if (gPlayers.Acquire<0>(player) == false)
        return false;

    udword_t active_channels = gPlayers[player].active_channels;

    BitsetForEach(channel, channels)
    {
        if (gPlayers[player].listeners_count[channel] != 0)
        {
            utils::bitset::set(active_channels, channel);
        }
    }

    if (active_channels == gPlayers[player].active_channels)
        return false;

    // Sending Command To Server...
    // -------------------------------------------------

    const auto command = static_cast<CommandPlayerSpeaker*>
        (CommandService::Instance().BeginCommand(CommandPackets::PlayerSpeaker));

    command->player = player;
    command->channels = active_channels;

    CommandService::Instance().EndCommand();
    if (!CommandService::Instance().SendCommand())
        return false;

    // Sending Packet to Player...
    // -------------------------------------------------

    const auto packet = static_cast<ControlSpeakerActiveChannels*>
        (ControlService::Instance().BeginPacket(ControlPackets::SpeakerActiveChannels));

    packet->channels = active_channels;

    ControlService::Instance().EndPacket();
    ControlService::Instance().SendPacket(player);

    // -------------------------------------------------

    gPlayers[player].active_channels = active_channels;

    return true;
}

static bool OnDisableSpeaker(const uword_t player, const udword_t channels) noexcept
{
    if (gPlayers.Acquire<0>(player) == false)
        return false;

    const udword_t active_channels = gPlayers[player].active_channels & ~channels;
    const udword_t played_channels = gPlayers[player].played_channels & ~channels;

    if (active_channels == gPlayers[player].active_channels)
        return false;

    // Sending Command To Server...
    // -------------------------------------------------

    const auto command = static_cast<CommandPlayerSpeaker*>
        (CommandService::Instance().BeginCommand(CommandPackets::PlayerSpeaker));

    command->player = player;
    command->channels = active_channels;

    CommandService::Instance().EndCommand();
    if (!CommandService::Instance().SendCommand())
        return false;

    // Sending Packet to Player...
    // -------------------------------------------------

    const auto packet = static_cast<ControlSpeakerActiveChannels*>
        (ControlService::Instance().BeginPacket(ControlPackets::SpeakerActiveChannels));

    packet->channels = active_channels;

    ControlService::Instance().EndPacket();
    ControlService::Instance().SendPacket(player);

    // -------------------------------------------------

    gPlayers[player].active_channels = active_channels;
    gPlayers[player].played_channels = played_channels;

    return true;
}

static bool OnCheckSpeaker(const uword_t player, const udword_t channels) noexcept
{
    if (gPlayers.Acquire<0>(player) == false)
        return false;

    return gPlayers[player].active_channels & channels;
}

static bool OnAttachStream(const uword_t player, const uword_t stream, const udword_t channels) noexcept
{
    if (gPlayers.Acquire<0>(player) == false)
        return false;

    if (gStreams.Acquire<0>(stream) == false)
        return false;

    const udword_t active_channels = channels | gPlayers[player].active_channels;
          udword_t attach_channels = channels;

    BitsetForEach(channel, channels)
    {
        if (gPlayers[player].listeners[channel].Test(stream))
        {
            utils::bitset::reset(attach_channels, channel);
        }
    }

    if (attach_channels == 0)
        return false;

    // Sending Command To Server...
    // -------------------------------------------------

    if (active_channels != gPlayers[player].active_channels)
    {
        const auto command = static_cast<CommandPlayerSpeaker*>
            (CommandService::Instance().BeginCommand(CommandPackets::PlayerSpeaker));

        command->player = player;
        command->channels = active_channels;

        CommandService::Instance().EndCommand();
        if (!CommandService::Instance().SendCommand())
            return false;
    }

    // -------------------------------------------------

    const auto command = static_cast<CommandPlayerAttachStream*>
        (CommandService::Instance().BeginCommand(CommandPackets::PlayerAttachStream));

    command->player = player;
    command->channels = attach_channels;
    command->stream = stream;

    CommandService::Instance().EndCommand();
    if (!CommandService::Instance().SendCommand())
        return false;

    // Sending Packet to Player...
    // -------------------------------------------------

    if (active_channels != gPlayers[player].active_channels)
    {
        const auto packet = static_cast<ControlSpeakerActiveChannels*>
            (ControlService::Instance().BeginPacket(ControlPackets::SpeakerActiveChannels));

        packet->channels = active_channels;

        ControlService::Instance().EndPacket();
        ControlService::Instance().SendPacket(player);
    }

    // -------------------------------------------------

    gPlayers[player].active_channels = active_channels;

    BitsetForEach(channel, attach_channels)
    {
        if (gPlayers[player].listeners[channel].Set(stream))
        {
            ++gPlayers[player].listeners_count[channel];
        }
    }

    return true;
}

static bool OnDetachStream(const uword_t player, const uword_t stream, const udword_t channels) noexcept
{
    if (gPlayers.Acquire<0>(player) == false)
        return false;

    udword_t active_channels = gPlayers[player].active_channels;
    udword_t detach_channels = channels;

    if (stream == None<uword_t>)
    {
        active_channels &= ~channels;

        BitsetForEach(channel, channels)
        {
            if (gPlayers[player].listeners_count[channel] == 0)
            {
                utils::bitset::reset(detach_channels, channel);
            }
        }
    }
    else
    {
        BitsetForEach(channel, channels)
        {
            if (gPlayers[player].listeners[channel].Test(stream))
            {
                if (gPlayers[player].listeners_count[channel] == 1)
                {
                    utils::bitset::reset(active_channels, channel);
                }
            }
            else
            {
                utils::bitset::reset(detach_channels, channel);
            }
        }
    }

    if (detach_channels == 0)
        return false;

    // Sending Command To Server...
    // -------------------------------------------------

    if (active_channels != gPlayers[player].active_channels)
    {
        const auto command = static_cast<CommandPlayerSpeaker*>
            (CommandService::Instance().BeginCommand(CommandPackets::PlayerSpeaker));

        command->player = player;
        command->channels = active_channels;

        CommandService::Instance().EndCommand();
        if (!CommandService::Instance().SendCommand())
            return false;
    }
    
    // -------------------------------------------------

    const auto command = static_cast<CommandPlayerDetachStream*>
        (CommandService::Instance().BeginCommand(CommandPackets::PlayerDetachStream));

    command->player = player;
    command->channels = detach_channels;
    command->stream = stream;

    CommandService::Instance().EndCommand();
    if (!CommandService::Instance().SendCommand())
        return false;

    // Sending Packet to Player...
    // -------------------------------------------------

    if (active_channels != gPlayers[player].active_channels)
    {
        const auto packet = static_cast<ControlSpeakerActiveChannels*>
            (ControlService::Instance().BeginPacket(ControlPackets::SpeakerActiveChannels));

        packet->channels = active_channels;

        ControlService::Instance().EndPacket();
        ControlService::Instance().SendPacket(player);
    }

    // -------------------------------------------------

    if (stream == None<uword_t>)
    {
        BitsetForEach(channel, detach_channels)
        {
            if (gPlayers[player].listeners_count[channel] != 0)
            {
                gPlayers[player].listeners_count[channel] = 0;
                gPlayers[player].listeners[channel].Clear();
            }
        }
    }
    else
    {
        BitsetForEach(channel, detach_channels)
        {
            if (gPlayers[player].listeners[channel].Reset(stream))
            {
                assert(gPlayers[player].listeners_count[channel] != 0);
                --gPlayers[player].listeners_count[channel];
            }
        }
    }

    gPlayers[player].active_channels  = active_channels;
    gPlayers[player].played_channels &= active_channels;

    return true;
}

static bool OnHasStream(const uword_t player, const uword_t stream, const udword_t channels) noexcept
{
    if (gPlayers.Acquire<0>(player) == false)
        return false;

    if (stream == None<uword_t>)
    {
        BitsetForEach(channel, channels)
        {
            if (gPlayers[player].listeners_count[channel] != 0)
                return true;
        }
    }
    else
    {
        BitsetForEach(channel, channels)
        {
            if (gPlayers[player].listeners[channel].Test(stream))
                return true;
        }
    }

    return false;
}

static bool OnSetKey(const uword_t player, const ubyte_t key, const udword_t channels) noexcept
{
    if (gPlayers.Acquire<0>(player) == false)
        return false;

    // Sending Packet to Player...
    // -------------------------------------------------

    const auto packet = static_cast<ControlSpeakerSetKey*>
        (ControlService::Instance().BeginPacket(ControlPackets::SpeakerSetKey));

    packet->channels = channels;
    packet->key = key;

    ControlService::Instance().EndPacket();
    ControlService::Instance().SendPacket(player);

    // -------------------------------------------------

    BitsetForEach(channel, channels)
    {
        gPlayers[player].keys[channel] = key;
    }

    return true;
}

static ubyte_t OnGetKey(const uword_t player, const ubyte_t channel) noexcept
{
    if (gPlayers.Acquire<0>(player) == false)
        return Zero<ubyte_t>;

    return gPlayers[player].keys[channel];
}

static bool OnPlay(const uword_t player, const udword_t channels) noexcept
{
    if (gPlayers.Acquire<0>(player) == false)
        return false;

    const udword_t played_channels = gPlayers[player].played_channels |
        (channels & gPlayers[player].active_channels);

    if (played_channels == gPlayers[player].played_channels)
        return false;

    // Sending Packet to Player...
    // -------------------------------------------------

    const auto packet = static_cast<ControlSpeakerPlayedChannels*>
        (ControlService::Instance().BeginPacket(ControlPackets::SpeakerPlayedChannels));

    packet->channels = played_channels;

    ControlService::Instance().EndPacket();
    ControlService::Instance().SendPacket(player);

    // -------------------------------------------------

    gPlayers[player].played_channels = played_channels;

    return true;
}

static bool OnStop(const uword_t player, const udword_t channels) noexcept
{
    if (gPlayers.Acquire<0>(player) == false)
        return false;

    const udword_t played_channels = gPlayers[player].played_channels & ~channels;

    if (played_channels == gPlayers[player].played_channels)
        return false;

    // Sending Packet to Player...
    // -------------------------------------------------

    const auto packet = static_cast<ControlSpeakerPlayedChannels*>
        (ControlService::Instance().BeginPacket(ControlPackets::SpeakerPlayedChannels));

    packet->channels = played_channels;

    ControlService::Instance().EndPacket();
    ControlService::Instance().SendPacket(player);

    // -------------------------------------------------

    gPlayers[player].played_channels = played_channels;

    return true;
}

// ----------------------------------------------------------------

static uword_t OnCreateStream(const fdword_t distance) noexcept
{
    const size_t stream = gStreams.Emplace(distance);
    if (stream != None<size_t>)
    {
        // Sending Command To Server...
        // -------------------------------------------------

        const auto command = static_cast<CommandStreamCreate*>
            (CommandService::Instance().BeginCommand(CommandPackets::StreamCreate));

        command->stream = stream;

        CommandService::Instance().EndCommand();
        CommandService::Instance().SendCommand();

        // -------------------------------------------------
    }

    return stream;
}

static bool OnEnableTransiter(const uword_t stream) noexcept
{
    if (gStreams.Acquire<0>(stream) == false)
        return false;

    if (gStreams[stream].transiter == true)
        return false;

    // Sending Command To Server...
    // -------------------------------------------------

    const auto command = static_cast<CommandStreamTransiter*>
        (CommandService::Instance().BeginCommand(CommandPackets::StreamTransiter));

    command->stream = stream;
    command->status = true;

    CommandService::Instance().EndCommand();
    if (!CommandService::Instance().SendCommand())
        return false;

    // -------------------------------------------------

    gStreams[stream].transiter = true;

    return true;
}

static bool OnDisableTransiter(const uword_t stream) noexcept
{
    if (gStreams.Acquire<0>(stream) == false)
        return false;

    if (gStreams[stream].transiter == false)
        return false;

    // Sending Command To Server...
    // -------------------------------------------------

    const auto command = static_cast<CommandStreamTransiter*>
        (CommandService::Instance().BeginCommand(CommandPackets::StreamTransiter));

    command->stream = stream;
    command->status = false;

    CommandService::Instance().EndCommand();
    if (!CommandService::Instance().SendCommand())
        return false;

    // -------------------------------------------------

    gStreams[stream].transiter = false;

    return true;
}

static bool OnCheckTransiter(const uword_t stream) noexcept
{
    if (gStreams.Acquire<0>(stream) == false)
        return false;

    return gStreams[stream].transiter;
}

static bool OnAttachListener(const uword_t stream, const uword_t player) noexcept
{
    if (gStreams.Acquire<0>(stream) == false)
        return false;

    if (gPlayers.Acquire<0>(player) == false)
        return false;

    // Sending Command To Server...
    // -------------------------------------------------

    if (gStreams[stream].listeners.Test(player) == false)
    {
        const auto command = static_cast<CommandStreamAttachListener*>
            (CommandService::Instance().BeginCommand(CommandPackets::StreamAttachListener));

        command->stream = stream;
        command->player = player;

        CommandService::Instance().EndCommand();
        if (!CommandService::Instance().SendCommand())
            return false;
    }

    // Sending Packet To Player...
    // -------------------------------------------------

    if (gPlayers[player].speakers.Test(stream) == false)
    {
        {
            const auto packet = static_cast<ControlStreamCreate*>
                (ControlService::Instance().BeginPacket(ControlPackets::StreamCreate));

            packet->stream = stream;
            packet->distance = gStreams[stream].distance;

            ControlService::Instance().EndPacket();
            ControlService::Instance().SendPacket(player);
        }
        if (gStreams[stream].volume != 1)
        {
            const auto packet = static_cast<ControlStreamSetVolume*>
                (ControlService::Instance().BeginPacket(ControlPackets::StreamSetVolume));

            packet->stream = stream;
            packet->volume = gStreams[stream].volume;

            ControlService::Instance().EndPacket();
            ControlService::Instance().SendPacket(player);
        }
        if (gStreams[stream].distance == 0)
        {
            if (gStreams[stream].panning != 0)
            {
                const auto packet = static_cast<ControlStreamSetPanning*>
                    (ControlService::Instance().BeginPacket(ControlPackets::StreamSetPanning));

                packet->stream = stream;
                packet->panning = gStreams[stream].panning;

                ControlService::Instance().EndPacket();
                ControlService::Instance().SendPacket(player);
            }
        }
        else
        {
            if (gStreams[stream].target != 0)
            {
                const auto packet = static_cast<ControlStreamSetTarget*>
                    (ControlService::Instance().BeginPacket(ControlPackets::StreamSetTarget));

                packet->stream = stream;
                packet->target = gStreams[stream].target;

                ControlService::Instance().EndPacket();
                ControlService::Instance().SendPacket(player);
            }
            else
            {
                const auto packet = static_cast<ControlStreamSetPosition*>
                    (ControlService::Instance().BeginPacket(ControlPackets::StreamSetPosition));

                packet->stream = stream;
                packet->x = gStreams[stream].position.fX;
                packet->y = gStreams[stream].position.fY;
                packet->z = gStreams[stream].position.fZ;

                ControlService::Instance().EndPacket();
                ControlService::Instance().SendPacket(player);
            }
        }
        if (gStreams[stream].effect != None<uword_t>)
        {
            assert(gEffects.Acquire<0>(gStreams[stream].effect));

            if (gPlayers[player].effects.Set(gStreams[stream].effect))
            {
                {
                    const auto packet = static_cast<ControlEffectCreate*>
                        (ControlService::Instance().BeginPacket(ControlPackets::EffectCreate));

                    packet->effect = gStreams[stream].effect;

                    ControlService::Instance().EndPacket();
                    ControlService::Instance().SendPacket(player);
                }
                ForwardForEach(filter, gEffects[gStreams[stream].effect].filters)
                {
                    const auto packet = static_cast<ControlEffectAppendFilter*>
                        (ControlService::Instance().BeginPacket(ControlPackets::EffectAppendFilter));

                    packet->effect = gStreams[stream].effect;
                    packet->type = filter->Type();
                    packet->priority = filter->Priority();
                    std::memcpy(packet->parameters, filter->Parameters().Data(),
                        filter->Parameters().Size());

                    ControlService::Instance().EndPacket();
                    ControlService::Instance().SendPacket(player);
                }
            }

            const auto packet = static_cast<ControlStreamSetEffect*>
                (ControlService::Instance().BeginPacket(ControlPackets::StreamSetEffect));

            packet->stream = stream;
            packet->effect = gStreams[stream].effect;

            ControlService::Instance().EndPacket();
            ControlService::Instance().SendPacket(player);
        }
        if (gStreams[stream].icon.Size() > 1)
        {
            const auto packet = static_cast<ControlStreamSetIcon*>
                (ControlService::Instance().BeginPacket(ControlPackets::StreamSetIcon));

            packet->stream = stream;
            std::memcpy(packet->icon, gStreams[stream].icon.Data(),
                gStreams[stream].icon.Size());

            ControlService::Instance().EndPacket();
            ControlService::Instance().SendPacket(player);
        }
    }

    // -------------------------------------------------

    gStreams[stream].listeners.Set(player);
    gPlayers[player].speakers.Set(stream);

    return true;
}

static bool OnDetachListener(const uword_t stream, const uword_t player) noexcept
{
    if (gStreams.Acquire<0>(stream) == false)
        return false;

    if (player == None<uword_t>)
    {
        // Sending Command To Server...
        // -------------------------------------------------

        gStreams[stream].listeners.ForEach([=](const size_t listener) noexcept -> void
        {
            const auto command = static_cast<CommandStreamDetachListener*>
                (CommandService::Instance().BeginCommand(CommandPackets::StreamDetachListener));

            command->stream = stream;
            command->player = listener;

            CommandService::Instance().EndCommand();
            CommandService::Instance().SendCommand();
        });

        // Sending Packet To Player...
        // -------------------------------------------------

        const auto packet = static_cast<ControlStreamDelete*>
            (ControlService::Instance().BeginPacket(ControlPackets::StreamDelete));

        packet->stream = stream;

        ControlService::Instance().EndPacket();

        gStreams[stream].listeners.ForEach([=](const size_t listener) noexcept -> void
        {
            if (gPlayers.Acquire<0>(listener))
            {
                if (gPlayers[listener].speakers.Reset(stream))
                {
                    ControlService::Instance().SendPacket(listener);
                }
            }
        });

        // -------------------------------------------------

        gStreams[stream].listeners.Clear();
    }
    else
    {
        // Sending Command To Server...
        // -------------------------------------------------

        if (gStreams[stream].listeners.Test(player))
        {
            const auto command = static_cast<CommandStreamDetachListener*>
                (CommandService::Instance().BeginCommand(CommandPackets::StreamDetachListener));

            command->stream = stream;
            command->player = player;

            CommandService::Instance().EndCommand();
            if (!CommandService::Instance().SendCommand())
                return false;
        }

        // Sending Packet To Player...
        // -------------------------------------------------

        if (gPlayers.Acquire<0>(player))
        {
            if (gPlayers[player].speakers.Reset(stream))
            {
                const auto packet = static_cast<ControlStreamDelete*>
                    (ControlService::Instance().BeginPacket(ControlPackets::StreamDelete));

                packet->stream = stream;

                ControlService::Instance().EndPacket();
                ControlService::Instance().SendPacket(player);
            }
        }

        // -------------------------------------------------

        gStreams[stream].listeners.Reset(player);
    }

    return true;
}

static bool OnHasListener(const uword_t stream, const uword_t player) noexcept
{
    if (gStreams.Acquire<0>(stream) == false)
        return false;

    return player != None<uword_t> ?
        gStreams[stream].listeners.Test(player) :
        gStreams[stream].listeners.Zeros() == false;
}

static void OnSetVolume(const uword_t stream, const fdword_t volume) noexcept
{
    if (gStreams.Acquire<0>(stream))
    {
        if (volume != gStreams[stream].volume)
        {
            // Sending Packet to Player...
            // -------------------------------------------------

            const auto packet = static_cast<ControlStreamSetVolume*>
                (ControlService::Instance().BeginPacket(ControlPackets::StreamSetVolume));

            packet->stream = stream;
            packet->volume = volume;

            ControlService::Instance().EndPacket();

            gStreams[stream].listeners.ForEach([](const size_t listener) noexcept -> void
            {
                ControlService::Instance().SendPacket(listener);
            });

            // -------------------------------------------------
        }

        gStreams[stream].volume = volume;
    }
}

static void OnSetPanning(const uword_t stream, const fdword_t panning) noexcept
{
    if (gStreams.Acquire<0>(stream))
    {
        if (gStreams[stream].distance == 0)
        {
            if (panning != gStreams[stream].panning)
            {
                // Sending Packet to Player...
                // -------------------------------------------------

                const auto packet = static_cast<ControlStreamSetPanning*>
                    (ControlService::Instance().BeginPacket(ControlPackets::StreamSetPanning));

                packet->stream = stream;
                packet->panning = panning;

                ControlService::Instance().EndPacket();

                gStreams[stream].listeners.ForEach([](const size_t listener) noexcept -> void
                {
                    ControlService::Instance().SendPacket(listener);
                });

                // -------------------------------------------------
            }

            gStreams[stream].panning = panning;
        }
    }
}

static void OnSetDistance(const uword_t stream, const fdword_t distance) noexcept
{
    if (gStreams.Acquire<0>(stream))
    {
        if (gStreams[stream].distance != 0)
        {
            if (distance != gStreams[stream].distance)
            {
                // Sending Packet to Player...
                // -------------------------------------------------

                const auto packet = static_cast<ControlStreamSetDistance*>
                    (ControlService::Instance().BeginPacket(ControlPackets::StreamSetDistance));

                packet->stream = stream;
                packet->distance = distance;

                ControlService::Instance().EndPacket();

                gStreams[stream].listeners.ForEach([](const size_t listener) noexcept -> void
                {
                    ControlService::Instance().SendPacket(listener);
                });

                // -------------------------------------------------
            }

            gStreams[stream].distance = distance;
        }
    }
}

static void OnSetPosition(const uword_t stream, const fdword_t x, const fdword_t y, const fdword_t z) noexcept
{
    if (gStreams.Acquire<0>(stream))
    {
        if (gStreams[stream].distance != 0 && gStreams[stream].target == 0)
        {
            // Sending Packet to Player...
            // -------------------------------------------------

            const auto packet = static_cast<ControlStreamSetPosition*>
                (ControlService::Instance().BeginPacket(ControlPackets::StreamSetPosition));

            packet->stream = stream;
            packet->x = x;
            packet->y = y;
            packet->z = z;

            ControlService::Instance().EndPacket();

            gStreams[stream].listeners.ForEach([](const size_t listener) noexcept -> void
            {
                ControlService::Instance().SendPacket(listener);
            });

            // -------------------------------------------------

            gStreams[stream].position = { x, y, z };
        }
    }
}

static void OnSetTarget(const uword_t stream, const uword_t target) noexcept
{
    if (gStreams.Acquire<0>(stream))
    {
        if (gStreams[stream].distance != 0)
        {
            if (target != gStreams[stream].target)
            {
                // Sending Packet to Player...
                // -------------------------------------------------

                const auto packet = static_cast<ControlStreamSetTarget*>
                    (ControlService::Instance().BeginPacket(ControlPackets::StreamSetTarget));

                packet->stream = stream;
                packet->target = target;

                ControlService::Instance().EndPacket();

                gStreams[stream].listeners.ForEach([](const size_t listener) noexcept -> void
                {
                    ControlService::Instance().SendPacket(listener);
                });

                // -------------------------------------------------
            }

            gStreams[stream].target = target;
        }
    }
}

static void OnSetEffect(const uword_t stream, const uword_t effect) noexcept
{
    if (gStreams.Acquire<0>(stream) && gEffects.Acquire<0>(effect))
    {
        if (effect != gStreams[stream].effect)
        {
            bool is_sync_required = false;

            // Create effect for players who don't have it
            {
                // Sending Packet to Player...
                // -------------------------------------------------

                const auto packet = static_cast<ControlEffectCreate*>
                    (ControlService::Instance().BeginPacket(ControlPackets::EffectCreate));

                packet->effect = effect;

                ControlService::Instance().EndPacket();

                gStreams[stream].listeners.ForEach([&](const size_t listener) noexcept -> void
                {
                    if (gPlayers.Acquire<0>(listener))
                    {
                        if (gPlayers[listener].effects.Test(effect) == false)
                        {
                            ControlService::Instance().SendPacket(listener);
                            is_sync_required = true;
                        }
                    }
                });

                // -------------------------------------------------
            }

            if (is_sync_required)
            {
                ForwardForEach(filter, gEffects[effect].filters) // Pass effect filters to players
                {
                    // Sending Packet to Player...
                    // -------------------------------------------------

                    const auto packet = static_cast<ControlEffectAppendFilter*>
                        (ControlService::Instance().BeginPacket(ControlPackets::EffectAppendFilter));

                    packet->effect = effect;
                    packet->type = filter->Type();
                    packet->priority = filter->Priority();
                    std::memcpy(packet->parameters, filter->Parameters().Data(),
                        filter->Parameters().Size());

                    ControlService::Instance().EndPacket();

                    gStreams[stream].listeners.ForEach([=](const size_t listener) noexcept -> void
                    {
                        if (gPlayers.Acquire<0>(listener))
                        {
                            if (gPlayers[listener].effects.Test(effect) == false)
                            {
                                ControlService::Instance().SendPacket(listener);
                            }
                        }
                    });

                    // -------------------------------------------------
                }

                gStreams[stream].listeners.ForEach([=](const size_t listener) noexcept -> void
                {
                    if (gPlayers.Acquire<0>(listener))
                    {
                        gPlayers[listener].effects.Set(effect);
                    }
                });
            }

            // Apply effect to stream
            {
                // Sending Packet to Player...
                // -------------------------------------------------

                const auto packet = static_cast<ControlStreamSetEffect*>
                    (ControlService::Instance().BeginPacket(ControlPackets::StreamSetEffect));

                packet->stream = stream;
                packet->effect = effect;

                ControlService::Instance().EndPacket();

                gStreams[stream].listeners.ForEach([](const size_t listener) noexcept -> void
                {
                    ControlService::Instance().SendPacket(listener);
                });

                // -------------------------------------------------
            }
        }

        gStreams[stream].effect = effect;
    }
}

static void OnSetIcon(const uword_t stream, Block<char>&& icon) noexcept
{
    if (gStreams.Acquire<0>(stream))
    {
        if (icon != gStreams[stream].icon)
        {
            // Sending Packet to Player...
            // -------------------------------------------------

            const auto packet = static_cast<ControlStreamSetIcon*>
                (ControlService::Instance().BeginPacket(ControlPackets::StreamSetIcon));

            packet->stream = stream;
            std::memcpy(packet->icon,
                icon.Data(), icon.Size());

            ControlService::Instance().EndPacket();

            gStreams[stream].listeners.ForEach([](const size_t listener) noexcept -> void
            {
                ControlService::Instance().SendPacket(listener);
            });

            // -------------------------------------------------
        }

        gStreams[stream].icon = std::move(icon);
    }
}

static void OnDeleteStream(const uword_t stream) noexcept
{
    if (gStreams.Acquire<0>(stream))
    {
        if (pNetGame != nullptr && pNetGame->pPlayerPool != nullptr && pNetGame->pPlayerPool->dwConnectedPlayers != 0)
        {
            for (size_t player = 0; player <= pNetGame->pPlayerPool->dwPlayerPoolSize; ++player)
            {
                if (gPlayers.Acquire<0>(player))
                {
                    OnDetachStream(player, stream, ~Zero<udword_t>);
                }
            }
        }

        // Sending Command To Server...
        // -------------------------------------------------

        const auto command = static_cast<CommandStreamDelete*>
            (CommandService::Instance().BeginCommand(CommandPackets::StreamDelete));

        command->stream = stream;

        CommandService::Instance().EndCommand();
        CommandService::Instance().SendCommand();

        // Sending Packet to Player...
        // -------------------------------------------------

        const auto packet = static_cast<ControlStreamDelete*>
            (ControlService::Instance().BeginPacket(ControlPackets::StreamDelete));

        packet->stream = stream;

        ControlService::Instance().EndPacket();

        gStreams[stream].listeners.ForEach([=](const size_t listener) noexcept -> void
        {
            if (gPlayers.Acquire<0>(listener))
            {
                ControlService::Instance().SendPacket(listener);
                gPlayers[listener].speakers.Reset(stream);
            }
        });

        // -------------------------------------------------

        gStreams.Remove(stream, false);
    }
}

// ----------------------------------------------------------------

static uword_t OnCreateEffect() noexcept
{
    return gEffects.Emplace();
}

static bool OnAppendFilter(const uword_t effect, const ubyte_t filter,
    const sword_t priority, Block<ubyte_t>&& parameters) noexcept
{
    if (gEffects.Acquire<0>(effect) == false)
        return false;

    // Sending Packet to Player...
    // -------------------------------------------------

    if (pNetGame != nullptr && pNetGame->pPlayerPool != nullptr && pNetGame->pPlayerPool->dwConnectedPlayers != 0)
    {
        const auto packet = static_cast<ControlEffectAppendFilter*>
            (ControlService::Instance().BeginPacket(ControlPackets::EffectAppendFilter));

        packet->effect = effect;
        packet->type = filter;
        packet->priority = priority;
        std::memcpy(packet->parameters,
            parameters.Data(), parameters.Size());

        ControlService::Instance().EndPacket();

        for (size_t player = 0; player <= pNetGame->pPlayerPool->dwPlayerPoolSize; ++player)
        {
            if (gPlayers.Acquire<0>(player) && gPlayers[player].effects.Test(effect))
            {
                ControlService::Instance().SendPacket(player);
            }
        }
    }

    // -------------------------------------------------

    return gEffects[effect].AppendFilter(filter, priority, std::move(parameters));
}

static void OnRemoveFilter(const uword_t effect, const ubyte_t filter,
    const sword_t priority) noexcept
{
    if (gEffects.Acquire<0>(effect))
    {
        // Sending Packet to Player...
        // -------------------------------------------------

        if (pNetGame != nullptr && pNetGame->pPlayerPool != nullptr && pNetGame->pPlayerPool->dwConnectedPlayers != 0)
        {
            const auto packet = static_cast<ControlEffectRemoveFilter*>
                (ControlService::Instance().BeginPacket(ControlPackets::EffectRemoveFilter));

            packet->effect = effect;
            packet->type = filter;
            packet->priority = priority;

            ControlService::Instance().EndPacket();

            for (size_t player = 0; player <= pNetGame->pPlayerPool->dwPlayerPoolSize; ++player)
            {
                if (gPlayers.Acquire<0>(player) && gPlayers[player].effects.Test(effect))
                {
                    ControlService::Instance().SendPacket(player);
                }
            }
        }

        // -------------------------------------------------

        gEffects[effect].RemoveFilter(filter, priority);
    }
}

static void OnDeleteEffect(const uword_t effect) noexcept
{
    if (gEffects.Acquire<0>(effect))
    {
        for (size_t stream = 0; stream != kMaxStreams; ++stream)
        {
            if (gStreams.Acquire<0>(stream) && gStreams[stream].effect == effect)
            {
                gStreams[stream].effect = None<uword_t>;
            }
        }

        // Sending Packet to Player...
        // -------------------------------------------------

        if (pNetGame != nullptr && pNetGame->pPlayerPool != nullptr && pNetGame->pPlayerPool->dwConnectedPlayers != 0)
        {
            const auto packet = static_cast<ControlEffectDelete*>
                (ControlService::Instance().BeginPacket(ControlPackets::EffectDelete));

            packet->effect = effect;

            ControlService::Instance().EndPacket();

            for (size_t player = 0; player <= pNetGame->pPlayerPool->dwPlayerPoolSize; ++player)
            {
                if (gPlayers.Acquire<0>(player) && gPlayers[player].effects.Reset(effect))
                {
                    ControlService::Instance().SendPacket(player);
                }
            }
        }

        // -------------------------------------------------

        gEffects.Remove(effect, false);
    }
}

// Control Server
// ----------------------------------------------------------------

static void OnControlConnect(const uword_t player, const ControlConnectData& data) noexcept
{
    if (gPlayers.EmplaceAt(player, true, data.version, data.micro != 0))
    {
        udword_t voice_key;
        do voice_key = utils::crypto::random();
        while (voice_key == 0);

        // Sending Command to Server...
        // -------------------------------------------------
        
        const auto command = static_cast<CommandPlayerCreate*>
            (CommandService::Instance().BeginCommand(CommandPackets::PlayerCreate));

        command->player = player;
        command->key = voice_key;

        CommandService::Instance().EndCommand();
        CommandService::Instance().SendCommand();

        // Sending Packet to Player...
        // -------------------------------------------------
        
        const auto packet = static_cast<ControlClientInitialize*>
            (ControlService::Instance().BeginPacket(ControlPackets::ClientInitialize));

        packet->voice_key = voice_key;
        packet->voice_host = gVoiceAddress.GetHost<true>();
        packet->voice_port = gVoiceAddress.GetPort<true>();
        packet->voice_id = player;

        ControlService::Instance().EndPacket();
        ControlService::Instance().SendPacket(player);
    }
}

static void OnControlDisconnect(const uword_t player) noexcept
{
    if (gPlayers.Acquire<0>(player))
    {
        // Sending Command to Server...
        // -------------------------------------------------

        const auto command = static_cast<CommandPlayerDelete*>
            (CommandService::Instance().BeginCommand(CommandPackets::PlayerDelete));

        command->player = player;

        CommandService::Instance().EndCommand();
        CommandService::Instance().SendCommand();

        // Processing...
        // -------------------------------------------------

        for (size_t stream = 0; stream != kMaxStreams; ++stream)
        {
            if (gStreams.Acquire<0>(stream))
            {
                if (gStreams[stream].target == MakeTarget(kPlayer, player))
                {
                    // Sending Packet to Player...
                    // -------------------------------------------------

                    const auto packet = static_cast<ControlStreamSetTarget*>
                        (ControlService::Instance().BeginPacket(ControlPackets::StreamSetTarget));

                    packet->stream = stream;
                    packet->target = Zero<uword_t>;

                    ControlService::Instance().EndPacket();

                    gStreams[stream].listeners.ForEach([=](const size_t listener) noexcept -> void
                    {
                        if (listener != player)
                        {
                            ControlService::Instance().SendPacket(listener);
                        }
                    });

                    // -------------------------------------------------

                    gStreams[stream].target = Zero<uword_t>;
                }
            }
        }

        gPlayers[player].speakers.ForEach([=](const size_t stream) noexcept -> void
        {
            if (gStreams.Acquire<0>(stream))
            {
                gStreams[stream].listeners.Reset(player);
            }
        });

        gPlayers.Remove(player, false);
    }
}

// Main Loop
// ----------------------------------------------------------------

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() noexcept
{
    CommandService::Instance().Tick();

    if (pNetGame != nullptr)
    {
        for (size_t stream = 0; stream != kMaxStreams; ++stream)
        {
            if (gStreams.Acquire<0>(stream) == false)
                continue;

            if (gStreams[stream].target == 0)
                continue;

            const uword_t type  = GetTargetType  (gStreams[stream].target);
            const uword_t index = GetTargetIndex (gStreams[stream].target);

            switch (type)
            {
                case kVehicle:
                {
                    if (pNetGame->pVehiclePool == nullptr) break;
                    if (pNetGame->pVehiclePool->pVehicle[index] == nullptr) break;

                    gStreams[stream].position = pNetGame->pVehiclePool->pVehicle[index]->vecPosition;

                    if (pNetGame->pPlayerPool == nullptr) break;
                    if (pNetGame->pPlayerPool->dwConnectedPlayers == 0) break;

                    for (size_t player = 0; player <= pNetGame->pPlayerPool->dwPlayerPoolSize; ++player)
                    {
                        if (gPlayers.Acquire<0>(player) == false)
                            goto VehicleDetach;

                        assert(pNetGame->pPlayerPool->pPlayer[player] != nullptr);

                        if (pNetGame->pPlayerPool->pPlayer[player]->byteVehicleStreamedIn[index] == 0)
                            goto VehicleDetach;

                        {
                            const CVector delta = pNetGame->pPlayerPool->pPlayer[player]->vecPosition -
                                gStreams[stream].position;

                            if (delta.fX * delta.fX + delta.fY * delta.fY +
                                delta.fZ * delta.fZ > gStreams[stream].distance * gStreams[stream].distance)
                                goto VehicleDetach;
                        }

                        if (gStreams[stream].listeners.Test(player) == false)
                        {
                            OnAttachListener(stream, player);
                        }

                        continue;

                    VehicleDetach:

                        if (gStreams[stream].listeners.Test(player) == true)
                        {
                            OnDetachListener(stream, player);
                        }
                    }

                    break;
                }
                case kPlayer:
                {
                    if (pNetGame->pPlayerPool == nullptr) break;
                    if (pNetGame->pPlayerPool->pPlayer[index] == nullptr) break;

                    gStreams[stream].position = pNetGame->pPlayerPool->pPlayer[index]->vecPosition;

                    if (pNetGame->pPlayerPool->dwConnectedPlayers == 0) break;

                    for (size_t player = 0; player <= pNetGame->pPlayerPool->dwPlayerPoolSize; ++player)
                    {
                        if (gPlayers.Acquire<0>(player) == false)
                            goto PlayerDetach;

                        assert(pNetGame->pPlayerPool->pPlayer[player] != nullptr);

                        if (pNetGame->pPlayerPool->pPlayer[player]->byteStreamedIn[index] == 0)
                            goto PlayerDetach;

                        {
                            const CVector delta = pNetGame->pPlayerPool->pPlayer[player]->vecPosition -
                                gStreams[stream].position;

                            if (delta.fX * delta.fX + delta.fY * delta.fY +
                                delta.fZ * delta.fZ > gStreams[stream].distance * gStreams[stream].distance)
                                goto PlayerDetach;
                        }

                        if (gStreams[stream].listeners.Test(player) == false)
                        {
                            OnAttachListener(stream, player);
                        }

                        continue;

                    PlayerDetach:

                        if (gStreams[stream].listeners.Test(player) == true)
                        {
                            OnDetachListener(stream, player);
                        }
                    }

                    break;
                }
                case kObject:
                {
                    if (pNetGame->pObjectPool == nullptr) break;
                    if (pNetGame->pObjectPool->pObjects[index] == nullptr) break;

                    gStreams[stream].position = pNetGame->pObjectPool->pObjects[index]->matWorld.pos;

                    if (pNetGame->pPlayerPool == nullptr) break;
                    if (pNetGame->pPlayerPool->dwConnectedPlayers == 0) break;

                    for (size_t player = 0; player <= pNetGame->pPlayerPool->dwPlayerPoolSize; ++player)
                    {
                        if (gPlayers.Acquire<0>(player) == false)
                            goto ObjectDetach;

                        assert(pNetGame->pPlayerPool->pPlayer[player] != nullptr);

                        {
                            const CVector delta = pNetGame->pPlayerPool->pPlayer[player]->vecPosition -
                                gStreams[stream].position;

                            if (delta.fX * delta.fX + delta.fY * delta.fY +
                                delta.fZ * delta.fZ > gStreams[stream].distance * gStreams[stream].distance)
                                goto ObjectDetach;
                        }

                        if (gStreams[stream].listeners.Test(player) == false)
                        {
                            OnAttachListener(stream, player);
                        }

                        continue;

                    ObjectDetach:

                        if (gStreams[stream].listeners.Test(player) == true)
                        {
                            OnDetachListener(stream, player);
                        }
                    }

                    break;
                }
            }
        }
    }
}

// Plugin Interface
// ----------------------------------------------------------------

PLUGIN_EXPORT void PLUGIN_CALL Unload() noexcept
{
    static bool unload_status = false;
    if (unload_status) return;
    unload_status = true;

    Logger::Instance().Log(" ------------------------------------------");
    Logger::Instance().Log("           SampVoice unloading...          ");
    Logger::Instance().Log(" ------------------------------------------");

    ControlService::Instance().Deinitialize();
    CommandService::Instance().Deinitialize();

    Logger::Instance().Deinitialize();
}

#ifdef _WIN32
static BOOL WINAPI WinExitHandler(const DWORD) noexcept
{
    return Unload(), FALSE;
}
#endif

PLUGIN_EXPORT bool PLUGIN_CALL Load(void** const ppData) noexcept
{
#ifdef _WIN32
    SetConsoleCtrlHandler(&WinExitHandler, TRUE);
#endif

    ppPluginData = ppData;
    pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
    logprintf = reinterpret_cast<logprintf_t>(ppData[PLUGIN_DATA_LOGPRINTF]);

    // Logger
    // ----------------------------------------------------------------

    if (!Logger::Instance().Initialize("control.log", logprintf))
    {
        logprintf("[sv:err:plugin] : failed to initialize logger");
        return false;
    }

    // Config
    // ----------------------------------------------------------------

    enum : size_t
    {
        PARAMETER_CONTROL_HOST,
        PARAMETER_CONTROL_PORT,
        PARAMETER_COMMAND_HOST,
        PARAMETER_COMMAND_PORT,
        PARAMETER_VOICE_HOST,
        PARAMETER_VOICE_PORT,

        PARAMETERS_COUNT
    };

    Config config;
    {
        using namespace std::literals;

        config.Reserve(PARAMETERS_COUNT);

        config.Register(PARAMETER_CONTROL_HOST, "control_host"s);
        config.Register(PARAMETER_CONTROL_PORT, "control_port"s);
        config.Register(PARAMETER_COMMAND_HOST, "command_host"s);
        config.Register(PARAMETER_COMMAND_PORT, "command_port"s);
        config.Register(PARAMETER_VOICE_HOST, "voice_host"s);
        config.Register(PARAMETER_VOICE_PORT, "voice_port"s);

        if (config.Load("control.cfg") < 0)
        {
            Logger::Instance().Log("Failed to load 'control.cfg'.");
            Logger::Instance().Deinitialize();
            return false;
        }
    }

    // Command Service
    // ----------------------------------------------------------------

    {
        IPv4Address control = IPv4Address::Loopback();
        IPv4Address command = IPv4Address::Loopback(2020);

        if (config.HasParameter(PARAMETER_CONTROL_HOST))
            control.SetHost(config.GetValueByIndex(PARAMETER_CONTROL_HOST).c_str());
        if (config.HasParameter(PARAMETER_CONTROL_PORT))
            control.SetPort(std::stoi(config.GetValueByIndex(PARAMETER_CONTROL_PORT)));

        if (config.HasParameter(PARAMETER_COMMAND_HOST))
            command.SetHost(config.GetValueByIndex(PARAMETER_COMMAND_HOST).c_str());
        if (config.HasParameter(PARAMETER_COMMAND_PORT))
            command.SetPort(std::stoi(config.GetValueByIndex(PARAMETER_COMMAND_PORT)));

        gVoiceAddress = IPv4Address::Any(2020);

        if (config.HasParameter(PARAMETER_VOICE_HOST))
            gVoiceAddress.SetHost(config.GetValueByIndex(PARAMETER_VOICE_HOST).c_str());
        if (config.HasParameter(PARAMETER_VOICE_PORT))
            gVoiceAddress.SetPort(std::stoi(config.GetValueByIndex(PARAMETER_VOICE_PORT)));

        Logger::Instance().Log("[sv:dbg:plugin] : connecting to command service...");

        if (!CommandService::Instance().Initialize(control, command))
        {
            Logger::Instance().Log("[sv:err:plugin] : failed to initialize command service");
            Logger::Instance().Deinitialize();
            return false;
        }
    }

    // Control Service
    // ----------------------------------------------------------------

    ControlService::Instance().OnConnect    = OnControlConnect;
    ControlService::Instance().OnDisconnect = OnControlDisconnect;

    if (!ControlService::Instance().Initialize(logprintf))
    {
        Logger::Instance().Log("[sv:err:plugin] : failed to initialize control service");
        CommandService::Instance().Deinitialize();
        Logger::Instance().Deinitialize();
        return false;
    }

    // Pawn Interface
    // ----------------------------------------------------------------

    Pawn::Instance().OnGetVersion       = OnGetVersion;
    Pawn::Instance().OnHasMicro         = OnHasMicro;
    Pawn::Instance().OnEnableListener   = OnEnableListener;
    Pawn::Instance().OnDisableListener  = OnDisableListener;
    Pawn::Instance().OnCheckListener    = OnCheckListener;
    Pawn::Instance().OnEnableSpeaker    = OnEnableSpeaker;
    Pawn::Instance().OnDisableSpeaker   = OnDisableSpeaker;
    Pawn::Instance().OnCheckSpeaker     = OnCheckSpeaker;
    Pawn::Instance().OnAttachStream     = OnAttachStream;
    Pawn::Instance().OnDetachStream     = OnDetachStream;
    Pawn::Instance().OnHasStream        = OnHasStream;
    Pawn::Instance().OnSetKey           = OnSetKey;
    Pawn::Instance().OnGetKey           = OnGetKey;
    Pawn::Instance().OnPlay             = OnPlay;
    Pawn::Instance().OnStop             = OnStop;

    Pawn::Instance().OnCreateStream     = OnCreateStream;
    Pawn::Instance().OnEnableTransiter  = OnEnableTransiter;
    Pawn::Instance().OnDisableTransiter = OnDisableTransiter;
    Pawn::Instance().OnCheckTransiter   = OnCheckTransiter;
    Pawn::Instance().OnAttachListener   = OnAttachListener;
    Pawn::Instance().OnDetachListener   = OnDetachListener;
    Pawn::Instance().OnHasListener      = OnHasListener;
    Pawn::Instance().OnSetVolume        = OnSetVolume;
    Pawn::Instance().OnSetPanning       = OnSetPanning;
    Pawn::Instance().OnSetDistance      = OnSetDistance;
    Pawn::Instance().OnSetPosition      = OnSetPosition;
    Pawn::Instance().OnSetTarget        = OnSetTarget;
    Pawn::Instance().OnSetEffect        = OnSetEffect;
    Pawn::Instance().OnSetIcon          = OnSetIcon;
    Pawn::Instance().OnDeleteStream     = OnDeleteStream;

    Pawn::Instance().OnCreateEffect     = OnCreateEffect;
    Pawn::Instance().OnAppendFilter     = OnAppendFilter;
    Pawn::Instance().OnRemoveFilter     = OnRemoveFilter;
    Pawn::Instance().OnDeleteEffect     = OnDeleteEffect;

    // ----------------------------------------------------------------

    Logger::Instance().Log(" -------------------------------------------    ");
    Logger::Instance().Log("   ___                __   __    _              ");
    Logger::Instance().Log("  / __| __ _ _ __  _ _\\ \\ / /__ (_) __ ___    ");
    Logger::Instance().Log("  \\__ \\/ _` | '  \\| '_ \\   / _ \\| |/ _/ -_)");
    Logger::Instance().Log("  |___/\\__,_|_|_|_| .__/\\_/\\___/|_|\\__\\___|");
    Logger::Instance().Log("                  |_|                           ");
    Logger::Instance().Log(" -------------------------------------------    ");
    Logger::Instance().Log("           SampVoice by MOR loaded              ");
    Logger::Instance().Log(" -------------------------------------------    ");

    return true;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX* const amx) noexcept
{
    if (pNetGame == nullptr)
        pNetGame = reinterpret_cast<CNetGame*(*)()>(ppPluginData[PLUGIN_DATA_NETGAME])();

    if (!Pawn::Instance().RegisterScript(amx))
    {
        Logger::Instance().Log("[sv:err:plugin] : failed to register script (error:%d)", amx->error);
    }

    return AMX_ERR_NONE;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX* const) noexcept
{
    return AMX_ERR_NONE;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() noexcept
{
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}
