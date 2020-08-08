/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CPlayerPool.h"

#if defined(SAMP_R1)

SAMP::CPlayerPool::CPlayerPool() {
	((void(__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x10AD0))(this);
}

SAMP::CPlayerPool::~CPlayerPool() {
	((void(__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x10C20))(this);
}

void SAMP::CPlayerPool::Process() {
	((void(__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x10320))(this);
}

void SAMP::CPlayerPool::SetAt(ID nIdx, CPlayerInfo* pPlayer) {
	((void(__thiscall*)(CPlayerPool*, ID, CPlayerInfo*))SAMP_ADDROF(0x10290))(this, nIdx, pPlayer);
}

BOOL SAMP::CPlayerPool::Delete(ID nId, int nReason) {
	return ((int(__thiscall*)(CPlayerPool*, ID, int))SAMP_ADDROF(0x10B90))(this, nId, nReason);
}

void SAMP::CPlayerPool::Deactivate() {
	((void(__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x10650))(this);
}

BOOL SAMP::CPlayerPool::Create(ID nId, const char* szName, BOOL bIsNPC) {
	return ((BOOL(__thiscall*)(CPlayerPool*, ID, const char*, BOOL))SAMP_ADDROF(0x10D50))(this, nId, szName, bIsNPC);
}

const char* SAMP::CPlayerPool::GetName(ID nId) {
	return ((const char* (__thiscall*)(CPlayerPool*, ID))SAMP_ADDROF(0x13CE0))(this, nId);
}

SAMP::ID SAMP::CPlayerPool::Find(::CPed* pPed) {
	return ((ID(__thiscall*)(CPlayerPool*, ::CPed*))SAMP_ADDROF(0x10420))(this, pPed);
}

int SAMP::CPlayerPool::GetCount(BOOL bIncludeNPC) {
	return ((int(__thiscall*)(CPlayerPool*, BOOL))SAMP_ADDROF(0x10520))(this, bIncludeNPC);
}

void SAMP::CPlayerPool::ForceCollision() {
	((void(__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x107B0))(this);
}

void SAMP::CPlayerPool::RestoreCollision() {
	((void(__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x10820))(this);
}

const char* SAMP::CPlayerPool::GetLocalPlayerName() {
	return ((const char* (__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x13CD0))(this);
}

SAMP::CRemotePlayer* SAMP::CPlayerPool::GetPlayer(ID nId) {
	return ((CRemotePlayer * (__thiscall*)(CPlayerPool*, ID))SAMP_ADDROF(0x10F0))(this, nId);
}

SAMP::CPlayerInfo* SAMP::CPlayerPool::GetAt(ID nId) {
	return ((CPlayerInfo * (__thiscall*)(CPlayerPool*, ID))SAMP_ADDROF(0x10D0))(this, nId);
}

BOOL SAMP::CPlayerPool::IsConnected(ID nId) {
	return ((BOOL(__thiscall*)(CPlayerPool*, ID))SAMP_ADDROF(0x10B0))(this, nId);
}

BOOL SAMP::CPlayerPool::IsNPC(ID nId) {
	return ((BOOL(__thiscall*)(CPlayerPool*, ID))SAMP_ADDROF(0xB680))(this, nId);
}

void SAMP::CPlayerPool::SetPing(ID nId, int nValue) {
	((void(__thiscall*)(CPlayerPool*, ID, int))SAMP_ADDROF(0xB705))(this, nId, nValue);
}

void SAMP::CPlayerPool::SetScore(ID nId, int nValue) {
	((void(__thiscall*)(CPlayerPool*, ID, int))SAMP_ADDROF(0xB6C0))(this, nId, nValue);
}

SAMP::CLocalPlayer* SAMP::CPlayerPool::GetLocalPlayer() {
	return ((CLocalPlayer * (__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x1A30))(this);
}

int SAMP::CPlayerPool::GetScore(ID nPlayer) {
	return ((int(__thiscall*)(CPlayerPool*, ID))SAMP_ADDROF(0x6A190))(this, nPlayer);
}

int SAMP::CPlayerPool::GetPing(ID nPlayer) {
	return ((int(__thiscall*)(CPlayerPool*, ID))SAMP_ADDROF(0x6A1C0))(this, nPlayer);
}

void SAMP::CPlayerPool::UpdateLargestId() {
	((void(__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x102B0))(this);
}

SAMP::CObjectSA* SAMP::CPlayerPool::FindAccessory(::CObject* pGameObject) {
	return ((CObjectSA * (__thiscall*)(CPlayerPool*, ::CObject*))SAMP_ADDROF(0x106A0))(this, pGameObject);
}

void SAMP::CPlayerPool::SetLocalPlayerName(const char* szName) {
	((void(__thiscall*)(CPlayerPool*, const char*))SAMP_ADDROF(0xB3E0))(this, szName);
}

BOOL SAMP::CPlayerPool::IsDisconnected(ID nId) {
	if (nId < 0 || nId >= MAX_PLAYERS)
		return 0;
	return m_pObject[nId] == nullptr;
}

#elif defined(SAMP_R3)

SAMP::CPlayerPool::CPlayerPool() {
	((void(__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x13BE0))(this);
}

SAMP::CPlayerPool::~CPlayerPool() {
	((void(__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x13D40))(this);
}

void SAMP::CPlayerPool::UpdateLargestId() {
	((void(__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x13400))(this);
}

void SAMP::CPlayerPool::Process() {
	((void(__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x13470))(this);
}

SAMP::ID SAMP::CPlayerPool::Find(::CPed* pGamePed) {
	return ((ID(__thiscall*)(CPlayerPool*, ::CPed*))SAMP_ADDROF(0x13570))(this, pGamePed);
}

void SAMP::CPlayerPool::Deactivate() {
	((void(__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x13790))(this);
}

void SAMP::CPlayerPool::ForceCollision() {
	((void(__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x138C0))(this);
}

void SAMP::CPlayerPool::RestoreCollision() {
	((void(__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x13930))(this);
}

BOOL SAMP::CPlayerPool::Delete(ID nId, int nReason) {
	return ((BOOL(__thiscall*)(CPlayerPool*, ID, int))SAMP_ADDROF(0x13CB0))(this, nId, nReason);
}

BOOL SAMP::CPlayerPool::Create(ID nId, const char* szName, BOOL bIsNPC) {
	return ((BOOL(__thiscall*)(CPlayerPool*, ID, const char*, BOOL))SAMP_ADDROF(0x13E80))(this, nId, szName, bIsNPC);
}

SAMP::CRemotePlayer* SAMP::CPlayerPool::GetPlayer(ID nId) {
	return ((CRemotePlayer * (__thiscall*)(CPlayerPool*, ID))SAMP_ADDROF(0x10F0))(this, nId);
}

const char* SAMP::CPlayerPool::GetLocalPlayerName() {
	return ((const char* (__thiscall*)(CPlayerPool*))SAMP_ADDROF(0xA170))(this);
}

BOOL SAMP::CPlayerPool::IsDisconnected(ID nId) {
	return ((BOOL(__thiscall*)(CPlayerPool*, ID))SAMP_ADDROF(0x10D0))(this, nId);
}

BOOL SAMP::CPlayerPool::IsConnected(ID nId) {
	return ((BOOL(__thiscall*)(CPlayerPool*, ID))SAMP_ADDROF(0x10B0))(this, nId);
}

void SAMP::CPlayerPool::SetLocalPlayerName(const char* szName) {
	((void(__thiscall*)(CPlayerPool*, const char*))SAMP_ADDROF(0xB5C0))(this, szName);
}

void SAMP::CPlayerPool::SetAt(ID nId, CPlayerInfo* pObject) {
	((void(__thiscall*)(CPlayerPool*, ID, CPlayerInfo*))SAMP_ADDROF(0x133E0))(this, nId, pObject);
}

int SAMP::CPlayerPool::GetScore(ID nId) {
	return ((int(__thiscall*)(CPlayerPool*, ID))SAMP_ADDROF(0x6E0E0))(this, nId);
}

int SAMP::CPlayerPool::GetPing(ID nId) {
	return ((int(__thiscall*)(CPlayerPool*, ID))SAMP_ADDROF(0x6E110))(this, nId);
}

const char* SAMP::CPlayerPool::GetName(ID nId) {
	return ((const char* (__thiscall*)(CPlayerPool*, ID))SAMP_ADDROF(0x16F00))(this, nId);
}

int SAMP::CPlayerPool::GetLocalPlayerPing() {
	return ((int(__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x6E150))(this);
}

int SAMP::CPlayerPool::GetLocalPlayerScore() {
	return ((int(__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x6E140))(this);
}

int SAMP::CPlayerPool::GetCount(BOOL bIncludeNPC) {
	return ((int(__thiscall*)(CPlayerPool*, BOOL))SAMP_ADDROF(0x13670))(this, bIncludeNPC);
}

SAMP::CLocalPlayer* SAMP::CPlayerPool::GetLocalPlayer() {
	return ((CLocalPlayer * (__thiscall*)(CPlayerPool*))SAMP_ADDROF(0x1A30))(this);
}

SAMP::CObjectSA* SAMP::CPlayerPool::FindAccessory(::CObject* pGameObject) {
	return ((CObjectSA * (__thiscall*)(CPlayerPool*, ::CObject*))SAMP_ADDROF(0x137F0))(this, pGameObject);
}

#endif
