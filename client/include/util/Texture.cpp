/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Texture.h"

#include <cassert>

#include "Logger.h"

Texture::~Texture() noexcept
{
    if (_texture != nullptr) _texture->Release();
    if (_sprite != nullptr) _sprite->Release();
}

Texture::Texture(Texture&& object) noexcept
    : _texture { object._texture }
    , _sprite  { object._sprite }
    , _width   { object._width }
    , _height  { object._height }
{
    object._texture = nullptr;
    object._sprite  = nullptr;
    object._width   = 0;
    object._height  = 0;
}

Texture& Texture::operator=(Texture&& object) noexcept
{
    if (&object != this)
    {
        if (_texture != nullptr) _texture->Release();
        if (_sprite != nullptr) _sprite->Release();

        _texture = object._texture;
        _sprite  = object._sprite;
        _width   = object._width;
        _height  = object._height;

        object._texture = nullptr;
        object._sprite  = nullptr;
        object._width   = 0;
        object._height  = 0;
    }

    return *this;
}

Texture::Texture(IDirect3DDevice9* const device, const Resource& resource) noexcept
{
    assert(device != nullptr);

    if (const auto result = D3DXCreateTextureFromFileInMemory(device,
        resource.GetDataPtr(), resource.GetDataSize(), &_texture); FAILED(result))
    {
        Logger::LogToFile("[err:texture] : failed to create texture (code:%ld)", result);
        return;
    }

    if (const auto result = D3DXCreateSprite(device, &_sprite); FAILED(result))
    {
        Logger::LogToFile("[err:texture] : failed to create sprite (code:%ld)", result);
        _texture->Release();
        _texture = nullptr;
        return;
    }

    D3DSURFACE_DESC texture_info;

    if (const auto result = _texture->GetLevelDesc(0, &texture_info); FAILED(result))
    {
        Logger::LogToFile("[err:texture] : failed to get texture info (code:%ld)", result);
        _texture->Release();
        _texture = nullptr;
        _sprite->Release();
        _sprite = nullptr;
        return;
    }

    _width  = texture_info.Width;
    _height = texture_info.Height;
}

bool Texture::Valid() const noexcept
{
    return _texture != nullptr &&
           _sprite  != nullptr &&
           _width   != 0       &&
           _height  != 0       ;
}

IDirect3DTexture9* Texture::GetTexture() const noexcept
{
    assert(_texture != nullptr);
    return _texture;
}

ID3DXSprite* Texture::GetSprite() const noexcept
{
    assert(_sprite != nullptr);
    return _sprite;
}

UINT Texture::GetWidth() const noexcept
{
    assert(_width != 0);
    return _width;
}

UINT Texture::GetHeight() const noexcept
{
    assert(_height != 0);
    return _height;
}

void Texture::Draw(const float x, const float y, const float width, const float height,
                   const D3DCOLOR color, const float angle) noexcept
{
    assert(_texture != nullptr);
    assert(_sprite  != nullptr);
    assert(_width   != 0);
    assert(_height  != 0);

    D3DXMATRIX matrix;

    D3DXMatrixTransformation2D(&matrix, nullptr, 0.f,
        &D3DXVECTOR2(width / static_cast<float>(_width), height / static_cast<float>(_height)),
        &D3DXVECTOR2(width / 2.f, height / 2.f), angle, &D3DXVECTOR2(x, y));

    if (SUCCEEDED(_sprite->SetTransform(&matrix)) && SUCCEEDED(_sprite->Begin(D3DXSPRITE_ALPHABLEND)))
    {
        _sprite->Draw(_texture, nullptr, nullptr, nullptr, color);
        _sprite->End();
    }
}
