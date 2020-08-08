/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CAudio.h"

#if defined(SAMP_R1)

void SAMP::CAudio::Play(int nSound, CVectorSA location) {
	((void(__thiscall*)(CAudio*, int, CVectorSA))SAMP_ADDROF(0x9D730))(this, nSound, location);
}

void SAMP::CAudio::StartRadio(unsigned char nStation) {
	((void(__thiscall*)(CAudio*, unsigned char))SAMP_ADDROF(0x9D860))(this, nStation);
}

float SAMP::CAudio::GetRadioVolume() {
	return ((float(__thiscall*)(CAudio*))SAMP_ADDROF(0x9D8A0))(this);
}

#elif defined(SAMP_R3)

int SAMP::CAudio::GetRadioStation() {
	return ((int(__thiscall*)(CAudio*))SAMP_ADDROF(0xA1AE0))(this);
}

void SAMP::CAudio::StartRadio(int nStation) {
	((void(__thiscall*)(CAudio*, int))SAMP_ADDROF(0xA1B10))(this, nStation);
}

void SAMP::CAudio::StopRadio() {
	((void(__thiscall*)(CAudio*))SAMP_ADDROF(0xA1B30))(this);
}

float SAMP::CAudio::GetRadioVolume() {
	return ((float(__thiscall*)(CAudio*))SAMP_ADDROF(0xA1B50))(this);
}

void SAMP::CAudio::StopOutdoorAmbienceTrack() {
	((void(__thiscall*)(CAudio*))SAMP_ADDROF(0xA1B60))(this);
}

void SAMP::CAudio::SetOutdoorAmbienceTrack(int nSound) {
	((void(__thiscall*)(CAudio*, int))SAMP_ADDROF(0xA1B70))(this, nSound);
}

bool SAMP::CAudio::IsOutdoorAmbienceTrackDisabled() {
	return ((bool(__thiscall*)(CAudio*))SAMP_ADDROF(0xA1C70))(this);
}

void SAMP::CAudio::Play(int nSound, CVectorSA location) {
	((void(__thiscall*)(CAudio*, int, CVectorSA))SAMP_ADDROF(0xA1B90))(this, nSound, location);
}

#endif
