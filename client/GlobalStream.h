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

#include "Stream.h"
#include "StreamInfo.h"

class GlobalStream : public Stream {
public:

	GlobalStream() = delete;
	GlobalStream(const GlobalStream& object) = delete;
	GlobalStream(GlobalStream&& object) = delete;

	GlobalStream& operator=(const GlobalStream& object) = delete;
	GlobalStream& operator=(GlobalStream&& object) = delete;

	GlobalStream(
		const PlayHandlerType& playHandler,
		const StopHandlerType& stopHandler,
		const std::string& name,
		const D3DCOLOR color
	) :

		Stream(
			NULL, playHandler, stopHandler,
			StreamType::GlobalStream,
			name, color
		)

	{}

};

using GlobalStreamPtr = std::shared_ptr<GlobalStream>;
#define MakeGlobalStream std::make_shared<GlobalStream>
