/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CActor.h"

#if defined(SAMP_R1)

SAMP::CActor::CActor(int nSkin, CVectorSA vPos, float fRotation) {
	((void(__thiscall*)(CActor*, int, CVectorSA, float))SAMP_ADDROF(0x97C60))(this, nSkin, vPos, fRotation);
}

void SAMP::CActor::Destroy() {
	((void(__thiscall*)(CActor*))SAMP_ADDROF(0x97DA0))(this);
}

void SAMP::CActor::PerformAnimation(const char* szAnim, const char* szIFP, float fFramedelta, int bLockA, int bLockX, int bLockY, int bLockF, int nTime) {
	((void(__thiscall*)(CActor*, const char*, const char*, float, int, int, int, int, int))SAMP_ADDROF(0x97E00))(this, szAnim, szIFP, fFramedelta, bLockA, bLockX, bLockY, bLockF, nTime);
}

void SAMP::CActor::SetRotation(float fValue) {
	((void(__thiscall*)(CActor*, float))SAMP_ADDROF(0x97F10))(this, fValue);
}

void SAMP::CActor::SetHealth(float fValue) {
	((void(__thiscall*)(CActor*, float))SAMP_ADDROF(0x97F70))(this, fValue);
}

float SAMP::CActor::GetHealth() {
	return ((float(__thiscall*)(CActor*))SAMP_ADDROF(0x97F50))(this);
}

void SAMP::CActor::SetInvulnerable(bool bInv) {
	((void(__thiscall*)(CActor*, bool))SAMP_ADDROF(0x980A0))(this, bInv);
}

void SAMP::CActor::SetArmour(float fValue) {
	((void(__thiscall*)(CActor*, float))SAMP_ADDROF(0x97FD0))(this, fValue);
}

float SAMP::CActor::GetArmour() {
	return ((float(__thiscall*)(CActor*))SAMP_ADDROF(0x97FB0))(this);
}

void SAMP::CActor::SetState(int nValue) {
	((void(__thiscall*)(CActor*, int))SAMP_ADDROF(0x98000))(this, nValue);
}

int SAMP::CActor::GetState() {
	return ((int(__thiscall*)(CActor*))SAMP_ADDROF(0x97FF0))(this);
}

BOOL SAMP::CActor::IsDead() {
	return ((BOOL(__thiscall*)(CActor*))SAMP_ADDROF(0x98020))(this);
}

void SAMP::CActor::SetStatus(int nValue) {
	((void(__thiscall*)(CActor*, int))SAMP_ADDROF(0x98060))(this, nValue);
}

int SAMP::CActor::GetStatus() {
	return ((int(__thiscall*)(CActor*))SAMP_ADDROF(0x98050))(this);
}

#elif defined(SAMP_R3)

SAMP::CActor::CActor(int nModel, CVectorSA position, float fRotation) {
	((void(__thiscall*)(CActor*, int, CVectorSA, float))SAMP_ADDROF(0x9BBA0))(this, nModel, position, fRotation);
}

void SAMP::CActor::Destroy() {
	((void(__thiscall*)(CActor*))SAMP_ADDROF(0x9BCF0))(this);
}

void SAMP::CActor::PerformAnimation(const char* szAnim, const char* szIFP, float fFramedelta, int bLockA, int bLockX, int bLockY, int bLockF, int nTime) {
	((void(__thiscall*)(CActor*, const char*, const char*, float, int, int, int, int, int))SAMP_ADDROF(0x9BD50))(this, szAnim, szIFP, fFramedelta, bLockA, bLockX, bLockY, bLockF, nTime);
}

void SAMP::CActor::SetRotation(float fAngle) {
	((void(__thiscall*)(CActor*, float))SAMP_ADDROF(0x9BE60))(this, fAngle);
}

float SAMP::CActor::GetHealth() {
	return ((float(__thiscall*)(CActor*))SAMP_ADDROF(0x9BEA0))(this);
}

void SAMP::CActor::SetHealth(float fValue) {
	((void(__thiscall*)(CActor*, float))SAMP_ADDROF(0x9BEC0))(this, fValue);
}

void SAMP::CActor::SetInvulnerable(bool bEnable) {
	((void(__thiscall*)(CActor*, bool))SAMP_ADDROF(0x9BFF0))(this, bEnable);
}

#endif
