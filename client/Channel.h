/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>
#include <functional>

#include <audio/bass.h>
#include <audio/opus.h>

#include <util/Logger.h>

#include "Header.h"

class Channel {

    Channel() = delete;
    Channel(const Channel&) = delete;
    Channel(Channel&&) = delete;
    Channel& operator=(const Channel&) = delete;
    Channel& operator=(Channel&&) = delete;

    using PlayHandlerType = std::function<void(const Channel&)>;
    using StopHandlerType = std::function<void(const Channel&)>;

public:

    explicit Channel(const DWORD channelFlags,
                     PlayHandlerType&& playHandler,
                     StopHandlerType&& stopHandler,
                     const WORD speaker = SV::kNonePlayer)
        : handle(BASS_StreamCreate(
            SV::kFrequency, 1, channelFlags,
            STREAMPROC_PUSH, nullptr))
        , decoder(opus_decoder_create(
            SV::kFrequency, 1, &opusErrorCode))
        , playHandler(std::move(playHandler))
        , stopHandler(std::move(stopHandler))
        , speaker(speaker)
    {
        if (!this->handle) Logger::LogToFile(
            "[sv:err:channel_%p] : failed to create bass channel (code:%d)",
            this, BASS_ErrorGetCode());
        if (!this->decoder) Logger::LogToFile(
            "[sv:err:channel_%p] : failed to create opus decoder (code:%d)",
            this, this->opusErrorCode);

        if (!this->handle || !this->decoder)
        {
            if (this->decoder) opus_decoder_destroy(this->decoder);
            if (this->handle) BASS_StreamFree(this->handle);
            throw std::exception();
        }
    }

    ~Channel() noexcept
    {
        if (this->playing && this->stopHandler)
            this->stopHandler(*this);

        opus_decoder_destroy(this->decoder);
        BASS_StreamFree(this->handle);
    }

public:

    bool IsActive() const noexcept
    {
        const DWORD bufferSize = BASS_ChannelGetData(
            this->handle, nullptr, BASS_DATA_AVAILABLE);

        return bufferSize != -1 && bufferSize != 0;
    }

    void Reset() noexcept
    {
        BASS_ChannelPause(this->handle);
        BASS_ChannelSetPosition(this->handle, 0, BASS_POS_BYTE);
        opus_decoder_ctl(this->decoder, OPUS_RESET_STATE);

        if (this->playing && this->stopHandler)
        {
            this->stopHandler(*this);
            this->playing = false;
        }

        this->speaker = SV::kNonePlayer;
        this->expectedPacketNumber = 0;
        this->initialized = false;
    }

    void Push(const DWORD packetNumber, const BYTE* dataPtr, const DWORD dataSize) noexcept
    {
        if (!this->initialized || !packetNumber)
        {
            Logger::LogToFile("[sv:dbg:channel_%p:push] : init channel", this);

            BASS_ChannelPause(this->handle);
            BASS_ChannelSetPosition(this->handle, 0, BASS_POS_BYTE);
            opus_decoder_ctl(this->decoder, OPUS_RESET_STATE);

            if (this->playing && this->stopHandler)
            {
                this->stopHandler(*this);
                this->playing = false;
            }

            this->initialized = true;
        }
        else if (packetNumber < this->expectedPacketNumber)
        {
            Logger::LogToFile(
                "[sv:dbg:channel_%p:push] : late packet to channel (pack:%u;expPack:%u)",
                this, packetNumber, this->expectedPacketNumber);

            return;
        }
        else if (packetNumber > this->expectedPacketNumber)
        {
            Logger::LogToFile(
                "[sv:dbg:channel_%p:push] : lost packet to channel (pack:%u;expPack:%u)",
                this, packetNumber, this->expectedPacketNumber);

            const DWORD length = opus_decode(
                this->decoder, dataPtr, dataSize, this->decBuffer,
                SV::kFrameSizeInSamples, true);

            if (length == SV::kFrameSizeInSamples)
            {
                BASS_StreamPutData(this->handle, this->decBuffer, SV::kFrameSizeInBytes);
            }
        }

        const DWORD length = opus_decode(
            this->decoder, dataPtr, dataSize, this->decBuffer,
            SV::kFrameSizeInSamples, false);

        if (length == SV::kFrameSizeInSamples)
        {
            BASS_StreamPutData(this->handle, this->decBuffer, SV::kFrameSizeInBytes);
        }

        const DWORD channelStatus = BASS_ChannelIsActive(this->handle);
        const DWORD bufferSize = BASS_ChannelGetData(this->handle, nullptr, BASS_DATA_AVAILABLE);

        if ((channelStatus == BASS_ACTIVE_PAUSED || channelStatus == BASS_ACTIVE_STOPPED) &&
            bufferSize != -1 && bufferSize >= SV::kChannelPreBufferFramesCount * SV::kFrameSizeInBytes)
        {
            Logger::LogToFile("[sv:dbg:channel_%p:push] : playing channel", this);

            BASS_ChannelPlay(this->handle, FALSE);

            if (!this->playing && this->playHandler)
            {
                this->playHandler(*this);
                this->playing = true;
            }
        }

        this->expectedPacketNumber = packetNumber + 1;
    }

public:

    const HSTREAM handle { NULL };
    WORD speaker { SV::kNonePlayer };

private:

    const PlayHandlerType playHandler { nullptr };
    const StopHandlerType stopHandler { nullptr };

    OpusDecoder* const decoder { nullptr };
    opus_int16 decBuffer[SV::kFrameSizeInSamples];

    DWORD expectedPacketNumber { 0 };
    bool initialized { false };
    bool playing { false };

    int opusErrorCode { -1 };

};

using ChannelPtr = std::shared_ptr<Channel>;
#define MakeChannel std::make_shared<Channel>
