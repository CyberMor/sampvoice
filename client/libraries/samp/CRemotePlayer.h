/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#include "CVehicleSA.h"
#include "Packets.h"
#include "CPedSA.h"
#include "Animation.h"
#include "SpecialAction.h"
#include "CVectorSA.h"
#include "CMatrixSA.h"

SAMP_BEGIN

enum PlayerState {

	PLAYER_STATE_NONE,
	PLAYER_STATE_ONFOOT = 17,
	PLAYER_STATE_DRIVER = 19,
	PLAYER_STATE_PASSENGER = 18,
	PLAYER_STATE_WASTED = 32,
	PLAYER_STATE_SPAWNED = 33

};

enum UpdateType {

	UPDATE_TYPE_NONE,
	UPDATE_TYPE_ONFOOT = 16,
	UPDATE_TYPE_DRIVER = 17,
	UPDATE_TYPE_PASSENGER = 18

};

enum PlayerStatus { PLAYER_STATUS_TIMEOUT = 2 };

class CRemotePlayer {
public:

	CPedSA*					m_pPed;
	CVehicleSA*				m_pVehicle;
	NUMBER					m_nTeam;
	NUMBER					m_nState;
	NUMBER					m_nSeatId;
	int field_b;
	BOOL					m_bPassengerDriveBy;
	char pad_13[64];
	CVectorSA				m_positionDifference; // target pos - current pos
	
	struct SAMP_API {

		float				real;
		CVectorSA			imag;

	}						m_incarTargetRotation;
	
	int pad_6f[3];
	CVectorSA				m_onfootTargetPosition;
	CVectorSA				m_onfootTargetSpeed;
	CVectorSA				m_incarTargetPosition;
	CVectorSA				m_incarTargetSpeed;
	ID						m_nId;
	ID						m_nVehicleId;
	int field_af;
	BOOL					m_bDrawLabels;
	BOOL					m_bHasJetPack;
	NUMBER					m_nSpecialAction;
	int pad_bc[3];
	
	PACKET::OnfootData		m_onfootData;
	PACKET::IncarData		m_incarData;
	PACKET::TrailerData		m_trailerData;
	PACKET::PassengerData	m_passengerData;
	PACKET::AimData			m_aimData;
	
	float					m_fReportedArmour;
	float					m_fReportedHealth;
	Animation				m_animation;
	NUMBER					m_nUpdateType;
	TICK					m_lastUpdate;
	TICK					m_lastTimestamp;
	BOOL                    m_bPerformingCustomAnimation;
	int						m_nStatus;
	
	struct SAMP_API {

		CVectorSA			m_direction;
		TICK				m_lastUpdate;
		TICK				m_lastLook;

	}						m_head;
	
	BOOL					m_bMarkerState;
	
	struct SAMP_API {

		int					x, y, z;

	}						m_markerPosition;

	GTAREF					m_marker;

	CRemotePlayer();
	~CRemotePlayer();
	
	BOOL Spawn(/* unused */ int a2, int nModel, CVectorSA* pPosition, float fRotation, D3DCOLOR color, char nFightingStyle);
	void ProcessHead();
	void SetMarkerState(BOOL bState);
	void SetMarkerPosition(int x, int y, int z);
	BOOL Surfing();
	BOOL SurfingOnObject();
	void ProcessSurfing();
	void OnEnterVehicle();
	void OnExitVehicle();
	void ProcessSpecialAction();
	void UpdateOnfootSpeedAndPosition();
	void UpdateOnfootRotation();
	void SetOnfootTargetSpeedAndPosition(CVectorSA* pPosition, CVectorSA* pSpeed);
	void UpdateIncarSpeedAndPosition();
	void UpdateIncarRotation();
	void SetIncarTargetSpeedAndPosition(CMatrixSA* pMatrix, CVectorSA* pPosition, CVectorSA* pSpeed);
	void UpdateTrain(CMatrixSA* pMatrix, CVectorSA* pSpeed, float fSpeed);
	void Update(PACKET::AimData* pData);
	void Update(PACKET::UnoccupiedData* pData);
	void Update(PACKET::TrailerData* pData);
	void ResetData();
	float GetDistanceToPlayer(CRemotePlayer* pPlayer);
	float GetDistanceToLocalPlayer();
	void SetColor(D3DCOLOR color); // rgba
	D3DCOLOR GetColorAsRGBA();
	D3DCOLOR GetColorAsARGB();
	void EnterVehicle(ID nId, BOOL bPassenger);
	void ExitVehicle();
	void ChangeState(char nOld, char nNew);
	int GetStatus();
	void Update(PACKET::BulletData* pData);
	void Process();
	void Update(PACKET::OnfootData* pData, TICK timestamp);
	void Update(PACKET::IncarData* pData, TICK timestamp);
	void Update(PACKET::PassengerData* pData, TICK timestamp = SAMP_UNUSED);
	void Remove();
	void Kill();
	void Chat(const char* szText);
	BOOL DoesExist();

};

