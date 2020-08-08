/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CNetStats.h"

#if defined(SAMP_R1)

SAMP::CNetStats*& SAMP::pNetStats() { return *(SAMP::CNetStats**)SAMP_ADDROF(0x21A0D0); }

SAMP::CNetStats::CNetStats(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CNetStats*, IDirect3DDevice9*))SAMP_ADDROF(0x5D220))(this, pDevice);
}

void SAMP::CNetStats::Draw() {
	((void(__thiscall*)(CNetStats*))SAMP_ADDROF(0x5D250))(this);
}

#elif defined(SAMP_R3)

SAMP::CNetStats*& SAMP::pNetStats() { return *(SAMP::CNetStats**)SAMP_ADDROF(0x26E8B4); }

SAMP::CNetStats::CNetStats(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CNetStats*, IDirect3DDevice9*))SAMP_ADDROF(0x605C0))(this, pDevice);
}

void SAMP::CNetStats::Draw() {
	((void(__thiscall*)(CNetStats*))SAMP_ADDROF(0x605F0))(this);
}

#endif
