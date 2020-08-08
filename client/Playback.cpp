/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Playback.h"

#include <audio/bass_fx.h>
#include <game/CCamera.h>
#include <util/Logger.h>

#include "PluginConfig.h"
#include "Header.h"

#pragma comment(lib, "bass.lib")
#pragma comment(lib, "bass_fx.lib")

bool Playback::initStatus(false);
bool Playback::loadStatus(false);

HSTREAM Playback::deviceOutputChannel(NULL);

Memory::CallHookPtr Playback::bassInitHook(nullptr);

BOOL WINAPI Playback::BassInitHookFunc(
	int device, DWORD freq, DWORD flags,
	HWND win, const GUID* dsguid
) {

	Logger::LogToFile("[sv:dbg:playback:bassinithook] : module loading...");

	BASS_SetConfig(BASS_CONFIG_UNICODE, TRUE);
	BASS_SetConfig(BASS_CONFIG_BUFFER, SV::ChannelBufferSizeInMs);
	BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD, SV::AudioUpdatePeriod);
	BASS_SetConfig(BASS_CONFIG_UPDATETHREADS, SV::AudioUpdateThreads);
	BASS_SetConfig(BASS_CONFIG_3DALGORITHM, BASS_3DALG_LIGHT);

	Logger::LogToFile(
		"[sv:dbg:playback:bassinithook] : hooked function BASS_Init(device:%d, freq:%u, flags:0x%x, win:0x%x, dsguid:0x%x)...",
		device, freq, flags, win, dsguid
	);

	Logger::LogToFile(
		"[sv:dbg:playback:bassinithook] : calling function BASS_Init(device:%d, freq:%u, flags:0x%x, win:0x%x, dsguid:0x%x)...",
		device, SV::Frequency, BASS_DEVICE_MONO | BASS_DEVICE_3D | flags, win, dsguid
	);

	if (!BASS_Init(device, SV::Frequency, BASS_DEVICE_MONO | BASS_DEVICE_3D | flags, win, dsguid)) {

		Logger::LogToFile("[sv:err:playback:bassinithook] : failed to init bass library (code:%d)", BASS_ErrorGetCode());
		return FALSE;

	}

	if (HIWORD(BASS_FX_GetVersion()) != BASSVERSION) {

		Logger::LogToFile("[sv:err:playback:init] : failed to check version bassfx library (code:%d)", BASS_ErrorGetCode());
		return FALSE;

	}

	if (!(Playback::deviceOutputChannel = BASS_StreamCreate(0, 0, NULL, STREAMPROC_DEVICE, nullptr))) {

		Logger::LogToFile("[sv:err:playback:init] : failed to create device output channel (code:%d)", BASS_ErrorGetCode());
		return FALSE;

	}

	BASS_Set3DFactors(1, 1, 0);
	BASS_Set3DPosition(
		&BASS_3DVECTOR(0, 0, 0),
		&BASS_3DVECTOR(0, 0, 0),
		&BASS_3DVECTOR(0, 0, 0),
		&BASS_3DVECTOR(0, 0, 0)
	); BASS_Apply3D();

	Logger::LogToFile("[sv:dbg:playback:bassinithook] : module loaded");

	Playback::loadStatus = true;

	Playback::SyncConfigs();

	return TRUE;

}

bool Playback::Init(const AddressesBase& addrBase) {

	if (Playback::initStatus) return false;

	Logger::LogToFile("[sv:dbg:playback:init] : module initializing...");

	Memory::FillWithNops(addrBase.bassSetConfigAddr, 8);

	if (!(Playback::bassInitHook = MakeCallHook(addrBase.bassInitCallAddr, &Playback::BassInitHookFunc))) {

		Logger::LogToFile("[sv:err:playback:init] : failed to create 'BASS_Init' function hook");
		return false;

	}

	if (!PluginConfig::IsPlaybackLoaded()) {

		PluginConfig::SetPlaybackLoaded(true);
		Playback::ResetConfigs();

	}

	Logger::LogToFile("[sv:dbg:playback:init] : module initialized");

	return Playback::initStatus = true;

}

bool Playback::GetSoundEnable() {

	return PluginConfig::GetSoundEnable();

}

int Playback::GetSoundVolume() {

	return PluginConfig::GetSoundVolume();

}

bool Playback::GetSoundBalancer() {

	return PluginConfig::GetSoundBalancer();

}

bool Playback::GetSoundFilter() {

	return PluginConfig::GetSoundFilter();

}

void Playback::SetSoundEnable(bool soundEnable) {

	if (!Playback::loadStatus) return;

	PluginConfig::SetSoundEnable(soundEnable);

	if (!PluginConfig::GetSoundEnable()) BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 0);
	else BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 100 * PluginConfig::GetSoundVolume());

}

