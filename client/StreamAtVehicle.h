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
#include <game/CVehicle.h>

#include "StreamInfo.h"
#include "LocalStream.h"
#include "Channel.h"

class StreamAtVehicle : public LocalStream {
private:

	const WORD vehicleId;

	void ChannelCreationHandler(const Channel& channel) override {

		this->LocalStream::ChannelCreationHandler(channel);

		const auto pNetGame = SAMP::pNetGame();
		if (!pNetGame) return;

		const auto pVehiclePool = pNetGame->GetVehiclePool();
		if (!pVehiclePool) return;

		const auto pVehicle = pVehiclePool->m_pGameObject[this->vehicleId];
		if (!pVehicle) return;

		const auto pVehicleMatrix = pVehicle->GetMatrix();
		if (!pVehicleMatrix) return;

		BASS_ChannelSet3DPosition(
			channel.handle, (BASS_3DVECTOR*)(&pVehicleMatrix->pos),
			&BASS_3DVECTOR(0, 0, 0), &BASS_3DVECTOR(0, 0, 0)
		);

	}

public:

	StreamAtVehicle() = delete;
	StreamAtVehicle(const StreamAtVehicle& object) = delete;
	StreamAtVehicle(StreamAtVehicle&& object) = delete;

	StreamAtVehicle& operator=(const StreamAtVehicle& object) = delete;
	StreamAtVehicle& operator=(StreamAtVehicle&& object) = delete;

	StreamAtVehicle(
		const PlayHandlerType& playHandler,
		const StopHandlerType& stopHandler,
		const std::string& name,
		const D3DCOLOR color,
		const WORD vehicleId,
		const float distance
	) :

		LocalStream(
			BASS_SAMPLE_3D | BASS_SAMPLE_MUTEMAX,
			playHandler, stopHandler,
			StreamType::LocalStreamAtVehicle,
			name, color, distance
		),

		vehicleId(vehicleId)

	{}

	void Tick() override {

		this->LocalStream::Tick();

		const auto pNetGame = SAMP::pNetGame();
		if (!pNetGame) return;

		const auto pVehiclePool = pNetGame->GetVehiclePool();
		if (!pVehiclePool) return;

		const auto pVehicle = pVehiclePool->m_pGameObject[this->vehicleId];
		if (!pVehicle) return;

		const auto pVehicleMatrix = pVehicle->GetMatrix();
		if (!pVehicleMatrix) return;

		for (const auto& iChan : this->channels) if (iChan->speaker != SV::NonePlayer)
			BASS_ChannelSet3DPosition(iChan->handle, (BASS_3DVECTOR*)(&pVehicleMatrix->pos), nullptr, nullptr);

	}

};

using StreamAtVehiclePtr = std::shared_ptr<StreamAtVehicle>;
#define MakeStreamAtVehicle std::make_shared<StreamAtVehicle>
