/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "Scripting.h"

#if defined(SAMP_R1)

CRunningScript*& SAMP::Scripting::pThread() { return *(CRunningScript**)SAMP_ADDROF(0x216048); }
unsigned char* SAMP::Scripting::aBuffer() { return (unsigned char*)SAMP_ADDROF(0x215F48); }
unsigned long& SAMP::Scripting::nLastUsedOpcode() { return *(unsigned long*)SAMP_ADDROF(0x21604C); }
unsigned long*& SAMP::Scripting::apThreadLocals() { return *(unsigned long**)SAMP_ADDROF(0x215F00); }
SAMP::Scripting::PROCESSOR& SAMP::Scripting::ProcessOneCommand() { return *(SAMP::Scripting::PROCESSOR*)SAMP_ADDROF(0x1023C4); }

void SAMP::Scripting::Initialize() {
	((void(__cdecl*)())SAMP_ADDROF(0xABF10))();
}

int SAMP::Scripting::ExecBuffer() {
	return ((int(__cdecl*)())SAMP_ADDROF(0xABC90))();
}

#elif defined(SAMP_R3)

CRunningScript*& SAMP::Scripting::pThread() { return *(CRunningScript**)SAMP_ADDROF(0x26B2A8); }
unsigned char* SAMP::Scripting::aBuffer() { return (unsigned char*)SAMP_ADDROF(0x26B1A8); }
unsigned long& SAMP::Scripting::nLastUsedOpcode() { return *(unsigned long*)SAMP_ADDROF(0x26B2AC); }
unsigned long*& SAMP::Scripting::apThreadLocals() { return *(unsigned long**)SAMP_ADDROF(0x26B160); }
SAMP::Scripting::PROCESSOR& SAMP::Scripting::ProcessOneCommand() { return *(SAMP::Scripting::PROCESSOR*)SAMP_ADDROF(0x1173A0); }
BOOL& SAMP::Scripting::bLocalDebug() { return *(BOOL*)SAMP_ADDROF(0x26B2B0); }

void SAMP::Scripting::Initialize() {
	((void(__cdecl*)())SAMP_ADDROF(0xB1CC0))();
}

int SAMP::Scripting::ExecBuffer() {
	return ((int(__cdecl*)())SAMP_ADDROF(0xB1A40))();
}

#endif
