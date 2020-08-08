/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CObjectPool.h"

#if defined(SAMP_R1)

TICK& SAMP::CObjectPool::lastProcess() { return *(TICK*)SAMP_ADDROF(0x1049B0); }

SAMP::CObjectPool::CObjectPool() {
	((void(__thiscall*)(CObjectPool*))SAMP_ADDROF(0xF3A0))(this);
}

SAMP::CObjectPool::~CObjectPool() {
	((void(__thiscall*)(CObjectPool*))SAMP_ADDROF(0xFCB0))(this);
}

void SAMP::CObjectPool::UpdateLargestId() {
	((void(__thiscall*)(CObjectPool*))SAMP_ADDROF(0xF340))(this);
}

int SAMP::CObjectPool::GetCount() {
	return ((int(__thiscall*)(CObjectPool*))SAMP_ADDROF(0xF3D0))(this);
}

BOOL SAMP::CObjectPool::Delete(ID nId) {
	return ((BOOL(__thiscall*)(CObjectPool*, ID))SAMP_ADDROF(0xF3F0))(this, nId);
}

BOOL SAMP::CObjectPool::Create(ID nId, int nModel, CVectorSA position, CVectorSA rotation, float fDrawDistance) {
	return ((BOOL(__thiscall*)(CObjectPool*, ID, int, CVectorSA, CVectorSA, float))SAMP_ADDROF(0xF470))(this, nId, nModel, position, rotation, fDrawDistance);
}

SAMP::CObjectSA* SAMP::CObjectPool::Find(::CObject* pGameObject) {
	return ((CObjectSA * (__thiscall*)(CObjectPool*, ::CObject*))SAMP_ADDROF(0xF520))(this, pGameObject);
}

int SAMP::CObjectPool::GetId(::CObject* pObject) {
	return ((int(__thiscall*)(CObjectPool*, ::CObject*))SAMP_ADDROF(0xF560))(this, pObject);
}

void SAMP::CObjectPool::Process() {
	((void(__thiscall*)(CObjectPool*))SAMP_ADDROF(0xF5A0))(this);
}

void SAMP::CObjectPool::ConstructMaterials() {
	((void(__thiscall*)(CObjectPool*))SAMP_ADDROF(0xF660))(this);
}

void SAMP::CObjectPool::ShutdownMaterials() {
	((void(__thiscall*)(CObjectPool*))SAMP_ADDROF(0xF6A0))(this);
}

void SAMP::CObjectPool::Draw() {
	((void(__thiscall*)(CObjectPool*))SAMP_ADDROF(0xF6E0))(this);
}

void SAMP::CObjectPool::DrawLast() {
	((void(__thiscall*)(CObjectPool*))SAMP_ADDROF(0xF720))(this);
}

#elif defined(SAMP_R3)

SAMP::CObjectPool::CObjectPool() {
	((void(__thiscall*)(CObjectPool*))SAMP_ADDROF(0x124B0))(this);
}

SAMP::CObjectPool::~CObjectPool() {
	((void(__thiscall*)(CObjectPool*))SAMP_ADDROF(0x12E10))(this);
}

void SAMP::CObjectPool::UpdateLargestId() {
	((void(__thiscall*)(CObjectPool*))SAMP_ADDROF(0x12450))(this);
}

int SAMP::CObjectPool::GetCount() {
	return ((int(__thiscall*)(CObjectPool*))SAMP_ADDROF(0x124E0))(this);
}

BOOL SAMP::CObjectPool::Delete(ID nId) {
	return ((BOOL(__thiscall*)(CObjectPool*, ID))SAMP_ADDROF(0x12500))(this, nId);
}

BOOL SAMP::CObjectPool::Create(ID nId, int nModel, CVectorSA position, CVectorSA rotation, float fDrawDistance) {
	return ((BOOL(__thiscall*)(CObjectPool*, ID, int, CVectorSA, CVectorSA, float))SAMP_ADDROF(0x12580))(this, nId, nModel, position, rotation, fDrawDistance);
}

SAMP::CObjectSA* SAMP::CObjectPool::Find(::CObject* pGameObject) {
	return ((CObjectSA * (__thiscall*)(CObjectPool*, ::CObject*))SAMP_ADDROF(0x12680))(this, pGameObject);
}

int SAMP::CObjectPool::GetId(::CObject* pGameObject) {
	return ((int(__thiscall*)(CObjectPool*, ::CObject*))SAMP_ADDROF(0x126C0))(this, pGameObject);
}

void SAMP::CObjectPool::Process() {
	((void(__thiscall*)(CObjectPool*))SAMP_ADDROF(0x12700))(this);
}

void SAMP::CObjectPool::ConstructMaterials() {
	((void(__thiscall*)(CObjectPool*))SAMP_ADDROF(0x127C0))(this);
}

void SAMP::CObjectPool::ShutdownMaterials() {
	((void(__thiscall*)(CObjectPool*))SAMP_ADDROF(0x12800))(this);
}

void SAMP::CObjectPool::Draw() {
	((void(__thiscall*)(CObjectPool*))SAMP_ADDROF(0x12840))(this);
}

void SAMP::CObjectPool::DrawLast() {
	((void(__thiscall*)(CObjectPool*))SAMP_ADDROF(0x12880))(this);
}

SAMP::CObjectSA* SAMP::CObjectPool::GetObject(ID nId) {
	return ((CObjectSA * (__thiscall*)(CObjectPool*, ID))SAMP_ADDROF(0x2DC0))(this, nId);
}

#endif
