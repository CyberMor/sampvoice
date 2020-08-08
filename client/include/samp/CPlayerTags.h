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

SAMP_BEGIN

class SAMP_API CPlayerTags {
public:

	IDirect3DDevice9*		m_pDevice;
	IDirect3DStateBlock9*	m_pStates;
	ID3DXSprite*			m_pSprite;

	CPlayerTags(IDirect3DDevice9* pDevice);
	~CPlayerTags();

	void EndHealthBar();
	void BeginLabel();
	void EndLabel();
	void DrawLabel(CVectorSA* pPosition, const char* szText, D3DCOLOR color, float fDistanceToCamera, bool bDrawStatus, int nStatus);
	void DrawHealthBar(CVectorSA* pPosition, float fHealth, float fArmour, float fDistanceToCamera);
	void OnLostDevice();
	void OnResetDevice();
	void BeginHealthBar();

};

CPlayerTags*& pPlayerTags();

SAMP_END

#elif defined(SAMP_R3)

#include "CVectorSA.h"

SAMP_BEGIN

class SAMP_API CPlayerTags {
public:

	IDirect3DDevice9*		m_pDevice;
	IDirect3DStateBlock9*	m_pStates;
	ID3DXSprite*			m_pSprite;

	CPlayerTags(IDirect3DDevice9* pDevice);
	~CPlayerTags();

	void EndHealthBar();
	void BeginLabel();
	void EndLabel();
	void DrawLabel(CVectorSA* pPosition, const char* szText, D3DCOLOR color, float fDistanceToCamera, bool bDrawStatus, int nStatus);
	void DrawHealthBar(CVectorSA* pPosition, float fHealth, float fArmour, float fDistanceToCamera);
	void OnLostDevice();
	void OnResetDevice();
	void BeginHealthBar();

};

CPlayerTags*& pPlayerTags();

SAMP_END

#endif
