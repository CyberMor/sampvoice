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

class SAMP_API CSrvNetStats {
public:

	unsigned long		m_dwLastTotalBytesSent;
	unsigned long		m_dwLastTotalBytesRecv;
	unsigned long		m_dwLastUpdateTick;
	unsigned long		m_dwBPSUpload;
	unsigned long		m_dwBPSDownload;
	IDirect3DDevice9*	m_pDevice;

	CSrvNetStats(IDirect3DDevice9* pDevice);

	void Draw();

};

CSrvNetStats*& pServerNetStatistics();

SAMP_END

#endif
