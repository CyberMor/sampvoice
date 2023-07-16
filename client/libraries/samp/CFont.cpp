/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CFont.h"

#if defined(SAMP_R3)

SAMP::CFont::CFont() {
	((void(__thiscall*)(CFont*))SAMP_ADDROF(0x6B160))(this);
}

SAMP::CFont::CFont(ID3DXFont* pFont) {
	*(void**)this = (void*)SAMP_ADDROF(0xEA3B8);
	m_pFont = pFont;
}

#endif
