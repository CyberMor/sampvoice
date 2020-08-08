/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>

#include <d3d9.h>

class PluginConfig {
public:

	/*
		All settings related to the screen are stored in the base resolution (640x480) and,
		upon receipt of the value, are converted to the screen resolution by the CRender module
	*/

	static constexpr int ConfigVersion = 2;

	static constexpr bool DefVal_SoundEnable = true;
	static constexpr int DefVal_SoundVolume = 100;
	static constexpr bool DefVal_SoundBalancer = false;
	static constexpr bool DefVal_SoundFilter = false;

	static constexpr float DefVal_SpeakerIconScale = 1.f;
	static constexpr int DefVal_SpeakerIconOffsetX = 0;
	static constexpr int DefVal_SpeakerIconOffsetY = 0;

	static constexpr bool DefVal_MicroEnable = true;
	static constexpr int DefVal_MicroVolume = 75;

	static constexpr float DefVal_MicroIconScale = 1.f;
	static constexpr int DefVal_MicroIconPositionX = -100;
	static constexpr int DefVal_MicroIconPositionY = -100;
	static constexpr D3DCOLOR DefVal_MicroIconColor = 0x00ffffff;
	static constexpr float DefVal_MicroIconAngle = 0.f;

private:

	static bool loadStatus;

	static bool playbackLoaded;
	static bool speakerLoaded;
	static bool recordLoaded;
	static bool microLoaded;

	static bool soundEnable;
	static int soundVolume;
	static bool soundBalancer;
	static bool soundFilter;

	static float speakerIconScale;
	static int speakerIconOffsetX;
	static int speakerIconOffsetY;

	static bool microEnable;
	static int microVolume;
	static std::string deviceName;

	static float microIconScale;
	static int microIconPositionX;
	static int microIconPositionY;
	static D3DCOLOR microIconColor;
	static float microIconAngle;

public:

	static bool Load(const std::string& configFilePath);
	static bool Save(const std::string& configFilePath);

	static void Reset();
	static bool IsLoaded();

	static bool IsPlaybackLoaded();
	static bool IsSpeakerLoaded();
	static bool IsRecordLoaded();
	static bool IsMicroLoaded();

	static void SetPlaybackLoaded(const bool status);
	static void SetSpeakerLoaded(const bool status);
	static void SetRecordLoaded(const bool status);
	static void SetMicroLoaded(const bool status);

	static bool GetSoundEnable();
	static int GetSoundVolume();
	static bool GetSoundBalancer();
	static bool GetSoundFilter();

	static float GetSpeakerIconScale();
	static int GetSpeakerIconOffsetX();
	static int GetSpeakerIconOffsetY();

	static bool GetMicroEnable();
	static int GetMicroVolume();
	static const std::string& GetDeviceName();

	static float GetMicroIconScale();
	static int GetMicroIconPositionX();
	static int GetMicroIconPositionY();
	static D3DCOLOR GetMicroIconColor();
	static float GetMicroIconAngle();

	static void SetSoundEnable(const bool soundEnable);
	static void SetSoundVolume(const int soundVolume);
	static void SetSoundBalancer(const bool soundBalancer);
	static void SetSoundFilter(const bool soundFilter);

	static void SetSpeakerIconScale(const float speakerIconScale);
	static void SetSpeakerIconOffsetX(const int speakerIconOffsetX);
	static void SetSpeakerIconOffsetY(const int speakerIconOffsetY);

	static void SetMicroEnable(const bool microEnable);
	static void SetMicroVolume(const int microVolume);
	static void SetDeviceName(const std::string& deviceName);

	static void SetMicroIconScale(const float microIconScale);
	static void SetMicroIconPositionX(const int microIconPositionX);
	static void SetMicroIconPositionY(const int microIconPositionY);
	static void SetMicroIconColor(const D3DCOLOR microIconColor);
	static void SetMicroIconAngle(const float microIconAngle);

};
