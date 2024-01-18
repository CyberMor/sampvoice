/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <functional>
#include <memory>

#include <pawn/amx/amx.h>
#include <pawn/plugincommon.h>
#include <util/logger.hpp>

#include "effect.hpp"
#include "stream.hpp"

struct Pawn {

    Pawn(const Pawn&) = delete;
    Pawn(Pawn&&) = delete;
    Pawn& operator=(const Pawn&) = delete;
    Pawn& operator=(Pawn&&) = delete;

private:

    Pawn() noexcept = default;
    ~Pawn() noexcept = default;

public:

    static Pawn& Instance() noexcept
    {
        static Pawn instance;
        return instance;
    }

public:

    void RegisterScript(AMX* const amx) noexcept
    {
        const AMX_NATIVE_INFO natives[]
        {
#define DefineNativeFunction(function) { #function, &n_##function }

            // Debug
            // --------------------------------

            DefineNativeFunction(SvEnableDebug),
            DefineNativeFunction(SvDisableDebug),
            DefineNativeFunction(SvCheckDebug),

            // Player
            // --------------------------------

            DefineNativeFunction(SvGetVersion),
            DefineNativeFunction(SvHasMicro),

            DefineNativeFunction(SvEnableListener),
            DefineNativeFunction(SvDisableListener),
            DefineNativeFunction(SvCheckListener),

            DefineNativeFunction(SvEnableSpeaker),
            DefineNativeFunction(SvDisableSpeaker),
            DefineNativeFunction(SvCheckSpeaker),

            DefineNativeFunction(SvAttachStream),
            DefineNativeFunction(SvDetachStream),
            DefineNativeFunction(SvHasStream),

            DefineNativeFunction(SvSetKey),
            DefineNativeFunction(SvGetKey),

            DefineNativeFunction(SvPlay),
            DefineNativeFunction(SvStop),

            // Stream
            // --------------------------------

            DefineNativeFunction(SvCreateStream),

            DefineNativeFunction(SvEnableTransiter),
            DefineNativeFunction(SvDisableTransiter),
            DefineNativeFunction(SvCheckTransiter),

            DefineNativeFunction(SvAttachListener),
            DefineNativeFunction(SvDetachListener),
            DefineNativeFunction(SvHasListener),

            DefineNativeFunction(SvSetVolume),
            DefineNativeFunction(SvSetPanning),
            DefineNativeFunction(SvSetDistance),
            DefineNativeFunction(SvSetPosition),
            DefineNativeFunction(SvSetTarget),
            DefineNativeFunction(SvSetEffect),
            DefineNativeFunction(SvSetIcon),

            DefineNativeFunction(SvDeleteStream),

            // Effect
            // --------------------------------

            DefineNativeFunction(SvCreateEffect),

            DefineNativeFunction(SvAppendFilter),
            DefineNativeFunction(SvRemoveFilter),

            DefineNativeFunction(SvDeleteEffect)

#undef  DefineNativeFunction
        };

        amx_Register(amx, natives, std::size(natives));
    }

private:

    // Debug
    // ----------------------------------------------------------------

    static cell AMX_NATIVE_CALL n_SvEnableDebug(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 0 * sizeof(cell))
            return None<cell>;

        if (Instance()._debug == false)
            Logger::Instance().Log("[sv:dbg:pawn:SvEnableDebug]");

        Instance()._debug = true;

