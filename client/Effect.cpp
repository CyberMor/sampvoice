#include "Effect.h"

#include <util/Logger.h>

Effect::~Effect() noexcept
{
    for (const auto& fx_handle : _fx_handles)
    {
        BASS_ChannelRemoveFX(fx_handle.first, fx_handle.second);
    }
}

Effect::Effect(const DWORD type, const int priority,
               const void* const param_data, const DWORD param_size)
    : _type     { type }
    , _priority { priority }
    , _params   ( param_size )
{
    std::memcpy(_params.data(), param_data, param_size);
}

void Effect::Apply(const Channel& channel)
{
    if (const auto fx_handle  = BASS_ChannelSetFX(channel.GetHandle(), _type, _priority);
                   fx_handle != NULL)
    {
        if (BASS_FXSetParameters(fx_handle, _params.data()) == FALSE)
        {
            Logger::LogToFile("[sv:err:effect:apply] : failed to set parameters (code:%d)",
                BASS_ErrorGetCode());
            BASS_ChannelRemoveFX(channel.GetHandle(), fx_handle);
        }
        else
        {
            _fx_handles[channel.GetHandle()] = fx_handle;
        }
    }
    else
    {
        Logger::LogToFile("[sv:err:effect:apply] : failed to create effect (code:%d)",
            BASS_ErrorGetCode());
    }
}
