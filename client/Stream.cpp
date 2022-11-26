#include "Stream.h"

#include <audio/bass.h>
#include <util/Logger.h>

#include "SetController.h"
#include "SlideController.h"

Stream::Stream(const DWORD flags, const StreamType type, const D3DCOLOR color, std::string name) noexcept
    : _stream_flags { flags }
    , _stream_info  { type, color, std::move(name) }
{}

const StreamInfo& Stream::GetInfo() const noexcept
{
    return _stream_info;
}

void Stream::Tick() noexcept
{
    for (const auto& channel : _channels)
    {
        if (channel->HasSpeaker() && !channel->IsActive()) channel->Reset();
    }
}

void Stream::Push(const VoicePacket& packet)
{
    const Channel* channelPtr = nullptr;

    for (const auto& channel : _channels)
    {
        if (channel->GetSpeaker() == packet.sender)
        {
            channelPtr = &channel;
            break;
        }
    }

    if (channelPtr == nullptr)
    {
        for (const auto& channel : _channels)
        {
            if (!channel->HasSpeaker())
            {
                Logger::LogToFile("[sv:dbg:stream:push] : channel %p was occupied by player %hu "
                    "(stream:%s)", channel.get(), packet.sender, _stream_info.GetName().c_str());

                channel->SetSpeaker(packet.sender);

                channelPtr = &channel;
                break;
            }
        }
    }

    if (channelPtr == nullptr)
    {
        auto& channelRef = _channels.emplace_back(MakeChannel(_stream_flags));
        channelRef->SetPlayCallback(std::bind(&Stream::OnChannelPlay, this, std::placeholders::_1));
        channelRef->SetStopCallback(std::bind(&Stream::OnChannelStop, this, std::placeholders::_1));
        channelRef->SetSpeaker(packet.sender);

        Logger::LogToFile("[sv:dbg:stream:push] : channel %p for player %hu created (stream:%s)",
            channelRef.get(), packet.sender, _stream_info.GetName().c_str());

        OnChannelCreate(*channelRef);

        channelPtr = &channelRef;
    }

    (*channelPtr)->Push(packet.packid, packet.data, packet.length);
}

void Stream::Reset() noexcept
{
    for (const auto& channel : _channels)
    {
        channel->Reset();
    }
}

void Stream::SetParameter(const BYTE parameter, const float value)
{
    const auto& parameterPtr = _parameters[parameter] = SetController(parameter, value);

    for (const auto& channel : _channels)
    {
        parameterPtr->Apply(*channel);
    }
}

void Stream::SlideParameter(const BYTE parameter, const float startValue,
                            const float endValue, const DWORD time)
{
    const auto& parameterPtr = _parameters[parameter] = SlideController(parameter, startValue, endValue, time);

    for (const auto& channel : _channels)
    {
        parameterPtr->Apply(*channel);
    }
}

void Stream::EffectCreate(const DWORD effect, const DWORD number, const int priority,
                          const void* const paramPtr, const DWORD paramSize)
{
    const auto& effectPtr = _effects[effect] = Effect(number, priority, paramPtr, paramSize);

    for (const auto& channel : _channels)
    {
        effectPtr->Apply(*channel);
    }
}

void Stream::EffectDelete(const DWORD effect)
{
    _effects.erase(effect);
}

void Stream::SetPlayCallback(PlayCallback&& callback) noexcept
{
    _play_callback = std::move(callback);
}

void Stream::SetStopCallback(StopCallback&& callback) noexcept
{
    _stop_callback = std::move(callback);
}

void Stream::OnChannelCreate(const Channel& channel) noexcept
{
    BASS_ChannelSetAttribute(channel.GetHandle(), BASS_ATTRIB_VOL, 4.f);

    for (const auto& parameter : _parameters)
    {
        parameter.second->Apply(channel);
    }
    for (const auto& effect : _effects)
    {
        effect.second->Apply(channel);
    }
}

void Stream::OnChannelPlay(const Channel& channel) noexcept
{
    if (channel.HasSpeaker() && _play_callback != nullptr)
        _play_callback(*this, channel.GetSpeaker());
}

void Stream::OnChannelStop(const Channel& channel) noexcept
{
    if (channel.HasSpeaker() && _stop_callback != nullptr)
        _stop_callback(*this, channel.GetSpeaker());
}

const std::vector<ChannelPtr>& Stream::GetChannels() const noexcept
{
    return _channels;
}