void Playback::SetSoundVolume(int soundVolume) {

	if (!Playback::loadStatus) return;

	if (soundVolume < 0) soundVolume = 0;
	if (soundVolume > 100) soundVolume = 100;

	PluginConfig::SetSoundVolume(soundVolume);

	if (PluginConfig::GetSoundEnable()) BASS_SetConfig(BASS_CONFIG_GVOL_STREAM,
		100 * PluginConfig::GetSoundVolume());

}

void Playback::SetSoundBalancer(bool soundBalancer) {

	static HFX balancerFxHandle = NULL;

	if (!Playback::loadStatus) return;

	PluginConfig::SetSoundBalancer(soundBalancer);

	if (PluginConfig::GetSoundBalancer() && !balancerFxHandle) {

		BASS_BFX_COMPRESSOR2 balancerParameters = {};

		balancerParameters.lChannel = BASS_BFX_CHANALL;

		balancerParameters.fGain = 10;
		balancerParameters.fAttack = 0.01f;
		balancerParameters.fRelease = 0.01f;
		balancerParameters.fThreshold = -40;
		balancerParameters.fRatio = 12;

		if (!(balancerFxHandle = BASS_ChannelSetFX(Playback::deviceOutputChannel, BASS_FX_BFX_COMPRESSOR2, 1))) {

			Logger::LogToFile("[sv:err:playback:setsoundbalancer] : failed to set balancer effect (code:%d)", BASS_ErrorGetCode());

			PluginConfig::SetSoundBalancer(false);

			return;

		}

		BASS_FXSetParameters(balancerFxHandle, &balancerParameters);

	} else if (!PluginConfig::GetSoundBalancer() && balancerFxHandle) {

		BASS_ChannelRemoveFX(Playback::deviceOutputChannel, balancerFxHandle);
		balancerFxHandle = NULL;

	}

}

void Playback::SetSoundFilter(bool soundFilter) {

	static HFX filterFxHandle = NULL;

	if (!Playback::loadStatus) return;

	PluginConfig::SetSoundFilter(soundFilter);

	if (PluginConfig::GetSoundFilter() && !filterFxHandle) {

		BASS_BFX_BQF parameqParameters = {};

		parameqParameters.lFilter = BASS_BFX_BQF_HIGHSHELF;
		parameqParameters.lChannel = BASS_BFX_CHANALL;

		parameqParameters.fCenter = 100;
		parameqParameters.fQ = 0.7f;

		if (!(filterFxHandle = BASS_ChannelSetFX(Playback::deviceOutputChannel, BASS_FX_BFX_BQF, 1))) {

			Logger::LogToFile("[sv:err:playback:setsoundfilter] : failed to set filter effect (code:%d)", BASS_ErrorGetCode());

			PluginConfig::SetSoundFilter(false);

			return;

		}

		BASS_FXSetParameters(filterFxHandle, &parameqParameters);

	} else if (!PluginConfig::GetSoundFilter() && filterFxHandle) {

		BASS_ChannelRemoveFX(Playback::deviceOutputChannel, filterFxHandle);
		filterFxHandle = NULL;

	}

}

void Playback::SyncConfigs() {

	Playback::SetSoundEnable(PluginConfig::GetSoundEnable());
	Playback::SetSoundVolume(PluginConfig::GetSoundVolume());
	Playback::SetSoundBalancer(PluginConfig::GetSoundBalancer());
	Playback::SetSoundFilter(PluginConfig::GetSoundFilter());

}

void Playback::ResetConfigs() {

	PluginConfig::SetSoundEnable(PluginConfig::DefVal_SoundEnable);
	PluginConfig::SetSoundVolume(PluginConfig::DefVal_SoundVolume);
	PluginConfig::SetSoundBalancer(PluginConfig::DefVal_SoundBalancer);
	PluginConfig::SetSoundFilter(PluginConfig::DefVal_SoundFilter);

}

void Playback::Update() {

	if (!Playback::loadStatus) return;

	BASS_Set3DPosition(
		(BASS_3DVECTOR*)(&TheCamera.GetPosition()), nullptr,
		(BASS_3DVECTOR*)(&TheCamera.GetMatrix()->at),
		(BASS_3DVECTOR*)(&TheCamera.GetMatrix()->up)
	);

	BASS_Apply3D();

}

void Playback::Free() {

	if (!Playback::initStatus) return;

	Logger::LogToFile("[sv:dbg:playback:free] : module releasing...");

	Playback::bassInitHook.reset();

	Playback::loadStatus = false;

	Logger::LogToFile("[sv:dbg:playback:free] : module released");

	Playback::initStatus = false;

}
