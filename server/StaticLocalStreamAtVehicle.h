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

class StaticLocalStreamAtVehicle : public LocalStream {
public:

	StaticLocalStreamAtVehicle() = delete;
	StaticLocalStreamAtVehicle(const StaticLocalStreamAtVehicle& object) = delete;
	StaticLocalStreamAtVehicle(StaticLocalStreamAtVehicle&& object) = delete;

	StaticLocalStreamAtVehicle& operator=(const StaticLocalStreamAtVehicle& object) = delete;
	StaticLocalStreamAtVehicle& operator=(StaticLocalStreamAtVehicle&& object) = delete;

	StaticLocalStreamAtVehicle(
		const float distance,
		const uint16_t vehicleId,
		const uint32_t color,
		const std::string& name
	);

};
