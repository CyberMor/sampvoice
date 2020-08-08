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
#include <samp/CNetGame.h>
#include <game/CObject.h>

#include "StreamInfo.h"
#include "LocalStream.h"
#include "Channel.h"

class StreamAtObject : public LocalStream {
private:

	const WORD objectId;

	void ChannelCreationHandler(const Channel& channel) override {

		this->LocalStream::ChannelCreationHandler(channel);

		const auto pNetGame = SAMP::pNetGame();
		if (!pNetGame) return;

		const auto pObjectPool = pNetGame->GetObjectPool();
		if (!pObjectPool) return;

		const auto pObject = pObjectPool->m_pObject[this->objectId];
		if (!pObject) return;

		const auto pObjectEntity = pObject->m_pGameEntity;
		if (!pObjectEntity) return;

		const auto pObjectMatrix = pObjectEntity->GetMatrix();
		if (!pObjectMatrix) return;

		BASS_ChannelSet3DPosition(
			channel.handle, (BASS_3DVECTOR*)(&pObjectMatrix->pos),
			&BASS_3DVECTOR(0, 0, 0), &BASS_3DVECTOR(0, 0, 0)
		);

	}

public:

	StreamAtObject() = delete;
	StreamAtObject(const StreamAtObject& object) = delete;
	StreamAtObject(StreamAtObject&& object) = delete;

	StreamAtObject& operator=(const StreamAtObject& object) = delete;
	StreamAtObject& operator=(StreamAtObject&& object) = delete;

	StreamAtObject(
		const PlayHandlerType& playHandler,
		const StopHandlerType& stopHandler,
		const std::string& name,
		const D3DCOLOR color,
		const WORD objectId,
		const float distance
	) :

		LocalStream(
			BASS_SAMPLE_3D | BASS_SAMPLE_MUTEMAX,
			playHandler, stopHandler,
			StreamType::LocalStreamAtObject,
			name, color, distance
		),

		objectId(objectId)

	{}

	void Tick() override {

		this->LocalStream::Tick();

		const auto pNetGame = SAMP::pNetGame();
		if (!pNetGame) return;

		const auto pObjectPool = pNetGame->GetObjectPool();
		if (!pObjectPool) return;

		const auto pObject = pObjectPool->m_pObject[this->objectId];
		if (!pObject) return;

		const auto pObjectEntity = pObject->m_pGameEntity;
		if (!pObjectEntity) return;

		const auto pObjectMatrix = pObjectEntity->GetMatrix();
		if (!pObjectMatrix) return;

		for (const auto& iChan : this->channels) if (iChan->speaker != SV::NonePlayer)
			BASS_ChannelSet3DPosition(iChan->handle, (BASS_3DVECTOR*)(&pObjectMatrix->pos), nullptr, nullptr);

	}

};

using StreamAtObjectPtr = std::shared_ptr<StreamAtObject>;
#define MakeStreamAtObject std::make_shared<StreamAtObject>
