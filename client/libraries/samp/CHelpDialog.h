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

class SAMP_API CHelpDialog {
public:

	IDirect3DDevice9 *m_pDevice;

	CHelpDialog(IDirect3DDevice9 *pDevice = SAMP_UNUSED);
	
	void Show();

};

CHelpDialog*& pHelpDialog();

SAMP_END

#endif
