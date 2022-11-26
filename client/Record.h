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

struct Record {

    Record() = delete;
    ~Record() = delete;
    Record(const Record&) = delete;
    Record(Record&&) = delete;
    Record& operator=(const Record&) = delete;
    Record& operator=(Record&&) = delete;

public:

    static bool Init(DWORD bitrate) noexcept;
    static void Free() noexcept;

    static void Tick() noexcept;
    static bool HasMicro() noexcept;

    static bool StartRecording() noexcept;
    static bool IsRecording() noexcept;
    static void StopRecording() noexcept;

    static bool StartChecking() noexcept;
    static bool IsChecking() noexcept;
    static void StopChecking() noexcept;

    static DWORD GetFrame(BYTE* buffer_ptr, DWORD buffer_size) noexcept;

    static bool GetMicroEnable() noexcept;
    static int GetMicroVolume() noexcept;
    static int GetMicroDevice() noexcept;

    static void SetMicroEnable(bool micro_enable) noexcept;
    static void SetMicroVolume(int micro_volume) noexcept;
    static void SetMicroDevice(int device_index) noexcept;

    static void SyncConfigs() noexcept;
    static void ResetConfigs() noexcept;

    static const std::vector<std::string>& GetDeviceNamesList() noexcept;
    static const std::vector<int>& GetDeviceNumbersList() noexcept;

private:

    static bool                     _init_status;

    static bool                     _check_status;
    static bool                     _record_status;

    static HRECORD                  _record_channel;
    static OpusEncoder*             _encoder;
    static opus_int16               _enc_buffer[SV::kFrameSizeInSamples];
    static HSTREAM                  _check_channel;

    static int                      _used_device_index;
    static std::vector<std::string> _device_names_list;
    static std::vector<int>         _device_numbers_list;

};
