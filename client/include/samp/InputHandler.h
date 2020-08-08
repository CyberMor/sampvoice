/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

SAMP_BEGIN_NP

void SwitchWindowedMode();

namespace InputHandler {

	void*& prevWindowProc();

	int WindowProc(unsigned int uMsg, unsigned int wParam, long lParam);
	BOOL KeyPressHandler(unsigned int nKey);
	BOOL CharInputHandler(unsigned int nChar);
	BOOL Initialize();

}

SAMP_END_NP