        return Zero<cell>;
    }

    static cell AMX_NATIVE_CALL n_SvDisableDebug(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 0 * sizeof(cell))
            return None<cell>;

        if (Instance()._debug == true)
            Logger::Instance().Log("[sv:dbg:pawn:SvDisableDebug]");

        Instance()._debug = false;

        return Zero<cell>;
    }

    static cell AMX_NATIVE_CALL n_SvCheckDebug(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 0 * sizeof(cell))
            return None<cell>;

        if (Instance()._debug) Logger::Instance().Log("[sv:dbg:pawn:SvCheckDebug]");

        return static_cast<cell>(Instance()._debug);
    }

    // Player
    // ----------------------------------------------------------------

    static cell AMX_NATIVE_CALL n_SvGetVersion(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 1 * sizeof(cell))
            return None<cell>;

        const auto player = static_cast<uword_t>(params[1]);
        if (player >= kMaxPlayers) return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvGetVersion:begin] : player(%hu)", player);

        const auto result = Instance().OnGetVersion(player);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvGetVersion:end] : result(0x%X)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvHasMicro(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 1 * sizeof(cell))
            return None<cell>;

        const auto player = static_cast<uword_t>(params[1]);
        if (player >= kMaxPlayers) return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvHasMicro:begin] : player(%hu)", player);

        const auto result = Instance().OnHasMicro(player);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvHasMicro:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvEnableListener(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 1 * sizeof(cell))
            return None<cell>;

        const auto player = static_cast<uword_t>(params[1]);
        if (player >= kMaxPlayers) return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvEnableListener:begin] : player(%hu)", player);

        const auto result = Instance().OnEnableListener(player);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvEnableListener:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvDisableListener(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 1 * sizeof(cell))
            return None<cell>;

        const auto player = static_cast<uword_t>(params[1]);
        if (player >= kMaxPlayers) return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvDisableListener:begin] : player(%hu)", player);

        const auto result = Instance().OnDisableListener(player);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvDisableListener:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvCheckListener(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 1 * sizeof(cell))
            return None<cell>;

        const auto player = static_cast<uword_t>(params[1]);
        if (player >= kMaxPlayers) return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvCheckListener:begin] : player(%hu)", player);

        const auto result = Instance().OnCheckListener(player);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvCheckListener:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvEnableSpeaker(AMX* const amx, cell* const params) noexcept
    {
        if (*params < 1 * sizeof(cell))
            return None<cell>;

        const auto player = static_cast<uword_t>(params[1]);
        if (player >= kMaxPlayers) return None<cell>;

        udword_t channels = *params == 1 * sizeof(cell) ? ~Zero<udword_t> : Zero<udword_t>;
        if (*params != 1 * sizeof(cell)) for (size_t i = 0; i != *params / sizeof(cell) - 1; ++i)
            if (cell* physical_address; amx_GetAddr(amx, params[2 + i], &physical_address) == 0)
                channels |= HighBit<udword_t> >> *physical_address;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvEnableSpeaker:begin] : player(%hu), channels(0x%X)",
             player, channels);

        const auto result = Instance().OnEnableSpeaker(player, channels);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvEnableSpeaker:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvDisableSpeaker(AMX* const amx, cell* const params) noexcept
    {
        if (*params < 1 * sizeof(cell))
            return None<cell>;

        const auto player = static_cast<uword_t>(params[1]);
        if (player >= kMaxPlayers) return None<cell>;

        udword_t channels = *params == 1 * sizeof(cell) ? ~Zero<udword_t> : Zero<udword_t>;
        if (*params != 1 * sizeof(cell)) for (size_t i = 0; i != *params / sizeof(cell) - 1; ++i)
            if (cell* physical_address; amx_GetAddr(amx, params[2 + i], &physical_address) == 0)
                channels |= HighBit<udword_t> >> *physical_address;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvDisableSpeaker:begin] : player(%hu), channels(0x%X)",
             player, channels);

        const auto result = Instance().OnDisableSpeaker(player, channels);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvDisableSpeaker:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvCheckSpeaker(AMX* const amx, cell* const params) noexcept
    {
        if (*params < 1 * sizeof(cell))
            return None<cell>;

        const auto player = static_cast<uword_t>(params[1]);
        if (player >= kMaxPlayers) return None<cell>;

        udword_t channels = *params == 1 * sizeof(cell) ? ~Zero<udword_t> : Zero<udword_t>;
        if (*params != 1 * sizeof(cell)) for (size_t i = 0; i != *params / sizeof(cell) - 1; ++i)
            if (cell* physical_address; amx_GetAddr(amx, params[2 + i], &physical_address) == 0)
                channels |= HighBit<udword_t> >> *physical_address;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvCheckSpeaker:begin] : player(%hu), channels(0x%X)",
             player, channels);

        const auto result = Instance().OnCheckSpeaker(player, channels);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvCheckSpeaker:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvAttachStream(AMX* const amx, cell* const params) noexcept
    {
        if (*params < 2 * sizeof(cell))
            return None<cell>;

        const auto player = static_cast<uword_t>(params[1]);
        if (player >= kMaxPlayers) return None<cell>;

        const auto stream = static_cast<uword_t>(params[2]);
        if (stream >= kMaxStreams) return None<cell>;

        udword_t channels = *params == 2 * sizeof(cell) ? HighBit<udword_t> : Zero<udword_t>;
        if (*params != 2 * sizeof(cell)) for (size_t i = 0; i != *params / sizeof(cell) - 2; ++i)
            if (cell* physical_address; amx_GetAddr(amx, params[3 + i], &physical_address) == 0)
                channels |= HighBit<udword_t> >> *physical_address;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvAttachStream:begin] : player(%hu), stream(%hu), channels(0x%X)",
             player, stream, channels);

        const auto result = Instance().OnAttachStream(player, stream, channels);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvAttachStream:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvDetachStream(AMX* const amx, cell* const params) noexcept
    {
        if (*params < 2 * sizeof(cell))
            return None<cell>;

        const auto player = static_cast<uword_t>(params[1]);
        if (player >= kMaxPlayers) return None<cell>;

        const auto stream = static_cast<uword_t>(params[2]);
        if (stream >= kMaxStreams && stream != None<uword_t>)
            return None<cell>;

        udword_t channels = *params == 2 * sizeof(cell) ? ~Zero<udword_t> : Zero<udword_t>;
        if (*params != 2 * sizeof(cell)) for (size_t i = 0; i != *params / sizeof(cell) - 2; ++i)
            if (cell* physical_address; amx_GetAddr(amx, params[3 + i], &physical_address) == 0)
                channels |= HighBit<udword_t> >> *physical_address;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvDetachStream:begin] : player(%hu), stream(%hu), channels(0x%X)",
             player, stream, channels);

        const auto result = Instance().OnDetachStream(player, stream, channels);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvDetachStream:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvHasStream(AMX* const amx, cell* const params) noexcept
    {
        if (*params < 2 * sizeof(cell))
            return None<cell>;

        const auto player = static_cast<uword_t>(params[1]);
        if (player >= kMaxPlayers) return None<cell>;

        const auto stream = static_cast<uword_t>(params[2]);
        if (stream >= kMaxStreams && stream != None<uword_t>)
            return None<cell>;

        udword_t channels = *params == 2 * sizeof(cell) ? ~Zero<udword_t> : Zero<udword_t>;
        if (*params != 2 * sizeof(cell)) for (size_t i = 0; i != *params / sizeof(cell) - 2; ++i)
            if (cell* physical_address; amx_GetAddr(amx, params[3 + i], &physical_address) == 0)
                channels |= HighBit<udword_t> >> *physical_address;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvHasStream:begin] : player(%hu), stream(%hu), channels(0x%X)",
             player, stream, channels);

        const auto result = Instance().OnHasStream(player, stream, channels);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvHasStream:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvSetKey(AMX* const amx, cell* const params) noexcept
    {
        if (*params < 2 * sizeof(cell))
            return None<cell>;

        const auto player = static_cast<uword_t>(params[1]);
        if (player >= kMaxPlayers) return None<cell>;

        const auto key = static_cast<ubyte_t>(params[2]);

        udword_t channels = *params == 2 * sizeof(cell) ? ~Zero<udword_t> : Zero<udword_t>;
        if (*params != 2 * sizeof(cell)) for (size_t i = 0; i != *params / sizeof(cell) - 2; ++i)
            if (cell* physical_address; amx_GetAddr(amx, params[3 + i], &physical_address) == 0)
                channels |= HighBit<udword_t> >> *physical_address;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvSetKey:begin] : player(%hu), key(0x%hhX), channels(0x%X)",
             player, key, channels);

        const auto result = Instance().OnSetKey(player, key, channels);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvSetKey:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvGetKey(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 2 * sizeof(cell))
            return None<cell>;

        const auto player = static_cast<uword_t>(params[1]);
        if (player >= kMaxPlayers) return None<cell>;

        const auto channel = static_cast<ubyte_t>(params[2]);
        if (channel >= Bits<udword_t>) return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvGetKey:begin] : player(%hu), channel(%hhu)",
             player, channel);

        const auto result = Instance().OnGetKey(player, channel);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvGetKey:end] : result(0x%hhX)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvPlay(AMX* const amx, cell* const params) noexcept
    {
        if (*params < 1 * sizeof(cell))
            return None<cell>;

        const auto player = static_cast<uword_t>(params[1]);
        if (player >= kMaxPlayers) return None<cell>;

        udword_t channels = *params == 1 * sizeof(cell) ? ~Zero<udword_t> : Zero<udword_t>;
        if (*params != 1 * sizeof(cell)) for (size_t i = 0; i != *params / sizeof(cell) - 1; ++i)
            if (cell* physical_address; amx_GetAddr(amx, params[2 + i], &physical_address) == 0)
                channels |= HighBit<udword_t> >> *physical_address;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvPlay:begin] : player(%hu), channels(0x%X)",
             player, channels);

        const auto result = Instance().OnPlay(player, channels);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvPlay:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvStop(AMX* const amx, cell* const params) noexcept
    {
        if (*params < 1 * sizeof(cell))
            return None<cell>;

        const auto player = static_cast<uword_t>(params[1]);
        if (player >= kMaxPlayers) return None<cell>;

        udword_t channels = *params == 1 * sizeof(cell) ? ~Zero<udword_t> : Zero<udword_t>;
        if (*params != 1 * sizeof(cell)) for (size_t i = 0; i != *params / sizeof(cell) - 1; ++i)
            if (cell* physical_address; amx_GetAddr(amx, params[2 + i], &physical_address) == 0)
                channels |= HighBit<udword_t> >> *physical_address;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvStop:begin] : player(%hu), channels(0x%X)",
             player, channels);

        const auto result = Instance().OnStop(player, channels);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvStop:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    // Stream
    // ----------------------------------------------------------------

    static cell AMX_NATIVE_CALL n_SvCreateStream(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 1 * sizeof(cell))
            return None<cell>;

        const auto distance = amx_ctof(params[1]);
        if (distance < Zero<float>) return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvCreateStream:begin]");

        const auto result = Instance().OnCreateStream(distance);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvCreateStream:end] : result(%hu)", result);

        return result != None<decltype(result)> ? static_cast<cell>(result) : None<cell>;
    }

    static cell AMX_NATIVE_CALL n_SvEnableTransiter(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 1 * sizeof(cell))
            return None<cell>;

        const auto stream = static_cast<uword_t>(params[1]);
        if (stream >= kMaxStreams) return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvEnableTransiter:begin] : stream(%hu)", stream);

        const auto result = Instance().OnEnableTransiter(stream);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvEnableTransiter:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvDisableTransiter(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 1 * sizeof(cell))
            return None<cell>;

        const auto stream = static_cast<uword_t>(params[1]);
        if (stream >= kMaxStreams) return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvDisableTransiter:begin] : stream(%hu)", stream);

        const auto result = Instance().OnDisableTransiter(stream);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvDisableTransiter:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvCheckTransiter(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 1 * sizeof(cell))
            return None<cell>;

        const auto stream = static_cast<uword_t>(params[1]);
        if (stream >= kMaxStreams) return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvCheckTransiter:begin] : stream(%hu)", stream);

        const auto result = Instance().OnCheckTransiter(stream);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvCheckTransiter:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvAttachListener(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 2 * sizeof(cell))
            return None<cell>;

        const auto stream = static_cast<uword_t>(params[1]);
        if (stream >= kMaxStreams) return None<cell>;

        const auto player = static_cast<uword_t>(params[2]);
        if (player >= kMaxPlayers) return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvAttachListener:begin] : stream(%hu), player(%hu)",
             stream, player);

        const auto result = Instance().OnAttachListener(stream, player);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvAttachListener:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvDetachListener(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 2 * sizeof(cell))
            return None<cell>;

        const auto stream = static_cast<uword_t>(params[1]);
        if (stream >= kMaxStreams) return None<cell>;

        const auto player = static_cast<uword_t>(params[2]);
        if (player >= kMaxPlayers && player != None<uword_t>)
            return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvDetachListener:begin] : stream(%hu), player(%hu)",
             stream, player);

        const auto result = Instance().OnDetachListener(stream, player);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvDetachListener:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvHasListener(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 2 * sizeof(cell))
            return None<cell>;

        const auto stream = static_cast<uword_t>(params[1]);
        if (stream >= kMaxStreams) return None<cell>;

        const auto player = static_cast<uword_t>(params[2]);
        if (player >= kMaxPlayers && player != None<uword_t>)
            return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvHasListener:begin] : stream(%hu), player(%hu)",
             stream, player);

        const auto result = Instance().OnHasListener(stream, player);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvHasListener:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvSetVolume(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 2 * sizeof(cell))
            return None<cell>;

        const auto stream = static_cast<uword_t>(params[1]);
        if (stream >= kMaxStreams) return None<cell>;

        const auto volume = amx_ctof(params[2]);
        if (volume < Zero<float>) return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvSetVolume:begin] : stream(%hu), volume(%.2f)",
             stream, volume);

        Instance().OnSetVolume(stream, volume);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvSetVolume:end]");

        return Zero<cell>;
    }

    static cell AMX_NATIVE_CALL n_SvSetPanning(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 2 * sizeof(cell))
            return None<cell>;

        const auto stream = static_cast<uword_t>(params[1]);
        if (stream >= kMaxStreams) return None<cell>;

        const auto panning = amx_ctof(params[2]);
        if (panning < -1.0F || panning > 1.0F)
            return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvSetPanning:begin] : stream(%hu), panning(%.2f)",
             stream, panning);

        Instance().OnSetPanning(stream, panning);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvSetPanning:end]");

        return Zero<cell>;
    }

    static cell AMX_NATIVE_CALL n_SvSetDistance(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 2 * sizeof(cell))
            return None<cell>;

        const auto stream = static_cast<uword_t>(params[1]);
        if (stream >= kMaxStreams) return None<cell>;

        const auto distance = amx_ctof(params[2]);
        if (distance <= Zero<float>) return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvSetDistance:begin] : stream(%hu), distance(%.2f)",
             stream, distance);

        Instance().OnSetDistance(stream, distance);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvSetDistance:end]");

        return Zero<cell>;
    }

    static cell AMX_NATIVE_CALL n_SvSetPosition(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 4 * sizeof(cell))
            return None<cell>;

        const auto stream = static_cast<uword_t>(params[1]);
        if (stream >= kMaxStreams) return None<cell>;

        const auto x = amx_ctof(params[2]);
        const auto y = amx_ctof(params[3]);
        const auto z = amx_ctof(params[4]);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvSetPosition:begin] : stream(%hu), x(%.2f), y(%.2f), z(%.2f)",
             stream, x, y, z);

        Instance().OnSetPosition(stream, x, y, z);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvSetPosition:end]");

        return Zero<cell>;
    }

    static cell AMX_NATIVE_CALL n_SvSetTarget(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 2 * sizeof(cell))
            return None<cell>;

        const auto stream = static_cast<uword_t>(params[1]);
        if (stream >= kMaxStreams) return None<cell>;

        const auto target = static_cast<uword_t>(params[2]);
        if (target != 0 && (GetTargetType(target) != kVehicle || GetTargetIndex(target) >= MAX_VEHICLES) &&
                           (GetTargetType(target) != kPlayer  || GetTargetIndex(target) >= MAX_PLAYERS)  &&
                           (GetTargetType(target) != kObject  || GetTargetIndex(target) >= MAX_OBJECTS))
        {
            return None<cell>;
        }

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvSetTarget:begin] : stream(%hu), target(%hu)",
             stream, target);

        Instance().OnSetTarget(stream, target);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvSetTarget:end]");

        return Zero<cell>;
    }

    static cell AMX_NATIVE_CALL n_SvSetEffect(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 2 * sizeof(cell))
            return None<cell>;

        const auto stream = static_cast<uword_t>(params[1]);
        if (stream >= kMaxStreams) return None<cell>;

        const auto effect = static_cast<uword_t>(params[2]);
        if (effect >= kMaxEffects && effect != None<uword_t>)
            return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvSetEffect:begin] : stream(%hu), effect(%hu)",
             stream, effect);

        Instance().OnSetEffect(stream, effect);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvSetEffect:end]");

        return Zero<cell>;
    }

    static cell AMX_NATIVE_CALL n_SvSetIcon(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 2 * sizeof(cell))
            return None<cell>;

        const auto stream = static_cast<uword_t>(params[1]);
        if (stream >= kMaxStreams) return None<cell>;

        cell* physical_address; int string_length;
        if (amx_GetAddr(amx, params[2], &physical_address) != 0 ||
            amx_StrLen(physical_address, &string_length) != 0)
            return None<cell>;

        assert(string_length >= 0);
        auto string_buffer = Block<char>::FromHeap(string_length + 1);
        if (string_buffer.Invalid()) return None<cell>;

        if (amx_GetString(string_buffer.Data(), physical_address, false, string_buffer.Size()) != 0)
            return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvSetIcon:begin] : stream(%hu), icon(%s)",
             stream, string_buffer.Data());

        Instance().OnSetIcon(stream, std::move(string_buffer));

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvSetIcon:end]");

        return Zero<cell>;
    }

    static cell AMX_NATIVE_CALL n_SvDeleteStream(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 1 * sizeof(cell))
            return None<cell>;

        const auto stream = static_cast<uword_t>(params[1]);
        if (stream >= kMaxStreams) return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvDeleteStream:begin] : stream(%hu)", stream);

        Instance().OnDeleteStream(stream);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvDeleteStream:end]");

        return Zero<cell>;
    }

    // Effect
    // ----------------------------------------------------------------

    static cell AMX_NATIVE_CALL n_SvCreateEffect(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 0 * sizeof(cell))
            return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvCreateEffect:begin]");

        const auto result = Instance().OnCreateEffect();

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvCreateEffect:end] : result(%hu)", result);

        return result != None<decltype(result)> ? static_cast<cell>(result) : None<cell>;
    }

    static cell AMX_NATIVE_CALL n_SvAppendFilter(AMX* const amx, cell* const params) noexcept
    {
        if (*params < 3 * sizeof(cell))
            return None<cell>;

        const auto effect = static_cast<uword_t>(params[1]);
        if (effect >= kMaxEffects) return None<cell>;

        const auto filter = static_cast<ubyte_t>(params[2]);
        const auto priority = static_cast<sword_t>(params[3]);

        Block<ubyte_t> parameters;

        switch (filter)
        {
            case Filter::Chorus:
            {
                if (*params != 10 * sizeof(cell))
                    return None<cell>;

                parameters = Block<ubyte_t>::FromHeap(sizeof(ChorusParameters));
                if (parameters.Invalid()) return None<cell>;

                if (cell* physical_address; amx_GetAddr(amx, params[4], &physical_address) == 0)
                    reinterpret_cast<ChorusParameters*>(parameters.Data())->wetdrymix = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[5], &physical_address) == 0)
                    reinterpret_cast<ChorusParameters*>(parameters.Data())->depth = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[6], &physical_address) == 0)
                    reinterpret_cast<ChorusParameters*>(parameters.Data())->feedback = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[7], &physical_address) == 0)
                    reinterpret_cast<ChorusParameters*>(parameters.Data())->frequency = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[8], &physical_address) == 0)
                    reinterpret_cast<ChorusParameters*>(parameters.Data())->waveform = *physical_address;
                if (cell* physical_address; amx_GetAddr(amx, params[9], &physical_address) == 0)
                    reinterpret_cast<ChorusParameters*>(parameters.Data())->delay = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[10], &physical_address) == 0)
                    reinterpret_cast<ChorusParameters*>(parameters.Data())->phase = *physical_address;

                if (Instance()._debug) Logger::Instance().Log
                    ("[sv:dbg:pawn:SvAppendFilter:begin] : effect(%hu), filter(%hhu), priority(%hd), "
                     "wetdrymix(%.2f), depth(%.2f), feedback(%.2f), frequency(%.2f), waveform(%u), delay(%.2f), phase(%u)",
                     effect, filter, priority,
                     reinterpret_cast<const ChorusParameters*>(parameters.Data())->wetdrymix,
                     reinterpret_cast<const ChorusParameters*>(parameters.Data())->depth,
                     reinterpret_cast<const ChorusParameters*>(parameters.Data())->feedback,
                     reinterpret_cast<const ChorusParameters*>(parameters.Data())->frequency,
                     reinterpret_cast<const ChorusParameters*>(parameters.Data())->waveform,
                     reinterpret_cast<const ChorusParameters*>(parameters.Data())->delay,
                     reinterpret_cast<const ChorusParameters*>(parameters.Data())->phase);

                break;
            }
            case Filter::Compressor:
            {
                if (*params != 9 * sizeof(cell))
                    return None<cell>;

                parameters = Block<ubyte_t>::FromHeap(sizeof(CompressorParameters));
                if (parameters.Invalid()) return None<cell>;

                if (cell* physical_address; amx_GetAddr(amx, params[4], &physical_address) == 0)
                    reinterpret_cast<CompressorParameters*>(parameters.Data())->gain = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[5], &physical_address) == 0)
                    reinterpret_cast<CompressorParameters*>(parameters.Data())->attack = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[6], &physical_address) == 0)
                    reinterpret_cast<CompressorParameters*>(parameters.Data())->release = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[7], &physical_address) == 0)
                    reinterpret_cast<CompressorParameters*>(parameters.Data())->threshold = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[8], &physical_address) == 0)
                    reinterpret_cast<CompressorParameters*>(parameters.Data())->ratio = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[9], &physical_address) == 0)
                    reinterpret_cast<CompressorParameters*>(parameters.Data())->predelay = amx_ctof(*physical_address);

                if (Instance()._debug) Logger::Instance().Log
                    ("[sv:dbg:pawn:SvAppendFilter:begin] : effect(%hu), filter(%hhu), priority(%hd), "
                     "gain(%.2f), attack(%.2f), release(%.2f), threshold(%.2f), ratio(%.2f), predelay(%.2f)",
                     effect, filter, priority,
                     reinterpret_cast<const CompressorParameters*>(parameters.Data())->gain,
                     reinterpret_cast<const CompressorParameters*>(parameters.Data())->attack,
                     reinterpret_cast<const CompressorParameters*>(parameters.Data())->release,
                     reinterpret_cast<const CompressorParameters*>(parameters.Data())->threshold,
                     reinterpret_cast<const CompressorParameters*>(parameters.Data())->ratio,
                     reinterpret_cast<const CompressorParameters*>(parameters.Data())->predelay);

                break;
            }
            case Filter::Distortion:
            {
                if (*params != 8 * sizeof(cell))
                    return None<cell>;

                parameters = Block<ubyte_t>::FromHeap(sizeof(DistortionParameters));
                if (parameters.Invalid()) return None<cell>;

                if (cell* physical_address; amx_GetAddr(amx, params[4], &physical_address) == 0)
                    reinterpret_cast<DistortionParameters*>(parameters.Data())->gain = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[5], &physical_address) == 0)
                    reinterpret_cast<DistortionParameters*>(parameters.Data())->edge = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[6], &physical_address) == 0)
                    reinterpret_cast<DistortionParameters*>(parameters.Data())->posteqcenterfrequency = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[7], &physical_address) == 0)
                    reinterpret_cast<DistortionParameters*>(parameters.Data())->posteqbandwidth = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[8], &physical_address) == 0)
                    reinterpret_cast<DistortionParameters*>(parameters.Data())->prelowpasscutoff = amx_ctof(*physical_address);

                if (Instance()._debug) Logger::Instance().Log
                    ("[sv:dbg:pawn:SvAppendFilter:begin] : effect(%hu), filter(%hhu), priority(%hd), "
                     "gain(%.2f), edge(%.2f), posteqcenterfrequency(%.2f), posteqbandwidth(%.2f), prelowpasscutoff(%.2f)",
                     effect, filter, priority,
                     reinterpret_cast<const DistortionParameters*>(parameters.Data())->gain,
                     reinterpret_cast<const DistortionParameters*>(parameters.Data())->edge,
                     reinterpret_cast<const DistortionParameters*>(parameters.Data())->posteqcenterfrequency,
                     reinterpret_cast<const DistortionParameters*>(parameters.Data())->posteqbandwidth,
                     reinterpret_cast<const DistortionParameters*>(parameters.Data())->prelowpasscutoff);

                break;
            }
            case Filter::Echo:
            {
                if (*params != 8 * sizeof(cell))
                    return None<cell>;

                parameters = Block<ubyte_t>::FromHeap(sizeof(EchoParameters));
                if (parameters.Invalid()) return None<cell>;

                if (cell* physical_address; amx_GetAddr(amx, params[4], &physical_address) == 0)
                    reinterpret_cast<EchoParameters*>(parameters.Data())->wetdrymix = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[5], &physical_address) == 0)
                    reinterpret_cast<EchoParameters*>(parameters.Data())->feedback = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[6], &physical_address) == 0)
                    reinterpret_cast<EchoParameters*>(parameters.Data())->leftdelay = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[7], &physical_address) == 0)
                    reinterpret_cast<EchoParameters*>(parameters.Data())->rightdelay = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[8], &physical_address) == 0)
                    reinterpret_cast<EchoParameters*>(parameters.Data())->pandelay = static_cast<bool>(*physical_address);

                if (Instance()._debug) Logger::Instance().Log
                    ("[sv:dbg:pawn:SvAppendFilter:begin] : effect(%hu), filter(%hhu), priority(%hd), "
                     "wetdrymix(%.2f), feedback(%.2f), leftdelay(%.2f), rightdelay(%.2f), pandelay(%hhu)",
                     effect, filter, priority,
                     reinterpret_cast<const EchoParameters*>(parameters.Data())->wetdrymix,
                     reinterpret_cast<const EchoParameters*>(parameters.Data())->feedback,
                     reinterpret_cast<const EchoParameters*>(parameters.Data())->leftdelay,
                     reinterpret_cast<const EchoParameters*>(parameters.Data())->rightdelay,
                     reinterpret_cast<const EchoParameters*>(parameters.Data())->pandelay);

                break;
            }
            case Filter::Flanger:
            {
                if (*params != 10 * sizeof(cell))
                    return None<cell>;

                parameters = Block<ubyte_t>::FromHeap(sizeof(FlangerParameters));
                if (parameters.Invalid()) return None<cell>;

                if (cell* physical_address; amx_GetAddr(amx, params[4], &physical_address) == 0)
                    reinterpret_cast<FlangerParameters*>(parameters.Data())->wetdrymix = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[5], &physical_address) == 0)
                    reinterpret_cast<FlangerParameters*>(parameters.Data())->depth = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[6], &physical_address) == 0)
                    reinterpret_cast<FlangerParameters*>(parameters.Data())->feedback = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[7], &physical_address) == 0)
                    reinterpret_cast<FlangerParameters*>(parameters.Data())->frequency = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[8], &physical_address) == 0)
                    reinterpret_cast<FlangerParameters*>(parameters.Data())->waveform = *physical_address;
                if (cell* physical_address; amx_GetAddr(amx, params[9], &physical_address) == 0)
                    reinterpret_cast<FlangerParameters*>(parameters.Data())->delay = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[10], &physical_address) == 0)
                    reinterpret_cast<FlangerParameters*>(parameters.Data())->phase = *physical_address;

                if (Instance()._debug) Logger::Instance().Log
                    ("[sv:dbg:pawn:SvAppendFilter:begin] : effect(%hu), filter(%hhu), priority(%hd), "
                     "wetdrymix(%.2f), depth(%.2f), feedback(%.2f), frequency(%.2f), waveform(%u), delay(%.2f), phase(%u)",
                     effect, filter, priority,
                     reinterpret_cast<const FlangerParameters*>(parameters.Data())->wetdrymix,
                     reinterpret_cast<const FlangerParameters*>(parameters.Data())->depth,
                     reinterpret_cast<const FlangerParameters*>(parameters.Data())->feedback,
                     reinterpret_cast<const FlangerParameters*>(parameters.Data())->frequency,
                     reinterpret_cast<const FlangerParameters*>(parameters.Data())->waveform,
                     reinterpret_cast<const FlangerParameters*>(parameters.Data())->delay,
                     reinterpret_cast<const FlangerParameters*>(parameters.Data())->phase);

                break;
            }
            case Filter::Gargle:
            {
                if (*params != 5 * sizeof(cell))
                    return None<cell>;

                parameters = Block<ubyte_t>::FromHeap(sizeof(GargleParameters));
                if (parameters.Invalid()) return None<cell>;

                if (cell* physical_address; amx_GetAddr(amx, params[4], &physical_address) == 0)
                    reinterpret_cast<GargleParameters*>(parameters.Data())->ratehz = *physical_address;
                if (cell* physical_address; amx_GetAddr(amx, params[5], &physical_address) == 0)
                    reinterpret_cast<GargleParameters*>(parameters.Data())->waveshape = *physical_address;

                if (Instance()._debug) Logger::Instance().Log
                    ("[sv:dbg:pawn:SvAppendFilter:begin] : effect(%hu), filter(%hhu), priority(%hd), "
                     "ratehz(%u), waveshape(%u)", effect, filter, priority,
                     reinterpret_cast<const GargleParameters*>(parameters.Data())->ratehz,
                     reinterpret_cast<const GargleParameters*>(parameters.Data())->waveshape);

                break;
            }
            case Filter::I3dl2reverb:
            {
                if (*params != 15 * sizeof(cell))
                    return None<cell>;

                parameters = Block<ubyte_t>::FromHeap(sizeof(I3dl2reverbParameters));
                if (parameters.Invalid()) return None<cell>;

                if (cell* physical_address; amx_GetAddr(amx, params[4], &physical_address) == 0)
                    reinterpret_cast<I3dl2reverbParameters*>(parameters.Data())->room = *physical_address;
                if (cell* physical_address; amx_GetAddr(amx, params[5], &physical_address) == 0)
                    reinterpret_cast<I3dl2reverbParameters*>(parameters.Data())->roomhf = *physical_address;
                if (cell* physical_address; amx_GetAddr(amx, params[6], &physical_address) == 0)
                    reinterpret_cast<I3dl2reverbParameters*>(parameters.Data())->roomrollofffactor = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[7], &physical_address) == 0)
                    reinterpret_cast<I3dl2reverbParameters*>(parameters.Data())->decaytime = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[8], &physical_address) == 0)
                    reinterpret_cast<I3dl2reverbParameters*>(parameters.Data())->decayhfratio = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[9], &physical_address) == 0)
                    reinterpret_cast<I3dl2reverbParameters*>(parameters.Data())->reflections = *physical_address;
                if (cell* physical_address; amx_GetAddr(amx, params[10], &physical_address) == 0)
                    reinterpret_cast<I3dl2reverbParameters*>(parameters.Data())->reflectionsdelay = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[11], &physical_address) == 0)
                    reinterpret_cast<I3dl2reverbParameters*>(parameters.Data())->reverb = *physical_address;
                if (cell* physical_address; amx_GetAddr(amx, params[12], &physical_address) == 0)
                    reinterpret_cast<I3dl2reverbParameters*>(parameters.Data())->reverbdelay = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[13], &physical_address) == 0)
                    reinterpret_cast<I3dl2reverbParameters*>(parameters.Data())->diffusion = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[14], &physical_address) == 0)
                    reinterpret_cast<I3dl2reverbParameters*>(parameters.Data())->density = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[15], &physical_address) == 0)
                    reinterpret_cast<I3dl2reverbParameters*>(parameters.Data())->hfreference = amx_ctof(*physical_address);

                if (Instance()._debug) Logger::Instance().Log
                    ("[sv:dbg:pawn:SvAppendFilter:begin] : effect(%hu), filter(%hhu), priority(%hd), "
                     "room(%d), roomhf(%d), roomrollofffactor(%.2f), decaytime(%.2f), "
                     "decayhfratio(%.2f), reflections(%d), reflectionsdelay(%.2f), reverb(%d), "
                     "reverbdelay(%.2f), diffusion(%.2f), density(%.2f), hfreference(%.2f)",
                     effect, filter, priority,
                     reinterpret_cast<const I3dl2reverbParameters*>(parameters.Data())->room,
                     reinterpret_cast<const I3dl2reverbParameters*>(parameters.Data())->roomhf,
                     reinterpret_cast<const I3dl2reverbParameters*>(parameters.Data())->roomrollofffactor,
                     reinterpret_cast<const I3dl2reverbParameters*>(parameters.Data())->decaytime,
                     reinterpret_cast<const I3dl2reverbParameters*>(parameters.Data())->decayhfratio,
                     reinterpret_cast<const I3dl2reverbParameters*>(parameters.Data())->reflections,
                     reinterpret_cast<const I3dl2reverbParameters*>(parameters.Data())->reflectionsdelay,
                     reinterpret_cast<const I3dl2reverbParameters*>(parameters.Data())->reverb,
                     reinterpret_cast<const I3dl2reverbParameters*>(parameters.Data())->reverbdelay,
                     reinterpret_cast<const I3dl2reverbParameters*>(parameters.Data())->diffusion,
                     reinterpret_cast<const I3dl2reverbParameters*>(parameters.Data())->density,
                     reinterpret_cast<const I3dl2reverbParameters*>(parameters.Data())->hfreference);

                break;
            }
            case Filter::Parameq:
            {
                if (*params != 6 * sizeof(cell))
                    return None<cell>;

                parameters = Block<ubyte_t>::FromHeap(sizeof(ParameqParameters));
                if (parameters.Invalid()) return None<cell>;

                if (cell* physical_address; amx_GetAddr(amx, params[4], &physical_address) == 0)
                    reinterpret_cast<ParameqParameters*>(parameters.Data())->center = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[5], &physical_address) == 0)
                    reinterpret_cast<ParameqParameters*>(parameters.Data())->bandwidth = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[6], &physical_address) == 0)
                    reinterpret_cast<ParameqParameters*>(parameters.Data())->gain = amx_ctof(*physical_address);

                if (Instance()._debug) Logger::Instance().Log
                    ("[sv:dbg:pawn:SvAppendFilter:begin] : effect(%hu), filter(%hhu), priority(%hd), "
                     "center(%.2f), bandwidth(%.2f), gain(%.2f)", effect, filter, priority,
                     reinterpret_cast<const ParameqParameters*>(parameters.Data())->center,
                     reinterpret_cast<const ParameqParameters*>(parameters.Data())->bandwidth,
                     reinterpret_cast<const ParameqParameters*>(parameters.Data())->gain);

                break;
            }
            case Filter::Reverb:
            {
                if (*params != 7 * sizeof(cell))
                    return None<cell>;

                parameters = Block<ubyte_t>::FromHeap(sizeof(ReverbParameters));
                if (parameters.Invalid()) return None<cell>;

                if (cell* physical_address; amx_GetAddr(amx, params[4], &physical_address) == 0)
                    reinterpret_cast<ReverbParameters*>(parameters.Data())->ingain = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[5], &physical_address) == 0)
                    reinterpret_cast<ReverbParameters*>(parameters.Data())->reverbmix = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[6], &physical_address) == 0)
                    reinterpret_cast<ReverbParameters*>(parameters.Data())->reverbtime = amx_ctof(*physical_address);
                if (cell* physical_address; amx_GetAddr(amx, params[7], &physical_address) == 0)
                    reinterpret_cast<ReverbParameters*>(parameters.Data())->highfreqrtratio = amx_ctof(*physical_address);

                if (Instance()._debug) Logger::Instance().Log
                    ("[sv:dbg:pawn:SvAppendFilter:begin] : effect(%hu), filter(%hhu), priority(%hd), "
                     "ingain(%.2f), reverbmix(%.2f), reverbtime(%.2f), highfreqrtratio(%.2f)",
                     effect, filter, priority,
                     reinterpret_cast<const ReverbParameters*>(parameters.Data())->ingain,
                     reinterpret_cast<const ReverbParameters*>(parameters.Data())->reverbmix,
                     reinterpret_cast<const ReverbParameters*>(parameters.Data())->reverbtime,
                     reinterpret_cast<const ReverbParameters*>(parameters.Data())->highfreqrtratio);

                break;
            }
            default:
            {
                return None<cell>;
            }
        }

        const auto result = Instance().OnAppendFilter(effect, filter, priority, std::move(parameters));

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvAppendFilter:end] : result(%hhu)", result);

        return static_cast<cell>(result);
    }

    static cell AMX_NATIVE_CALL n_SvRemoveFilter(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 3 * sizeof(cell))
            return None<cell>;

        const auto effect = static_cast<uword_t>(params[1]);
        if (effect >= kMaxEffects) return None<cell>;

        const auto filter = static_cast<ubyte_t>(params[2]);
        if (filter >= Filter::ENUM_END) return None<cell>;

        const auto priority = static_cast<sword_t>(params[3]);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvRemoveFilter:begin] : effect(%hu), filter(%hhu), priority(%hd)",
             effect, filter, priority);

        Instance().OnRemoveFilter(effect, filter, priority);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvRemoveFilter:end]");

        return Zero<cell>;
    }

    static cell AMX_NATIVE_CALL n_SvDeleteEffect(AMX* const amx, cell* const params) noexcept
    {
        if (*params != 1 * sizeof(cell))
            return None<cell>;

        const auto effect = static_cast<uword_t>(params[1]);
        if (effect >= kMaxEffects) return None<cell>;

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvDeleteEffect:begin] : effect(%hu)", effect);

        Instance().OnDeleteEffect(effect);

        if (Instance()._debug) Logger::Instance().Log
            ("[sv:dbg:pawn:SvDeleteEffect:end]");

        return Zero<cell>;
    }

