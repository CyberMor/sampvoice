#include "LocalStream.h"

#include <audio/bass.h>

LocalStream::LocalStream(const StreamType type, const D3DCOLOR color, std::string name, const float distance) noexcept
    : Stream    { BASS_SAMPLE_3D | BASS_SAMPLE_MUTEMAX, type, color, std::move(name) }
    , _distance { distance }
{}

void LocalStream::SetDistance(const float distance) noexcept
{
    _distance = distance;

    for (const auto& channel : GetChannels())
    {
        BASS_ChannelSet3DAttributes(channel.GetHandle(), BASS_3DMODE_NORMAL,
            distance * 0.1f, distance, -1, -1, -1);
    }
}

void LocalStream::OnChannelCreate(const Channel& channel) noexcept
{
    Stream::OnChannelCreate(channel);

    BASS_ChannelSet3DAttributes(channel.GetHandle(), BASS_3DMODE_NORMAL,
        _distance * 0.1f, _distance, -1, -1, -1);
}
