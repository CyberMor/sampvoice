/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CObjectSA.h"

#if defined(SAMP_R1)

SAMP::CObjectSA::CObjectSA(int nModel, CVectorSA position, CVectorSA rotation, float fDrawDistance, int arg5) {
	((void(__thiscall*)(CObjectSA*, int, CVectorSA, CVectorSA, float, int))SAMP_ADDROF(0xA3AB0))(this, nModel, position, rotation, fDrawDistance, arg5);
}

float SAMP::CObjectSA::GetDistance(const CMatrixSA* pMatrix) {
	return ((float(__thiscall*)(CObjectSA*, const CMatrixSA*))SAMP_ADDROF(0xA2960))(this, pMatrix);
}

void SAMP::CObjectSA::Stop() {
	((void(__thiscall*)(CObjectSA*))SAMP_ADDROF(0xA29D0))(this);
}

void SAMP::CObjectSA::SetRotation(const CVectorSA* pRotation) {
	((void(__thiscall*)(CObjectSA*, const CVectorSA*))SAMP_ADDROF(0xA2A40))(this, pRotation);
}

void SAMP::CObjectSA::AttachToVehicle(CVehicleSA* pVehicle) {
	((void(__thiscall*)(CObjectSA*, CVehicleSA * pVehicle))SAMP_ADDROF(0xA2BE0))(this, pVehicle);
}

void SAMP::CObjectSA::AttachToObject(CObjectSA* pObject) {
	((void(__thiscall*)(CObjectSA*, CObjectSA*))SAMP_ADDROF(0xA2C60))(this, pObject);
}

void SAMP::CObjectSA::Rotate(CVectorSA vRotation) {
	((void(__thiscall*)(CObjectSA*, CVectorSA))SAMP_ADDROF(0xA2D60))(this, vRotation);
}

void SAMP::CObjectSA::Process(float fTimeElapsed) {
	((void(__thiscall*)(CObjectSA*, float))SAMP_ADDROF(0xA3F70))(this, fTimeElapsed);
}

void SAMP::CObjectSA::SetAttachedToVehicle(ID nId, const CVectorSA* pOffset, const CVectorSA* pRotation) {
	((void(__thiscall*)(CObjectSA*, ID, const CVectorSA*, const CVectorSA*))SAMP_ADDROF(0xA2AB0))(this, nId, pOffset, pRotation);
}

void SAMP::CObjectSA::SetAttachedToObject(ID nId, const CVectorSA* pOffset, const CVectorSA* pRotation, char a5) {
	((void(__thiscall*)(CObjectSA*, ID, const CVectorSA*, const CVectorSA*, char))SAMP_ADDROF(0xA2B40))(this, nId, pOffset, pRotation, a5);
}

BOOL SAMP::CObjectSA::AttachedToMovingEntity() {
	return ((BOOL(__thiscall*)(CObjectSA*))SAMP_ADDROF(0xA2E60))(this);
}

void SAMP::CObjectSA::SetMaterial(int nModel, int nIndex, const char* szTxd, const char* szTexture, D3DCOLOR color) {
	((void(__thiscall*)(CObjectSA*, int, int, const char*, const char*, D3DCOLOR))SAMP_ADDROF(0xA2ED0))(this, nModel, nIndex, szTxd, szTexture, color);
}

void SAMP::CObjectSA::SetMaterialText(int nIndex, const char* szText, char nMaterialSize, const char* szFont, char nFontSize, bool bBold, D3DCOLOR fontColor, D3DCOLOR backgroundColor, char align) {
	((void(__thiscall*)(CObjectSA*, int, const char*, char, const char*, char, bool, D3DCOLOR, D3DCOLOR, char))SAMP_ADDROF(0xA3050))(this, nIndex, szText, nMaterialSize, szFont, nFontSize, bBold, fontColor, backgroundColor, align);
}

bool SAMP::CObjectSA::GetMaterialSize(int nSize, int* x, int* y) {
	return ((bool(__thiscall*)(CObjectSA*, int, int*, int*))SAMP_ADDROF(0xA3620))(this, nSize, x, y);
}

void SAMP::CObjectSA::Render() {
	((void(__thiscall*)(CObjectSA*))SAMP_ADDROF(0xA3900))(this);
}

void SAMP::CObjectSA::ConstructMaterialText() {
	((void(__thiscall*)(CObjectSA*))SAMP_ADDROF(0xA47F0))(this);
}

void SAMP::CObjectSA::Draw() {
	((void(__thiscall*)(CObjectSA*))SAMP_ADDROF(0xA48A0))(this);
}

void SAMP::CObjectSA::ShutdownMaterialText() {
	((void(__thiscall*)(CObjectSA*))SAMP_ADDROF(0xA3870))(this);
}

