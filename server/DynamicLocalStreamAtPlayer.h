/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>
#include <cstdint>

#include "DynamicStream.h"

class DynamicLocalStreamAtPlayer : public DynamicStream {
public:

	DynamicLocalStreamAtPlayer() = delete;
	DynamicLocalStreamAtPlayer(const DynamicLocalStreamAtPlayer& object) = delete;
	DynamicLocalStreamAtPlayer(DynamicLocalStreamAtPlayer&& object) = delete;

	DynamicLocalStreamAtPlayer& operator=(const DynamicLocalStreamAtPlayer& object) = delete;
	DynamicLocalStreamAtPlayer& operator=(DynamicLocalStreamAtPlayer&& object) = delete;

	DynamicLocalStreamAtPlayer(
		const float distance,
		const uint32_t maxPlayers,
		const uint16_t playerId,
		const uint32_t color,
		const std::string& name
	);

	void Tick() override;

};
