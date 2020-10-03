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

    Record() = delete;
    ~Record() = delete;
    Record(const Record&) = delete;
    Record(Record&&) = delete;
    Record& operator=(const Record&) = delete;
    Record& operator=(Record&&) = delete;

public:

    static bool Init(DWORD bitrate);
    static void Free() noexcept;

    static void Tick() noexcept;

    static DWORD GetFrame(BYTE* bufferPtr, DWORD bufferSize) noexcept;

    static bool HasMicro() noexcept;

    static bool StartRecording() noexcept;
    static bool IsRecording() noexcept;
    static void StopRecording() noexcept;

    static bool GetMicroEnable() noexcept;
    static int GetMicroVolume() noexcept;
    static int GetMicroDevice() noexcept;

    static void SetMicroEnable(bool microEnable) noexcept;
    static void SetMicroVolume(int microVolume) noexcept;
    static void SetMicroDevice(int deviceIndex) noexcept;

    static void SyncConfigs() noexcept;
    static void ResetConfigs() noexcept;

    static const std::vector<std::string>& GetDeviceNamesList() noexcept;
    static const std::vector<int>& GetDeviceNumbersList() noexcept;

    static bool StartChecking() noexcept;
    static bool IsChecking() noexcept;
    static void StopChecking() noexcept;

private:

    static bool initStatus;
    static bool checkStatus;
    static bool recordStatus;

    static HRECORD recordChannel;
    static OpusEncoder* encoder;

    static opus_int16 encBuffer[SV::kFrameSizeInSamples];

    static HSTREAM checkChannel;

    static int usedDeviceIndex;
    static std::vector<std::string> deviceNamesList;
    static std::vector<int> deviceNumbersList;

};
