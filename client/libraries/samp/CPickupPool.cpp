/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CPickupPool.h"

#if defined(SAMP_R1)

SAMP::CPickupPool::CPickupPool() {
	((void(__thiscall*)(CPickupPool*))SAMP_ADDROF(0x80F0))(this);
}

SAMP::CPickupPool::~CPickupPool() {
	((void(__thiscall*)(CPickupPool*))SAMP_ADDROF(0xFF60))(this);
}

void SAMP::CPickupPool::Create(Pickup* pPickup, ID nId) {
	((void(__thiscall*)(CPickupPool*, Pickup*, ID))SAMP_ADDROF(0xFDC0))(this, pPickup, nId);
}

void SAMP::CPickupPool::CreateWeapon(int nModel, CVectorSA position, int nAmmo, ID nExOwner) {
	((void(__thiscall*)(CPickupPool*, int, CVectorSA, int, ID))SAMP_ADDROF(0xFCD0))(this, nModel, position, nAmmo, nExOwner);
}

int SAMP::CPickupPool::GetIndex(int nId) {
	return ((int(__thiscall*)(CPickupPool*, int))SAMP_ADDROF(0xFF30))(this, nId);
}

void SAMP::CPickupPool::Delete(int nId) {
	((void(__thiscall*)(CPickupPool*, int))SAMP_ADDROF(0xFE70))(this, nId);
}

void SAMP::CPickupPool::DeleteWeapon(ID nExOwner) {
	((void(__thiscall*)(CPickupPool*, ID))SAMP_ADDROF(0xFED0))(this, nExOwner);
}

void SAMP::CPickupPool::SendNotification(int nId) {
	((void(__thiscall*)(CPickupPool*, int))SAMP_ADDROF(0xFF90))(this, nId);
}

void SAMP::CPickupPool::Process() {
	((void(__thiscall*)(CPickupPool*))SAMP_ADDROF(0x10070))(this);
}

#elif defined(SAMP_R3)

SAMP::CPickupPool::CPickupPool() {
	((void(__thiscall*)(CPickupPool*))SAMP_ADDROF(0x8130))(this);
}

SAMP::CPickupPool::~CPickupPool() {
	((void(__thiscall*)(CPickupPool*))SAMP_ADDROF(0x130C0))(this);
}

void SAMP::CPickupPool::Create(Pickup* pData, ID nId) {
	((void(__thiscall*)(CPickupPool*, Pickup*, ID))SAMP_ADDROF(0x12F20))(this, pData, nId);
}

void SAMP::CPickupPool::CreateWeapon(int nModel, CVectorSA position, int nAmmo, ID nExOwner) {
	((void(__thiscall*)(CPickupPool*, int, CVectorSA, int, ID))SAMP_ADDROF(0x12E30))(this, nModel, position, nAmmo, nExOwner);
}

void SAMP::CPickupPool::Delete(int nId) {
	((void(__thiscall*)(CPickupPool*, int))SAMP_ADDROF(0x12FD0))(this, nId);
}

void SAMP::CPickupPool::DeleteWeapon(ID nExOwner) {
	((void(__thiscall*)(CPickupPool*, ID))SAMP_ADDROF(0x13030))(this, nExOwner);
}

int SAMP::CPickupPool::GetIndex(int nId) {
	return ((int(__thiscall*)(CPickupPool*, int))SAMP_ADDROF(0x13090))(this, nId);
}

void SAMP::CPickupPool::SendNotification(int nId) {
	((void(__thiscall*)(CPickupPool*, int))SAMP_ADDROF(0x130F0))(this, nId);
}

void SAMP::CPickupPool::Process() {
	((void(__thiscall*)(CPickupPool*))SAMP_ADDROF(0x131D0))(this);
}

#endif
