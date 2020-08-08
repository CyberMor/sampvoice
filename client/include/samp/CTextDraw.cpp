/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CTextDraw.h"

#if defined(SAMP_R1)

SAMP::CTextDraw::CTextDraw(Transmit* pTransmit, const char* szText) {
	((void(__thiscall*)(CTextDraw*, Transmit*, const char*))SAMP_ADDROF(0xACF10))(this, pTransmit, szText);
}

SAMP::CTextDraw::~CTextDraw() {
	((void(__thiscall*)(CTextDraw*))SAMP_ADDROF(0xAC860))(this);
}

void SAMP::CTextDraw::SetText(const char* szText) {
	((void(__thiscall*)(CTextDraw*, const char*))SAMP_ADDROF(0xAC870))(this, szText);
}

void SAMP::CTextDraw::Draw() {
	((void(__thiscall*)(CTextDraw*))SAMP_ADDROF(0xACD90))(this);
}

#elif defined(SAMP_R3)

SAMP::CTextDraw::CTextDraw(Transmit* pData, const char* szText) {
	((void(__thiscall*)(CTextDraw*, Transmit*, const char*))SAMP_ADDROF(0xB2E50))(this, pData, szText);
}

SAMP::CTextDraw::~CTextDraw() {
	((void(__thiscall*)(CTextDraw*))SAMP_ADDROF(0xB26C0))(this);
}

void SAMP::CTextDraw::SetText(const char* szText) {
	((void(__thiscall*)(CTextDraw*, const char*))SAMP_ADDROF(0xB26D0))(this, szText);
}

void SAMP::CTextDraw::Draw() {
	((void(__thiscall*)(CTextDraw*))SAMP_ADDROF(0xB2BF0))(this);
}

#endif
