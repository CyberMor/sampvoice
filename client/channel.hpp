/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <functional>

#include <audio/bass.h>
#include <game/CVector.h>

#include <system/types.hpp>
#include <memory/structures/array.hpp>

#include "main.hpp"

// Constants
// ----------------------------------------------------------------

constexpr size_t kMaxChannels = 16;

// Channel
// ----------------------------------------------------------------

struct Channel {

    Channel() noexcept = default;
    ~Channel() noexcept
    {
        BASS_StreamFree(_handle);
    }

    Channel(const Channel&) = delete;
    Channel(Channel&&) = delete;
    Channel& operator=(const Channel&) = delete;
    Channel& operator=(Channel&&) = delete;

public:

    static constexpr bool Global = false;
    static constexpr bool  Local = true;

public:

    bool Initialize(const bool type) noexcept
    {
        BASS_StreamFree(_handle);

        _handle = BASS_StreamCreate(kFrequency, 1, BASS_SAMPLE_MONO |
            BASS_SAMPLE_FLOAT | (type == Local ? BASS_SAMPLE_3D | BASS_SAMPLE_MUTEMAX : 0),
            reinterpret_cast<STREAMPROC*>(OnProcess), this);
        if (_handle == NULL) return false;

        /*
            if (BASS_ChannelSetSync(_handle, BASS_SYNC_STALL | BASS_SYNC_MIXTIME, 0,
                reinterpret_cast<SYNCPROC*>(OnEvent), nullptr) == NULL)
                return false;
        */

        if (BASS_ChannelSetAttribute(_handle, BASS_ATTRIB_BUFFER, 0) == FALSE)
            return false;
        if (BASS_ChannelSetAttribute(_handle, BASS_ATTRIB_GRANULE, kFrameSamples) == FALSE)
            return false;

        return true;
    }

    void Deinitialize() noexcept
    {
        BASS_StreamFree(_handle);

        _handle = NULL;
    }

public:

    HSTREAM Handle() const noexcept
    {
        return _handle;
    }

public:

    bool SetVolume(const float volume) noexcept
    {
        return BASS_ChannelSetAttribute(_handle, BASS_ATTRIB_VOL, volume) == TRUE;
    }

    bool SetPanning(const float panning) noexcept
    {
        return BASS_ChannelSetAttribute(_handle, BASS_ATTRIB_PAN, panning) == TRUE;
    }

    bool SetDistance(const float distance) noexcept
    {
        return BASS_ChannelSet3DAttributes(_handle, BASS_3DMODE_NORMAL,
            distance * 0.1F, distance, -1, -1, -1) == TRUE;
    }

    bool SetPosition(const CVector& position) noexcept
    {
        static_assert(sizeof(float[3]) == sizeof(CVector));
        static_assert(sizeof(float[3]) == sizeof(BASS_3DVECTOR));

        return BASS_ChannelSet3DPosition(_handle,
            reinterpret_cast<const BASS_3DVECTOR*>(&position),
            nullptr, nullptr) == TRUE;
    }

public:

    bool Play() noexcept
    {
        return BASS_ChannelPlay(_handle, TRUE) == TRUE;
    }

    bool Stop() noexcept
    {
        return BASS_ChannelStop(_handle) == TRUE;
    }

private:

    /*
        static void CALLBACK OnEvent(const HSYNC, const DWORD handle,
            const DWORD status, Channel* const channel) noexcept
        {
            if (status == 0)
            {
                BASS_ChannelStop(handle);

                if (channel->OnStop != nullptr)
                    channel->OnStop();
            }
        }
    */

    static DWORD CALLBACK OnProcess(const HSTREAM, float* buffer,
        DWORD length, Channel* const channel) noexcept
    {
        DWORD result = 0;

        if (channel->OnFrame != nullptr)
        {
            while (length >= kFrameBytes && channel->OnFrame(buffer))
            {
                result += kFrameBytes;
                buffer += kFrameSamples;
                length -= kFrameBytes;
            }
        }

        return result;
    }

private:

    HSTREAM _handle = NULL;

public:

    std::function<bool(float*)> OnFrame;
    // std::function<void()>    OnStop;

};

// Channel Pool
// ----------------------------------------------------------------

struct ChannelPool {

    ChannelPool(const ChannelPool&) = delete;
    ChannelPool(ChannelPool&&) = delete;
    ChannelPool& operator=(const ChannelPool&) = delete;
    ChannelPool& operator=(ChannelPool&&) = delete;

private:

    ChannelPool() noexcept = default;
    ~ChannelPool() noexcept = default;

public:

    static ChannelPool& Instance() noexcept
    {
        static ChannelPool instance;
        return instance;
    }

public:

    bool Initialize() noexcept
    {
        for (size_t index = 0; index != kMaxChannels / 4; ++index)
        {
            if (!_channels[index].Initialize(Channel::Global))
                return false;
        }
        for (size_t index = kMaxChannels / 4; index != kMaxChannels; ++index)
        {
            if (!_channels[index].Initialize(Channel::Local))
                return false;
        }

        return true;
    }

    void Deinitialize() noexcept
    {
        for (size_t index = 0; index != kMaxChannels; ++index)
        {
            _channels[index].Deinitialize();
        }
    }

public:

    size_t Acquire(const bool type) noexcept
    {
        for (size_t index = (type ? kMaxChannels / 4 : 0); index != (type ? kMaxChannels : kMaxChannels / 4); ++index)
            if (_owners[index] == false) { _owners[index] = true; return index; }
        return None<size_t>;
    }

    void Release(const size_t index) noexcept
    {
        _owners[index] = false;
    }

public:

    Channel& operator[](const size_t index) noexcept
    {
        return _channels[index];
    }

public:

    bool HasOwner(const size_t index) const noexcept
    {
        return _owners[index] == true;
    }

private:

    Array<bool, kMaxChannels> _owners = false;
    Array<Channel, kMaxChannels> _channels;

};
