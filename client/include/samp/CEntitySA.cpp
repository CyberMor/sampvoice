/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CEntitySA.h"

#if defined(SAMP_R1)

void SAMP::CEntitySA::GetMatrix(CMatrixSA* pMatrix) {
	((void(__thiscall*)(CEntitySA*, CMatrixSA*))SAMP_ADDROF(0x9A150))(this, pMatrix);
}

void SAMP::CEntitySA::SetMatrix(CMatrixSA matrix) {
	((void(__thiscall*)(CEntitySA*, CMatrixSA))SAMP_ADDROF(0x9A200))(this, matrix);
}

void SAMP::CEntitySA::UpdateRwFrame() {
	((void(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9A2C0))(this);
}

void SAMP::CEntitySA::GetSpeed(CVectorSA* pVec) {
	((void(__thiscall*)(CEntitySA*, CVectorSA*))SAMP_ADDROF(0x9A320))(this, pVec);
}

void SAMP::CEntitySA::SetSpeed(CVectorSA vec) {
	((void(__thiscall*)(CEntitySA*, CVectorSA))SAMP_ADDROF(0x9A350))(this, vec);
}

void SAMP::CEntitySA::GetTurnSpeed(CVectorSA* pSpeed) {
	((void(__thiscall*)(CEntitySA*, CVectorSA*))SAMP_ADDROF(0x9A470))(this, pSpeed);
}

void SAMP::CEntitySA::SetTurnSpeed(CVectorSA speed) {
	((void(__thiscall*)(CEntitySA*, CVectorSA))SAMP_ADDROF(0x9A4A0))(this, speed);
}

void SAMP::CEntitySA::ApplyTurnSpeed() {
	((void(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9A4D0))(this);
}

float SAMP::CEntitySA::GetDistanceFromCentreOfMassToBaseOfModel() {
	return ((float(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9A4F0))(this);
}

void SAMP::CEntitySA::GetBoundCentre(CVectorSA* pCentre) {
	((void(__thiscall*)(CEntitySA*, CVectorSA*))SAMP_ADDROF(0x9A530))(this, pCentre);
}

void SAMP::CEntitySA::SetModelIndex(int nModel) {
	((void(__thiscall*)(CEntitySA*, int))SAMP_ADDROF(0x9A590))(this, nModel);
}

int SAMP::CEntitySA::GetModelIndex() {
	return ((int(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9A670))(this);
}

void SAMP::CEntitySA::Teleport(CVectorSA position) {
	((void(__thiscall*)(CEntitySA*, CVectorSA))SAMP_ADDROF(0x9A680))(this, position);
}

float SAMP::CEntitySA::GetDistanceToLocalPlayer() {
	return ((float(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9A700))(this);
}

float SAMP::CEntitySA::GetDistanceToCamera() {
	return ((float(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9A7D0))(this);
}

float SAMP::CEntitySA::GetDistanceToLocalPlayerNoHeight() {
	return ((float(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9A830))(this);
}

float SAMP::CEntitySA::GetDistanceToPoint(CVectorSA position) {
	return ((float(__thiscall*)(CEntitySA*, CVectorSA))SAMP_ADDROF(0x9A8F0))(this, position);
}

BOOL SAMP::CEntitySA::DoesExist() {
	return ((int(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9AA10))(this);
}

int SAMP::CEntitySA::EnforceWorldBoundries(float fPX, float fZX, float fPY, float fNY) {
	return ((int(__thiscall*)(CEntitySA*, float, float, float, float))SAMP_ADDROF(0x9AA60))(this, fPX, fZX, fPY, fNY);
}

int SAMP::CEntitySA::HasExceededWorldBoundries(float fPX, float fZX, float fPY, float fNY) {
	return ((int(__thiscall*)(CEntitySA*, float, float, float, float))SAMP_ADDROF(0x9AC00))(this, fPX, fZX, fPY, fNY);
}

void SAMP::CEntitySA::SetClumpAlpha(int nValue) {
	((void(__thiscall*)(CEntitySA*, int))SAMP_ADDROF(0x9ADD0))(this, nValue);
}

void SAMP::CEntitySA::SetFromEuler(CVectorSA angles) {
	((void(__thiscall*)(CEntitySA*, CVectorSA))SAMP_ADDROF(0x9AE30))(this, angles);
}

void SAMP::CEntitySA::GetEulerInverted(float* pX, float* pY, float* pZ) {
	((void(__thiscall*)(CEntitySA*, float*, float*, float*))SAMP_ADDROF(0x9AF30))(this, pX, pY, pZ);
}

void SAMP::CEntitySA::ApplyTurnForce(CVectorSA direction, CVectorSA velocity) {
	((void(__thiscall*)(CEntitySA*, CVectorSA, CVectorSA))SAMP_ADDROF(0x9B010))(this, direction, velocity);
}

SAMP::CEntitySA::CEntitySA() {
	((void(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x97C10))(this);
}

BOOL SAMP::CEntitySA::IsStationary() {
	return ((BOOL(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9B420))(this);
}

RwObject* SAMP::CEntitySA::GetRwObject() {
	return ((RwObject * (__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9B0A0))(this);
}

BOOL SAMP::CEntitySA::IsIgnored() {
	return ((BOOL(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9B320))(this);
}

BOOL SAMP::CEntitySA::GetCollisionFlag() {
	return ((BOOL(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9ACA0))(this);
}

void SAMP::CEntitySA::SetCollisionFlag(BOOL bEnable) {
	((void(__thiscall*)(CEntitySA*, BOOL))SAMP_ADDROF(0x9AC70))(this, bEnable);
}

void SAMP::CEntitySA::SetCollisionProcessed(BOOL bProcessed) {
	((void(__thiscall*)(CEntitySA*, BOOL))SAMP_ADDROF(0x9ACC0))(this, bProcessed);
}

void SAMP::CEntitySA::DeleteRwObject() {
	((void(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9B1F0))(this);
}

#elif defined(SAMP_R3)

SAMP::CEntitySA::CEntitySA() {
	((void(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9BB50))(this);
}

void SAMP::CEntitySA::GetMatrix(CMatrixSA* pMatrix) {
	((void(__thiscall*)(CEntitySA*, CMatrixSA*))SAMP_ADDROF(0x9E400))(this, pMatrix);
}

void SAMP::CEntitySA::SetMatrix(CMatrixSA matrix) {
	((void(__thiscall*)(CEntitySA*, CMatrixSA))SAMP_ADDROF(0x9E4B0))(this, matrix);
}

void SAMP::CEntitySA::GetSpeed(CVectorSA* pVec) {
	((void(__thiscall*)(CEntitySA*, CVectorSA*))SAMP_ADDROF(0x9E5D0))(this, pVec);
}

void SAMP::CEntitySA::SetSpeed(CVectorSA vec) {
	((void(__thiscall*)(CEntitySA*, CVectorSA))SAMP_ADDROF(0x9E600))(this, vec);
}

void SAMP::CEntitySA::GetTurnSpeed(CVectorSA* pVec) {
	((void(__thiscall*)(CEntitySA*, CVectorSA*))SAMP_ADDROF(0x9E720))(this, pVec);
}

void SAMP::CEntitySA::SetTurnSpeed(CVectorSA vec) {
	((void(__thiscall*)(CEntitySA*, CVectorSA))SAMP_ADDROF(0x9E750))(this, vec);
}

void SAMP::CEntitySA::ApplyTurnSpeed() {
	((void(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9E780))(this);
}

float SAMP::CEntitySA::GetDistanceFromCentreOfMassToBaseOfModel() {
	return ((float(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9E7A0))(this);
}

void SAMP::CEntitySA::GetBoundCentre(CVectorSA* pVec) {
	((void(__thiscall*)(CEntitySA*, CVectorSA*))SAMP_ADDROF(0x9E7E0))(this, pVec);
}

void SAMP::CEntitySA::SetModelIndex(int nModel) {
	((void(__thiscall*)(CEntitySA*, int))SAMP_ADDROF(0x9E840))(this, nModel);
}

int SAMP::CEntitySA::GetModelIndex() {
	return ((int(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9E920))(this);
}

void SAMP::CEntitySA::Teleport(CVectorSA position) {
	((void(__thiscall*)(CEntitySA*, CVectorSA))SAMP_ADDROF(0x9E930))(this, position);
}

float SAMP::CEntitySA::GetDistanceToLocalPlayer() {
	return ((float(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9E9B0))(this);
}

float SAMP::CEntitySA::GetDistanceToCamera() {
	return ((float(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9EA80))(this);
}

float SAMP::CEntitySA::GetDistanceToPoint(CVectorSA position) {
	return ((float(__thiscall*)(CEntitySA*, CVectorSA))SAMP_ADDROF(0x9EBA0))(this, position);
}

BOOL SAMP::CEntitySA::DoesExist() {
	return ((BOOL(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9ECC0))(this);
}

BOOL SAMP::CEntitySA::EnforceWorldBoundries(float fPX, float fZX, float fPY, float fNY) {
	return ((BOOL(__thiscall*)(CEntitySA*, float, float, float, float))SAMP_ADDROF(0x9ED10))(this, fPX, fZX, fPY, fNY);
}

BOOL SAMP::CEntitySA::HasExceededWorldBoundries(float fPX, float fZX, float fPY, float fNY) {
	return ((BOOL(__thiscall*)(CEntitySA*, float, float, float, float))SAMP_ADDROF(0x9EEB0))(this, fPX, fZX, fPY, fNY);
}

void SAMP::CEntitySA::GetEulerInverted(float* x, float* y, float* z) {
	((void(__thiscall*)(CEntitySA*, float*, float*, float*))SAMP_ADDROF(0x9F1E0))(this, x, y, z);
}

BOOL SAMP::CEntitySA::IsIgnored() {
	return ((BOOL(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9F5D0))(this);
}

BOOL SAMP::CEntitySA::IsStationary() {
	return ((BOOL(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9F6D0))(this);
}

BOOL SAMP::CEntitySA::GetCollisionFlag() {
	return ((BOOL(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9EF50))(this);
}

void SAMP::CEntitySA::SetCollisionFlag(BOOL bEnable) {
	((void(__thiscall*)(CEntitySA*, BOOL))SAMP_ADDROF(0x9EF20))(this, bEnable);
}

RwObject* SAMP::CEntitySA::GetRwObject() {
	return ((::RwObject * (__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9F350))(this);
}

void SAMP::CEntitySA::DeleteRwObject() {
	((void(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9F4A0))(this);
}

void SAMP::CEntitySA::UpdateRwFrame() {
	((void(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9E570))(this);
}

float SAMP::CEntitySA::GetDistanceToLocalPlayerNoHeight() {
	return ((float(__thiscall*)(CEntitySA*))SAMP_ADDROF(0x9EAE0))(this);
}

void SAMP::CEntitySA::SetCollisionProcessed(BOOL bProcessed) {
	((void(__thiscall*)(CEntitySA*, BOOL))SAMP_ADDROF(0x9EF70))(this, bProcessed);
}

#endif
