/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>

#include <d3d9.h>
#include <d3dx9.h>

#include "Resource.h"

class Texture {

    Texture() = delete;
    Texture(const Texture&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = delete;

public:

    explicit Texture(IDirect3DDevice9* pDevice,
                     const Resource& rTexture);

    ~Texture() noexcept;

public:

    IDirect3DTexture9* GetTexture() const noexcept;
    ID3DXSprite* GetSprite() const noexcept;
    UINT GetWidth() const noexcept;
    UINT GetHeight() const noexcept;

public:

    void Draw(float x, float y, float width, float height,
              D3DCOLOR color, float angle) const noexcept;

private:

    IDirect3DTexture9* pTexture { nullptr };
    ID3DXSprite* pSprite { nullptr };
    UINT textureWidth { 0 };
    UINT textureHeight { 0 };

};

using TexturePtr = std::unique_ptr<Texture>;
#define MakeTexture std::make_unique<Texture>
