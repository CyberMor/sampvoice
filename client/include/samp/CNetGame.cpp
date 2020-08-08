/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CNetGame.h"

#if defined(SAMP_R1)

SAMP::CNetGame*& SAMP::pNetGame() { return *(SAMP::CNetGame**)SAMP_ADDROF(0x21A0F8); }
int& SAMP::CNetGame::nVehiclePoolProcessFlag() { return *(int*)SAMP_ADDROF(0x10496C); }
int& SAMP::CNetGame::nPickupPoolProcessFlag() { return *(int*)SAMP_ADDROF(0x104970); }
TICK& SAMP::CNetGame::lastPlayersUpdateRequest() { return *(TICK*)SAMP_ADDROF(0x104978); }

void SAMP::CNetGame::InitializeGameLogic() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8510))(this);
}

void SAMP::CNetGame::Packet_DisconnectionNotification(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x88E0))(this, pPacket);
}

void SAMP::CNetGame::DeleteMarker(NUMBER nIndex) {
	((void(__thiscall*)(CNetGame*, NUMBER))SAMP_ADDROF(0x8AB0))(this, nIndex);
}

void SAMP::CNetGame::ResetVehiclePool() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8B80))(this);
}

void SAMP::CNetGame::ResetTextDrawPool() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8C20))(this);
}

void SAMP::CNetGame::ResetObjectPool() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8CC0))(this);
}

void SAMP::CNetGame::ResetGangZonePool() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8D60))(this);
}

void SAMP::CNetGame::ResetPickupPool() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8E00))(this);
}

void SAMP::CNetGame::ResetMenuPool() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8E60))(this);
}

void SAMP::CNetGame::ResetLabelPool() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8F00))(this);
}

void SAMP::CNetGame::ResetActorPool() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8FA0))(this);
}

SAMP::CNetGame::~CNetGame() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x9380))(this);
}

void SAMP::CNetGame::Packet_UnoccupiedSync(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x9550))(this, pPacket);
}

void SAMP::CNetGame::Packet_BulletSync(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x9650))(this, pPacket);
}

void SAMP::CNetGame::Packet_AimSync(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x9750))(this, pPacket);
}

void SAMP::CNetGame::Packet_PassengerSync(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x9840))(this, pPacket);
}

void SAMP::CNetGame::Packet_TrailerSync(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x9930))(this, pPacket);
}

void SAMP::CNetGame::Packet_MarkersSync(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x9A20))(this, pPacket);
}

void SAMP::CNetGame::Packet_AuthKey(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x9C10))(this, pPacket);
}

void SAMP::CNetGame::Packet_PlayerSync(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0xA250))(this, pPacket);
}

void SAMP::CNetGame::Packet_VehicleSync(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0xA520))(this, pPacket);
}

void SAMP::CNetGame::Packet_ConnectionLost(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0xA800))(this, pPacket);
}

void SAMP::CNetGame::Packet_ConnectionSucceeded(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0xA890))(this, pPacket);
}

void SAMP::CNetGame::UpdateNetwork() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0xAD70))(this);
}

void SAMP::CNetGame::ShutdownForRestart() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0xA060))(this);
}

void SAMP::CNetGame::CreateMarker(NUMBER nIndex, CVectorSA position, char nIcon, int nColor, int nType) {
	((void(__thiscall*)(CNetGame*, NUMBER, CVectorSA, char, int, int))SAMP_ADDROF(0x9E20))(this, nIndex, position, nIcon, nColor, nType);
}

void SAMP::CNetGame::ResetMarkers() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x9DE0))(this);
}

SAMP::CPlayerPool* SAMP::CNetGame::GetPlayerPool() {
	return ((CPlayerPool * (__thiscall*)(CNetGame*))SAMP_ADDROF(0x1160))(this);
}

SAMP::CVehiclePool* SAMP::CNetGame::GetVehiclePool() {
	return ((CVehiclePool * (__thiscall*)(CNetGame*))SAMP_ADDROF(0x1170))(this);
}

SAMP::CPickupPool* SAMP::CNetGame::GetPickupPool() {
	return ((CPickupPool * (__thiscall*)(CNetGame*))SAMP_ADDROF(0x8130))(this);
}

SAMP::CMenuPool* SAMP::CNetGame::GetMenuPool() {
	return ((CMenuPool * (__thiscall*)(CNetGame*))SAMP_ADDROF(0x8140))(this);
}

