/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <atomic>
#include <cstdint>
#include <vector>
#include <array>

#include <ysf/structs.h>

#include "ControlPacket.h"
#include "VoicePacket.h"

class Stream {

    Stream(const Stream&) = delete;
    Stream(Stream&&) = delete;
    Stream& operator=(const Stream&) = delete;
    Stream& operator=(Stream&&) = delete;

protected:

    explicit Stream();

public:

    virtual ~Stream() noexcept = default;

public:

    void PushVoicePacket(VoicePacket& packet) const;

    virtual bool AttachListener(uint16_t playerId);
    bool HasListener(uint16_t playerId) const noexcept;
    virtual bool DetachListener(uint16_t playerId);
    virtual std::vector<uint16_t> DetachAllListeners();

    bool AttachSpeaker(uint16_t playerId) noexcept;
    bool HasSpeaker(uint16_t playerId) const noexcept;
    bool DetachSpeaker(uint16_t playerId) noexcept;
    std::vector<uint16_t> DetachAllSpeakers();

protected:

    int attachedSpeakersCount { 0 };
    int attachedListenersCount { 0 };

    std::array<std::atomic_bool, MAX_PLAYERS> attachedSpeakers {};
    std::array<std::atomic_bool, MAX_PLAYERS> attachedListeners {};

    ControlPacketContainerPtr packetCreateStream { nullptr };
    ControlPacketContainerPtr packetDeleteStream { nullptr };

};
