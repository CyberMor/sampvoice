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
#include "DynamicStream.h"

class DynamicLocalStreamAtPoint : public PointStream, public DynamicStream {
public:

	DynamicLocalStreamAtPoint() = delete;
	DynamicLocalStreamAtPoint(const DynamicLocalStreamAtPoint& object) = delete;
	DynamicLocalStreamAtPoint(DynamicLocalStreamAtPoint&& object) = delete;

	DynamicLocalStreamAtPoint& operator=(const DynamicLocalStreamAtPoint& object) = delete;
	DynamicLocalStreamAtPoint& operator=(DynamicLocalStreamAtPoint&& object) = delete;

	DynamicLocalStreamAtPoint(
		const float distance,
		const uint32_t maxPlayers,
		const CVector& position,
		const uint32_t color,
		const std::string& name
	);

	void Tick() override;

};
