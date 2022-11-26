/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cstdint>
#include <functional>
#include <vector>
#include <map>

#include "StreamInfo.h"
#include "VoicePacket.h"
#include "Parameter.h"
#include "Channel.h"
#include "Effect.h"

struct Stream {

    Stream() = delete;
    virtual ~Stream() noexcept = default;
    Stream(const Stream&) = delete;
    Stream(Stream&&) = delete;
    Stream& operator=(const Stream&) = delete;
    Stream& operator=(Stream&&) = delete;

private:

    using PlayCallback = std::function<void(const Stream&, WORD)>;
    using StopCallback = std::function<void(const Stream&, WORD)>;

protected:

    Stream(DWORD flags, StreamType type, D3DCOLOR color, std::string name) noexcept;

public:

    const StreamInfo& GetInfo() const noexcept;

    virtual void Tick() noexcept;

    void Push(const VoicePacket& packet);
    void Reset() noexcept;

    void SetParameter(BYTE parameter, float value);
    void SlideParameter(BYTE parameter, float startValue, float endValue, DWORD time);

    void EffectCreate(DWORD effect, DWORD number, int priority, const void* paramPtr, DWORD paramSize);
    void EffectDelete(DWORD effect);

public:

    void SetPlayCallback(PlayCallback&& callback) noexcept;
    void SetStopCallback(StopCallback&& callback) noexcept;

protected:

    virtual void OnChannelCreate(const Channel& channel) noexcept;

private:

    void OnChannelPlay(const Channel& channel) noexcept;
    void OnChannelStop(const Channel& channel) noexcept;

protected:

    const std::vector<Channel>& GetChannels() const noexcept;

private:

    DWORD                     _stream_flags;
    StreamInfo                _stream_info;

    std::vector<Channel>      _channels;

    PlayCallback              _play_callback = nullptr;
    StopCallback              _stop_callback = nullptr;

    std::map<BYTE, Parameter> _parameters;
    std::map<DWORD, Effect>   _effects;

};
