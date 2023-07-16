/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "AimStuff.h"

#if defined(SAMP_R1)

float& SAMP::AimStuff::fLocalPlayerAspectRatio() {
	return *(float*)SAMP_ADDROF(0x132758);
}

float*& SAMP::AimStuff::pInternalCameraExtZoom() {
	return *(float**)SAMP_ADDROF(0xF159C);
}

float*& SAMP::AimStuff::pInternalAspectRatio() {
	return *(float**)SAMP_ADDROF(0xF1598);
}

float* SAMP::AimStuff::aCameraExtZoom() {
	return (float*)SAMP_ADDROF(0x12FC80);
}

float* SAMP::AimStuff::aAspectRatio() {
	return (float*)SAMP_ADDROF(0x132788);
}

char* SAMP::AimStuff::aCameraMode() {
	return (char*)SAMP_ADDROF(0x12FBA8);
}

char*& SAMP::AimStuff::pInternalCameraMode() {
	return *(char**)SAMP_ADDROF(0x10153C);
}

SAMP::AimStuff::Aim& SAMP::AimStuff::localPlayerAim() {
	return *(SAMP::AimStuff::Aim*)SAMP_ADDROF(0x12FFC8);
}

SAMP::AimStuff::Aim* SAMP::AimStuff::aPlayerAim() {
	return (SAMP::AimStuff::Aim*)SAMP_ADDROF(0x12FFF8);
}

SAMP::AimStuff::Aim*& SAMP::AimStuff::pInternalAim() {
	return *(SAMP::AimStuff::Aim**)SAMP_ADDROF(0xF1590);
}

float SAMP::AimStuff::GetCameraExtZoom() {
	return ((float(__stdcall*)())SAMP_ADDROF(0x981D0))();
}

void SAMP::AimStuff::ApplyCameraExtZoomAndAspectRatio(NUMBER nPlayer) {
	((void(__stdcall*)(NUMBER))SAMP_ADDROF(0x981F0))(nPlayer);
}

void SAMP::AimStuff::SetCameraMode(char nMode, NUMBER nPlayer) {
	((void(__stdcall*)(char, NUMBER))SAMP_ADDROF(0x98230))(nMode, nPlayer);
}

char SAMP::AimStuff::GetCameraMode(NUMBER nPlayer) {
	return ((char(__stdcall*)(NUMBER))SAMP_ADDROF(0x98250))(nPlayer);
}

char SAMP::AimStuff::GetCameraMode() {
	return ((char(__stdcall*)())SAMP_ADDROF(0x98260))();
}

void SAMP::AimStuff::Initialize() {
	((void(__stdcall*)())SAMP_ADDROF(0x98270))();
}

void SAMP::AimStuff::UpdateAim() {
	((void(__stdcall*)())SAMP_ADDROF(0x982E0))();
}

void SAMP::AimStuff::ApplyAim() {
	((void(__stdcall*)())SAMP_ADDROF(0x98300))();
}

SAMP::AimStuff::Aim* SAMP::AimStuff::GetAim() {
	return ((Aim * (__stdcall*)())SAMP_ADDROF(0x98320))();
}

void SAMP::AimStuff::SetAim(int nPlayer, const Aim* pAim) {
	((void(__stdcall*)(int, const Aim*))SAMP_ADDROF(0x98330))(nPlayer, pAim);
}

void SAMP::AimStuff::ApplyAim(int nPlayer) {
	((void(__stdcall*)(int))SAMP_ADDROF(0x98360))(nPlayer);
}

SAMP::AimStuff::Aim* SAMP::AimStuff::GetAim(int nPlayer) {
	return ((Aim * (__stdcall*)(int))SAMP_ADDROF(0x98390))(nPlayer);
}

void SAMP::AimStuff::UpdateCameraExtZoomAndAspectRatio() {
	((void(__stdcall*)())SAMP_ADDROF(0x98160))();
}

void SAMP::AimStuff::ApplyCameraExtZoomAndAspectRatio() {
	((void(__stdcall*)())SAMP_ADDROF(0x98180))();
}

void SAMP::AimStuff::SetCameraExtZoomAndAspectRatio(NUMBER nPlayer, float fCameraExtZoom, float fAspectRatio) {
	((void(__stdcall*)(NUMBER, float, float))SAMP_ADDROF(0x981A0))(nPlayer, fCameraExtZoom, fAspectRatio);
}

float SAMP::AimStuff::GetAspectRatio() {
	return ((float(__stdcall*)())SAMP_ADDROF(0x981C0))();
}

#elif defined(SAMP_R3)

