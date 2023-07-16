/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#include "CEntitySA.h"
#include "CVectorSA.h"

class CPed;

SAMP_BEGIN

class SAMP_API CActor : public CEntitySA {
public:

	// void **lpVtbl = 0xD9EC8;

	::CPed*	m_pGamePed;
	GTAREF	m_marker;
	GTAREF	m_arrow;
	bool	m_bNeedsToCreateMarker;
	bool	m_bInvulnerable;

	CActor() { *(void**)this = (void*)SAMP_ADDROF(0xD9EC8); }
	CActor(int nSkin, CVectorSA vPos, float fRotation);

	virtual ~CActor() {}

	void Destroy();
	void PerformAnimation(const char* szAnim, const char* szIFP, float fFramedelta, int bLockA, int bLockX, int bLockY, int bLockF, int nTime);
	void SetRotation(float fValue);
	void SetHealth(float fValue);
	float GetHealth();
	void SetInvulnerable(bool bInv);
	void SetArmour(float fValue);
	float GetArmour();
	// state/status is a flags
	void SetState(int nValue);
	int GetState();
	BOOL IsDead();
	void SetStatus(int nValue);
	int GetStatus();

};

SAMP_END

#elif defined(SAMP_R3)

#include "CEntitySA.h"

class CPed;

SAMP_BEGIN

class SAMP_API CActor : public CEntitySA {
public:

	// void **m_lpVtbl = 0xEC298;

	::CPed*	m_pGamePed;
	GTAREF	m_marker;
	GTAREF	m_arrow;
	bool	m_bNeedsToCreateMarker;
	bool	m_bInvulnerable;

	virtual ~CActor() SAMP_VIRTUAL;

	CActor() { *(void**)this = (void*)SAMP_ADDROF(0xEC298); }
	CActor(int nModel, CVectorSA position, float fRotation);

	void Destroy();
	void PerformAnimation(const char* szAnim, const char* szIFP, float fFramedelta, int bLockA, int bLockX, int bLockY, int bLockF, int nTime);
	void SetRotation(float fAngle);
	float GetHealth();
	void SetHealth(float fAngle);
	void SetInvulnerable(bool bEnable);

};

SAMP_END

#endif
