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

class BlurEffect {

    BlurEffect() = delete;
    BlurEffect(const BlurEffect&) = delete;
    BlurEffect(BlurEffect&&) = delete;
    BlurEffect& operator=(const BlurEffect&) = delete;
    BlurEffect& operator=(BlurEffect&&) = delete;

public:

    explicit BlurEffect(IDirect3DDevice9* pDevice,
                        const Resource& rEffect);

    ~BlurEffect() noexcept;

public:

    void Render(float level) const noexcept;

private:

    void Draw() const noexcept;

private:

    IDirect3DDevice9* const pDevice;

    IDirect3DSurface9* pDeviceBackBuffer { nullptr };

    UINT backBufferWidth { 0 };
    UINT backBufferHeight { 0 };

    ID3DXEffect* pEffect { nullptr };
    IDirect3DVertexDeclaration9* pVertexDeclaration { nullptr };

    IDirect3DTexture9* pBackBufferTexture { nullptr };
    IDirect3DSurface9* pBackBufferSurface { nullptr };
    IDirect3DTexture9* pTempBufferTexture { nullptr };
    IDirect3DSurface9* pTempBufferSurface { nullptr };
    IDirect3DTexture9* pFrontBufferTexture { nullptr };
    IDirect3DSurface9* pFrontBufferSurface { nullptr };

};

using BlurEffectPtr = std::unique_ptr<BlurEffect>;
#define MakeBlurEffect std::make_unique<BlurEffect>
