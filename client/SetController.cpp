#include "SetController.h"

SetController::SetController(const DWORD parameter, const float value) noexcept
    : Parameter(parameter), value(value) {}

void SetController::Apply(const Channel& channel) const noexcept
{
    BASS_ChannelSetAttribute(channel.GetHandle(), this->parameter, this->value);
}
