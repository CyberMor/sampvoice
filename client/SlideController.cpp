#include "SlideController.h"

SlideController::SlideController(const DWORD parameter, const float startValue,
                                 const float endValue, const DWORD time) noexcept
    : Parameter(parameter)
    , ratio((endValue - startValue) / static_cast<float>(time))
    , endTime(Timer::Get() + time)
    , endValue(endValue)
{}

void SlideController::Apply(const Channel& channel) const noexcept
{
    if (Timer::Get() < this->endTime)
    {
        const Timer::time_t timeLeft = this->endTime - Timer::Get();
        const float startValue = this->endValue - static_cast<float>(timeLeft) * this->ratio;

        BASS_ChannelSetAttribute(channel.GetHandle(), this->parameter, startValue);
        BASS_ChannelSlideAttribute(channel.GetHandle(), this->parameter, this->endValue,
            static_cast<DWORD>(timeLeft));
    }
    else
    {
        BASS_ChannelSetAttribute(channel.GetHandle(), this->parameter, this->endValue);
    }
}
