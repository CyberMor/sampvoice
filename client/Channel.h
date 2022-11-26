/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <functional>

#include <audio/bass.h>
#include <audio/opus.h>

#include "Header.h"

struct Channel {

    Channel() = delete;
    ~Channel() noexcept;
    Channel(const Channel&) = delete;
    Channel(Channel&&) = delete;
    Channel& operator=(const Channel&) = delete;
    Channel& operator=(Channel&&) = delete;

private:

    using PlayCallback = std::function<void(const Channel&)>;
    using StopCallback = std::function<void(const Channel&)>;

public:

    Channel(DWORD flags) noexcept;

public:

    bool Valid() const noexcept;

public:

    HSTREAM GetHandle() const noexcept;

    void SetSpeaker(WORD speaker) noexcept;
    bool HasSpeaker() const noexcept;
    WORD GetSpeaker() const noexcept;

    bool IsActive() const noexcept;

    void Reset() noexcept;
    void Push(DWORD packet, const BYTE* data, DWORD size) noexcept;

public:

    void SetPlayCallback(PlayCallback&& callback) noexcept;
    void SetStopCallback(StopCallback&& callback) noexcept;

private:

    HSTREAM      _handle          = NULL;
    WORD         _speaker         = SV::kNonePlayer;

    PlayCallback _play_callback   = nullptr;
    StopCallback _stop_callback   = nullptr;

    OpusDecoder* _decoder         = nullptr;
    opus_int16   _dec_buffer[SV::kFrameSizeInSamples];

    DWORD        _expected_packet = 0;
    bool         _initialized     = false;
    bool         _playing         = false;

    int          _opus_error      = -1;

};
