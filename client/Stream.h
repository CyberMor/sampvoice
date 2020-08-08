/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>
#include <functional>
#include <vector>

#include <audio/bass.h>

#include <util/Logger.h>

#include "StreamInfo.h"
#include "VoicePacket.h"
#include "Channel.h"

class Stream {
public:

	using PlayHandlerType = std::function<void(const Stream& stream, WORD speaker)>;
	using StopHandlerType = std::function<void(const Stream& stream, WORD speaker)>;

private:

	const DWORD streamFlags;
	const StreamInfoPtr streamInfo;

	const PlayHandlerType playHandler;
	const StopHandlerType stopHandler;

	void OnPlayChannel(const Channel& channel) {

		if (channel.speaker != SV::NonePlayer && this->playHandler)
			this->playHandler(*this, channel.speaker);

	}

	void OnStopChannel(const Channel& channel) {

		if (channel.speaker != SV::NonePlayer && this->stopHandler)
			this->stopHandler(*this, channel.speaker);

	}

protected:

	std::vector<ChannelPtr> channels;

	virtual void ChannelCreationHandler(const Channel& channel) {

		BASS_ChannelSetAttribute(channel.handle, BASS_ATTRIB_VOL, 4.f);

	}

public:

	Stream() = delete;
	Stream(const Stream& object) = delete;
	Stream(Stream&& object) = delete;

	Stream& operator=(const Stream& object) = delete;
	Stream& operator=(Stream&& object) = delete;

	Stream(
		const DWORD streamFlags,
		const PlayHandlerType& playHandler,
		const StopHandlerType& stopHandler,
		const StreamType type,
		const std::string& name,
		const D3DCOLOR color
	) :

		streamFlags(streamFlags),
		streamInfo(MakeStreamInfo(type, name, color)),

		playHandler(playHandler),
		stopHandler(stopHandler)

	{}

	const StreamInfoPtr& GetInfo() const {

		return this->streamInfo;

	}

	virtual void Tick() {

		for (const auto& iChan : this->channels)
			if (iChan->speaker != SV::NonePlayer && !iChan->IsActive())
				iChan->Reset();

	}

	void Push(const VoicePacket& packet) {

		ChannelPtr channel = nullptr;

		for (const auto& iChan : this->channels)
			if (iChan->speaker == packet.sender) {

				channel = iChan;
				break;

			}

		if (!channel) for (const auto& iChan : this->channels)
			if (iChan->speaker == SV::NonePlayer) {

				Logger::LogToFile(
					"[sv:dbg:stream_%p:push] : channel %p was occupied by player %hu",
					this, iChan.get(), packet.sender
				);

				iChan->speaker = packet.sender;

				channel = iChan;
				break;

			}

		if (!channel) {

			if (!(channel = MakeChannel(this->streamFlags,
				std::bind(&Stream::OnPlayChannel, this, std::placeholders::_1),
				std::bind(&Stream::OnStopChannel, this, std::placeholders::_1),
				packet.sender))) return;

			Logger::LogToFile(
				"[sv:dbg:stream_%p:push] : channel %p for player %hu created",
				this, channel.get(), packet.sender
			);

			this->ChannelCreationHandler(*channel);
			this->channels.push_back(channel);

		}

		channel->Push(packet.packid, packet.data, packet.length);

	}

	void Reset() {

		Logger::LogToFile("[sv:dbg:stream_%p:reset] : resetting stream...", this);

		for (const auto& iChan : this->channels) iChan->Reset();

	}

	virtual ~Stream() {}

};

using StreamPtr = std::shared_ptr<Stream>;
#define MakeStream std::make_shared<Stream>
