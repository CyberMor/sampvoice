/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CMenuPool.h"

#if defined(SAMP_R1)

SAMP::CMenuPool::CMenuPool() {
	((void(__thiscall*)(CMenuPool*))SAMP_ADDROF(0x7AB0))(this);
}

SAMP::CMenuPool::~CMenuPool() {
	((void(__thiscall*)(CMenuPool*))SAMP_ADDROF(0x7E20))(this);
}

SAMP::CMenu* SAMP::CMenuPool::Create(NUMBER nId, const char* szTitle, float fX, float fY, char nColumns, float fFirstColumnWidth, float fSecondColumnWidth, const CMenu::Interaction* pInteraction) {
	return ((CMenu * (__thiscall*)(CMenuPool*, NUMBER, const char*, float, float, char, float, float, const CMenu::Interaction*))SAMP_ADDROF(0x7B00))(this, nId, szTitle, fX, fY, nColumns, fFirstColumnWidth, fSecondColumnWidth, pInteraction);
}

BOOL SAMP::CMenuPool::Delete(NUMBER nId) {
	return ((BOOL(__thiscall*)(CMenuPool*, NUMBER))SAMP_ADDROF(0x7BD0))(this, nId);
}

void SAMP::CMenuPool::Show(NUMBER nId) {
	((void(__thiscall*)(CMenuPool*, NUMBER))SAMP_ADDROF(0x7C20))(this, nId);
}

void SAMP::CMenuPool::Hide(NUMBER nId) {
	((void(__thiscall*)(CMenuPool*, NUMBER))SAMP_ADDROF(0x7C80))(this, nId);
}

char* SAMP::CMenuPool::GetTextPointer(const char* szName) {
	return ((char* (__thiscall*)(CMenuPool*, const char*))SAMP_ADDROF(0x7CC0))(this, szName);
}

void SAMP::CMenuPool::Process() {
	((void(__thiscall*)(CMenuPool*))SAMP_ADDROF(0x7E60))(this);
}

#elif defined(SAMP_R3)

SAMP::CMenuPool::CMenuPool() {
	((void(__thiscall*)(CMenuPool*))SAMP_ADDROF(0x7AE0))(this);
}

SAMP::CMenuPool::~CMenuPool() {
	((void(__thiscall*)(CMenuPool*))SAMP_ADDROF(0x7E50))(this);
}

SAMP::CMenu* SAMP::CMenuPool::Create(NUMBER nId, const char* szTitle, float fX, float fY, char nColumns, float fFirstColumnWidth, float fSecondColumnWidth, const CMenu::Interaction* pInteraction) {
	return ((CMenu * (__thiscall*)(CMenuPool*, NUMBER, const char*, float, float, char, float, float, const CMenu::Interaction*))SAMP_ADDROF(0x7B30))(this, nId, szTitle, fX, fY, nColumns, fFirstColumnWidth, fSecondColumnWidth, pInteraction);
}

BOOL SAMP::CMenuPool::Delete(NUMBER nId) {
	return ((BOOL(__thiscall*)(CMenuPool*, NUMBER))SAMP_ADDROF(0x7C00))(this, nId);
}

void SAMP::CMenuPool::Show(NUMBER nId) {
	((void(__thiscall*)(CMenuPool*, NUMBER))SAMP_ADDROF(0x7C50))(this, nId);
}

void SAMP::CMenuPool::Hide(NUMBER nId) {
	((void(__thiscall*)(CMenuPool*, NUMBER))SAMP_ADDROF(0x7CB0))(this, nId);
}

char* SAMP::CMenuPool::GetTextPointer(const char* szName) {
	return ((char* (__thiscall*)(CMenuPool*, const char*))SAMP_ADDROF(0x7CF0))(this, szName);
}

void SAMP::CMenuPool::Process() {
	((void(__thiscall*)(CMenuPool*))SAMP_ADDROF(0x7E90))(this);
}

#endif
