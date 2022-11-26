#include "StreamAtObject.h"

#include <audio/bass.h>
#include <samp/CNetGame.h>
#include <game/CObject.h>

#include "StreamInfo.h"

StreamAtObject::StreamAtObject(const D3DCOLOR color, std::string name, const float distance, const WORD object_id) noexcept
    : LocalStream { StreamType::LocalStreamAtObject, color, std::move(name), distance }
    , _object_id  { object_id }
{}

void StreamAtObject::Tick() noexcept
{
    LocalStream::Tick();

    if (const auto net_game = SAMP::pNetGame(); net_game != nullptr)
    {
        if (const auto object_pool = net_game->GetObjectPool(); object_pool != nullptr)
        {
            if (const auto object = object_pool->m_pObject[object_id]; object != nullptr)
            {
                if (const auto object_entity = object->m_pGameEntity; object_entity != nullptr)
                {
                    if (const auto object_matrix = object_entity->GetMatrix(); object_matrix != nullptr)
                    {
                        for (const auto& channel : GetChannels())
                        {
                            if (channel.HasSpeaker())
                            {
                                BASS_ChannelSet3DPosition(channel.GetHandle(),
                                    reinterpret_cast<BASS_3DVECTOR*>(&object_matrix->pos),
                                    nullptr, nullptr);
                            }
                        }
                    }
                }
            }
        }
    }
}

void StreamAtObject::OnChannelCreate(const Channel& channel) noexcept
{
    static const BASS_3DVECTOR kZeroVector { 0, 0, 0 };

    LocalStream::OnChannelCreate(channel);

    if (const auto net_game = SAMP::pNetGame(); net_game != nullptr)
    {
        if (const auto object_pool = net_game->GetObjectPool(); object_pool != nullptr)
        {
            if (const auto object = object_pool->m_pObject[_object_id]; object != nullptr)
            {
                if (const auto object_entity = object->m_pGameEntity; object_entity != nullptr)
                {
                    if (const auto object_matrix = object_entity->GetMatrix(); object_matrix != nullptr)
                    {
                        BASS_ChannelSet3DPosition(channel.GetHandle(),
                            reinterpret_cast<BASS_3DVECTOR*>(&object_matrix->pos),
                            &kZeroVector, &kZeroVector);
                    }
                }
            }
        }
    }
}
