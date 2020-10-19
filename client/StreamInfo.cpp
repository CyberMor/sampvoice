#include "StreamInfo.h"

StreamInfo::StreamInfo(const StreamType type, const D3DCOLOR color, std::string name) noexcept
    : type(type), color(color), name(std::move(name)) {}

StreamType StreamInfo::GetType() const noexcept
{
    return this->type;
}

D3DCOLOR StreamInfo::GetColor() const noexcept
{
    return this->color;
}

const std::string& StreamInfo::GetName() const noexcept
{
    return this->name;
}
