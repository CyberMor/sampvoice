/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CPlayerInfo.h"

#if defined(SAMP_R1)

SAMP::CPlayerInfo::CPlayerInfo(const char* szName, BOOL bIsNPC) {
	((void(__thiscall*)(CPlayerInfo*, const char*, BOOL))SAMP_ADDROF(0x10CB0))(this, szName, bIsNPC);
}

SAMP::CPlayerInfo::~CPlayerInfo() {
	((void(__thiscall*)(CPlayerInfo*))SAMP_ADDROF(0x10A50))(this);
}

#elif defined(SAMP_R3)

SAMP::CPlayerInfo::CPlayerInfo(const char* szName, BOOL bIsNPC) {
	((void(__thiscall*)(CPlayerInfo*, const char*, BOOL))SAMP_ADDROF(0x13DE0))(this, szName, bIsNPC);
}

SAMP::CPlayerInfo::~CPlayerInfo() {
	((void(__thiscall*)(CPlayerInfo*))SAMP_ADDROF(0x13B60))(this);
}

#endif
