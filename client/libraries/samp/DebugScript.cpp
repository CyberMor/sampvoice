/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "DebugScript.h"

#if defined(SAMP_R1)

SAMP::CObjectPool*& SAMP::DebugScript::pPrivateObjectPool() { return *(SAMP::CObjectPool**)SAMP_ADDROF(0x13BB74); }
unsigned short& SAMP::DebugScript::nObjectCount() { return *(unsigned short*)SAMP_ADDROF(0x13BB78); }
SAMP::CVectorSA& SAMP::DebugScript::vNewCameraPos() { return *(SAMP::CVectorSA*)SAMP_ADDROF(0x13BB68); }

void SAMP::DebugScript::Initialize(const char* szFile) {
	((void(__cdecl*)(const char*))SAMP_ADDROF(0x99FF0))(szFile);
}

void SAMP::DebugScript::ProcessLine(const char* szLine) {
	((void(__cdecl*)(const char*))SAMP_ADDROF(0x99EE0))(szLine);
}

char* SAMP::DebugScript::GetCommandParams(char* szLine) {
	return ((char* (__cdecl*)(char*))SAMP_ADDROF(0x99AF0))(szLine);
}

void SAMP::DebugScript::CreateVehicle(const char* szParams) {
	((void(__cdecl*)(const char*))SAMP_ADDROF(0x99C50))(szParams);
}

#elif defined(SAMP_R3)

SAMP::CObjectPool*& SAMP::DebugScript::pPrivateObjectPool() { return *(SAMP::CObjectPool**)SAMP_ADDROF(0x14FCFC); }
unsigned short& SAMP::DebugScript::nObjectCount() { return *(unsigned short*)SAMP_ADDROF(0x14FD00); }
SAMP::CVectorSA& SAMP::DebugScript::vNewCameraPos() { return *(SAMP::CVectorSA*)SAMP_ADDROF(0x14FCF0); }

void SAMP::DebugScript::Initialize(const char* szFile) {
	((void(__cdecl*)(const char*))SAMP_ADDROF(0x9E2A0))(szFile);
}

void SAMP::DebugScript::ProcessLine(const char* szLine) {
	((void(__cdecl*)(const char*))SAMP_ADDROF(0x9E190))(szLine);
}

char* SAMP::DebugScript::GetCommandParams(char* szLine) {
	return ((char* (__cdecl*)(char*))SAMP_ADDROF(0x9DDA0))(szLine);
}

void SAMP::DebugScript::CreateVehicle(const char* szParams) {
	((void(__cdecl*)(const char*))SAMP_ADDROF(0x9DF00))(szParams);
}

#endif