float& SAMP::AimStuff::fLocalPlayerCameraExtZoom() { return *(float*)SAMP_ADDROF(0x143D20); }
float& SAMP::AimStuff::fLocalPlayerAspectRatio() { return *(float*)SAMP_ADDROF(0x1468D8); }
float*& SAMP::AimStuff::pInternalCameraExtZoom() { return *(float**)SAMP_ADDROF(0x1039BC); }
float*& SAMP::AimStuff::pInternalAspectRatio() { return *(float**)SAMP_ADDROF(0x1039B8); }
float* SAMP::AimStuff::aCameraExtZoom() { return (float*)SAMP_ADDROF(0x143E00); }
float* SAMP::AimStuff::aAspectRatio() { return (float*)SAMP_ADDROF(0x146908); }
char* SAMP::AimStuff::aCameraMode() { return (char*)SAMP_ADDROF(0x143D28); }
char*& SAMP::AimStuff::pInternalCameraMode() { return *(char**)SAMP_ADDROF(0x11395C); }
SAMP::AimStuff::Aim& SAMP::AimStuff::localPlayerAim() { return *(SAMP::AimStuff::Aim*)SAMP_ADDROF(0x144148); }
SAMP::AimStuff::Aim* SAMP::AimStuff::aPlayerAim() { return (SAMP::AimStuff::Aim*)SAMP_ADDROF(0x144178); }
SAMP::AimStuff::Aim*& SAMP::AimStuff::pInternalAim() { return *(SAMP::AimStuff::Aim**)SAMP_ADDROF(0x1039B0); }

void SAMP::AimStuff::UpdateCameraExtZoomAndAspectRatio() {
	((void(__stdcall*)())SAMP_ADDROF(0x9C0B0))();
}

void SAMP::AimStuff::ApplyCameraExtZoomAndAspectRatio() {
	((void(__stdcall*)())SAMP_ADDROF(0x9C0D0))();
}

void SAMP::AimStuff::SetCameraExtZoomAndAspectRatio(NUMBER nPlayer, float fCameraExtZoom, float fAspectRatio) {
	((void(__stdcall*)(NUMBER, float, float))SAMP_ADDROF(0x9C0F0))(nPlayer, fCameraExtZoom, fAspectRatio);
}

float SAMP::AimStuff::GetAspectRatio() {
	return ((float(__stdcall*)())SAMP_ADDROF(0x9C110))();
}

float SAMP::AimStuff::GetCameraExtZoom() {
	return ((float(__stdcall*)())SAMP_ADDROF(0x9C120))();
}

void SAMP::AimStuff::ApplyCameraExtZoomAndAspectRatio(NUMBER nPlayer) {
	((void(__stdcall*)(NUMBER))SAMP_ADDROF(0x9C140))(nPlayer);
}

void SAMP::AimStuff::SetCameraMode(char nMode, NUMBER nPlayer) {
	((void(__stdcall*)(char, NUMBER))SAMP_ADDROF(0x9C180))(nMode, nPlayer);
}

char SAMP::AimStuff::GetCameraMode(NUMBER nPlayer) {
	return ((char(__stdcall*)(NUMBER))SAMP_ADDROF(0x9C1A0))(nPlayer);
}

char SAMP::AimStuff::GetCameraMode() {
	return ((char(__stdcall*)())SAMP_ADDROF(0x9C1B0))();
}

void SAMP::AimStuff::Initialize() {
	((void(__stdcall*)())SAMP_ADDROF(0x9C1C0))();
}

void SAMP::AimStuff::UpdateAim() {
	((void(__stdcall*)())SAMP_ADDROF(0x9C230))();
}

void SAMP::AimStuff::ApplyAim() {
	((void(__stdcall*)())SAMP_ADDROF(0x9C250))();
}

SAMP::AimStuff::Aim* SAMP::AimStuff::GetAim() {
	return ((Aim * (__stdcall*)())SAMP_ADDROF(0x9C270))();
}

void SAMP::AimStuff::SetAim(int nPlayer, const Aim * pAim) {
	((void(__stdcall*)(int, const Aim*))SAMP_ADDROF(0x9C280))(nPlayer, pAim);
}

void SAMP::AimStuff::ApplyAim(int nPlayer) {
	((void(__stdcall*)(int))SAMP_ADDROF(0x9C2B0))(nPlayer);
}

SAMP::AimStuff::Aim* SAMP::AimStuff::GetAim(int nPlayer) {
	return ((Aim * (__stdcall*)(int))SAMP_ADDROF(0x9C2E0))(nPlayer);
}

#endif