#elif defined(SAMP_R3)

SAMP::CObjectSA::CObjectSA(int nModel, CVectorSA position, CVectorSA rotation, float fDrawDistance, int a10, char a11, char a12) {
	((void(__thiscall*)(CObjectSA*, int, CVectorSA, CVectorSA, float, int, char, char))SAMP_ADDROF(0xA8880))(this, nModel, position, rotation, fDrawDistance, a10, a11, a12);
}

float SAMP::CObjectSA::GetDistance(const CMatrixSA* pMatrix) {
	return ((float(__thiscall*)(CObjectSA*, const CMatrixSA*))SAMP_ADDROF(0xA7730))(this, pMatrix);
}

void SAMP::CObjectSA::Stop() {
	((void(__thiscall*)(CObjectSA*))SAMP_ADDROF(0xA77A0))(this);
}

void SAMP::CObjectSA::SetRotation(const CVectorSA* pVector) {
	((void(__thiscall*)(CObjectSA*, const CVectorSA*))SAMP_ADDROF(0xA7810))(this, pVector);
}

void SAMP::CObjectSA::SetAttachedToVehicle(ID nId, const CVectorSA* pOffset, const CVectorSA* pRotation) {
	((void(__thiscall*)(CObjectSA*, ID, const CVectorSA*, const CVectorSA*))SAMP_ADDROF(0xA7880))(this, nId, pOffset, pRotation);
}

void SAMP::CObjectSA::SetAttachedToObject(ID nId, const CVectorSA* pOffset, const CVectorSA* pRotation, char a5) {
	((void(__thiscall*)(CObjectSA*, ID, const CVectorSA*, const CVectorSA*, char))SAMP_ADDROF(0xA7910))(this, nId, pOffset, pRotation, a5);
}

void SAMP::CObjectSA::AttachToVehicle(CVehicleSA* pVehicle) {
	((void(__thiscall*)(CObjectSA*, CVehicleSA*))SAMP_ADDROF(0xA79B0))(this, pVehicle);
}

void SAMP::CObjectSA::AttachToObject(CObjectSA* pObject) {
	((void(__thiscall*)(CObjectSA*, CObjectSA*))SAMP_ADDROF(0xA7A30))(this, pObject);
}

void SAMP::CObjectSA::Rotate(CVectorSA vector) {
	((void(__thiscall*)(CObjectSA*, CVectorSA))SAMP_ADDROF(0xA7B30))(this, vector);
}

BOOL SAMP::CObjectSA::AttachedToMovingEntity() {
	return ((BOOL(__thiscall*)(CObjectSA*))SAMP_ADDROF(0xA7C30))(this);
}

void SAMP::CObjectSA::SetMaterial(int nModel, int nIndex, const char* szTxd, const char* szTexture, D3DCOLOR color) {
	((void(__thiscall*)(CObjectSA*, int, int, const char*, const char*, D3DCOLOR))SAMP_ADDROF(0xA7CA0))(this, nModel, nIndex, szTxd, szTexture, color);
}

void SAMP::CObjectSA::SetMaterialText(int nIndex, const char* szText, char nMaterialSize, const char* szFont, char nFontSize, bool bBold, D3DCOLOR fontColor, D3DCOLOR backgroundColor, char align) {
	((void(__thiscall*)(CObjectSA*, int, const char*, char, const char*, char, bool, D3DCOLOR, D3DCOLOR, char))SAMP_ADDROF(0xA7E20))(this, nIndex, szText, nMaterialSize, szFont, nFontSize, bBold, fontColor, backgroundColor, align);
}

bool SAMP::CObjectSA::GetMaterialSize(int nSize, int* x, int* y) {
	return ((bool(__thiscall*)(CObjectSA*, int, int*, int*))SAMP_ADDROF(0xA83F0))(this, nSize, x, y);
}

void SAMP::CObjectSA::Render() {
	((void(__thiscall*)(CObjectSA*))SAMP_ADDROF(0xA86D0))(this);
}

void SAMP::CObjectSA::Process(float fElapsedTime) {
	((void(__thiscall*)(CObjectSA*, float))SAMP_ADDROF(0xA8DC0))(this, fElapsedTime);
}

void SAMP::CObjectSA::ConstructMaterialText() {
	((void(__thiscall*)(CObjectSA*))SAMP_ADDROF(0xA9650))(this);
}

void SAMP::CObjectSA::Draw() {
	((void(__thiscall*)(CObjectSA*))SAMP_ADDROF(0xA9700))(this);
}

void SAMP::CObjectSA::ShutdownMaterialText() {
	((void(__thiscall*)(CObjectSA*))SAMP_ADDROF(0xA8640))(this);
}

#endif
