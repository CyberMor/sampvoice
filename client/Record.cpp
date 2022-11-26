/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Record.h"

#include <algorithm>

#include <util/Logger.h>

#include "PluginConfig.h"

bool Record::Init(const DWORD bitrate) noexcept
{
    if (_init_status) return false;
    if (BASS_IsStarted() == FALSE) return false;

    Logger::LogToFile("[sv:dbg:record:init] : module initializing...");

    _device_names_list.clear();
    _device_numbers_list.clear();

    {
        BASS_DEVICEINFO dev_info;

        for (int dev_number = 0; BASS_RecordGetDeviceInfo(dev_number, &dev_info); ++dev_number)
        {
            const bool device_enabled = dev_info.flags & BASS_DEVICE_ENABLED;
            const bool device_loopback = dev_info.flags & BASS_DEVICE_LOOPBACK;
            const DWORD device_type = dev_info.flags & BASS_DEVICE_TYPE_MASK;

            Logger::LogToFile("[sv:dbg:record:init] : device detect "
                "[ id(%d) enabled(%hhu) loopback(%hhu) name(%s) type(0x%x) ]",
                dev_number, device_enabled, device_loopback, dev_info.name != nullptr
                ? dev_info.name : "none", device_type);

            if (device_enabled && !device_loopback && dev_info.name != nullptr)
            {
                _device_numbers_list.emplace_back(dev_number);
                _device_names_list.emplace_back(dev_info.name);
            }
        }
    }

    Memory::ScopeExit deviceListsResetScope { []{ _used_device_index = -1;
                                                  _device_names_list.clear();
                                                  _device_numbers_list.clear(); } };

    if (_device_names_list.empty() || _device_numbers_list.empty())
    {
        Logger::LogToFile("[sv:inf:record:init] : failed to find microphone");
        return false;
    }

    {
        int opusErrorCode = -1;

        _encoder = opus_encoder_create(SV::kFrequency, 1,
            OPUS_APPLICATION_VOIP, &opusErrorCode);

        if (_encoder == nullptr || opusErrorCode < 0)
        {
            Logger::LogToFile("[sv:err:record:init] : failed to "
                "create _encoder (code:%d)", opusErrorCode);
            return false;
        }
    }

    Memory::ScopeExit encoderResetScope { []{ opus_encoder_destroy(_encoder); } };

    if (const auto error = opus_encoder_ctl(_encoder,
        OPUS_SET_BITRATE(bitrate)); error < 0)
    {
        Logger::LogToFile("[sv:err:record:init] : failed to "
            "set bitrate for _encoder (code:%d)", error);
        return false;
    }

    if (const auto error = opus_encoder_ctl(_encoder,
        OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE)); error < 0)
    {
        Logger::LogToFile("[sv:err:record:init] : failed to "
            "set audiosignal type for _encoder (code:%d)", error);
        return false;
    }

    if (const auto error = opus_encoder_ctl(_encoder,
        OPUS_SET_COMPLEXITY(10)); error < 0)
    {
        Logger::LogToFile("[sv:err:record:init] : failed to "
            "set complexity for _encoder (code:%d)", error);
        return false;
    }

    if (const auto error = opus_encoder_ctl(_encoder,
        OPUS_SET_PREDICTION_DISABLED(FALSE)); error < 0)
    {
        Logger::LogToFile("[sv:err:record:init] : failed to "
            "enable prediction for _encoder (code:%d)", error);
        return false;
    }

    if (const auto error = opus_encoder_ctl(_encoder,
        OPUS_SET_LSB_DEPTH(16)); error < 0)
    {
        Logger::LogToFile("[sv:err:record:init] : failed to "
            "set lsb depth for _encoder (code:%d)", error);
        return false;
    }

    if (const auto error = opus_encoder_ctl(_encoder,
        OPUS_SET_FORCE_CHANNELS(TRUE)); error < 0)
    {
        Logger::LogToFile("[sv:err:record:init] : failed to "
            "set count channels for _encoder (code:%d)", error);
        return false;
    }

    if (const auto error = opus_encoder_ctl(_encoder,
        OPUS_SET_DTX(FALSE)); error < 0)
    {
        Logger::LogToFile("[sv:err:record:init] : failed to "
            "set dtx for _encoder (code:%d)", error);
        return false;
    }

    if (const auto error = opus_encoder_ctl(_encoder,
        OPUS_SET_INBAND_FEC(TRUE)); error < 0)
    {
        Logger::LogToFile("[sv:err:record:init] : failed to "
            "set inband fec for _encoder (code:%d)", error);
        return false;
    }

    if (const auto error = opus_encoder_ctl(_encoder,
        OPUS_SET_PACKET_LOSS_PERC(10)); error < 0)
    {
        Logger::LogToFile("[sv:err:record:init] : failed to set "
            "packet loss percent for _encoder (code:%d)", error);
        return false;
    }

    _used_device_index = -1;

    if (PluginConfig::IsRecordLoaded() && !PluginConfig::GetDeviceName().empty())
    {
        for (std::size_t i = 0; i < _device_names_list.size(); ++i)
        {
            if (_device_names_list[i] == PluginConfig::GetDeviceName())
            {
                _used_device_index = i;
                break;
            }
        }
    }

    bool initRecordStatus = BASS_RecordInit(_used_device_index != -1 ?
                            _device_numbers_list[_used_device_index] : -1);

    if (!initRecordStatus && _used_device_index != -1)
    {
        initRecordStatus = BASS_RecordInit(_used_device_index = -1);
    }

    if (initRecordStatus && _used_device_index == -1)
    {
        for (std::size_t i = 0; i < _device_numbers_list.size(); ++i)
        {
            if (_device_numbers_list[i] == BASS_RecordGetDevice())
            {
                _used_device_index = i;
                break;
            }
        }
    }

    Memory::ScopeExit recordResetScope { []{ BASS_RecordFree(); } };

    if (!initRecordStatus || _used_device_index == -1)
    {
        Logger::LogToFile("[sv:err:record:init] : failed to "
            "init device (code:%d)", BASS_ErrorGetCode());
        return false;
    }

    if (!PluginConfig::IsRecordLoaded())
    {
        PluginConfig::SetDeviceName(_device_names_list[_used_device_index]);
    }

    _record_channel = BASS_RecordStart(SV::kFrequency, 1,
        BASS_RECORD_PAUSE, nullptr, nullptr);

    if (_record_channel == NULL)
    {
        Logger::LogToFile("[sv:err:record:init] : failed to create record "
            "stream (code:%d)", BASS_ErrorGetCode());
        return false;
    }

    Memory::ScopeExit channelResetScope { []{ BASS_ChannelStop(_record_channel); } };

    _check_channel = BASS_StreamCreate(SV::kFrequency, 1,
        NULL, STREAMPROC_PUSH, nullptr);

    if (_check_channel == NULL)
    {
        Logger::LogToFile("[sv:err:record:init] : failed to create "
            "check stream (code:%d)", BASS_ErrorGetCode());
        return false;
    }

    BASS_ChannelSetAttribute(_check_channel, BASS_ATTRIB_VOL, 4.f);

    if (!PluginConfig::IsRecordLoaded())
    {
        PluginConfig::SetRecordLoaded(true);
        ResetConfigs();
    }

    deviceListsResetScope.Release();
    encoderResetScope.Release();
    recordResetScope.Release();
    channelResetScope.Release();

    Logger::LogToFile("[sv:dbg:record:init] : module initialized");

    _init_status = true;
    SyncConfigs();

    return true;
}

