/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <set>
#include <atomic>
#include <cstdint>

#include "Stream.h"

struct PlayerInfo {

	const uint8_t pluginVersion = NULL;
	const bool microStatus = false;

	std::atomic_bool muteStatus = { false };
	std::atomic_bool recordStatus = { false };

	std::set<Stream*> listenerStreams;
	std::set<Stream*> speakerStreams;

	std::set<uint8_t> keys;

	PlayerInfo(
		const uint8_t pluginVersion,
		const bool microStatus
	) :
		pluginVersion(pluginVersion),
		microStatus(microStatus)
	{}

};
