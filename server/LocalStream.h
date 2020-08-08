/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include "ControlPacket.h"
#include "Stream.h"

class LocalStream : public Stream {
protected:

	ControlPacketContainerPtr packetStreamUpdateDistance = nullptr;

protected:

	LocalStream(const float distance);

public:

	LocalStream() = delete;
	LocalStream(const LocalStream& object) = delete;
	LocalStream(LocalStream&& object) = delete;

	LocalStream& operator=(const LocalStream& object) = delete;
	LocalStream& operator=(LocalStream&& object) = delete;

	void UpdateDistance(const float distance);

	virtual ~LocalStream();

};
