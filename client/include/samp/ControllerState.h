/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

SAMP_BEGIN

struct SAMP_API ControllerState {

	short					m_sLeftStickX; // move/steer left = -128, right = 128
	short					m_sLeftStickY; // move back = 128, forwards = -128

	union {

		struct {

			unsigned char	m_bLeftShoulder1 : 1; // fire weapon alt
			unsigned char	m_bShockButtonL : 1; // crouch
			unsigned char	m_bButtonCircle : 1; // fire weapon
			unsigned char	m_bButtonCross : 1; // sprint, accelerate
			unsigned char	m_bButtonTriangle : 1; // enter/exit vehicle
			unsigned char	m_bButtonSquare : 1; // jump, reverse
			unsigned char	m_bRightShoulder2 : 1; // look right (incar)
			unsigned char	m_bRightShoulder1 : 1; // hand brake, target

			unsigned char	m_bLeftShoulder2 : 1; // look left
			unsigned char	m_bShockButtonR : 1; // look behind
			unsigned char	m_bPedWalk : 1; // walking
			unsigned char	m_bRightStickDown : 1;
			unsigned char	m_bRightStickUp : 1;
			unsigned char	m_bRightStickRight : 1; // num 4
			unsigned char	m_bRightStickLeft : 1; // num 6

			// 16th bit is unused

		};

		short				m_value;

	};

	ControllerState() {

		m_sLeftStickX = 0;
		m_sLeftStickY = 0;
		m_value = 0;

	}

};

SAMP_END
