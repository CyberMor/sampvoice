/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <atomic>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>

#include <audio/bass.h>
#include <audio/opus.h>

#include <execution/scope_exit.hpp>
#include <util/logger.hpp>

#include "plugin_config.hpp"
#include "main.hpp"

struct Speaker {

    Speaker(const Speaker&) = delete;
    Speaker(Speaker&&) = delete;
    Speaker& operator=(const Speaker&) = delete;
    Speaker& operator=(Speaker&&) = delete;

private:

    Speaker() noexcept = default;
    ~Speaker() noexcept = default;

public:

    static Speaker& Instance() noexcept
    {
        static Speaker instance;
        return instance;
    }

public:

    bool Initialize() noexcept
    {
        if (BASS_IsStarted() == FALSE)
            return false;

        Logger::Instance().LogToFile("[sv:dbg:speaker:initialize] module initializing...");

        {
            BASS_DEVICEINFO device_info;

            for (int device_number = 0; BASS_RecordGetDeviceInfo(device_number, &device_info) == TRUE; ++device_number)
            {
                const bool  device_enabled  = device_info.flags & BASS_DEVICE_ENABLED;
                const bool  device_loopback = device_info.flags & BASS_DEVICE_LOOPBACK;
                const DWORD device_type     = device_info.flags & BASS_DEVICE_TYPE_MASK;

                Logger::Instance().LogToFile("[sv:dbg:speaker:initialize] device detect "
                    "[ id(%d) enabled(%hhu) loopback(%hhu) name(%s) type(0x%X) ]",
                    device_number, device_enabled, device_loopback, device_info.name != nullptr ?
                    device_info.name : "none", device_type);

                if (device_enabled && !device_loopback && device_info.name != nullptr)
                    _devices.emplace_back(device_info.name, device_number);
            }
        }

        if (_devices.empty())
        {
            Logger::Instance().LogToFile("[sv:inf:speaker:initialize] failed to find microphones");
            return false;
        }

        ScopeExit devices_scope
        {
            [this]() noexcept -> void
            {
                _devices.clear();
            }
        };
        ScopeExit encoder_scope
        {
            [this]() noexcept -> void
            {
                opus_encoder_destroy(_encoder);
                _encoder = nullptr;
            }
        };

        {
            int error = -1;
            _encoder = opus_encoder_create(kFrequency, 1, OPUS_APPLICATION_VOIP, &error);
            if (_encoder == nullptr || error < 0)
            {
                Logger::Instance().LogToFile("[sv:err:speaker:initialize] failed to "
                    "create encoder (code:%d)", error);
                return false;
            }
        }
        if (const int error = opus_encoder_ctl(_encoder, OPUS_SET_COMPLEXITY(10)); error < 0)
        {
            Logger::Instance().LogToFile("[sv:err:speaker:initialize] failed to "
                "set complexity for encoder (code:%d)", error);
            return false;
        }
        if (const int error = opus_encoder_ctl(_encoder, OPUS_SET_BITRATE(OPUS_BITRATE_MAX)); error < 0)
        {
            Logger::Instance().LogToFile("[sv:err:speaker:initialize] failed to "
                "set bitrate for encoder (code:%d)", error);
            return false;
        }
        if (const int error = opus_encoder_ctl(_encoder, OPUS_SET_FORCE_CHANNELS(1)); error < 0)
        {
            Logger::Instance().LogToFile("[sv:err:speaker:initialize] failed to "
                "set count channels for encoder (code:%d)", error);
            return false;
        }
        if (const int error = opus_encoder_ctl(_encoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE)); error < 0)
        {
            Logger::Instance().LogToFile("[sv:err:speaker:initialize] failed to "
                "set signal type for encoder (code:%d)", error);
            return false;
        }
        if (const int error = opus_encoder_ctl(_encoder, OPUS_SET_INBAND_FEC(0)); error < 0)
        {
            Logger::Instance().LogToFile("[sv:err:speaker:initialize] failed to "
                "set inband fec for encoder (code:%d)", error);
            return false;
        }
        if (const int error = opus_encoder_ctl(_encoder, OPUS_SET_DTX(0)); error < 0)
        {
            Logger::Instance().LogToFile("[sv:err:speaker:initialize] failed to "
                "set dtx for encoder (code:%d)", error);
            return false;
        }
        if (const int error = opus_encoder_ctl(_encoder, OPUS_SET_LSB_DEPTH(16)); error < 0)
        {
            Logger::Instance().LogToFile("[sv:err:speaker:initialize] failed to "
                "set lsb depth for encoder (code:%d)", error);
            return false;
        }
        if (const int error = opus_encoder_ctl(_encoder, OPUS_SET_PREDICTION_DISABLED(TRUE)); error < 0)
        {
            Logger::Instance().LogToFile("[sv:err:speaker:initialize] failed to "
                "disable prediction for encoder (code:%d)", error);
            return false;
        }

