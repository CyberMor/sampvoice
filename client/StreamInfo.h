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

enum class StreamType {

	None,

	GlobalStream,

	LocalStreamAtPoint,
	LocalStreamAtVehicle,
	LocalStreamAtPlayer,
	LocalStreamAtObject

};

struct StreamInfo {

	const StreamType type;
	const std::string name;
	const D3DCOLOR color;

	StreamInfo(
		const StreamType type,
		const std::string& name,
		const D3DCOLOR color
	) :
		type(type),
		name(name),
		color(color)
	{}

};

using StreamInfoPtr = std::shared_ptr<StreamInfo>;
#define MakeStreamInfo std::make_shared<StreamInfo>
