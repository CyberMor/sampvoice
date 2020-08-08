/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>
#include <cstdint>

#include "LocalStream.h"

class StaticLocalStreamAtPlayer : public LocalStream {
public:

	StaticLocalStreamAtPlayer() = delete;
	StaticLocalStreamAtPlayer(const StaticLocalStreamAtPlayer& object) = delete;
	StaticLocalStreamAtPlayer(StaticLocalStreamAtPlayer&& object) = delete;

	StaticLocalStreamAtPlayer& operator=(const StaticLocalStreamAtPlayer& object) = delete;
	StaticLocalStreamAtPlayer& operator=(StaticLocalStreamAtPlayer&& object) = delete;

	StaticLocalStreamAtPlayer(
		const float distance,
		const uint16_t playerId,
		const uint32_t color,
		const std::string& name
	);

};
