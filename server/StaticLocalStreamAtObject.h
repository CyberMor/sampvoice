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

class StaticLocalStreamAtObject : public LocalStream {
public:

	StaticLocalStreamAtObject() = delete;
	StaticLocalStreamAtObject(const StaticLocalStreamAtObject& object) = delete;
	StaticLocalStreamAtObject(StaticLocalStreamAtObject&& object) = delete;

	StaticLocalStreamAtObject& operator=(const StaticLocalStreamAtObject& object) = delete;
	StaticLocalStreamAtObject& operator=(StaticLocalStreamAtObject&& object) = delete;

	StaticLocalStreamAtObject(
		const float distance,
		const uint16_t objectId,
		const uint32_t color,
		const std::string& name
	);

};
