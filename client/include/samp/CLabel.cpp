/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CLabel.h"

#if defined(SAMP_R1)

SAMP::CLabel*& SAMP::pLabel() { return *(SAMP::CLabel**)SAMP_ADDROF(0x21A0C0); }

SAMP::CLabel::CLabel(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CLabel*, IDirect3DDevice9*))SAMP_ADDROF(0x674D0))(this, pDevice);
}

SAMP::CLabel::~CLabel() {
	((void(__thiscall*)(CLabel*))SAMP_ADDROF(0x674F0))(this);
}

void SAMP::CLabel::Begin() {
	((void(__thiscall*)(CLabel*))SAMP_ADDROF(0x67590))(this);
}

void SAMP::CLabel::End() {
	((void(__thiscall*)(CLabel*))SAMP_ADDROF(0x675A0))(this);
}

void SAMP::CLabel::OnLostDevice() {
	((void(__thiscall*)(CLabel*))SAMP_ADDROF(0x67510))(this);
}

void SAMP::CLabel::OnResetDevice() {
	((void(__thiscall*)(CLabel*))SAMP_ADDROF(0x67520))(this);
}

void SAMP::CLabel::Draw(CVectorSA* pPosition, const char* szText, D3DCOLOR color, BOOL bShadow, bool bNoObstacles) {
	((void(__thiscall*)(CLabel*, CVectorSA*, const char*, D3DCOLOR, bool, bool))SAMP_ADDROF(0x675B0))(this, pPosition, szText, color, bShadow, bNoObstacles);
}

BOOL SAMP::CLabel::HasNoObstacles(CVectorSA position) {
	return ((BOOL(__thiscall*)(CLabel*, CVectorSA))SAMP_ADDROF(0x67530))(this, position);
}

#elif defined(SAMP_R3)

SAMP::CLabel*& SAMP::pLabel() { return *(SAMP::CLabel**)SAMP_ADDROF(0x26E8A4); }

SAMP::CLabel::CLabel(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CLabel*, IDirect3DDevice9*))SAMP_ADDROF(0x6B440))(this, pDevice);
}

SAMP::CLabel::~CLabel() {
	((void(__thiscall*)(CLabel*))SAMP_ADDROF(0x6B460))(this);
}

void SAMP::CLabel::OnLostDevice() {
	((void(__thiscall*)(CLabel*))SAMP_ADDROF(0x6B480))(this);
}

void SAMP::CLabel::OnResetDevice() {
	((void(__thiscall*)(CLabel*))SAMP_ADDROF(0x6B490))(this);
}

BOOL SAMP::CLabel::HasNoObstacles(CVectorSA position) {
	return ((BOOL(__thiscall*)(CLabel*, CVectorSA))SAMP_ADDROF(0x6B4A0))(this, position);
}

void SAMP::CLabel::Begin() {
	((void(__thiscall*)(CLabel*))SAMP_ADDROF(0x6B500))(this);
}

void SAMP::CLabel::End() {
	((void(__thiscall*)(CLabel*))SAMP_ADDROF(0x6B510))(this);
}

void SAMP::CLabel::Draw(CVectorSA* pPosition, const char* szText, D3DCOLOR color, BOOL bShadow, bool bNoObstacles) {
	((void(__thiscall*)(CLabel*, CVectorSA*, const char*, D3DCOLOR, BOOL, bool))SAMP_ADDROF(0x6B520))(this, pPosition, szText, color, bShadow, bNoObstacles);
}

#endif
