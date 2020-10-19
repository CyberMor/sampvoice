#include "Effect.h"

#include <util/Logger.h>

Effect::Effect(const DWORD type, const int priority,
               const void* const paramPtr, const DWORD paramSize)
    : type(type), priority(priority), params(paramSize)
{
    std::memcpy(this->params.data(), paramPtr, paramSize);
}

Effect::~Effect() noexcept
{
    for (const auto& fxHandle : this->fxHandles)
    {
        BASS_ChannelRemoveFX(fxHandle.first, fxHandle.second);
    }
}

void Effect::Apply(const Channel& channel)
{
    if (const auto fxHandle = BASS_ChannelSetFX(channel.GetHandle(),
        this->type, this->priority); fxHandle != NULL)
    {
        if (BASS_FXSetParameters(fxHandle, this->params.data()) == FALSE)
        {
            Logger::LogToFile("[sv:err:effect:apply] : failed "
                "to set parameters (code:%d)", BASS_ErrorGetCode());
            BASS_ChannelRemoveFX(channel.GetHandle(), fxHandle);
        }
        else
        {
            this->fxHandles[channel.GetHandle()] = fxHandle;
        }
    }
    else
    {
        Logger::LogToFile("[sv:err:effect:apply] : failed to create "
            "effect (code:%d)", BASS_ErrorGetCode());
    }
}
