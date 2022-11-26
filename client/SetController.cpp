#include "SetController.h"

SetController::SetController(const DWORD parameter, const float value) noexcept
    : Parameter { parameter }
    , _value    { value }
{}

void SetController::Apply(const Channel& channel) noexcept
{
    BASS_ChannelSetAttribute(channel.GetHandle(), _parameter, _value);
}
