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
#include <game/CVector.h>

#include "StreamInfo.h"
#include "LocalStream.h"
#include "Channel.h"

class StreamAtPoint : public LocalStream {
private:

	CVector position;

	void ChannelCreationHandler(const Channel& channel) override {

		this->LocalStream::ChannelCreationHandler(channel);

		BASS_ChannelSet3DPosition(
			channel.handle, (BASS_3DVECTOR*)(&this->position),
			&BASS_3DVECTOR(0, 0, 0), &BASS_3DVECTOR(0, 0, 0)
		);

	}

public:

	StreamAtPoint() = delete;
	StreamAtPoint(const StreamAtPoint& object) = delete;
	StreamAtPoint(StreamAtPoint&& object) = delete;

	StreamAtPoint& operator=(const StreamAtPoint& object) = delete;
	StreamAtPoint& operator=(StreamAtPoint&& object) = delete;

	StreamAtPoint(
		const PlayHandlerType& playHandler,
		const StopHandlerType& stopHandler,
		const std::string& name,
		const D3DCOLOR color,
		const CVector& position,
		const float distance
	) :

		LocalStream(
			BASS_SAMPLE_3D | BASS_SAMPLE_MUTEMAX,
			playHandler, stopHandler,
			StreamType::LocalStreamAtPoint,
			name, color, distance
		),

		position(position)

	{}

	void UpdatePosition(const CVector& position) {

		this->position = position;

		for (const auto& iChan : this->channels)
			BASS_ChannelSet3DPosition(
				iChan->handle,
				(BASS_3DVECTOR*)(&this->position),
				nullptr, nullptr
			);

	}

};

using StreamAtPointPtr = std::shared_ptr<StreamAtPoint>;
#define MakeStreamAtPoint std::make_shared<StreamAtPoint>