        ScopeExit device_scope
        {
            [this]() noexcept -> void
            {
                _device = None<size_t>;
            }
        };
        ScopeExit record_scope
        {
            []() noexcept -> void
            {
                BASS_RecordFree();
            }
        };

        if (PluginConfig::Instance().HasMicroDevice())
        {
            for (size_t i = 0; i < _devices.size(); ++i)
            {
                if (_devices[i].first == PluginConfig::Instance().GetMicroDevice())
                {
                    _device = i;
                    break;
                }
            }
        }

        {
            BOOL status = BASS_RecordInit(_device != None<size_t> ? _devices[_device].second : -1);
            if (status == FALSE && _device != None<size_t>)
            {
                status = BASS_RecordInit(-1);
                _device = None<size_t>;
            }
            if (status == TRUE && _device == None<size_t>)
            {
                for (size_t i = 0; i < _devices.size(); ++i)
                {
                    if (_devices[i].second == BASS_RecordGetDevice())
                    {
                        _device = i;
                        break;
                    }
                }
            }
            if (status == FALSE || _device == None<size_t>)
            {
                Logger::Instance().LogToFile("[sv:err:speaker:initialize] failed to "
                    "initialize device (code:%d)", BASS_ErrorGetCode());
                return false;
            }
        }

        PluginConfig::Instance().SetMicroDevice(_devices[_device].first.c_str());

        _channel_record = BASS_RecordStart(kFrequency, 1,
            MAKELONG(BASS_SAMPLE_FLOAT | BASS_RECORD_PAUSE, kInterval),
            reinterpret_cast<RECORDPROC*>(OnProcess), nullptr);
        if (_channel_record == NULL)
        {
            Logger::Instance().LogToFile("[sv:err:speaker:initialize] failed to create speaker "
                "stream (code:%d)", BASS_ErrorGetCode());
            return false;
        }

        ScopeExit channel_scope
        {
            [this]() noexcept -> void
            {
                BASS_ChannelStop(_channel_record);
                _channel_record = NULL;
            }
        };

        if (BASS_ChannelSetAttribute(_channel_record, BASS_ATTRIB_GRANULE, kFrameSamples) == FALSE)
        {
            Logger::Instance().LogToFile("[sv:err:speaker:initialize] failed to set "
                "'BASS_ATTRIB_GRANULE' parameter (code:%d)", BASS_ErrorGetCode());
            return false;
        }

        _channel_check = BASS_StreamCreate(kFrequency, 1, BASS_SAMPLE_MONO | BASS_SAMPLE_FLOAT, STREAMPROC_PUSH, nullptr);
        if (_channel_check == NULL)
        {
            Logger::Instance().LogToFile("[sv:err:speaker:initialize] failed to create "
                "check stream (code:%d)", BASS_ErrorGetCode());
            return false;
        }

        SyncConfigs();

        Logger::Instance().LogToFile("[sv:dbg:speaker:initialize] module initialized");

        devices_scope.Release();
        encoder_scope.Release();
        device_scope.Release();
        record_scope.Release();
        channel_scope.Release();

