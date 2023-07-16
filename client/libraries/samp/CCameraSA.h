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
#include "CMatrixSA.h"

class CEntity;

SAMP_BEGIN

class SAMP_API CCameraSA {
public:

	CEntitySA* m_pAttachedTo;
	CMatrixSA* m_pMatrix;

	CCameraSA();
	~CCameraSA() { Detach(); }

	void Fade(BOOL bin);
	void GetMatrix(CMatrixSA* pMatrix);
	void SetMatrix(CMatrixSA matrix);
	void TakeControl(::CEntity* pTarget, short nModeToGoTo, short nTypeOfSwitch);
	void SetMoveVector(CVectorSA* pCamera, CVectorSA* pPosition, int nTime, bool bSmoothTransmition);
	void SetTrackVector(CVectorSA* pPointAt, CVectorSA* pTransverseTo, int nTime, bool bSmooth);
	void Attach(CEntitySA* pOwner);
	void SetToOwner();
	float GetDistanceToPoint(CVectorSA* pPoint);
	void Restore();
	void Set(CVectorSA position, CVectorSA rotation);
	void PointAt(CVectorSA point, int nSwitchStyle);
	void Detach();

};

SAMP_END

#elif defined(SAMP_R3)

#include "CVectorSA.h"
#include "CMatrixSA.h"
#include "CEntitySA.h"

class CEntity;

SAMP_BEGIN

class SAMP_API CCameraSA {
public:

	CEntitySA* m_pOwner;
	CMatrixSA* m_pMatrix;

	CCameraSA();
	~CCameraSA();

	void Fade(BOOL bin);
	void GetMatrix(CMatrixSA* pMatrix);
	void SetMatrix(CMatrixSA matrix);
	void TakeControl(::CEntity* pTarget, short nModeToGoTo, short nTypeOfSwitch);
	void SetMoveVector(CVectorSA* pCamera, CVectorSA* pPosition, int nTime, bool bSmoothTransmition);
	void SetTrackVector(CVectorSA* pPointAt, CVectorSA* pTransverseTo, int nTime, bool bSmooth);
	void Attach(CEntitySA* pOwner);
	void SetToOwner();
	float GetDistanceToPoint(CVectorSA* pPoint);
	void Restore();
	void Set(CVectorSA position, CVectorSA rotation);
	void PointAt(CVectorSA point, int nSwitchStyle);
	void Detach();

};

SAMP_END

#endif
