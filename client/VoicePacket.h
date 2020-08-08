/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cstddef>

#include <Windows.h>

#include <util/Memory.hpp>

struct VoicePacket {
#pragma pack(push, 1)

	DWORD hash;
	DWORD svrkey;
	DWORD packet;
	DWORD stream;
	WORD sender;
	WORD length;
	DWORD packid;
	BYTE data[];

	DWORD GetFullSize() const;

	bool CheckHeader() const;
	void CalcHash();

#pragma pack(pop)
};

static_assert(offsetof(VoicePacket, hash) == 0, "[VoicePacket] : 'hash' field should be located at beginning of packet struct");

using VoicePacketContainer = Memory::ObjectContainer<VoicePacket>;
using VoicePacketContainerPtr = Memory::ObjectContainerPtr<VoicePacket>;
#define MakeVoicePacketContainer MakeObjectContainer(VoicePacket)
