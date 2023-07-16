/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

SAMP_BEGIN_NP

typedef void(__cdecl *CMDPROC)(const char *);

namespace Commands {

	CMDPROC Default();
	CMDPROC TestDeathWindow();
	CMDPROC ToggleCameraTargetLabels();
	CMDPROC SetChatPageSize();
	CMDPROC SetChatFontSize();
	CMDPROC DrawNameTagStatus();
	CMDPROC DrawChatTimestamps();
	CMDPROC ToggleAudioStreamMessages();
	CMDPROC PrintMemory();
	CMDPROC SetFrameLimiter();
	CMDPROC ToggleHeadMoves();
	CMDPROC Quit();
	CMDPROC CmpStat();
	CMDPROC SavePosition();
	CMDPROC SavePositionOnly();
	CMDPROC PrintCurrentInterior();
	CMDPROC ToggleObjectsLight();
	CMDPROC ToggleDebugLabels();
	CMDPROC SendRconCommand();

	namespace Debug {

		CMDPROC SetPlayerSkin();
		CMDPROC CreateVehicle();
		CMDPROC EnableVehicleSelection();
		CMDPROC SetWorldWeather();
		CMDPROC SetWorldTime();

	}

	void Setup();

}

SAMP_END_NP

#elif defined(SAMP_R3)

SAMP_BEGIN

typedef void(__cdecl* CMDPROC)(const char*);

namespace Commands {

	CMDPROC Default();
	CMDPROC TestDeathWindow();
	CMDPROC ToggleCameraTargetLabels();
	CMDPROC SetChatPageSize();
	CMDPROC SetChatFontSize();
	CMDPROC DrawNameTagStatus();
	CMDPROC DrawChatTimestamps();
	CMDPROC ToggleAudioStreamMessages();
	CMDPROC ToggleURLMessages();
	CMDPROC ToggleHUDScaleFix();
	CMDPROC PrintMemory();
	CMDPROC SetFrameLimiter();
	CMDPROC ToggleHeadMoves();
	CMDPROC Quit();
	CMDPROC CmpStat();
	CMDPROC SavePosition();
	CMDPROC SavePositionOnly();
	CMDPROC PrintCurrentInterior();
	CMDPROC ToggleObjectsLight();
	CMDPROC ToggleDebugLabels();
	CMDPROC SendRconCommand();

	namespace Debug {

		CMDPROC SetPlayerSkin();
		CMDPROC CreateVehicle();
		CMDPROC EnableVehicleSelection();
		CMDPROC SetWorldWeather();
		CMDPROC SetWorldTime();

	}

	void Setup();

}

SAMP_END

#endif
