/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <array>
#include <vector>
#include <set>
#include <map>

#include <ysf/structs.h>

#include "ControlPacket.h"
#include "VoicePacket.h"
#include "Parameter.h"
#include "Effect.h"

class Stream {

    Stream(const Stream&) = delete;
    Stream(Stream&&) = delete;
    Stream& operator=(const Stream&) = delete;
    Stream& operator=(Stream&&) = delete;

private:

    using PlayerCallback = std::function<void(Stream*, uint16_t)>;
    using DeleteCallback = std::function<void(Stream*)>;

protected:

    explicit Stream();

public:

    virtual ~Stream() noexcept;

public:

    void SendVoicePacket(VoicePacket& packet) const;
    void SendControlPacket(ControlPacket& packet) const;

    virtual bool AttachListener(uint16_t playerId);
    bool HasListener(uint16_t playerId) const noexcept;
    virtual bool DetachListener(uint16_t playerId);
    virtual std::vector<uint16_t> DetachAllListeners();

    bool AttachSpeaker(uint16_t playerId) noexcept;
    bool HasSpeaker(uint16_t playerId) const noexcept;
    bool DetachSpeaker(uint16_t playerId) noexcept;
    std::vector<uint16_t> DetachAllSpeakers();

    void SetParameter(uint8_t parameter, float value) noexcept;
    void ResetParameter(uint8_t parameter) noexcept;
    bool HasParameter(uint8_t parameter) const noexcept;
    float GetParameter(uint8_t parameter) noexcept;
    void SlideParameterFromTo(uint8_t parameter, float startValue, float endValue, uint32_t time) noexcept;
    void SlideParameterTo(uint8_t parameter, float endValue, uint32_t time) noexcept;
    void SlideParameter(uint8_t parameter, float deltaValue, uint32_t time) noexcept;

    std::size_t AddPlayerCallback(PlayerCallback playerCallback) noexcept;
    std::size_t AddDeleteCallback(DeleteCallback deleteCallback) noexcept;
    void RemovePlayerCallback(std::size_t callback) noexcept;
    void RemoveDeleteCallback(std::size_t callback) noexcept;

protected:

    int attachedSpeakersCount { 0 };
    int attachedListenersCount { 0 };

    std::array<std::atomic_bool, MAX_PLAYERS> attachedSpeakers {};
    std::array<std::atomic_bool, MAX_PLAYERS> attachedListeners {};

    ControlPacketContainerPtr packetCreateStream { nullptr };
    ControlPacketContainerPtr packetDeleteStream { nullptr };

private:

    std::vector<PlayerCallback> playerCallbacks;
    std::vector<DeleteCallback> deleteCallbacks;

    std::map<uint8_t, Parameter> parameters;

};