void SAMP::CNetGame::SetState(int nState) {
	((void(__thiscall*)(CNetGame*, int))SAMP_ADDROF(0x8150))(this, nState);
}

void SAMP::CNetGame::ProcessGameStuff() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8680))(this);
}

void SAMP::CNetGame::Packet_RSAPublicKeyMismatch(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet * pPacket))SAMP_ADDROF(0x8850))(this, pPacket);
}

void SAMP::CNetGame::Packet_ConnectionBanned(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet * pPacket))SAMP_ADDROF(0x8870))(this, pPacket);
}

void SAMP::CNetGame::Packet_NoFreeIncomingConnections(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet * pPacket))SAMP_ADDROF(0x88B0))(this, pPacket);
}

void SAMP::CNetGame::Packet_InvalidPassword(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet * pPacket))SAMP_ADDROF(0x8920))(this, pPacket);
}

void SAMP::CNetGame::Packet_ConnectionAttemptFailed(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet * pPacket))SAMP_ADDROF(0x8960))(this, pPacket);
}

void SAMP::CNetGame::UpdatePlayers() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8A10))(this);
}

void SAMP::CNetGame::InitializePools() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8160))(this);
}

__int64 SAMP::CNetGame::GetCounter() {
	return ((__int64(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8500))(this);
}

void SAMP::CNetGame::ResetPlayerPool() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8AE0))(this);
}

void SAMP::CNetGame::Packet_ConnectionRequestAcepted(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x8890))(this, pPacket);
}

void SAMP::CNetGame::ResetPools() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0xA010))(this);
}

#elif defined(SAMP_R3)

SAMP::CNetGame*& SAMP::pNetGame() { return *(SAMP::CNetGame**)SAMP_ADDROF(0x26E8DC); }
TICK& SAMP::CNetGame::lastPlayersUpdateRequest() { return *(TICK*)SAMP_ADDROF(0x1189F0); }

SAMP::CNetGame::CNetGame(const char* szHostAddress, int nPort, const char* szNick, const char* szPass) {
	((void(__thiscall*)(CNetGame*, const char*, int, const char*, const char*))SAMP_ADDROF(0xB5F0))(this, szHostAddress, nPort, szNick, szPass);
}

SAMP::CNetGame::~CNetGame() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x9510))(this);
}

SAMP::CPickupPool* SAMP::CNetGame::GetPickupPool() {
	return ((CPickupPool * (__thiscall*)(CNetGame*))SAMP_ADDROF(0x8170))(this);
}

SAMP::CMenuPool* SAMP::CNetGame::GetMenuPool() {
	return ((CMenuPool * (__thiscall*)(CNetGame*))SAMP_ADDROF(0x8180))(this);
}

void SAMP::CNetGame::SetState(int nValue) {
	((void(__thiscall*)(CNetGame*, int))SAMP_ADDROF(0x8190))(this, nValue);
}

void SAMP::CNetGame::InitializePools() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x81D0))(this);
}

void SAMP::CNetGame::InitialNotification() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x83F0))(this);
}

void SAMP::CNetGame::InitializeGameLogic() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8580))(this);
}

void SAMP::CNetGame::Connect() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x85D0))(this);
}

void SAMP::CNetGame::SpawnScreen() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8640))(this);
}

void SAMP::CNetGame::Packet_RSAPublicKeyMismatch(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x89E0))(this, pPacket);
}

void SAMP::CNetGame::Packet_ConnectionBanned(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x8A00))(this, pPacket);
}

void SAMP::CNetGame::Packet_ConnectionRequestAcepted(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x8A20))(this, pPacket);
}

void SAMP::CNetGame::Packet_NoFreeIncomingConnections(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x8A40))(this, pPacket);
}

void SAMP::CNetGame::Packet_DisconnectionNotification(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x8A70))(this, pPacket);
}

void SAMP::CNetGame::Packet_InvalidPassword(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x8AB0))(this, pPacket);
}

void SAMP::CNetGame::Packet_ConnectionAttemptFailed(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x8AF0))(this, pPacket);
}

void SAMP::CNetGame::UpdatePlayers() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8BA0))(this);
}

void SAMP::CNetGame::DeleteMarker(NUMBER nIndex) {
	((void(__thiscall*)(CNetGame*, NUMBER))SAMP_ADDROF(0x8C40))(this, nIndex);
}

void SAMP::CNetGame::ResetPlayerPool() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8C70))(this);
}

