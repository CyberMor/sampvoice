#include "GlobalStream.h"

#include "StreamInfo.h"

GlobalStream::GlobalStream(const D3DCOLOR color, std::string name) noexcept
    : Stream(NULL, StreamType::GlobalStream, color, std::move(name)) {}