SAMP_END

#elif defined(SAMP_R3)

#include "CPedSA.h"
#include "CMatrixSA.h"
#include "CVectorSA.h"
#include "Packets.h"
#include "CVehicleSA.h"
#include "Animation.h"

SAMP_BEGIN

enum PlayerState {

	PLAYER_STATE_NONE,
	PLAYER_STATE_ONFOOT = 17,
	PLAYER_STATE_DRIVER = 19,
	PLAYER_STATE_PASSENGER = 18,
	PLAYER_STATE_WASTED = 32,
	PLAYER_STATE_SPAWNED = 33

};

enum UpdateType {

	UPDATE_TYPE_NONE,
	UPDATE_TYPE_ONFOOT = 16,
	UPDATE_TYPE_INCAR = 17,
	UPDATE_TYPE_PASSENGER = 18

};

enum PlayerStatus { PLAYER_STATUS_TIMEOUT = 2 };

class SAMP_API CRemotePlayer {
public:

	CPedSA*					m_pPed;
	CVehicleSA*				m_pVehicle;
	ID						m_nId;
	ID						m_nVehicleId;
	int field_1;
	BOOL					m_bDrawLabels;
	BOOL					m_bHasJetpack;
	unsigned char			m_nSpecialAction;
	PACKET::IncarData		m_incarData;
	PACKET::TrailerData		m_trailerData;
	PACKET::AimData			m_aimData;
	PACKET::PassengerData	m_passengerData;
	PACKET::OnfootData		m_onfootData;
	unsigned char           m_nTeam;
	unsigned char			m_nState;
	unsigned char			m_nSeatId;
	int field_3;
	BOOL					m_bPassengerDriveBy;
	CVectorSA				m_onfootTargetPosition;
	CVectorSA				m_onfootTargetSpeed;
	CVectorSA				m_incarTargetPosition;
	CVectorSA				m_incarTargetSpeed;
	char pad_1[76];
	CVectorSA				m_positionDifference;

	struct SAMP_API {

		float				real;
		CVectorSA			imag;

	}						m_incarTargetRotation;

	float					m_fReportedArmour;
	float					m_fReportedHealth;
	char pad_2[12];
	Animation				m_animation;
	unsigned char			m_nUpdateType;
	TICK					m_lastUpdate;
	TICK					m_lastTimestamp;
	BOOL                    m_bPerformingCustomAnimation;
	int						m_nStatus;

	struct SAMP_API {

		CVectorSA			m_direction;
		TICK				m_lastUpdate;
		TICK				m_lastLook;

	}						m_head;

	BOOL					m_bMarkerState;

	struct SAMP_API {

		int					x, y, z;

	}						m_markerPosition;

	GTAREF					m_marker;

	CRemotePlayer();
	~CRemotePlayer();

	void ProcessHead();
	void SetMarkerState(BOOL bState);
	void SetMarkerPosition(int x, int y, int z);
	BOOL SurfingOnVehicle();
	BOOL SurfingOnObject();
	void ProcessSurfing();
	void OnEnterVehicle();
	void OnExitVehicle();
	void ProcessSpecialAction();
	void UpdateOnfootSpeedAndPosition();
	void UpdateOnfootRotation();
	void SetOnfootTargetSpeedAndPosition(CVectorSA* pPosition, CVectorSA* pSpeed);
	void UpdateIncarSpeedAndPosition();
	void UpdateIncarRotation();
	void SetIncarTargetSpeedAndPosition(CMatrixSA* pMatrix, CVectorSA* pPosition, CVectorSA* pSpeed);
	void UpdateTrain(CMatrixSA* pMatrix, CVectorSA* pSpeed, float fSpeed);
	void Update(PACKET::AimData* pData);
	void Update(PACKET::UnoccupiedData* pData);
	void Update(PACKET::TrailerData* pData);
	void ResetData();
	float GetDistanceToPlayer(CRemotePlayer* pPlayer);
	float GetDistanceToLocalPlayer();
	void SetColor(D3DCOLOR color); // rgba
	D3DCOLOR GetColorAsRGBA();
	D3DCOLOR GetColorAsARGB();
	void EnterVehicle(ID nId, BOOL bPassenger);
	void ExitVehicle();
	void ChangeState(char nOld, char nNew);
	int GetStatus();
	void Update(PACKET::BulletData* pData);
	void Process();
	BOOL Spawn(/* unused */ int a2, int nModel, /* unused */ int a4, CVectorSA* pPosition, float fRotation, D3DCOLOR color, char nFightingStyle);
	void Update(PACKET::OnfootData* pData, TICK timestamp);
	void Update(PACKET::IncarData* pData, TICK timestamp);
	void Update(PACKET::PassengerData* pData, TICK timestamp = SAMP_UNUSED);
	void Remove();
	void Kill();
	void Chat(const char* szText);
	BOOL DoesExist();

};

SAMP_END

#endif
