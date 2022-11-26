#include "Channel.h"

#include <util/Logger.h>

Channel::~Channel() noexcept
{
    if (_playing && _stop_callback != nullptr)
        _stop_callback(*this);

    opus_decoder_destroy(_decoder);
    BASS_StreamFree(_handle);
}

Channel::Channel(const DWORD flags) noexcept
{
    _handle = BASS_StreamCreate(SV::kFrequency, 1, flags, STREAMPROC_PUSH, nullptr);
    if (_handle == NULL)
    {
        Logger::LogToFile("[sv:err:channel] : failed to create" " "
            "bass channel (code:%d)", BASS_ErrorGetCode());
        return;
    }

    _decoder = opus_decoder_create(SV::kFrequency, 1, &_opus_error);
    if (_decoder == nullptr)
    {
        Logger::LogToFile("[sv:err:channel] : failed to create" " "
            "opus decoder (code:%d)", _opus_error);
        BASS_StreamFree(_handle);
        _handle = NULL;
        return;
    }
}

bool Channel::Valid() const noexcept
{
    return _handle != NULL && _decoder != nullptr;
}

HSTREAM Channel::GetHandle() const noexcept
{
    return _handle;
}

void Channel::SetSpeaker(const WORD speaker) noexcept
{
    _speaker = speaker;
}

bool Channel::HasSpeaker() const noexcept
{
    return _speaker != SV::kNonePlayer;
}

WORD Channel::GetSpeaker() const noexcept
{
    return _speaker;
}

bool Channel::IsActive() const noexcept
{
    const auto buffer_size = BASS_ChannelGetData(_handle, nullptr, BASS_DATA_AVAILABLE);
    return buffer_size != -1 && buffer_size != 0;
}

void Channel::Reset() noexcept
{
    BASS_ChannelPause(_handle);
    BASS_ChannelSetPosition(_handle, 0, BASS_POS_BYTE);
    opus_decoder_ctl(_decoder, OPUS_RESET_STATE);

    if (_playing && _stop_callback != nullptr)
        _stop_callback(*this);

    _speaker = SV::kNonePlayer;
    _expected_packet = 0;
    _initialized = false;
    _playing = false;
}

void Channel::Push(const DWORD packet, const BYTE* const data, const DWORD size) noexcept
{
    if (!_initialized || packet == 0)
    {
        Logger::LogToFile("[sv:dbg:channel:push] : init channel (speaker:%hu)", _speaker);

        BASS_ChannelPause(_handle);
        BASS_ChannelSetPosition(_handle, 0, BASS_POS_BYTE);
        opus_decoder_ctl(_decoder, OPUS_RESET_STATE);

        if (_playing && _stop_callback != nullptr)
            _stop_callback(*this);

        _initialized = true;
        _playing = false;
    }
    else if (packet < _expected_packet)
    {
        Logger::LogToFile("[sv:dbg:channel:push] : late packet to channel (speaker:%hu) "
            "(packet:%u;expected:%u)", _speaker, packet, _expected_packet);

        return;
    }
    else if (packet > _expected_packet)
    {
        Logger::LogToFile("[sv:dbg:channel:push] : lost packet to channel (speaker:%hu) "
            "(packet:%u;expected:%u)", _speaker, packet, _expected_packet);

        if (const int length = opus_decode(_decoder, data, size, _dec_buffer,
            SV::kFrameSizeInSamples, true); length == static_cast<int>(SV::kFrameSizeInSamples))
        {
            BASS_StreamPutData(_handle, _dec_buffer, SV::kFrameSizeInBytes);
        }
    }

    if (const int length = opus_decode(_decoder, data, size, _dec_buffer,
        SV::kFrameSizeInSamples, false); length == static_cast<int>(SV::kFrameSizeInSamples))
    {
        BASS_StreamPutData(_handle, _dec_buffer, SV::kFrameSizeInBytes);
    }

    const auto channel_status = BASS_ChannelIsActive(_handle);
    const auto buffer_size = BASS_ChannelGetData(_handle, nullptr, BASS_DATA_AVAILABLE);

    if ((channel_status == BASS_ACTIVE_PAUSED || channel_status == BASS_ACTIVE_STOPPED) &&
        buffer_size != -1 && buffer_size >= SV::kChannelPreBufferFramesCount * SV::kFrameSizeInBytes)
    {
        Logger::LogToFile("[sv:dbg:channel:push] : _playing channel (speaker:%hu)", _speaker);

        BASS_ChannelPlay(_handle, FALSE);

        if (!_playing && _play_callback != nullptr)
            _play_callback(*this);

        _playing = true;
    }

    _expected_packet = packet + 1;
}

void Channel::SetPlayCallback(PlayCallback&& callback) noexcept
{
    _play_callback = std::move(callback);
}

void Channel::SetStopCallback(StopCallback&& callback) noexcept
{
    _stop_callback = std::move(callback);
}
