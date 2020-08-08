/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

class CPad;

SAMP_BEGIN_NP

namespace KeyStuff {

	CPad*	pLocalPlayerKeys();
	CPad*	aPlayerKeys();
	CPad*&	pInternalKeys();
	bool*&	pDriveByLeft();
	bool*&	pDriveByRight();
	bool&	bSavedDriveByLeft();
	bool&	bSavedDriveByRight();

	void Initialize();
	void UpdateKeys();
	void ApplyKeys();
	void SetKeys(int nPlayer, const CPad* pKeys);
	void ApplyKeys(int nPlayer);
	CPad* GetInternalKeys();
	CPad* GetKeys();
	CPad* GetKeys(int nPlayer);
	void ResetKeys(int nPlayer);
	void ResetInternalKeys();

}

SAMP_END_NP

#elif defined(SAMP_R3)

class CPad;

SAMP_BEGIN

namespace KeyStuff {

	CPad*	pLocalPlayerKeys();
	CPad*	aPlayerKeys();
	CPad*&	pInternalKeys();
	bool*&	pDriveByLeft();
	bool*&	pDriveByRight();
	bool&	bSavedDriveByLeft();
	bool&	bSavedDriveByRight();

	void Initialize();
	void UpdateKeys();
	void ApplyKeys();
	void SetKeys(int nPlayer, const CPad* pKeys);
	void ApplyKeys(int nPlayer);
	CPad* GetInternalKeys();
	CPad* GetKeys();
	CPad* GetKeys(int nPlayer);
	void ResetKeys(int nPlayer);
	void ResetInternalKeys();

}

SAMP_END

#endif
