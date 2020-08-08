/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#include "CVectorSA.h"
#include "CMatrixSA.h"

class CEntity;
struct RwObject;

SAMP_BEGIN

class SAMP_API CEntitySA {
public:

	// void **m_lpVtbl = 0xD9EBC;

	char pad_4[60];
	::CEntity*	m_pGameEntity;
	GTAREF		m_handle;

	CEntitySA();

	virtual ~CEntitySA() SAMP_VIRTUAL;
	virtual void Add() SAMP_VIRTUAL;
	virtual void Remove() SAMP_VIRTUAL;

	void GetMatrix(CMatrixSA* pMatrix);
	void SetMatrix(CMatrixSA matrix);
	void GetSpeed(CVectorSA* pVec);
	void SetSpeed(CVectorSA vec);
	void GetTurnSpeed(CVectorSA* pVec);
	void SetTurnSpeed(CVectorSA vec);
	void ApplyTurnSpeed();
	float GetDistanceFromCentreOfMassToBaseOfModel();
	void GetBoundCentre(CVectorSA* pVec);
	void SetModelIndex(int nModel);
	int GetModelIndex();
	void Teleport(CVectorSA position);
	float GetDistanceToLocalPlayer();
	float GetDistanceToCamera();
	float GetDistanceToPoint(CVectorSA position);
	BOOL DoesExist(); // does entity exist in the game world?
	BOOL EnforceWorldBoundries(float fPX, float fZX, float fPY, float fNY);
	BOOL HasExceededWorldBoundries(float fPX, float fZX, float fPY, float fNY);
	void GetEulerInverted(float* x, float* y, float* z);
	BOOL IsIgnored();
	BOOL IsStationary();
	BOOL GetCollisionFlag();
	void SetCollisionFlag(BOOL bEnable);
	RwObject* GetRwObject();
	void DeleteRwObject();
	void UpdateRwFrame();
	float GetDistanceToLocalPlayerNoHeight();
	void SetCollisionProcessed(BOOL bProcessed);
	void ApplyTurnForce(CVectorSA direction, CVectorSA velocity);
	void SetFromEuler(CVectorSA angles);
	void SetClumpAlpha(int nValue);

};

SAMP_END

#elif defined(SAMP_R3)

#include "CVectorSA.h"
#include "CMatrixSA.h"

class CEntity;
struct RwObject;

SAMP_BEGIN

class SAMP_API CEntitySA {
public:

	// void **m_lpVtbl = samp.dll+0xEC28C;

	char pad_4[60];
	::CEntity*	m_pGameEntity;
	GTAREF		m_handle;

	CEntitySA();

	virtual ~CEntitySA() SAMP_VIRTUAL;
	virtual void Add() SAMP_VIRTUAL; // adds entity to the game world
	virtual void Remove() SAMP_VIRTUAL; // removes entity from the game world

	void GetMatrix(CMatrixSA* pMatrix);
	void SetMatrix(CMatrixSA matrix);
	void GetSpeed(CVectorSA* pVec);
	void SetSpeed(CVectorSA vec);
	void GetTurnSpeed(CVectorSA* pVec);
	void SetTurnSpeed(CVectorSA vec);
	void ApplyTurnSpeed();
	float GetDistanceFromCentreOfMassToBaseOfModel();
	void GetBoundCentre(CVectorSA* pVec);
	void SetModelIndex(int nModel);
	int GetModelIndex();
	void Teleport(CVectorSA position);
	float GetDistanceToLocalPlayer();
	float GetDistanceToCamera();
	float GetDistanceToPoint(CVectorSA position);
	BOOL DoesExist(); // does entity exist in the game world?
	BOOL EnforceWorldBoundries(float fPX, float fZX, float fPY, float fNY);
	BOOL HasExceededWorldBoundries(float fPX, float fZX, float fPY, float fNY);
	void GetEulerInverted(float* x, float* y, float* z);
	BOOL IsIgnored();
	BOOL IsStationary();
	BOOL GetCollisionFlag();
	void SetCollisionFlag(BOOL bEnable);
	RwObject* GetRwObject();
	void DeleteRwObject();
	void UpdateRwFrame();
	float GetDistanceToLocalPlayerNoHeight();
	void SetCollisionProcessed(BOOL bProcessed);

};

SAMP_END

#endif
