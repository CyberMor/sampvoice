/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>
#include <string>

#include <d3d9.h>

#include <audio/bass.h>

#include "Stream.h"
#include "StreamInfo.h"
#include "Channel.h"

class LocalStream : public Stream {
protected:

	float distance = 0.f;

	virtual void ChannelCreationHandler(const Channel& channel) override {

		this->Stream::ChannelCreationHandler(channel);

		BASS_ChannelSet3DAttributes(
			channel.handle, BASS_3DMODE_NORMAL,
			this->distance * 0.1f, this->distance,
			-1, -1, -1
		);

	}

public:

	LocalStream() = delete;
	LocalStream(const LocalStream& object) = delete;
	LocalStream(LocalStream&& object) = delete;

	LocalStream& operator=(const LocalStream& object) = delete;
	LocalStream& operator=(LocalStream&& object) = delete;

	LocalStream(
		const DWORD streamFlags,
		const PlayHandlerType& playHandler,
		const StopHandlerType& stopHandler,
		const StreamType type,
		const std::string& name,
		const D3DCOLOR color,
		const float distance
	) :

		Stream(
			streamFlags,
			playHandler,
			stopHandler,
			type, name, color
		),

		distance(distance)

	{}

	void UpdateDistance(const float distance) {

		this->distance = distance;

		for (const auto& iChan : this->channels)
			BASS_ChannelSet3DAttributes(
				iChan->handle, BASS_3DMODE_NORMAL,
				this->distance * 0.1f, this->distance,
				-1, -1, -1
			);

	}

	virtual ~LocalStream() {}

};

using LocalStreamPtr = std::shared_ptr<LocalStream>;
#define MakeLocalStream std::make_shared<LocalStream>
