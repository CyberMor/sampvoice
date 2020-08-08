/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CPlayerTags.h"

#if defined(SAMP_R1)

SAMP::CPlayerTags*& SAMP::pPlayerTags() { return *(SAMP::CPlayerTags**)SAMP_ADDROF(0x21A0B0); }

SAMP::CPlayerTags::CPlayerTags(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CPlayerTags*, IDirect3DDevice9*))SAMP_ADDROF(0x68610))(this, pDevice);
}

SAMP::CPlayerTags::~CPlayerTags() {
	((void(__thiscall*)(CPlayerTags*))SAMP_ADDROF(0x68640))(this);
}

void SAMP::CPlayerTags::BeginLabel() {
	((void(__thiscall*)(CPlayerTags*))SAMP_ADDROF(0x686A0))(this);
}

void SAMP::CPlayerTags::DrawLabel(CVectorSA* pPosition, const char* szText, D3DCOLOR color, float fDistanceToCamera, bool bDrawStatus, int nStatus) {
	((void(__thiscall*)(CPlayerTags*, CVectorSA*, const char*, D3DCOLOR, float, bool, int))SAMP_ADDROF(0x686C0))(this, pPosition, szText, color, fDistanceToCamera, bDrawStatus, nStatus);
}

void SAMP::CPlayerTags::EndLabel() {
	((void(__thiscall*)(CPlayerTags*))SAMP_ADDROF(0x686B0))(this);
}

void SAMP::CPlayerTags::BeginHealthBar() {
	((void(__thiscall*)(CPlayerTags*))SAMP_ADDROF(0x68FD0))(this);
}

void SAMP::CPlayerTags::DrawHealthBar(CVectorSA* pPosition, float fHealth, float fArmour, float fDistanceToCamera) {
	((void(__thiscall*)(CPlayerTags*, CVectorSA*, float, float, float))SAMP_ADDROF(0x689C0))(this, pPosition, fHealth, fArmour, fDistanceToCamera);
}

void SAMP::CPlayerTags::EndHealthBar() {
	((void(__thiscall*)(CPlayerTags*))SAMP_ADDROF(0x68670))(this);
}

void SAMP::CPlayerTags::OnLostDevice() {
	((void(__thiscall*)(CPlayerTags*))SAMP_ADDROF(0x68F70))(this);
}

void SAMP::CPlayerTags::OnResetDevice() {
	((void(__thiscall*)(CPlayerTags*))SAMP_ADDROF(0x68FA0))(this);
}

#elif defined(SAMP_R3)

SAMP::CPlayerTags*& SAMP::pPlayerTags() { return *(SAMP::CPlayerTags**)SAMP_ADDROF(0x26E890); }

SAMP::CPlayerTags::CPlayerTags(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CPlayerTags*, IDirect3DDevice9*))SAMP_ADDROF(0x6C580))(this, pDevice);
}

SAMP::CPlayerTags::~CPlayerTags() {
	((void(__thiscall*)(CPlayerTags*))SAMP_ADDROF(0x6C5B0))(this);
}

void SAMP::CPlayerTags::EndHealthBar() {
	((void(__thiscall*)(CPlayerTags*))SAMP_ADDROF(0x6C5E0))(this);
}

void SAMP::CPlayerTags::BeginLabel() {
	((void(__thiscall*)(CPlayerTags*))SAMP_ADDROF(0x6C610))(this);
}

void SAMP::CPlayerTags::EndLabel() {
	((void(__thiscall*)(CPlayerTags*))SAMP_ADDROF(0x6C620))(this);
}

void SAMP::CPlayerTags::DrawLabel(CVectorSA* pPosition, const char* szText, D3DCOLOR color, float fDistanceToCamera, bool bDrawStatus, int nStatus) {
	((void(__thiscall*)(CPlayerTags*, CVectorSA*, const char*, D3DCOLOR, float, bool, int))SAMP_ADDROF(0x6C630))(this, pPosition, szText, color, fDistanceToCamera, bDrawStatus, nStatus);
}

void SAMP::CPlayerTags::DrawHealthBar(CVectorSA* pPosition, float fHealth, float fArmour, float fDistanceToCamera) {
	((void(__thiscall*)(CPlayerTags*, CVectorSA*, float, float, float))SAMP_ADDROF(0x6C930))(this, pPosition, fHealth, fArmour, fDistanceToCamera);
}

void SAMP::CPlayerTags::OnLostDevice() {
	((void(__thiscall*)(CPlayerTags*))SAMP_ADDROF(0x6CEE0))(this);
}

void SAMP::CPlayerTags::OnResetDevice() {
	((void(__thiscall*)(CPlayerTags*))SAMP_ADDROF(0x6CF10))(this);
}

void SAMP::CPlayerTags::BeginHealthBar() {
	((void(__thiscall*)(CPlayerTags*))SAMP_ADDROF(0x6CF40))(this);
}

#endif
