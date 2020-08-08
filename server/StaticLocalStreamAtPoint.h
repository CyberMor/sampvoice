/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>
#include <cstdint>

#include <ysf/utils/cvector.h>

#include "PointStream.h"

class StaticLocalStreamAtPoint : public PointStream {
public:

	StaticLocalStreamAtPoint() = delete;
	StaticLocalStreamAtPoint(const StaticLocalStreamAtPoint& object) = delete;
	StaticLocalStreamAtPoint(StaticLocalStreamAtPoint&& object) = delete;

	StaticLocalStreamAtPoint& operator=(const StaticLocalStreamAtPoint& object) = delete;
	StaticLocalStreamAtPoint& operator=(StaticLocalStreamAtPoint&& object) = delete;

	StaticLocalStreamAtPoint(
		const float distance,
		const CVector& position,
		const uint32_t color,
		const std::string& name
	);

};
