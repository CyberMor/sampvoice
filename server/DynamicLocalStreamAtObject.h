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

class DynamicLocalStreamAtObject : public DynamicStream {
public:

	DynamicLocalStreamAtObject() = delete;
	DynamicLocalStreamAtObject(const DynamicLocalStreamAtObject& object) = delete;
	DynamicLocalStreamAtObject(DynamicLocalStreamAtObject&& object) = delete;

	DynamicLocalStreamAtObject& operator=(const DynamicLocalStreamAtObject& object) = delete;
	DynamicLocalStreamAtObject& operator=(DynamicLocalStreamAtObject&& object) = delete;

	DynamicLocalStreamAtObject(
		const float distance,
		const uint32_t maxPlayers,
		const uint16_t objectId,
		const uint32_t color,
		const std::string& name
	);

	void Tick() override;

};
