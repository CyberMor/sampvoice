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

class DynamicLocalStreamAtVehicle : public DynamicStream {
public:

	DynamicLocalStreamAtVehicle() = delete;
	DynamicLocalStreamAtVehicle(const DynamicLocalStreamAtVehicle& object) = delete;
	DynamicLocalStreamAtVehicle(DynamicLocalStreamAtVehicle&& object) = delete;

	DynamicLocalStreamAtVehicle& operator=(const DynamicLocalStreamAtVehicle& object) = delete;
	DynamicLocalStreamAtVehicle& operator=(DynamicLocalStreamAtVehicle&& object) = delete;

	DynamicLocalStreamAtVehicle(
		const float distance,
		const uint32_t maxPlayers,
		const uint16_t vehicleId,
		const uint32_t color,
		const std::string& name
	);

	void Tick() override;

};
