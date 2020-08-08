/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <Windows.h>

#include <util/Memory.hpp>

struct ControlPacket {
#pragma pack(push, 1)

	WORD packet;
	WORD length;
	BYTE data[];

	DWORD GetFullSize() const;

#pragma pack(pop)
};

using ControlPacketContainer = Memory::ObjectContainer<ControlPacket>;
using ControlPacketContainerPtr = Memory::ObjectContainerPtr<ControlPacket>;
#define MakeControlPacketContainer MakeObjectContainer(ControlPacket)
