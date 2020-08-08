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

SAMP_BEGIN_NP

namespace AimStuff {

	struct Aim {

		CVectorSA f1;
		CVectorSA pos1;
		CVectorSA pos2;
		CVectorSA f2;

	};

	float& fLocalPlayerAspectRatio();
	float*& pInternalCameraExtZoom();
	float*& pInternalAspectRatio();
	float* aCameraExtZoom();
	float* aAspectRatio();
	char* aCameraMode();
	char*& pInternalCameraMode();
	Aim& localPlayerAim();
	Aim* aPlayerAim();
	Aim*& pInternalAim();

	void UpdateCameraExtZoomAndAspectRatio();
	void ApplyCameraExtZoomAndAspectRatio();
	void SetCameraExtZoomAndAspectRatio(NUMBER nPlayer, float fCameraExtZoom, float fAspectRatio);
	float GetAspectRatio();
	float GetCameraExtZoom();
	void ApplyCameraExtZoomAndAspectRatio(NUMBER nPlayer);
	void SetCameraMode(char nMode, NUMBER nPlayer);
	char GetCameraMode(NUMBER nPlayer);
	char GetCameraMode();
	void Initialize();
	void UpdateAim();
	void ApplyAim();
	Aim* GetAim();
	void SetAim(int nPlayer, const Aim* pAim);
	void ApplyAim(int nPlayer);
	Aim* GetAim(int nPlayer);

}

SAMP_END_NP

#elif defined(SAMP_R3)

#include "CVectorSA.h"

SAMP_BEGIN

namespace AimStuff {

	struct Aim {

		CVectorSA f1;
		CVectorSA pos1;
		CVectorSA pos2;
		CVectorSA f2;

	};

	float& fLocalPlayerCameraExtZoom();
	float& fLocalPlayerAspectRatio();
	float*& pInternalCameraExtZoom();
	float*& pInternalAspectRatio();
	float* aCameraExtZoom();
	float* aAspectRatio();
	char* aCameraMode();
	char*& pInternalCameraMode();
	Aim& localPlayerAim();
	Aim* aPlayerAim();
	Aim*& pInternalAim();

	void UpdateCameraExtZoomAndAspectRatio();
	void ApplyCameraExtZoomAndAspectRatio();
	void SetCameraExtZoomAndAspectRatio(NUMBER nPlayer, float fCameraExtZoom, float fAspectRatio);
	float GetAspectRatio();
	float GetCameraExtZoom();
	void ApplyCameraExtZoomAndAspectRatio(NUMBER nPlayer);
	void SetCameraMode(char nMode, NUMBER nPlayer);
	char GetCameraMode(NUMBER nPlayer);
	char GetCameraMode();
	void Initialize();
	void UpdateAim();
	void ApplyAim();
	Aim* GetAim();
	void SetAim(int nPlayer, const Aim* pAim);
	void ApplyAim(int nPlayer);
	Aim* GetAim(int nPlayer);

}

SAMP_END

#endif
