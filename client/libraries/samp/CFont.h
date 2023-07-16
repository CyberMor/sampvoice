/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R3)

/*
	this class handles RGB-formatted tags in ANSI text
	like a "{FFFFFF}Hello {FF5000}World!"
*/

SAMP_BEGIN

#if defined(__D3DX9CORE_H__)

class SAMP_API CFont : ID3DXFont {
public:

	// void **m_lpVtbl = 0xEA3B8;

	ID3DXFont* m_pFont;

	CFont();
	CFont(ID3DXFont* pFont);

	// IUnknown
	STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID* ppv) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;

	// ID3DXFont
	STDMETHOD(GetDevice)(THIS_ LPDIRECT3DDEVICE9* ppDevice) PURE;
	STDMETHOD(GetDescA)(THIS_ D3DXFONT_DESCA* pDesc) PURE;
	STDMETHOD(GetDescW)(THIS_ D3DXFONT_DESCW* pDesc) PURE;
	STDMETHOD_(BOOL, GetTextMetricsA)(THIS_ TEXTMETRICA* pTextMetrics) PURE;
	STDMETHOD_(BOOL, GetTextMetricsW)(THIS_ TEXTMETRICW* pTextMetrics) PURE;

	STDMETHOD_(HDC, GetDC)(THIS) PURE;
	STDMETHOD(GetGlyphData)(THIS_ UINT Glyph, LPDIRECT3DTEXTURE9* ppTexture, RECT* pBlackBox, POINT* pCellInc) PURE;

	STDMETHOD(PreloadCharacters)(THIS_ UINT First, UINT Last) PURE;
	STDMETHOD(PreloadGlyphs)(THIS_ UINT First, UINT Last) PURE;
	STDMETHOD(PreloadTextA)(THIS_ LPCSTR pString, INT Count) PURE;
	STDMETHOD(PreloadTextW)(THIS_ LPCWSTR pString, INT Count) PURE;

	STDMETHOD_(INT, DrawTextA)(THIS_ LPD3DXSPRITE pSprite, LPCSTR pString, INT Count, LPRECT pRect, DWORD Format, D3DCOLOR Color) PURE;
	STDMETHOD_(INT, DrawTextW)(THIS_ LPD3DXSPRITE pSprite, LPCWSTR pString, INT Count, LPRECT pRect, DWORD Format, D3DCOLOR Color) PURE;

	STDMETHOD(OnLostDevice)(THIS) PURE;
	STDMETHOD(OnResetDevice)(THIS) PURE;

};

#else

class SAMP_API CFont {
public:

	void **m_lpVtbl;
	ID3DXFont *m_pFont;

	CFont();
	CFont(ID3DXFont *pFont);

};

#endif

SAMP_END

#endif