void SAMP::CNetGame::ResetVehiclePool() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8D10))(this);
}

void SAMP::CNetGame::ResetTextDrawPool() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8DB0))(this);
}

void SAMP::CNetGame::ResetObjectPool() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8E50))(this);
}

void SAMP::CNetGame::ResetGangZonePool() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8EF0))(this);
}

void SAMP::CNetGame::ResetPickupPool() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8F90))(this);
}

void SAMP::CNetGame::ResetMenuPool() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8FF0))(this);
}

void SAMP::CNetGame::ResetLabelPool() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x9080))(this);
}

void SAMP::CNetGame::ResetActorPool() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x9120))(this);
}

void SAMP::CNetGame::Packet_UnoccupiedSync(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x96D0))(this, pPacket);
}

void SAMP::CNetGame::Packet_BulletSync(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x97D0))(this, pPacket);
}

void SAMP::CNetGame::Packet_AimSync(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x98D0))(this, pPacket);
}

void SAMP::CNetGame::Packet_PassengerSync(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x99C0))(this, pPacket);
}

void SAMP::CNetGame::Packet_TrailerSync(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x9AB0))(this, pPacket);
}

void SAMP::CNetGame::Packet_MarkersSync(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x9BA0))(this, pPacket);
}

void SAMP::CNetGame::Packet_AuthKey(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0x9D90))(this, pPacket);
}

void SAMP::CNetGame::ResetMarkers() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x9F50))(this);
}

void SAMP::CNetGame::CreateMarker(NUMBER nIndex, CVectorSA position, char nIcon, int nColor, int nType) {
	((void(__thiscall*)(CNetGame*, NUMBER, CVectorSA, char, int, int))SAMP_ADDROF(0x9F90))(this, nIndex, position, nIcon, nColor, nType);
}

void SAMP::CNetGame::ResetPools() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0xA190))(this);
}

void SAMP::CNetGame::ShutdownForRestart() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0xA1E0))(this);
}

void SAMP::CNetGame::Packet_PlayerSync(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0xA3E0))(this, pPacket);
}

void SAMP::CNetGame::Packet_VehicleSync(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0xA6B0))(this, pPacket);
}

void SAMP::CNetGame::Packet_ConnectionLost(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0xA990))(this, pPacket);
}

void SAMP::CNetGame::Packet_ConnectionSucceeded(Packet* pPacket) {
	((void(__thiscall*)(CNetGame*, Packet*))SAMP_ADDROF(0xAA20))(this, pPacket);
}

void SAMP::CNetGame::UpdateNetwork() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0xAF20))(this);
}

void SAMP::CNetGame::Process() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0xB270))(this);
}

void SAMP::CNetGame::ProcessGameStuff() {
	((void(__thiscall*)(CNetGame*))SAMP_ADDROF(0x87B0))(this);
}

SAMP::CPlayerPool* SAMP::CNetGame::GetPlayerPool() {
	return ((CPlayerPool * (__thiscall*)(CNetGame*))SAMP_ADDROF(0x1160))(this);
}

SAMP::CObjectPool* SAMP::CNetGame::GetObjectPool() {
	return ((CObjectPool * (__thiscall*)(CNetGame*))SAMP_ADDROF(0x2DF0))(this);
}

SAMP::CActorPool* SAMP::CNetGame::GetActorPool() {
	return ((CActorPool * (__thiscall*)(CNetGame*))SAMP_ADDROF(0x2E00))(this);
}

int SAMP::CNetGame::GetState() {
	return ((int(__thiscall*)(CNetGame*))SAMP_ADDROF(0x2E10))(this);
}

BOOL SAMP::CNetGame::LanMode() {
	return ((BOOL(__thiscall*)(CNetGame*))SAMP_ADDROF(0x2E20))(this);
}

SAMP::CVehiclePool* SAMP::CNetGame::GetVehiclePool() {
	return ((CVehiclePool * (__thiscall*)(CNetGame*))SAMP_ADDROF(0x1170))(this);
}

RakClientInterface* SAMP::CNetGame::GetRakClient() {
	return ((RakClientInterface * (__thiscall*)(CNetGame*))SAMP_ADDROF(0x1A40))(this);
}

__int64 SAMP::CNetGame::GetCounter() {
	return ((__int64(__thiscall*)(CNetGame*))SAMP_ADDROF(0x8570))(this);
}

#endif