void Record::Free() noexcept
{
    if (_init_status)
    {
        Logger::LogToFile("[sv:dbg:record:free] : module releasing...");

        StopRecording();
        BASS_ChannelStop(_record_channel);
        BASS_RecordFree();

        StopChecking();
        BASS_StreamFree(_check_channel);

        opus_encoder_destroy(_encoder);

        _used_device_index = -1;
        _device_numbers_list.clear();
        _device_names_list.clear();

        Logger::LogToFile("[sv:dbg:record:free] : module released");

        _init_status = false;
    }
}

void Record::Tick() noexcept
{
    if (_init_status && _check_status)
    {
        if (const auto buffer_size = BASS_ChannelGetData(_record_channel, nullptr, BASS_DATA_AVAILABLE);
            buffer_size != -1 && buffer_size != 0)
        {
            if (const auto read_data_size = BASS_ChannelGetData(_record_channel, _enc_buffer,
                std::clamp(buffer_size, 0ul, SV::kFrameSizeInBytes));
                read_data_size != -1 && read_data_size != 0)
            {
                BASS_StreamPutData(_check_channel, _enc_buffer, read_data_size);
            }
        }
    }
}

DWORD Record::GetFrame(BYTE* const buffer_ptr, const DWORD buffer_size) noexcept
{
    if (!_init_status || !_record_status || _check_status) return NULL;

    const auto channel_buffer_size = BASS_ChannelGetData(_record_channel, nullptr, BASS_DATA_AVAILABLE);
    if (channel_buffer_size == -1 || channel_buffer_size < SV::kFrameSizeInBytes) return NULL;

    if (BASS_ChannelGetData(_record_channel, _enc_buffer,
        SV::kFrameSizeInBytes) != SV::kFrameSizeInBytes) return NULL;

    const auto enc_data_length = opus_encode(_encoder, _enc_buffer,
        SV::kFrameSizeInSamples, buffer_ptr, buffer_size);

    return enc_data_length > 0 ? static_cast<DWORD>(enc_data_length) : 0;
}

bool Record::HasMicro() noexcept
{
    BASS_DEVICEINFO dev_info;

    for (DWORD dev_number = 0; BASS_RecordGetDeviceInfo(dev_number, &dev_info); ++dev_number)
    {
        const bool device_enabled = dev_info.flags & BASS_DEVICE_ENABLED;
        const bool device_loopback = dev_info.flags & BASS_DEVICE_LOOPBACK;

        if (device_enabled && !device_loopback && dev_info.name != nullptr)
            return true;
    }

    return false;
}

