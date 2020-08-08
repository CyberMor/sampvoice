/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CConfig.h"

#if defined(SAMP_R1)

SAMP::CConfig*& SAMP::pConfig() { return *(SAMP::CConfig**)SAMP_ADDROF(0x21A0E0); }

SAMP::CConfig::CConfig(const char* szFile) {
	((void(__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x62820))(this, szFile);
}

SAMP::CConfig::~CConfig() {
	((void(__thiscall*)(CConfig*))SAMP_ADDROF(0x62056))(this);
}

void SAMP::CConfig::FindFirstFree() {
	((void(__thiscall*)(CConfig*))SAMP_ADDROF(0x62080))(this);
}

int SAMP::CConfig::GetIndex(const char* szEntry) {
	return ((int(__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x620D0))(this, szEntry);
}

bool SAMP::CConfig::DoesExist(const char* szEntry) {
	return ((BOOL(__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x62170))(this, szEntry);
}

int SAMP::CConfig::CreateEntry(const char* szName) {
	return ((int(__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x62190))(this, szName);
}

int SAMP::CConfig::GetIntValue(const char* szEntry) {
	return ((int(__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x62250))(this, szEntry);
}

const char* SAMP::CConfig::GetStringValue(const char* szEntry) {
	return ((const char* (__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x62280))(this, szEntry);
}

float SAMP::CConfig::GetFloatValue(const char* szEntry) {
	return ((float(__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x622B0))(this, szEntry);
}

BOOL SAMP::CConfig::Free(const char* szEntry) {
	return ((BOOL(__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x622E0))(this, szEntry);
}

int SAMP::CConfig::GetValueType(const char* szEntry) {
	return ((int(__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x62340))(this, szEntry);
}

SAMP::CConfig::Entry* SAMP::CConfig::GetEntry(int nIndex) {
	return ((Entry * (__thiscall*)(CConfig*, int))SAMP_ADDROF(0x62370))(this, nIndex);
}

BOOL SAMP::CConfig::Save() {
	return ((BOOL(__thiscall*)(CConfig*))SAMP_ADDROF(0x62410))(this);
}

BOOL SAMP::CConfig::WriteIntValue(const char* szEntry, int nValue, BOOL bReadOnly) {
	return ((BOOL(__thiscall*)(CConfig*, const char*, int, BOOL))SAMP_ADDROF(0x624C0))(this, szEntry, nValue, bReadOnly);
}

BOOL SAMP::CConfig::WriteStringValue(const char* szEntry, const char* szValue, BOOL bReadOnly) {
	return ((BOOL(__thiscall*)(CConfig*, const char*, const char*, BOOL))SAMP_ADDROF(0x62520))(this, szEntry, szValue, bReadOnly);
}

BOOL SAMP::CConfig::WriteFloatValue(const char* szEntry, float fValue, BOOL bReadOnly) {
	return ((BOOL(__thiscall*)(CConfig*, const char*, float, BOOL))SAMP_ADDROF(0x625C0))(this, szEntry, fValue, bReadOnly);
}

void SAMP::CConfig::Write(const char* szEntry, char* szBuffer) {
	((void(__thiscall*)(CConfig*, const char*, const char*))SAMP_ADDROF(0x62620))(this, szEntry, szBuffer);
}

BOOL SAMP::CConfig::Load() {
	return ((BOOL(__thiscall*)(CConfig*))SAMP_ADDROF(0x626B0))(this);
}

int SAMP::CConfig::GetType(const char* szString) {
	return ((int(__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x623A0))(this, szString);
}

#elif defined(SAMP_R3)

SAMP::CConfig*& SAMP::pConfig() { return *(SAMP::CConfig**)SAMP_ADDROF(0x26E8C4); }

SAMP::CConfig::CConfig(const char* szFile) {
	((void(__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x65C70))(this, szFile);
}

SAMP::CConfig::~CConfig() {
	((void(__thiscall*)(CConfig*))SAMP_ADDROF(0x65490))(this);
}

void SAMP::CConfig::FindFirstFree() {
	((void(__thiscall*)(CConfig*))SAMP_ADDROF(0x654D0))(this);
}

int SAMP::CConfig::GetIndex(const char* szEntry) {
	return ((int(__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x65520))(this, szEntry);
}

bool SAMP::CConfig::DoesExist(const char* szEntry) {
	return ((bool(__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x655C0))(this, szEntry);
}

int SAMP::CConfig::CreateEntry(const char* szName) {
	return ((int(__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x655E0))(this, szName);
}

int SAMP::CConfig::GetIntValue(const char* szEntry) {
	return ((int(__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x656A0))(this, szEntry);
}

const char* SAMP::CConfig::GetStringValue(const char* szEntry) {
	return ((const char* (__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x656D0))(this, szEntry);
}

float SAMP::CConfig::GetFloatValue(const char* szEntry) {
	return ((float(__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x65700))(this, szEntry);
}

BOOL SAMP::CConfig::Free(const char* szEntry) {
	return ((BOOL(__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x65730))(this, szEntry);
}

int SAMP::CConfig::GetValueType(const char* szEntry) {
	return ((int(__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x65790))(this, szEntry);
}

SAMP::CConfig::Entry* SAMP::CConfig::GetEntry(int nIndex) {
	return ((Entry * (__thiscall*)(CConfig*, int))SAMP_ADDROF(0x657C0))(this, nIndex);
}

int SAMP::CConfig::GetType(const char* szString) {
	return ((int(__thiscall*)(CConfig*, const char*))SAMP_ADDROF(0x657F0))(this, szString);
}

BOOL SAMP::CConfig::Save() {
	return ((BOOL(__thiscall*)(CConfig*))SAMP_ADDROF(0x65860))(this);
}

BOOL SAMP::CConfig::WriteIntValue(const char* szEntry, int nValue, BOOL bReadOnly) {
	return ((BOOL(__thiscall*)(CConfig*, const char*, int, BOOL))SAMP_ADDROF(0x65910))(this, szEntry, nValue, bReadOnly);
}

BOOL SAMP::CConfig::WriteStringValue(const char* szEntry, const char* szValue, BOOL bReadOnly) {
	return ((BOOL(__thiscall*)(CConfig*, const char*, const char*, BOOL))SAMP_ADDROF(0x65970))(this, szEntry, szValue, bReadOnly);
}

BOOL SAMP::CConfig::WriteFloatValue(const char* szEntry, float fValue, BOOL bReadOnly) {
	return ((BOOL(__thiscall*)(CConfig*, const char*, float, BOOL))SAMP_ADDROF(0x65A10))(this, szEntry, fValue, bReadOnly);
}

void SAMP::CConfig::Write(const char* szEntry, char* szBuffer) {
	((void(__thiscall*)(CConfig*, const char*, char*))SAMP_ADDROF(0x65A70))(this, szEntry, szBuffer);
}

BOOL SAMP::CConfig::Load() {
	return ((BOOL(__thiscall*)(CConfig*))SAMP_ADDROF(0x65B00))(this);
}

#endif
