#include "StreamAtObject.h"

#include <audio/bass.h>
#include <samp/CNetGame.h>
#include <game/CObject.h>

#include "StreamInfo.h"

StreamAtObject::StreamAtObject(const D3DCOLOR color, std::string name,
                               const float distance, const WORD objectId) noexcept
    : LocalStream(StreamType::LocalStreamAtObject, color, std::move(name), distance)
    , objectId(objectId)
{}

void StreamAtObject::Tick() noexcept
{
    this->LocalStream::Tick();

    const auto pNetGame = SAMP::pNetGame();
    if (!pNetGame) return;

    const auto pObjectPool = pNetGame->GetObjectPool();
    if (!pObjectPool) return;

    const auto pObject = pObjectPool->m_pObject[this->objectId];
    if (!pObject) return;

    const auto pObjectEntity = pObject->m_pGameEntity;
    if (!pObjectEntity) return;

    const auto pObjectMatrix = pObjectEntity->GetMatrix();
    if (!pObjectMatrix) return;

    for (const auto& channel : this->GetChannels())
    {
        if (channel->HasSpeaker())
        {
            BASS_ChannelSet3DPosition(channel->GetHandle(),
                reinterpret_cast<BASS_3DVECTOR*>(&pObjectMatrix->pos),
                nullptr, nullptr);
        }
    }
}

void StreamAtObject::OnChannelCreate(const Channel& channel) noexcept
{
    static const BASS_3DVECTOR kZeroVector { 0, 0, 0 };

    this->LocalStream::OnChannelCreate(channel);

    const auto pNetGame = SAMP::pNetGame();
    if (!pNetGame) return;

    const auto pObjectPool = pNetGame->GetObjectPool();
    if (!pObjectPool) return;

    const auto pObject = pObjectPool->m_pObject[this->objectId];
    if (!pObject) return;

    const auto pObjectEntity = pObject->m_pGameEntity;
    if (!pObjectEntity) return;

    const auto pObjectMatrix = pObjectEntity->GetMatrix();
    if (!pObjectMatrix) return;

    BASS_ChannelSet3DPosition(channel.GetHandle(),
        reinterpret_cast<BASS_3DVECTOR*>(&pObjectMatrix->pos),
        &kZeroVector, &kZeroVector);
}
