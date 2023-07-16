/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#include "CRectSA.h"

#undef DrawText

SAMP_BEGIN

class CFont;

class SAMP_API CFonts {
public:

	CFont*				m_pFont;
	CFont*				m_pLittleFont;
	CFont*				m_pShadow;
	CFont*				m_pLittleShadow;
	CFont*				m_pLicensePlateFont;
	ID3DXSprite*		m_pDefaultSprite;
	IDirect3DDevice9*	m_pDevice;
	char*				m_szTempBuffer;

	CFonts(IDirect3DDevice9* pDevice);
	~CFonts();

	void OnLostDevice();
	void OnResetDevice();
	void GetTextScreenSize(void* pSize, const char* szText, int nFormat);
	void GetLittleTextScreenSize(void* pSize, const char* szText, int nFormat);
	void DrawText(ID3DXSprite* pSprite, const char* szText, CRectSA rect, D3DCOLOR color, BOOL bShadow);
	void DrawLittleText(ID3DXSprite* pSprite, const char* szText, CRectSA rect, int nFormat, D3DCOLOR color, BOOL bShadow);
	void DrawLicensePlateText(const char* szText, CRectSA rect, D3DCOLOR color);
	void Reset();

};

CFonts*& pFontRenderer();

SAMP_END

#elif defined(SAMP_R3)

#include "CRectSA.h"
#include "CFont.h"

#undef DrawText

SAMP_BEGIN

class SAMP_API CFonts {
public:

	CFont*				m_pFont;
	CFont*				m_pLittleFont;
	CFont*				m_pShadow;
	CFont*				m_pLittleShadow;
	CFont*				m_pLicensePlateFont;
	ID3DXSprite*		m_pDefaultSprite;
	IDirect3DDevice9*	m_pDevice;
	char*				m_szTempBuffer;
	int					m_nCharHeight;
	int					m_nLittleCharHeight;

	CFonts(IDirect3DDevice9* pDevice);
	~CFonts();

	void OnLostDevice();
	void OnResetDevice();
	void GetTextScreenSize(void* pSize, const char* szText, int nFormat);
	void GetLittleTextScreenSize(void* pSize, const char* szText, int nFormat);
	void DrawText(ID3DXSprite* pSprite, const char* szText, CRectSA rect, D3DCOLOR color, BOOL bShadow);
	void DrawLittleText(ID3DXSprite* pSprite, const char* szText, CRectSA rect, int nFormat, D3DCOLOR color, BOOL bShadow);
	void DrawLicensePlateText(const char* szText, CRectSA rect, D3DCOLOR color);
	void Reset();

};

CFonts*& pFontRenderer();

SAMP_END

#endif
