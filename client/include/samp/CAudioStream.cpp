/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CAudioStream.h"

#if defined(SAMP_R1)

int& SAMP::CAudioStream::hStream() { return *(int*)SAMP_ADDROF(0x11A60C); }
bool& SAMP::CAudioStream::bNeedsToDestroy() { return *(bool*)SAMP_ADDROF(0xF03AA); }
SAMP::CVectorSA& SAMP::CAudioStream::position() { return *(SAMP::CVectorSA*)SAMP_ADDROF(0x11A614); }
bool& SAMP::CAudioStream::bIsPlaying() { return *(bool*)SAMP_ADDROF(0x11A610); }
char* SAMP::CAudioStream::szUrl() { return (char*)SAMP_ADDROF(0x11A2F8); }
bool& SAMP::CAudioStream::bIs3d() { return *(bool*)SAMP_ADDROF(0x11A620); }
SAMP::CAudioStream*& SAMP::pAudioStream() { return *(SAMP::CAudioStream**)SAMP_ADDROF(0x21A0F0); }
char* SAMP::CAudioStream::szInfo() { return (char*)SAMP_ADDROF(0x11A400); }
char* SAMP::CAudioStream::szIcyName() { return (char*)SAMP_ADDROF(0x11A1F0); }
char* SAMP::CAudioStream::szIcyUrl() { return (char*)SAMP_ADDROF(0x11A508); }
float& SAMP::CAudioStream::fRadius() { return *(float*)SAMP_ADDROF(0xF03AC); }

BOOL SAMP::CAudioStream::Reset() {
	return ((BOOL(__thiscall*)(CAudioStream*))SAMP_ADDROF(0x628C0))(this);
}

BOOL SAMP::CAudioStream::Stop(bool bWait) {
	return ((BOOL(__thiscall*)(CAudioStream*, bool))SAMP_ADDROF(0x629A0))(this, bWait);
}

void SAMP::CAudioStream::ConstructInfo() {
	((void(__cdecl*)())SAMP_ADDROF(0x62A00))();
}

void SAMP::CAudioStream::SyncProc(int handle, int channel, int data, void* user) {
	((void(__stdcall*)(int, int, int, void*))SAMP_ADDROF(0x62B30))(handle, channel, data, user);
}

void SAMP::CAudioStream::Process(void* arglist) {
	((void(__cdecl*)(void*))SAMP_ADDROF(0x62B40))(arglist);
}

BOOL SAMP::CAudioStream::Play(const char* szUrl, CVectorSA position, float fRadius, bool bIs3d) {
	return ((BOOL(__thiscall*)(CAudioStream*, const char*, CVectorSA, float, bool))SAMP_ADDROF(0x62DA0))(this, szUrl, position, fRadius, bIs3d);
}

void SAMP::CAudioStream::ControlGameRadio() {
	((void(__thiscall*)(CAudioStream*))SAMP_ADDROF(0x62EC0))(this);
}

void SAMP::CAudioStream::DrawInfo() {
	((void(__thiscall*)(CAudioStream*))SAMP_ADDROF(0x62EF0))(this);
}

#elif defined(SAMP_R3)

int& SAMP::CAudioStream::hStream() { return *(int*)SAMP_ADDROF(0x12E68C); }
bool& SAMP::CAudioStream::bIsPlaying() { return *(bool*)SAMP_ADDROF(0x12E690); }
SAMP::CVectorSA& SAMP::CAudioStream::position() { return *(SAMP::CVectorSA*)SAMP_ADDROF(0x12E694); }
bool& SAMP::CAudioStream::bIs3d() { return *(bool*)SAMP_ADDROF(0x12E6A0); }
char* SAMP::CAudioStream::szIcyUrl() { return (char*)SAMP_ADDROF(0x12E588); }
char* SAMP::CAudioStream::szInfo() { return (char*)SAMP_ADDROF(0x12E480); }
char* SAMP::CAudioStream::szUrl() { return (char*)SAMP_ADDROF(0x12E378); }
bool& SAMP::CAudioStream::bNeedsToDestroy() { return *(bool*)SAMP_ADDROF(0x1027BA); }
float& SAMP::CAudioStream::fRadius() { return *(float*)SAMP_ADDROF(0x1027BC); }
char* SAMP::CAudioStream::szIcyName() { return (char*)SAMP_ADDROF(0x12E270); }
SAMP::CAudioStream*& SAMP::pAudioStream() { return *(SAMP::CAudioStream**)SAMP_ADDROF(0x26E8D4); }

BOOL SAMP::CAudioStream::Reset() {
	return ((BOOL(__thiscall*)(CAudioStream*))SAMP_ADDROF(0x65D10))(this);
}

BOOL SAMP::CAudioStream::Stop(bool bWait) {
	return ((BOOL(__thiscall*)(CAudioStream*, bool))SAMP_ADDROF(0x65DF0))(this, bWait);
}

BOOL SAMP::CAudioStream::Play(const char* szUrl, CVectorSA position, float fRadius, bool bIs3d) {
	return ((BOOL(__thiscall*)(CAudioStream*, const char*, CVectorSA, float, bool))SAMP_ADDROF(0x661F0))(this, szUrl, position, fRadius, bIs3d);
}

void SAMP::CAudioStream::ControlGameRadio() {
	((void(__thiscall*)(CAudioStream*))SAMP_ADDROF(0x66310))(this);
}

void SAMP::CAudioStream::DrawInfo() {
	((void(__thiscall*)(CAudioStream*))SAMP_ADDROF(0x66340))(this);
}

void SAMP::CAudioStream::ConstructInfo() {
	((void(__cdecl*)())SAMP_ADDROF(0x65E50))();
}

void SAMP::CAudioStream::SyncProc(int handle, int channel, int data, void* user) {
	((void(__stdcall*)(int, int, int, void*))SAMP_ADDROF(0x65F80))(handle, channel, data, user);
}

void SAMP::CAudioStream::Process(void* arglist) {
	((void(__cdecl*)(void*))SAMP_ADDROF(0x65F90))(arglist);
}

#endif
