/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <system/types.hpp>
#include <memory/structures/array.hpp>
#include <memory/structures/pool.hpp>

#include <audio/bass.h>
#include <samp/CNetGame.h>
#include <game/CVehicle.h>
#include <game/CPed.h>
#include <game/CObject.h>

#include "channel.hpp"
#include "effect.hpp"
#include "source.hpp"

// Target
// ----------------------------------------------------------------

constexpr uword_t kVehicle = 1;
constexpr uword_t kPlayer  = 2;
constexpr uword_t kObject  = 3;

// ----------------------------------------------------------------

constexpr uword_t GetTargetType(const uword_t target) noexcept
{
    return target >> 14;
}

constexpr uword_t GetTargetIndex(const uword_t target) noexcept
{
    return target & 0x3FFF;
}

// ----------------------------------------------------------------

constexpr uword_t MakeTarget(const uword_t type, const uword_t index) noexcept
{
    return (type << 14) | (index & 0x3FFF);
}

// Stream
// ----------------------------------------------------------------

extern Array<struct Source, kMaxPlayers> gSources;

extern Pool<struct Stream, kMaxStreams> gStreams;
extern Pool<struct Effect, kMaxEffects> gEffects;

// ----------------------------------------------------------------

struct Stream {

    Stream() = delete;
    ~Stream() noexcept
    {
        for (size_t channel = 0; channel != kMaxChannels; ++channel)
        {
            if (_table[channel] != None<uword_t>)
            {
                ChannelPool::Instance()[channel].Stop();

                if (effect != None<uword_t>)
                {
                    if (gEffects.Acquire<0>(effect))
                    {
                        gEffects[effect].Detach(ChannelPool::Instance()[channel].Handle());
                    }
                }

                ChannelPool::Instance().Release(channel);
            }
        }
    }

    Stream(const Stream&) = delete;
    Stream(Stream&&) = delete;
    Stream& operator=(const Stream&) = delete;
    Stream& operator=(Stream&&) = delete;

public:

    Stream(const fdword_t distance) noexcept
        : distance { distance }
    {}

public:

    void SetVolume(const fdword_t volume) noexcept
    {
        this->volume = volume;

        ForEach([=](const size_t channel, const uword_t) noexcept -> void
        {
            ChannelPool::Instance()[channel].SetVolume(volume);
        });
    }

    void SetPanning(const fdword_t panning) noexcept
    {
        this->panning = panning;

        ForEach([=](const size_t channel, const uword_t) noexcept -> void
        {
            ChannelPool::Instance()[channel].SetPanning(panning);
        });
    }

    void SetDistance(const fdword_t distance) noexcept
    {
        this->distance = distance;

        ForEach([=](const size_t channel, const uword_t) noexcept -> void
        {
            ChannelPool::Instance()[channel].SetDistance(distance);
        });
    }

    void SetPosition(const CVector& position) noexcept
    {
        this->position = position;

        ForEach([&](const size_t channel, const uword_t) noexcept -> void
        {
            ChannelPool::Instance()[channel].SetPosition(position);
        });
    }

    void SetEffect(const uword_t effect) noexcept
    {
        if (this->effect != None<uword_t>)
        {
            if (gEffects.Acquire<0>(this->effect))
            {
                ForEach([=](const size_t channel, const uword_t) noexcept -> void
                {
                    gEffects[this->effect].Detach(ChannelPool::Instance()[channel].Handle());
                });
            }
        }

        this->effect = effect;

        if (this->effect != None<uword_t>)
        {
            if (gEffects.Acquire<0>(this->effect))
            {
                ForEach([=](const size_t channel, const uword_t) noexcept -> void
                {
                    gEffects[this->effect].Attach(ChannelPool::Instance()[channel].Handle());
                });
            }
        }
    }

public:

    bool Play(const uword_t stream, const uword_t source, const udword_t packet) noexcept
    {
        for (size_t channel = 0; channel != kMaxChannels; ++channel)
            if (_table[channel] == source) return false;

        const size_t channel = ChannelPool::Instance().Acquire(distance != 0);
        if (channel == None<size_t>) return false;

        _packet[channel] = packet;

        ChannelPool::Instance()[channel].OnFrame = std::bind
            (&Source::PopFrame, &gSources[source], std::ref(_packet[channel]), stream, std::placeholders::_1);

        ChannelPool::Instance()[channel].SetVolume(volume);

        if (distance != 0)
        {
            ChannelPool::Instance()[channel].SetDistance(distance);
            ChannelPool::Instance()[channel].SetPosition(position);
        }
        else
        {
            ChannelPool::Instance()[channel].SetPanning(panning);
        }

        if (effect != None<uword_t>)
        {
            if (gEffects.Acquire<0>(effect))
            {
                gEffects[effect].Attach(ChannelPool::Instance()[channel].Handle());
            }
        }

        _table[channel] = source;

        return ChannelPool::Instance()[channel].Play();
    }

