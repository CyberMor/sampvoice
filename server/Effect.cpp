#include "Effect.h"

#include <functional>

#include "Stream.h"
#include "Network.h"
#include "Header.h"

Effect::~Effect()
{
    for (const auto stream : this->attachedStreams)
    {
        {
            const auto iter = this->streamPlayerCallbacks.find(stream);
            if (iter != this->streamPlayerCallbacks.end())
            {
                stream->RemovePlayerCallback(iter->second);
            }
        }

        {
            const auto iter = this->streamDeleteCallbacks.find(stream);
            if (iter != this->streamDeleteCallbacks.end())
            {
                stream->RemoveDeleteCallback(iter->second);
            }
        }

        PackGetStruct(&*this->packetDeleteEffect, SV::DeleteEffectPacket)->stream
            = reinterpret_cast<uint32_t>(stream);

        stream->SendControlPacket(*&*this->packetDeleteEffect);
    }
}

void Effect::AttachStream(Stream* const stream)
{
    if (this->attachedStreams.insert(stream).second)
    {
        this->streamPlayerCallbacks[stream] =
            stream->AddPlayerCallback(std::bind(&Effect::PlayerCallback,
                this, std::placeholders::_1, std::placeholders::_2));

        this->streamDeleteCallbacks[stream] =
            stream->AddDeleteCallback(std::bind(&Effect::DeleteCallback,
                this, std::placeholders::_1));

        PackGetStruct(&*this->packetCreateEffect, SV::CreateEffectPacket)->stream
            = reinterpret_cast<uint32_t>(stream);

        stream->SendControlPacket(*&*this->packetCreateEffect);
    }
}

void Effect::DetachStream(Stream* const stream)
{
    if (this->attachedStreams.erase(stream))
    {
        {
            const auto iter = this->streamPlayerCallbacks.find(stream);
            if (iter != this->streamPlayerCallbacks.end())
            {
                stream->RemovePlayerCallback(iter->second);
                this->streamPlayerCallbacks.erase(iter);
            }
        }

        {
            const auto iter = this->streamDeleteCallbacks.find(stream);
            if (iter != this->streamDeleteCallbacks.end())
            {
                stream->RemoveDeleteCallback(iter->second);
                this->streamDeleteCallbacks.erase(iter);
            }
        }

        PackGetStruct(&*this->packetDeleteEffect, SV::DeleteEffectPacket)->stream
            = reinterpret_cast<uint32_t>(stream);

        stream->SendControlPacket(*&*this->packetDeleteEffect);
    }
}

void Effect::PlayerCallback(Stream* const stream, const uint16_t player)
{
    PackGetStruct(&*this->packetCreateEffect, SV::CreateEffectPacket)->stream
        = reinterpret_cast<uint32_t>(stream);

    Network::SendControlPacket(player, *&*this->packetCreateEffect);
}

void Effect::DeleteCallback(Stream* const stream)
{
    this->attachedStreams.erase(stream);

    this->streamPlayerCallbacks.erase(stream);
    this->streamDeleteCallbacks.erase(stream);
}
