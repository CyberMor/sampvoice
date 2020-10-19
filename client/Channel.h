/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>
#include <functional>
#include <array>

#include <audio/bass.h>
#include <audio/opus.h>

#include "Header.h"

class Channel {

    Channel() = delete;
    Channel(const Channel&) = delete;
    Channel(Channel&&) = delete;
    Channel& operator=(const Channel&) = delete;
    Channel& operator=(Channel&&) = delete;

private:

    using PlayCallback = std::function<void(const Channel&)>;
    using StopCallback = std::function<void(const Channel&)>;

public:

    explicit Channel(DWORD channelFlags);

    ~Channel() noexcept;

public:

    HSTREAM GetHandle() const noexcept;
    void SetSpeaker(WORD speaker) noexcept;
    bool HasSpeaker() const noexcept;
    WORD GetSpeaker() const noexcept;

    bool IsActive() const noexcept;
    void Reset() noexcept;
    void Push(DWORD packetNumber, const BYTE* dataPtr, DWORD dataSize) noexcept;

    void SetPlayCallback(PlayCallback playCallback) noexcept;
    void SetStopCallback(StopCallback stopCallback) noexcept;

private:

    const HSTREAM handle;
    WORD speaker { SV::kNonePlayer };

    PlayCallback playCallback { nullptr };
    StopCallback stopCallback { nullptr };

    OpusDecoder* const decoder;
    std::array<opus_int16, SV::kFrameSizeInSamples> decBuffer;

    DWORD expectedPacketNumber { 0 };
    bool initialized { false };
    bool playing { false };

    int opusErrorCode { -1 };

};

using ChannelPtr = std::unique_ptr<Channel>;
#define MakeChannel std::make_unique<Channel>
