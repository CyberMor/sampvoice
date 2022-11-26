#include "StreamInfo.h"

StreamInfo::StreamInfo(const StreamType type, const D3DCOLOR color, std::string name) noexcept
    : _type  { type }
    , _color { color }
    , _name  { std::move(name) }
{}

StreamType StreamInfo::GetType() const noexcept
{
    return _type;
}

D3DCOLOR StreamInfo::GetColor() const noexcept
{
    return _color;
}

const std::string& StreamInfo::GetName() const noexcept
{
    return _name;
}