        return true;
    }

    void Deinitialize() noexcept
    {
        Logger::Instance().LogToFile("[sv:dbg:speaker:deinitialize] module releasing...");

        StopRecording();
        StopChecking();

        BASS_ChannelStop(_channel_record);
        BASS_StreamFree(_channel_check);

        BASS_RecordFree();

        _channel_record = NULL;
        _channel_check = NULL;

        opus_encoder_destroy(_encoder);
        _encoder = nullptr;

        _device = None<size_t>;
        _devices.clear();

        Logger::Instance().LogToFile("[sv:dbg:speaker:deinitialize] module released");
    }

public:

    bool HasMicro() const noexcept
    {
        BASS_DEVICEINFO device_info;

        for (DWORD device_number = 0; BASS_RecordGetDeviceInfo(device_number, &device_info); ++device_number)
        {
            const bool device_enabled = device_info.flags & BASS_DEVICE_ENABLED;
            const bool device_loopback = device_info.flags & BASS_DEVICE_LOOPBACK;

            if (device_enabled == true && device_loopback == false && device_info.name != nullptr)
                return true;
        }

        return false;
    }

public:

    bool StartRecording() noexcept
    {
        if (_is_recording == true)
            return false;

        if (!PluginConfig::Instance().IsMicroEnable())
            return false;

        opus_encoder_ctl(_encoder, OPUS_RESET_STATE);

        if (_is_checking == false)
        {
            if (const DWORD available = BASS_ChannelGetData(_channel_record, nullptr, BASS_DATA_AVAILABLE);
                available != -1 && available != 0) BASS_ChannelGetData(_channel_record, nullptr, available);

            if (BASS_ChannelStart(_channel_record) == FALSE)
            {
                Logger::Instance().LogToFile("[sv:err:speaker] failed to "
                    "play record channel (code:%d)", BASS_ErrorGetCode());
            }
        }

        _is_recording = true;

        return true;
    }

    bool IsRecording() const noexcept
    {
        return _is_recording;
    }

    void StopRecording() noexcept
    {
        _is_recording = false;

        if (_is_checking == false)
        {
            if (BASS_ChannelPause(_channel_record) == FALSE)
            {
                Logger::Instance().LogToFile("[sv:err:speaker] failed to "
                    "pause record channel (code:%d)", BASS_ErrorGetCode());
            }
        }
    }

public:

    bool StartChecking() noexcept
    {
        if (_is_checking == true)
            return false;

        if (!PluginConfig::Instance().IsMicroEnable())
            return false;

        Logger::Instance().LogToFile("[sv:dbg:speaker] checking device starting...");

        if (_is_recording == false)
        {
            if (const DWORD available = BASS_ChannelGetData(_channel_record, nullptr, BASS_DATA_AVAILABLE);
                available != -1 && available != 0) BASS_ChannelGetData(_channel_record, nullptr, available);

            if (BASS_ChannelStart(_channel_record) == FALSE)
            {
                Logger::Instance().LogToFile("[sv:err:speaker] failed to "
                    "play record channel (code:%d)", BASS_ErrorGetCode());
            }
        }

        if (BASS_ChannelPlay(_channel_check, TRUE) == FALSE)
        {
            Logger::Instance().LogToFile("[sv:err:speaker] failed to "
                "play checking channel (code:%d)", BASS_ErrorGetCode());
        }

        _is_checking = true;

        return true;
    }

    bool IsChecking() const noexcept
    {
        return _is_checking;
    }

    void StopChecking() noexcept
    {
        if (_is_checking == true)
        {
            Logger::Instance().LogToFile("[sv:dbg:speaker] checking device stoped");

            if (BASS_ChannelStop(_channel_check) == FALSE)
            {
                Logger::Instance().LogToFile("[sv:err:speaker] failed to "
                    "stop checking channel (code:%d)", BASS_ErrorGetCode());
            }

            _is_checking = false;
        }
        if (_is_recording == false)
        {
            if (BASS_ChannelPause(_channel_record) == FALSE)
            {
                Logger::Instance().LogToFile("[sv:err:speaker] failed to "
                    "pause record channel (code:%d)", BASS_ErrorGetCode());
            }
        }
    }

