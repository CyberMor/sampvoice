#include "StreamAtPoint.h"

#include <audio/bass.h>

#include "StreamInfo.h"

StreamAtPoint::StreamAtPoint(const D3DCOLOR color, std::string name,
                             const float distance, const CVector& position) noexcept
    : LocalStream(StreamType::LocalStreamAtPoint, color, std::move(name), distance)
    , position(position)
{}

void StreamAtPoint::SetPosition(const CVector& position) noexcept
{
    this->position = position;

    for (const auto& channel : this->GetChannels())
    {
        BASS_ChannelSet3DPosition(channel->GetHandle(),
            reinterpret_cast<BASS_3DVECTOR*>(&this->position),
            nullptr, nullptr);
    }
}

void StreamAtPoint::OnChannelCreate(const Channel& channel) noexcept
{
    static const BASS_3DVECTOR kZeroVector { 0, 0, 0 };

    this->LocalStream::OnChannelCreate(channel);

    BASS_ChannelSet3DPosition(channel.GetHandle(),
        reinterpret_cast<BASS_3DVECTOR*>(&this->position),
        &kZeroVector, &kZeroVector);
}
