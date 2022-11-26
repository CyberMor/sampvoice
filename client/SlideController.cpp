#include "SlideController.h"

SlideController::SlideController(const DWORD parameter, const float start_value, const float end_value, const Timer::time_t time) noexcept
    : Parameter  { parameter }
    , _ratio     { (end_value - start_value) / static_cast<float>(time) }
    , _end_time  { Timer::Get() + time }
    , _end_value { end_value }
{}

void SlideController::Apply(const Channel& channel) noexcept
{
    if (Timer::Get() < _end_time)
    {
        const Timer::time_t time_left = _end_time - Timer::Get();
        const float start_value = _end_value - static_cast<float>(time_left) * _ratio;

        BASS_ChannelSetAttribute(channel.GetHandle(), _parameter, start_value);
        BASS_ChannelSlideAttribute(channel.GetHandle(), _parameter, _end_value, static_cast<DWORD>(time_left));
    }
    else
    {
        BASS_ChannelSetAttribute(channel.GetHandle(), _parameter, _end_value);
    }
}