public:

    // Player
    // --------------------------------

    std::function<Version(uword_t)> OnGetVersion;
    std::function<bool(uword_t)>    OnHasMicro;

    std::function<bool(uword_t)> OnEnableListener;
    std::function<bool(uword_t)> OnDisableListener;
    std::function<bool(uword_t)> OnCheckListener;

    std::function<bool(uword_t, udword_t)> OnEnableSpeaker;
    std::function<bool(uword_t, udword_t)> OnDisableSpeaker;
    std::function<bool(uword_t, udword_t)> OnCheckSpeaker;

    std::function<bool(uword_t, uword_t, udword_t)> OnAttachStream;
    std::function<bool(uword_t, uword_t, udword_t)> OnDetachStream;
    std::function<bool(uword_t, uword_t, udword_t)> OnHasStream;

    std::function<bool(uword_t, ubyte_t, udword_t)> OnSetKey;
    std::function<ubyte_t(uword_t, ubyte_t)>        OnGetKey;

    std::function<bool(uword_t, udword_t)> OnPlay;
    std::function<bool(uword_t, udword_t)> OnStop;

    // Stream
    // --------------------------------

    std::function<uword_t(fdword_t)> OnCreateStream;

    std::function<bool(uword_t)> OnEnableTransiter;
    std::function<bool(uword_t)> OnDisableTransiter;
    std::function<bool(uword_t)> OnCheckTransiter;

    std::function<bool(uword_t, uword_t)> OnAttachListener;
    std::function<bool(uword_t, uword_t)> OnDetachListener;
    std::function<bool(uword_t, uword_t)> OnHasListener;

    std::function<void(uword_t, fdword_t)>                     OnSetVolume;
    std::function<void(uword_t, fdword_t)>                     OnSetPanning;
    std::function<void(uword_t, fdword_t)>                     OnSetDistance;
    std::function<void(uword_t, fdword_t, fdword_t, fdword_t)> OnSetPosition;
    std::function<void(uword_t, uword_t)>                      OnSetTarget;
    std::function<void(uword_t, uword_t)>                      OnSetEffect;
    std::function<void(uword_t, Block<char>&&)>                OnSetIcon;

    std::function<void(uword_t)> OnDeleteStream;

    // Effect
    // --------------------------------

    std::function<uword_t()> OnCreateEffect;

    std::function<bool(uword_t, ubyte_t, sword_t, Block<ubyte_t>&&)> OnAppendFilter;
    std::function<void(uword_t, ubyte_t, sword_t)>                   OnRemoveFilter;

    std::function<void(uword_t)> OnDeleteEffect;

private:

    volatile bool _debug = false;

};
