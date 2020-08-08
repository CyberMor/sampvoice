/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R3)

SAMP_BEGIN

class SAMP_API CObjectMaterialText {
public:

	IDirect3DDevice9*	m_pDevice;
	ID3DXSprite*		m_pSprite;
	ID3DXSprite*		m_pSprite_0; // maybe unused

	CObjectMaterialText(IDirect3DDevice9* pDevice);
	~CObjectMaterialText();

	void OnResetDevice();
	void OnLostDevice();
	IDirect3DTexture9* Create(const char* szText, const char* szFont, char nFontSize, int nBgSizeX, int nBgSizeY, D3DCOLOR fontColor, D3DCOLOR bgColor, bool bBold, char align);

};

CObjectMaterialText*& pObjectMaterialTextManager();

SAMP_END

#endif
