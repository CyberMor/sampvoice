/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CObjectSelection.h"

#if defined(SAMP_R1)

SAMP::CObjectSelection*& SAMP::pObjectSelection() { return *(SAMP::CObjectSelection**)SAMP_ADDROF(0x21A0C8); }

SAMP::CObjectSelection::CObjectSelection() {
	((void(__thiscall*)(CObjectSelection*))SAMP_ADDROF(0x69320))(this);
}

SAMP::ID SAMP::CObjectSelection::DefineObject() {
	return ((ID(__thiscall*)(CObjectSelection*))SAMP_ADDROF(0x69330))(this);
}

void SAMP::CObjectSelection::DrawLabels() {
	((void(__thiscall*)(CObjectSelection*))SAMP_ADDROF(0x69380))(this);
}

void SAMP::CObjectSelection::Enable(BOOL bEnable) {
	((void(__thiscall*)(CObjectSelection*, BOOL))SAMP_ADDROF(0x694A0))(this, bEnable);
}

void SAMP::CObjectSelection::Draw() {
	((void(__thiscall*)(CObjectSelection*))SAMP_ADDROF(0x69520))(this);
}

void SAMP::CObjectSelection::SendNotification() {
	((void(__thiscall*)(CObjectSelection*))SAMP_ADDROF(0x695F0))(this);
}

BOOL SAMP::CObjectSelection::MsgProc(int uMsg, int wParam, int lParam) {
	return ((BOOL(__thiscall*)(CObjectSelection*, int, int, int))SAMP_ADDROF(0x69760))(this, uMsg, wParam, lParam);
}

#elif defined(SAMP_R3)

SAMP::CObjectSelection*& SAMP::pObjectSelection() { return *(SAMP::CObjectSelection**)SAMP_ADDROF(0x26E8AC); }

SAMP::CObjectSelection::CObjectSelection() {
	((void(__thiscall*)(CObjectSelection*))SAMP_ADDROF(0x6D290))(this);
}

SAMP::ID SAMP::CObjectSelection::DefineObject() {
	return ((ID(__thiscall*)(CObjectSelection*))SAMP_ADDROF(0x6D2A0))(this);
}

void SAMP::CObjectSelection::DrawLabels() {
	((void(__thiscall*)(CObjectSelection*))SAMP_ADDROF(0x6D2F0))(this);
}

void SAMP::CObjectSelection::Enable(BOOL bEnable) {
	((void(__thiscall*)(CObjectSelection*, BOOL))SAMP_ADDROF(0x6D410))(this, bEnable);
}

void SAMP::CObjectSelection::Draw() {
	((void(__thiscall*)(CObjectSelection*))SAMP_ADDROF(0x6D490))(this);
}

void SAMP::CObjectSelection::SendNotification() {
	((void(__thiscall*)(CObjectSelection*))SAMP_ADDROF(0x6D560))(this);
}

BOOL SAMP::CObjectSelection::MsgProc(int uMsg, int wParam, int lParam) {
	return ((BOOL(__thiscall*)(CObjectSelection*, int, int, int))SAMP_ADDROF(0x6D6D0))(this, uMsg, wParam, lParam);
}

#endif
