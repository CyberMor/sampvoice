#include "Parameter.h"

#include <functional>

#include "Header.h"
#include "Network.h"
#include "Stream.h"

Parameter::Parameter(Stream* const stream, const uint8_t parameter, const float initValue)
{
    PackWrap(this->packetSetParameter, SV::ControlPacketType::setStreamParameter, sizeof(SV::SetStreamParameterPacket));

    PackGetStruct(&*this->packetSetParameter, SV::SetStreamParameterPacket)->stream = reinterpret_cast<uint32_t>(stream);
    PackGetStruct(&*this->packetSetParameter, SV::SetStreamParameterPacket)->parameter = parameter;
    PackGetStruct(&*this->packetSetParameter, SV::SetStreamParameterPacket)->value = initValue;

    PackWrap(this->packetSlideParameter, SV::ControlPacketType::slideStreamParameter, sizeof(SV::SlideStreamParameterPacket));

    PackGetStruct(&*this->packetSlideParameter, SV::SlideStreamParameterPacket)->stream = reinterpret_cast<uint32_t>(stream);
    PackGetStruct(&*this->packetSlideParameter, SV::SlideStreamParameterPacket)->parameter = parameter;
}

float Parameter::Get() const noexcept
{
    return   this->sliding && Timer::Get() < this->endTime
           ? this->endValue - static_cast<float>(this->endTime - Timer::Get()) * this->ratio
           : PackGetStruct(&*this->packetSetParameter, SV::SetStreamParameterPacket)->value;
}

void Parameter::Set(float value) noexcept
{
    PackGetStruct(&*this->packetSetParameter, SV::SetStreamParameterPacket)->value = value;

    this->sliding = false;
}

void Parameter::SlideFromTo(float startValue, float endValue, uint32_t time) noexcept
{
    this->ratio = (endValue - startValue) / static_cast<float>(time);
    this->endTime = Timer::Get() + static_cast<Timer::time_t>(time);
    this->endValue = endValue;

    this->sliding = true;

    PackGetStruct(&*this->packetSetParameter, SV::SetStreamParameterPacket)->value = endValue;
}

void Parameter::SlideTo(float endValue, uint32_t time) noexcept
{
    const float startValue = this->Get();

    this->SlideFromTo(startValue, endValue, time);
}

void Parameter::Slide(float deltaValue, uint32_t time) noexcept
{
    const float startValue = this->Get();
    const float endValue = startValue + deltaValue;

    this->SlideFromTo(startValue, endValue, time);
}

void Parameter::Update() noexcept
{
    if (this->sliding)
    {
        if (Timer::Get() < this->endTime)
        {
            const Timer::time_t timeLeft = this->endTime - Timer::Get();
            const float startValue = this->endValue - static_cast<float>(timeLeft) * this->ratio;

            PackGetStruct(&*this->packetSlideParameter, SV::SlideStreamParameterPacket)->startvalue = startValue;
            PackGetStruct(&*this->packetSlideParameter, SV::SlideStreamParameterPacket)->endvalue = this->endValue;
            PackGetStruct(&*this->packetSlideParameter, SV::SlideStreamParameterPacket)->time = static_cast<uint32_t>(timeLeft);
        }
        else
        {
            this->sliding = false;
        }
    }
}

void Parameter::ApplyForPlayer(const uint16_t player) const
{
    const auto& activePacketRef = this->sliding
                                ? this->packetSlideParameter
                                : this->packetSetParameter;

    Network::SendControlPacket(player, *&*activePacketRef);
}
