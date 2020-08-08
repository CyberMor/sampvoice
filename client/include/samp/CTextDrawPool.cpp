/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CTextDrawPool.h"

#if defined(SAMP_R1)

SAMP::CTextDrawPool::CTextDrawPool() {
	((void(__thiscall*)(CTextDrawPool*))SAMP_ADDROF(0x1ACB0))(this);
}

SAMP::CTextDrawPool::~CTextDrawPool() {
	((void(__thiscall*)(CTextDrawPool*))SAMP_ADDROF(0x1ADE0))(this);
}

SAMP::CTextDraw* SAMP::CTextDrawPool::Create(int nId, CTextDraw::Transmit* pTransmit, const char* szText) {
	return ((CTextDraw * (__thiscall*)(CTextDrawPool*, int, CTextDraw::Transmit*, const char*))SAMP_ADDROF(0x1AE20))(this, nId, pTransmit, szText);
}

void SAMP::CTextDrawPool::Delete(ID nId) {
	((void(__thiscall*)(CTextDrawPool*, ID))SAMP_ADDROF(0x1AD00))(this, nId);
}

void SAMP::CTextDrawPool::Draw() {
	((void(__thiscall*)(CTextDrawPool*))SAMP_ADDROF(0x1AD40))(this);
}

#elif defined(SAMP_R3)

SAMP::CTextDrawPool::CTextDrawPool() {
	((void(__thiscall*)(CTextDrawPool*))SAMP_ADDROF(0x1E050))(this);
}

SAMP::CTextDrawPool::~CTextDrawPool() {
	((void(__thiscall*)(CTextDrawPool*))SAMP_ADDROF(0x1E180))(this);
}

void SAMP::CTextDrawPool::Delete(ID nId) {
	((void(__thiscall*)(CTextDrawPool*, ID))SAMP_ADDROF(0x1E0A0))(this, nId);
}

void SAMP::CTextDrawPool::Draw() {
	((void(__thiscall*)(CTextDrawPool*))SAMP_ADDROF(0x1E0E0))(this);
}

SAMP::CTextDraw* SAMP::CTextDrawPool::Create(int nId, CTextDraw::Transmit* pData, const char* szText) {
	return ((CTextDraw * (__thiscall*)(CTextDrawPool*, int, CTextDraw::Transmit*, const char*))SAMP_ADDROF(0x1E1C0))(this, nId, pData, szText);
}

#endif
