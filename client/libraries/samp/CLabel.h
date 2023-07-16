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

class SAMP_API CLabel {
public:

	IDirect3DDevice9*	m_pDevice;
	ID3DXSprite*		m_pSprite;

	CLabel(IDirect3DDevice9* pDevice);
	~CLabel();

	void OnLostDevice();
	void OnResetDevice();
	BOOL HasNoObstacles(CVectorSA position);
	void Begin();
	void End();
	void Draw(CVectorSA* pPosition, const char* szText, D3DCOLOR color, BOOL bShadow, bool bNoObstacles);

};

CLabel*& pLabel();

SAMP_END

#elif defined(SAMP_R3)

#include "CVectorSA.h"

SAMP_BEGIN

class SAMP_API CLabel {
public:

	IDirect3DDevice9*	m_pDevice;
	ID3DXSprite*		m_pSprite;

	CLabel(IDirect3DDevice9* pDevice);
	~CLabel();

	void OnLostDevice();
	void OnResetDevice();
	BOOL HasNoObstacles(CVectorSA position);
	void Begin();
	void End();
	void Draw(CVectorSA* pPosition, const char* szText, D3DCOLOR color, BOOL bShadow, bool bNoObstacles);

};

CLabel*& pLabel();

SAMP_END

#endif
