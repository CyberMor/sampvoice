/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <ysf/utils/cvector.h>

#include "ControlPacket.h"
#include "LocalStream.h"

class PointStream : public virtual LocalStream {
protected:

	ControlPacketContainerPtr packetStreamUpdatePosition = nullptr;

protected:

	PointStream(const float distance, const CVector& position);

public:

	PointStream() = delete;
	PointStream(const PointStream& object) = delete;
	PointStream(PointStream&& object) = delete;

	PointStream& operator=(const PointStream& object) = delete;
	PointStream& operator=(PointStream&& object) = delete;

	void UpdatePosition(const CVector& position);

	virtual ~PointStream();

};
