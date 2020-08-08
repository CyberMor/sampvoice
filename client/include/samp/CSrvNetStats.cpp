/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CSrvNetStats.h"

#if defined(SAMP_R3)

SAMP::CSrvNetStats*& SAMP::pServerNetStatistics() { return *(SAMP::CSrvNetStats**)SAMP_ADDROF(0x26E8B8); }

SAMP::CSrvNetStats::CSrvNetStats(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CSrvNetStats*, IDirect3DDevice9*))SAMP_ADDROF(0x70B30))(this, pDevice);
}

void SAMP::CSrvNetStats::Draw() {
	((void(__thiscall*)(CSrvNetStats*))SAMP_ADDROF(0x70B70))(this);
}

#endif
