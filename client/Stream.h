/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <vector>
#include <functional>
#include <memory>

#include <audio/bass.h>

#include <util/Logger.h>

#include "StreamInfo.h"
#include "VoicePacket.h"
#include "Channel.h"

class Stream {

    Stream() = delete;
    Stream(const Stream&) = delete;
    Stream(Stream&&) = delete;
    Stream& operator=(const Stream&) = delete;
    Stream& operator=(Stream&&) = delete;

protected:

    using PlayHandlerType = std::function<void(const Stream&, const WORD)>;
    using StopHandlerType = std::function<void(const Stream&, const WORD)>;

public:

    explicit Stream(
        const DWORD streamFlags,
        PlayHandlerType&& playHandler,
        StopHandlerType&& stopHandler,
        const StreamType type,
        const std::string& name,
        const D3DCOLOR color
    ) :
        streamFlags(streamFlags),
        streamInfo(MakeStreamInfo(type, name, color)),
        playHandler(std::move(playHandler)),
        stopHandler(std::move(stopHandler))
    {}

    virtual ~Stream() noexcept = default;

public:

    const StreamInfoPtr& GetInfo() const noexcept
    {
        return this->streamInfo;
    }

    virtual void Tick() noexcept
    {
        for (const auto& iChan : this->channels)
        {
            if (iChan->speaker != SV::kNonePlayer && !iChan->IsActive())
            {
                iChan->Reset();
            }
        }
    }

    void Push(const VoicePacket& packet)
    {
        ChannelPtr channel { nullptr };

        for (const auto& iChan : this->channels)
        {
            if (iChan->speaker == packet.sender)
            {
                channel = iChan;
                break;
            }
        }

        if (!channel) for (const auto& iChan : this->channels)
        {
            if (iChan->speaker == SV::kNonePlayer)
            {
                Logger::LogToFile(
                    "[sv:dbg:stream_%p:push] : channel %p was occupied by player %hu",
                    this, iChan.get(), packet.sender);

                iChan->speaker = packet.sender;

                channel = iChan;
                break;
            }
        }

        if (!channel)
        {
            channel = MakeChannel(this->streamFlags,
                std::bind(&Stream::OnPlayChannel, this, std::placeholders::_1),
                std::bind(&Stream::OnStopChannel, this, std::placeholders::_1),
                packet.sender);

            Logger::LogToFile(
                "[sv:dbg:stream_%p:push] : channel %p for player %hu created",
                this, channel.get(), packet.sender);

            this->ChannelCreationHandler(*channel);
            channel->Push(packet.packid, packet.data, packet.length);
            this->channels.emplace_back(std::move(channel));

            return;
        }

        channel->Push(packet.packid, packet.data, packet.length);
    }

    void Reset() noexcept
    {
        Logger::LogToFile("[sv:dbg:stream_%p:reset] : resetting stream...", this);

        for (const auto& iChan : this->channels)
        {
            iChan->Reset();
        }
    }

protected:

    virtual void ChannelCreationHandler(const Channel& channel) noexcept
    {
        BASS_ChannelSetAttribute(channel.handle, BASS_ATTRIB_VOL, 4.f);
    }

protected:

    std::vector<ChannelPtr> channels;

private:

    void OnPlayChannel(const Channel& channel) noexcept
    {
        if (channel.speaker != SV::kNonePlayer && this->playHandler)
            this->playHandler(*this, channel.speaker);
    }

    void OnStopChannel(const Channel& channel) noexcept
    {
        if (channel.speaker != SV::kNonePlayer && this->stopHandler)
            this->stopHandler(*this, channel.speaker);
    }

private:

    const DWORD streamFlags { NULL };
    const StreamInfoPtr streamInfo { nullptr };

    const PlayHandlerType playHandler { nullptr };
    const StopHandlerType stopHandler { nullptr };

};

using StreamPtr = std::shared_ptr<Stream>;
#define MakeStream std::make_shared<Stream>
