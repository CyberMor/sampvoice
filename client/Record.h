/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <vector>
#include <string>

#include <audio/bass.h>
#include <audio/opus.h>

#include "Header.h"

class Record {
private:

	static bool initStatus;
	static bool checkStatus;
	static bool recordStatus;

	static HRECORD recordChannel;
	static OpusEncoder* encoder;

	static opus_int16 encBuffer[SV::FrameSizeInSamples];

	static HSTREAM checkChannel;

	static int usedDeviceIndex;
	static std::vector<std::string> deviceNamesList;
	static std::vector<int> deviceNumbersList;

public:

	static bool Init(const DWORD bitrate);

	static void Tick();

	static DWORD GetFrame(BYTE* bufferPtr, DWORD bufferSize);

	static bool HasMicro();

	static bool StartRecording();
	static bool IsRecording();
	static void StopRecording();

	static bool GetMicroEnable();
	static int GetMicroVolume();
	static int GetMicroDevice();

	static void SetMicroEnable(bool microEnable);
	static void SetMicroVolume(int microVolume);
	static void SetMicroDevice(int deviceIndex);

	static void SyncConfigs();
	static void ResetConfigs();

	static const std::vector<std::string>& GetDeviceNamesList();
	static const std::vector<int>& GetDeviceNumbersList();

	static bool StartChecking();
	static bool IsChecking();
	static void StopChecking();

	static void Free();

};