bool Record::StartRecording() noexcept
{
    if (!_init_status || _record_status || _check_status)
        return false;

    if (!PluginConfig::GetMicroEnable())
        return false;

    Logger::LogToFile("[sv:dbg:record:startrecording] : channel recording starting...");

    BASS_ChannelPlay(_record_channel, FALSE);
    _record_status = true;

    return true;
}

bool Record::IsRecording() noexcept
{
    return _record_status;
}

void Record::StopRecording() noexcept
{
    if (_init_status)
    {
        _record_status = false;

        if (!_check_status)
        {
            BASS_ChannelPause(_record_channel);
            opus_encoder_ctl(_encoder, OPUS_RESET_STATE);

            Logger::LogToFile("[sv:dbg:record:stoprecording] : channel recording stoped");

            const auto buffer_size = BASS_ChannelGetData(_record_channel, nullptr, BASS_DATA_AVAILABLE);
            if (buffer_size != -1 && buffer_size != 0)
            {
                BASS_ChannelGetData(_record_channel, nullptr, buffer_size);
            }
        }
    }
}

bool Record::StartChecking() noexcept
{
    if (!_init_status || _check_status)
        return false;

    if (!PluginConfig::GetMicroEnable())
        return false;

    StopRecording();

    Logger::LogToFile("[sv:dbg:record:startchecking] : checking device starting...");

    BASS_ChannelPlay(_check_channel, TRUE);
    BASS_ChannelPlay(_record_channel, TRUE);
    _check_status = true;

    return true;
}

bool Record::IsChecking() noexcept
{
    return _check_status;
}

void Record::StopChecking() noexcept
{
    if (_init_status && _check_status)
    {
        Logger::LogToFile("[sv:dbg:record:stopchecking] : checking device stoped");

        BASS_ChannelStop(_check_channel);
        _check_status = false;
    }
}

bool Record::GetMicroEnable() noexcept
{
    return PluginConfig::GetMicroEnable();
}

int Record::GetMicroVolume() noexcept
{
    return PluginConfig::GetMicroVolume();
}

int Record::GetMicroDevice() noexcept
{
    return _used_device_index;
}

void Record::SetMicroEnable(const bool micro_enable) noexcept
{
    if (_init_status)
    {
        PluginConfig::SetMicroEnable(micro_enable);
        if (!PluginConfig::GetMicroEnable())
        {
            StopRecording();
            StopChecking();
        }
    }
}

void Record::SetMicroVolume(const int micro_volume) noexcept
{
    if (_init_status)
    {
        PluginConfig::SetMicroVolume(std::clamp(micro_volume, 0, 100));
        BASS_RecordSetInput(-1, BASS_INPUT_ON, static_cast<float>
            (PluginConfig::GetMicroVolume()) / 100.f);
    }
}

void Record::SetMicroDevice(const int device_index) noexcept
{
    if (_init_status)
    {
        if (device_index >= 0 && device_index < std::min(_device_names_list.size(), _device_numbers_list.size()))
        {
            if (device_index != _used_device_index)
            {
                BASS_ChannelStop(_record_channel);
                BASS_RecordFree();

                const auto old_device_index = _used_device_index;

                if (BASS_RecordInit(_device_numbers_list[_used_device_index = device_index]) == FALSE &&
                    BASS_RecordInit(_device_numbers_list[_used_device_index = old_device_index]) == FALSE)
                    return;

                _record_channel = BASS_RecordStart(SV::kFrequency, 1, !_record_status &&
                    !_check_status ? BASS_RECORD_PAUSE : NULL, nullptr, nullptr);

                PluginConfig::SetDeviceName(_device_names_list[_used_device_index]);
            }
        }
    }
}

void Record::SyncConfigs() noexcept
{
    SetMicroEnable(PluginConfig::GetMicroEnable());
    SetMicroVolume(PluginConfig::GetMicroVolume());
}

void Record::ResetConfigs() noexcept
{
    PluginConfig::SetMicroEnable(PluginConfig::kDefValMicroEnable);
    PluginConfig::SetMicroVolume(PluginConfig::kDefValMicroVolume);
}

const std::vector<std::string>& Record::GetDeviceNamesList() noexcept
{
    return _device_names_list;
}

const std::vector<int>& Record::GetDeviceNumbersList() noexcept
{
    return _device_numbers_list;
}

bool                     Record::_init_status = false;
bool                     Record::_check_status = false;
bool                     Record::_record_status = false;
HRECORD                  Record::_record_channel = NULL;
OpusEncoder*             Record::_encoder = nullptr;
opus_int16               Record::_enc_buffer[SV::kFrameSizeInSamples];
HSTREAM                  Record::_check_channel = NULL;
int                      Record::_used_device_index = -1;
std::vector<std::string> Record::_device_names_list;
std::vector<int>         Record::_device_numbers_list;
