/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>
#include <cstdint>

#include "Stream.h"

class GlobalStream : public Stream {
public:

	GlobalStream() = delete;
	GlobalStream(const GlobalStream& object) = delete;
	GlobalStream(GlobalStream&& object) = delete;

	GlobalStream& operator=(const GlobalStream& object) = delete;
	GlobalStream& operator=(GlobalStream&& object) = delete;

	GlobalStream(const uint32_t color, const std::string& name);

};