    bool Stop(const uword_t source) noexcept
    {
        for (size_t channel = 0; channel != kMaxChannels; ++channel)
        {
            if (_table[channel] == source)
            {
                ChannelPool::Instance()[channel].Stop();

                _packet[channel] = 0;

                if (effect != None<uword_t>)
                {
                    if (gEffects.Acquire<0>(effect))
                    {
                        gEffects[effect].Detach(ChannelPool::Instance()[channel].Handle());
                    }
                }

                ChannelPool::Instance().Release(channel);

                _table[channel] = None<uword_t>;

                return true;
            }
        }

        return false;
    }

public:

    bool UpdatePosition() noexcept
    {
        static_assert(sizeof(float[3]) == sizeof(CVector));
        static_assert(sizeof(float[3]) == sizeof(BASS_3DVECTOR));

        const uword_t type  = GetTargetType  (target);
        const uword_t index = GetTargetIndex (target);

        switch (type)
        {
            case kVehicle:
            {
                if (const auto pNetGame = SAMP::pNetGame(); pNetGame != nullptr)
                    if (const auto pVehiclePool = pNetGame->GetVehiclePool(); pVehiclePool != nullptr)
                        if (const auto pVehicle = pVehiclePool->m_pGameObject[index]; pVehicle != nullptr)
                            if (const auto pVehicleMatrix = pVehicle->GetMatrix(); pVehicleMatrix != nullptr)
                                return SetPosition(pVehicleMatrix->pos), true;
                break;
            }
            case kPlayer:
            {
                if (const auto pNetGame = SAMP::pNetGame(); pNetGame != nullptr)
                    if (const auto pPlayerPool = pNetGame->GetPlayerPool(); pPlayerPool != nullptr)
                        if (const auto pPlayer = pPlayerPool->GetPlayer(index); pPlayer != nullptr)
                            if (const auto pPlayerPed = pPlayer->m_pPed; pPlayerPed != nullptr)
                                if (const auto pPlayerGamePed = pPlayerPed->m_pGamePed; pPlayerGamePed != nullptr)
                                    if (const auto pPlayerMatrix = pPlayerGamePed->GetMatrix(); pPlayerMatrix != nullptr)
                                        return SetPosition(pPlayerMatrix->pos), true;
                break;
            }
            case kObject:
            {
                if (const auto pNetGame = SAMP::pNetGame(); pNetGame != nullptr)
                    if (const auto pObjectPool = pNetGame->GetObjectPool(); pObjectPool != nullptr)
                        if (const auto pObject = pObjectPool->m_pObject[index]; pObject != nullptr)
                            if (const auto pObjectEntity = pObject->m_pGameEntity; pObjectEntity != nullptr)
                                if (const auto pObjectMatrix = pObjectEntity->GetMatrix(); pObjectMatrix != nullptr)
                                    return SetPosition(pObjectMatrix->pos), true;
                break;
            }
        }

        return false;
    }

public:

    void Reset() noexcept
    {
        for (size_t channel = 0; channel != kMaxChannels; ++channel)
        {
            if (_table[channel] != None<uword_t>)
            {
                ChannelPool::Instance()[channel].Stop();

                if (effect != None<uword_t>)
                {
                    if (gEffects.Acquire<0>(effect))
                    {
                        gEffects[effect].Detach(ChannelPool::Instance()[channel].Handle());
                    }
                }

                ChannelPool::Instance().Release(channel);

                _table[channel] = None<uword_t>;
            }
        }
    }

public:

    /*
        Callback = void(size_t channel, uword_t source)
    */
    template <class Callback>
    void ForEach(Callback&& callback) const noexcept
    {
        for (size_t channel = 0; channel != kMaxChannels; ++channel)
        {
            if (_table[channel] != None<uword_t>)
            {
                callback(channel, _table[channel]);
            }
        }
    }

public:

    fdword_t volume = 1;
    fdword_t panning = 0;

    fdword_t distance;
    CVector  position;

    uword_t target = Zero<uword_t>;
    uword_t effect = None<uword_t>;

    std::string icon;

private:

    Array<uword_t, kMaxChannels> _table = None<uword_t>;
    Array<udword_t, kMaxChannels> _packet = Zero<udword_t>;

};
