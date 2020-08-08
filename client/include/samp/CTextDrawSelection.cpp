/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CTextDrawSelection.h"

#if defined(SAMP_R1)

SAMP::CTextDrawSelection*& SAMP::pTextdrawSelection() { return *(SAMP::CTextDrawSelection**)SAMP_ADDROF(0x21A0CC); }

BOOL SAMP::CTextDrawSelection::MsgProc(int uMsg, int wParam, int lParam) {
	return ((BOOL(__thiscall*)(CTextDrawSelection*, int, int, int))SAMP_ADDROF(0x6CF90))(this, uMsg, wParam, lParam);
}

void SAMP::CTextDrawSelection::Disable() {
	((void(__thiscall*)(CTextDrawSelection*))SAMP_ADDROF(0x6CF40))(this);
}

void SAMP::CTextDrawSelection::SendNotification() {
	((void(__thiscall*)(CTextDrawSelection*))SAMP_ADDROF(0x6CEA0))(this);
}

void SAMP::CTextDrawSelection::Enable(D3DCOLOR hoveredColor) {
	((void(__thiscall*)(CTextDrawSelection*, D3DCOLOR))SAMP_ADDROF(0x6CE60))(this, hoveredColor);
}

void SAMP::CTextDrawSelection::ResetTextDraws() {
	((void(__thiscall*)(CTextDrawSelection*))SAMP_ADDROF(0x6CCD0))(this);
}

void SAMP::CTextDrawSelection::RawProcess() {
	((void(__thiscall*)(CTextDrawSelection*))SAMP_ADDROF(0x6CD30))(this);
}

void SAMP::CTextDrawSelection::Process() {
	((void(__thiscall*)(CTextDrawSelection*))SAMP_ADDROF(0x6CE30))(this);
}

#elif defined(SAMP_R3)

SAMP::CTextDrawSelection*& SAMP::pTextDrawSelection() { return *(SAMP::CTextDrawSelection**)SAMP_ADDROF(0x26E8B0); }

void SAMP::CTextDrawSelection::ResetTextDraws() {
	((void(__thiscall*)(CTextDrawSelection*))SAMP_ADDROF(0x70BC0))(this);
}

void SAMP::CTextDrawSelection::RawProcess() {
	((void(__thiscall*)(CTextDrawSelection*))SAMP_ADDROF(0x70C20))(this);
}

void SAMP::CTextDrawSelection::Process() {
	((void(__thiscall*)(CTextDrawSelection*))SAMP_ADDROF(0x70D20))(this);
}

void SAMP::CTextDrawSelection::Enable(D3DCOLOR hoveredColor) {
	((void(__thiscall*)(CTextDrawSelection*, D3DCOLOR))SAMP_ADDROF(0x70D50))(this, hoveredColor);
}

void SAMP::CTextDrawSelection::SendNotification() {
	((void(__thiscall*)(CTextDrawSelection*))SAMP_ADDROF(0x70D90))(this);
}

void SAMP::CTextDrawSelection::Disable() {
	((void(__thiscall*)(CTextDrawSelection*))SAMP_ADDROF(0x70E30))(this);
}

BOOL SAMP::CTextDrawSelection::MsgProc(int uMsg, int wParam, int lParam) {
	return ((BOOL(__thiscall*)(CTextDrawSelection*, int, int, int))SAMP_ADDROF(0x70E80))(this, uMsg, wParam, lParam);
}

#endif
