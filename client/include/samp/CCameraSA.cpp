/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CCameraSA.h"

#if defined(SAMP_R1)

SAMP::CCameraSA::CCameraSA() {
	((void(__thiscall*)(CCameraSA*))SAMP_ADDROF(0x9B5A0))(this);
}

void SAMP::CCameraSA::Detach() {
	((void(__thiscall*)(CCameraSA*))SAMP_ADDROF(0x990E0))(this);
}

void SAMP::CCameraSA::Set(CVectorSA position, CVectorSA rotation) {
	((void(__thiscall*)(CCameraSA*, CVectorSA, CVectorSA))SAMP_ADDROF(0x99120))(this, position, rotation);
}

void SAMP::CCameraSA::PointAt(CVectorSA position, int nSwitchStyle) {
	((void(__thiscall*)(CCameraSA*, CVectorSA, int))SAMP_ADDROF(0x99180))(this, position, nSwitchStyle);
}

void SAMP::CCameraSA::Restore() {
	((void(__thiscall*)(CCameraSA*))SAMP_ADDROF(0x991D0))(this);
}

void SAMP::CCameraSA::SetTrackVector(CVectorSA* pPointAt, CVectorSA* pTransverseTo, int nTime, bool bSmooth) {
	((void(__thiscall*)(CCameraSA*, CVectorSA*, CVectorSA*, unsigned int, bool))SAMP_ADDROF(0x98FA0))(this, pPointAt, pTransverseTo, nTime, bSmooth);
}

void SAMP::CCameraSA::SetMoveVector(CVectorSA* pCamera, CVectorSA* pPosition, int nTime, bool bSmoothTransmition) {
	((void(__thiscall*)(CCameraSA*, CVectorSA*, CVectorSA*, unsigned int, bool))SAMP_ADDROF(0x98F30))(this, pCamera, pPosition, nTime, bSmoothTransmition);
}

void SAMP::CCameraSA::SetToOwner() {
	((void(__thiscall*)(CCameraSA*))SAMP_ADDROF(0x99050))(this);
}

void SAMP::CCameraSA::Attach(CEntitySA* pEntity) {
	((void(__thiscall*)(CCameraSA*, CEntitySA*))SAMP_ADDROF(0x99000))(this, pEntity);
}

void SAMP::CCameraSA::GetMatrix(CMatrixSA* pMatrix) {
	((void(__thiscall*)(CCameraSA*, CMatrixSA*))SAMP_ADDROF(0x98E00))(this, pMatrix);
}

float SAMP::CCameraSA::GetDistanceToPoint(CVectorSA* pPosition) {
	return ((float(__thiscall*)(CCameraSA*, CVectorSA*))SAMP_ADDROF(0x990A0))(this, pPosition);
}

void SAMP::CCameraSA::Fade(BOOL bIn) {
	((void(__thiscall*)(CCameraSA*, bool))SAMP_ADDROF(0x98DE0))(this, bIn);
}

void SAMP::CCameraSA::SetMatrix(CMatrixSA matrix) {
	((void(__thiscall*)(CCameraSA*, CMatrixSA))SAMP_ADDROF(0x98E80))(this, matrix);
}

void SAMP::CCameraSA::TakeControl(::CEntity* pTarget, short nModeToGoTo, short nTypeOfSwitch) {
	((void(__thiscall*)(CCameraSA*, ::CEntity*, short, short))SAMP_ADDROF(0x98F10))(this, pTarget, nModeToGoTo, nTypeOfSwitch);
}

#elif defined(SAMP_R3)

SAMP::CCameraSA::CCameraSA() {
	((void(__thiscall*)(CCameraSA*))SAMP_ADDROF(0x9F850))(this);
}

SAMP::CCameraSA::~CCameraSA() {
	((void(__thiscall*)(CCameraSA*))SAMP_ADDROF(0x9F860))(this);
}

void SAMP::CCameraSA::Fade(BOOL bIn) {
	((void(__thiscall*)(CCameraSA*, BOOL))SAMP_ADDROF(0x9CD30))(this, bIn);
}

void SAMP::CCameraSA::GetMatrix(CMatrixSA* pMatrix) {
	((void(__thiscall*)(CCameraSA*, CMatrixSA*))SAMP_ADDROF(0x9CD50))(this, pMatrix);
}

void SAMP::CCameraSA::SetMatrix(CMatrixSA matrix) {
	((void(__thiscall*)(CCameraSA*, CMatrixSA))SAMP_ADDROF(0x9CDD0))(this, matrix);
}

void SAMP::CCameraSA::TakeControl(::CEntity* pTarget, short nModeToGoTo, short nTypeOfSwitch) {
	((void(__thiscall*)(CCameraSA*, ::CEntity*, short, short))SAMP_ADDROF(0x9CE60))(this, pTarget, nModeToGoTo, nTypeOfSwitch);
}

void SAMP::CCameraSA::SetMoveVector(CVectorSA* pCamera, CVectorSA* pPosition, int nTime, bool bSmoothTransmition) {
	((void(__thiscall*)(CCameraSA*, CVectorSA*, CVectorSA*, int, bool))SAMP_ADDROF(0x9CE80))(this, pCamera, pPosition, nTime, bSmoothTransmition);
}

void SAMP::CCameraSA::SetTrackVector(CVectorSA* pPointAt, CVectorSA* pTransverseTo, int nTime, bool bSmooth) {
	((void(__thiscall*)(CCameraSA*, CVectorSA*, CVectorSA*, int, bool))SAMP_ADDROF(0x9CEF0))(this, pPointAt, pTransverseTo, nTime, bSmooth);
}

void SAMP::CCameraSA::Attach(CEntitySA* pOwner) {
	((void(__thiscall*)(CCameraSA*, CEntitySA*))SAMP_ADDROF(0x9CF50))(this, pOwner);
}

void SAMP::CCameraSA::SetToOwner() {
	((void(__thiscall*)(CCameraSA*))SAMP_ADDROF(0x9CFA0))(this);
}

float SAMP::CCameraSA::GetDistanceToPoint(CVectorSA* pPoint) {
	return ((float(__thiscall*)(CCameraSA*, CVectorSA*))SAMP_ADDROF(0x9CFF0))(this, pPoint);
}

void SAMP::CCameraSA::Restore() {
	((void(__thiscall*)(CCameraSA*))SAMP_ADDROF(0x9D030))(this);
}

void SAMP::CCameraSA::Set(CVectorSA position, CVectorSA rotation) {
	((void(__thiscall*)(CCameraSA*, CVectorSA, CVectorSA))SAMP_ADDROF(0x9D070))(this, position, rotation);
}

void SAMP::CCameraSA::PointAt(CVectorSA point, int nSwitchStyle) {
	((void(__thiscall*)(CCameraSA*, CVectorSA, int))SAMP_ADDROF(0x9D0D0))(this, point, nSwitchStyle);
}

void SAMP::CCameraSA::Detach() {
	((void(__thiscall*)(CCameraSA*))SAMP_ADDROF(0x9D120))(this);
}

#endif
