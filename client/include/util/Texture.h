/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "Resource.h"

struct Texture {

    Texture() noexcept = default;
    ~Texture() noexcept;
    Texture(const Texture&) = delete;
    Texture(Texture&& object) noexcept;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&& object) noexcept;

public:

    Texture(IDirect3DDevice9* device, const Resource& resource) noexcept;

public:

    bool Valid() const noexcept;

public:

    IDirect3DTexture9* GetTexture() const noexcept;
    ID3DXSprite* GetSprite() const noexcept;

    UINT GetWidth() const noexcept;
    UINT GetHeight() const noexcept;

public:

    void Draw(float x, float y, float width, float height, D3DCOLOR color, float angle) noexcept;

private:

    IDirect3DTexture9* _texture = nullptr;
    ID3DXSprite*       _sprite  = nullptr;

    UINT               _width   = 0;
    UINT               _height  = 0;

};
