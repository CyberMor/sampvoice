/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>
#include <cstdint>
#include <functional>
#include <vector>
#include <map>

#include "StreamInfo.h"
#include "VoicePacket.h"
#include "Parameter.h"
#include "Channel.h"
#include "Effect.h"

class Stream {

    Stream() = delete;
    Stream(const Stream&) = delete;
    Stream(Stream&&) = delete;
    Stream& operator=(const Stream&) = delete;
    Stream& operator=(Stream&&) = delete;

private:

    using PlayCallback = std::function<void(const Stream&, WORD)>;
    using StopCallback = std::function<void(const Stream&, WORD)>;

protected:

    explicit Stream(DWORD streamFlags, StreamType type,
                    D3DCOLOR color, std::string name) noexcept;

public:

    virtual ~Stream() noexcept = default;

public:

    const StreamInfo& GetInfo() const noexcept;

    virtual void Tick() noexcept;
    void Push(const VoicePacket& packet);
    void Reset() noexcept;
    void SetParameter(BYTE parameter, float value);
    void SlideParameter(BYTE parameter, float startValue, float endValue, DWORD time);
    void EffectCreate(DWORD effect, DWORD number, int priority, const void* paramPtr, DWORD paramSize);
    void EffectDelete(DWORD effect);

    std::size_t AddPlayCallback(PlayCallback playCallback);
    std::size_t AddStopCallback(StopCallback stopCallback);
    void RemovePlayCallback(std::size_t callback) noexcept;
    void RemoveStopCallback(std::size_t callback) noexcept;

protected:

    virtual void OnChannelCreate(const Channel& channel);

private:

    void OnChannelPlay(const Channel& channel) noexcept;
    void OnChannelStop(const Channel& channel) noexcept;

protected:

    const std::vector<ChannelPtr>& GetChannels() const noexcept;

private:

    const DWORD streamFlags;
    const StreamInfo streamInfo;

    std::vector<ChannelPtr> channels;

    std::vector<PlayCallback> playCallbacks;
    std::vector<StopCallback> stopCallbacks;

    std::map<BYTE, ParameterPtr> parameters;
    std::map<DWORD, EffectPtr> effects;

};

using StreamPtr = std::unique_ptr<Stream>;