public:

    bool IsMicroEnable() const noexcept
    {
        return PluginConfig::Instance().IsMicroEnable();
    }

    sdword_t GetMicroVolume() const noexcept
    {
        return PluginConfig::Instance().GetMicroVolume();
    }

    size_t GetMicroDevice() const noexcept
    {
        return _device;
    }

public:

    void SetMicroEnable(const bool enable) noexcept
    {
        PluginConfig::Instance().SetMicroEnable(enable);
        if (!PluginConfig::Instance().IsMicroEnable())
        {
            StopRecording();
            StopChecking();
        }
    }

    void SetMicroVolume(const sdword_t volume) noexcept
    {
        PluginConfig::Instance().SetMicroVolume(std::clamp(volume, 0, 100));
        BASS_ChannelSetAttribute(_channel_record, BASS_ATTRIB_VOLDSP,
            1.25F * static_cast<float>(PluginConfig::Instance().GetMicroVolume()) / 100);
    }

    void SetMicroDevice(const size_t device) noexcept
    {
        if (device < _devices.size() && device != _device)
        {
            if (BASS_RecordInit(_devices[device].second))
            {
                const HRECORD channel_record = BASS_RecordStart(kFrequency, 1,
                    MAKELONG(BASS_SAMPLE_FLOAT | BASS_RECORD_PAUSE, kInterval),
                    reinterpret_cast<RECORDPROC*>(OnProcess), nullptr);
                if (channel_record == NULL)
                {
                    BASS_RecordFree();
                    BASS_RecordSetDevice(_devices[_device].second);
                    return;
                }
                if (BASS_ChannelSetAttribute(channel_record, BASS_ATTRIB_GRANULE, kFrameSamples) == FALSE)
                {
                    BASS_ChannelStop(channel_record);
                    BASS_RecordFree();
                    BASS_RecordSetDevice(_devices[_device].second);
                    return;
                }

                BASS_ChannelStop(_channel_record);

                if (_is_checking || _is_recording)
                {
                    BASS_ChannelStart(channel_record);
                }

                _channel_record = channel_record;

                BASS_RecordSetDevice(_devices[_device].second);
                BASS_RecordFree();
                BASS_RecordSetDevice(_devices[device].second);

                _device = device;

                PluginConfig::Instance().SetMicroDevice(_devices[device].first.c_str());
            }
        }
    }

public:

    void SyncConfigs() noexcept
    {
        SetMicroEnable(PluginConfig::Instance().IsMicroEnable());
        SetMicroVolume(PluginConfig::Instance().GetMicroVolume());
    }

    void ResetConfigs() noexcept
    {
        PluginConfig::Instance().SetMicroEnable(PluginConfig::kDefaultMicroEnable);
        PluginConfig::Instance().SetMicroVolume(PluginConfig::kDefaultMicroVolume);
    }

public:

    const auto& Devices() const noexcept
    {
        return _devices;
    }

private:

    static BOOL CALLBACK OnProcess(const HRECORD, const float* const buffer,
        const DWORD length, const ptr_t) noexcept
    {
        assert(length >= kFrameBytes);

        if (Instance()._is_checking)
        {
            BASS_StreamPutData(Instance()._channel_check, buffer, kFrameBytes);
        }

        if (Instance()._is_recording && Instance().Buffer.Valid())
        {
            if (const opus_int32 bytes = opus_encode_float(Instance()._encoder, buffer, kFrameSamples,
                Instance().Buffer.Data(), Instance().Buffer.Bytes()); bytes > 0)
            {
                Instance().OnFrame(bytes);
            }
        }

        return TRUE;
    }

private:

    volatile bool _is_checking  = false;
    volatile bool _is_recording = false;

    HSTREAM       _channel_check  = NULL;
    HRECORD       _channel_record = NULL;

    OpusEncoder*  _encoder = nullptr;

private:

    size_t _device = None<size_t>;

    std::vector<std::pair<std::string, int>>
        _devices;

public:

    std::function<void(size_t)> OnFrame;

    DataView<ubyte_t> Buffer;

};
