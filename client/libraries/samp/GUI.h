/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

class CDXUTControl;
class CDXUTDialogResourceManager;

SAMP_BEGIN

namespace GUI {

	CDXUTDialogResourceManager*&	pResourceMgr();

	CDXUTDialog*&					pGameUi();
	CDXUTDialog*&					pScoreboard();
	CDXUTDialog*&					pDialog();
	CDXUTDialog*&					pClassSelection();

	IDirect3DSurface9*&				pCursor();
	IDirect3DDevice9*&				pDevice();

	void Initialize();

	void OnLostDevice();
	void OnResetDevice();

	void GameUIEventHandler(unsigned int nEvent, int nControlId, CDXUTControl* pControl, void* pUserContext);
	void ScoreboardEventHandler(unsigned int nEvent, int nControlId, CDXUTControl* pControl, void* pUserContext);
	void DialogEventHandler(unsigned int nEvent, int nControlId, CDXUTControl* pControl, void* pUserContext);
	void ClassSelectorEventHandler(unsigned int nEvent, int nControlId, CDXUTControl* pControl, void* pUserContext);

}

SAMP_END

#elif defined(SAMP_R3)

class CDXUTControl;
class CDXUTDialogResourceManager;

SAMP_BEGIN

namespace GUI {

	CDXUTDialogResourceManager*&	pResourceMgr();

	CDXUTDialog*&					pGameUi();
	CDXUTDialog*&					pScoreboard();
	CDXUTDialog*&					pDialog();
	CDXUTDialog*&					pClassSelection();

	IDirect3DSurface9*&				pCursor();
	IDirect3DDevice9*&				pDevice();

	void Initialize();

	void OnLostDevice();
	void OnResetDevice();

	void GameUIEventHandler(unsigned int nEvent, int nControlId, CDXUTControl* pControl, void* pUserContext);
	void ScoreboardEventHandler(unsigned int nEvent, int nControlId, CDXUTControl* pControl, void* pUserContext);
	void DialogEventHandler(unsigned int nEvent, int nControlId, CDXUTControl* pControl, void* pUserContext);
	void ClassSelectionEventHandler(unsigned int nEvent, int nControlId, CDXUTControl* pControl, void* pUserContext);

}

SAMP_END

#endif
