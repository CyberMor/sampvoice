/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "Settings.h"

#if defined(SAMP_R1)

SAMP::Settings& SAMP::settings() { return *(SAMP::Settings*)SAMP_ADDROF(0x219760); }

void SAMP::Settings::Initialize() {
	((void(__cdecl*)())SAMP_ADDROF(0xB2FF0))();
}

void SAMP::Settings::GetFromCommandLine(const char* szLine, char* szString) {
	((void(__cdecl*)(const char*, char*))SAMP_ADDROF(0xB28F0))(szLine, szString);
}

void SAMP::Settings::GetFromQuotes(const char* szLine, char* szString) {
	((void(__cdecl*)(const char*, char*))SAMP_ADDROF(0xB2940))(szLine, szString);
}

#elif defined(SAMP_R3)

SAMP::Settings& SAMP::settings() { return *(SAMP::Settings*)SAMP_ADDROF(0x26DD30); }

void SAMP::Settings::Initialize() {
	((void(__cdecl*)())SAMP_ADDROF(0xC4E00))();
}

void SAMP::Settings::GetFromCommandLine(const char* szLine, char* szBuffer) {
	((void(__cdecl*)(const char*, char*))SAMP_ADDROF(0xC4700))(szLine, szBuffer);
}

void SAMP::Settings::GetFromQuotes(const char* szLine, char* szBuffer) {
	((void(__cdecl*)(const char*, char*))SAMP_ADDROF(0xC4750))(szLine, szBuffer);
}

#endif
