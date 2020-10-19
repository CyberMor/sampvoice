#include "Channel.h"

#include <util/Logger.h>

Channel::Channel(const DWORD channelFlags)
    : handle(BASS_StreamCreate(SV::kFrequency, 1, channelFlags, STREAMPROC_PUSH, nullptr))
    , decoder(opus_decoder_create(SV::kFrequency, 1, &opusErrorCode))
{
    if (this->handle == NULL) Logger::LogToFile("[sv:err:channel] : "
        "failed to create bass channel (code:%d)", BASS_ErrorGetCode());
    if (this->decoder == nullptr) Logger::LogToFile("[sv:err:channel] : "
        "failed to create opus decoder (code:%d)", this->opusErrorCode);

    if (this->handle == NULL || this->decoder == nullptr)
    {
        if (this->decoder != nullptr) opus_decoder_destroy(this->decoder);
        if (this->handle != NULL) BASS_StreamFree(this->handle);
        throw std::exception();
    }
}

Channel::~Channel() noexcept
{
    if (this->playing && this->stopCallback != nullptr)
        this->stopCallback(*this);

    opus_decoder_destroy(this->decoder);
    BASS_StreamFree(this->handle);
}

HSTREAM Channel::GetHandle() const noexcept
{
    return this->handle;
}

void Channel::SetSpeaker(const WORD speaker) noexcept
{
    this->speaker = speaker;
}

bool Channel::HasSpeaker() const noexcept
{
    return this->speaker != SV::kNonePlayer;
}

WORD Channel::GetSpeaker() const noexcept
{
    return this->speaker;
}

bool Channel::IsActive() const noexcept
{
    const auto bufferSize = BASS_ChannelGetData(this->handle, nullptr, BASS_DATA_AVAILABLE);
    return bufferSize != -1 && bufferSize != 0;
}

void Channel::Reset() noexcept
{
    BASS_ChannelPause(this->handle);
    BASS_ChannelSetPosition(this->handle, 0, BASS_POS_BYTE);
    opus_decoder_ctl(this->decoder, OPUS_RESET_STATE);

    if (this->playing && this->stopCallback != nullptr)
        this->stopCallback(*this);

    this->speaker = SV::kNonePlayer;
    this->expectedPacketNumber = 0;
    this->initialized = false;
    this->playing = false;
}

void Channel::Push(const DWORD packetNumber, const BYTE* const dataPtr, const DWORD dataSize) noexcept
{
    if (!this->initialized || packetNumber == NULL)
    {
        Logger::LogToFile("[sv:dbg:channel:push] : init channel (speaker:%hu)", this->speaker);

        BASS_ChannelPause(this->handle);
        BASS_ChannelSetPosition(this->handle, 0, BASS_POS_BYTE);
        opus_decoder_ctl(this->decoder, OPUS_RESET_STATE);

        if (this->playing && this->stopCallback != nullptr)
            this->stopCallback(*this);

        this->initialized = true;
        this->playing = false;
    }
    else if (packetNumber < this->expectedPacketNumber)
    {
        Logger::LogToFile("[sv:dbg:channel:push] : late packet to channel (speaker:%hu) "
            "(pack:%u;expPack:%u)", this->speaker, packetNumber, this->expectedPacketNumber);

        return;
    }
    else if (packetNumber > this->expectedPacketNumber)
    {
        Logger::LogToFile("[sv:dbg:channel:push] : lost packet to channel (speaker:%hu) "
            "(pack:%u;expPack:%u)", this->speaker, packetNumber, this->expectedPacketNumber);

        if (const int length = opus_decode(this->decoder, dataPtr, dataSize, this->decBuffer.data(),
            SV::kFrameSizeInSamples, true); length == static_cast<int>(SV::kFrameSizeInSamples))
        {
            BASS_StreamPutData(this->handle, this->decBuffer.data(), SV::kFrameSizeInBytes);
        }
    }

    if (const int length = opus_decode(this->decoder, dataPtr, dataSize, this->decBuffer.data(),
        SV::kFrameSizeInSamples, false); length == static_cast<int>(SV::kFrameSizeInSamples))
    {
        BASS_StreamPutData(this->handle, this->decBuffer.data(), SV::kFrameSizeInBytes);
    }

    const auto channelStatus = BASS_ChannelIsActive(this->handle);
    const auto bufferSize = BASS_ChannelGetData(this->handle, nullptr, BASS_DATA_AVAILABLE);

    if ((channelStatus == BASS_ACTIVE_PAUSED || channelStatus == BASS_ACTIVE_STOPPED) &&
        bufferSize != -1 && bufferSize >= SV::kChannelPreBufferFramesCount * SV::kFrameSizeInBytes)
    {
        Logger::LogToFile("[sv:dbg:channel:push] : playing channel (speaker:%hu)", this->speaker);

        BASS_ChannelPlay(this->handle, FALSE);

        if (!this->playing && this->playCallback != nullptr)
            this->playCallback(*this);

        this->playing = true;
    }

    this->expectedPacketNumber = packetNumber + 1;
}

void Channel::SetPlayCallback(PlayCallback playCallback) noexcept
{
    this->playCallback = std::move(playCallback);
}

void Channel::SetStopCallback(StopCallback stopCallback) noexcept
{
    this->stopCallback = std::move(stopCallback);
}
