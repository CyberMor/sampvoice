#include "StreamAtPoint.h"

#include <audio/bass.h>

#include "StreamInfo.h"

StreamAtPoint::StreamAtPoint(const D3DCOLOR color, std::string name, const float distance, const CVector& position) noexcept
    : LocalStream { StreamType::LocalStreamAtPoint, color, std::move(name), distance }
    , _position   { position }
{}

void StreamAtPoint::SetPosition(const CVector& position) noexcept
{
    _position = position;

    for (const auto& channel : GetChannels())
    {
        BASS_ChannelSet3DPosition(channel.GetHandle(),
            reinterpret_cast<BASS_3DVECTOR*>(&position),
            nullptr, nullptr);
    }
}

void StreamAtPoint::OnChannelCreate(const Channel& channel) noexcept
{
    static const BASS_3DVECTOR kZeroVector { 0, 0, 0 };

    LocalStream::OnChannelCreate(channel);

    BASS_ChannelSet3DPosition(channel.GetHandle(),
        reinterpret_cast<BASS_3DVECTOR*>(&_position),
        &kZeroVector, &kZeroVector);
}
