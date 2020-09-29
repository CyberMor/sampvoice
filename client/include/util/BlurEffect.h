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
                        D3DPRESENT_PARAMETERS* pParameters,
                        const Resource& rEffect);

    ~BlurEffect() noexcept;

public:

    void Render(float level) const noexcept;

private:

    void Draw() const noexcept;

private:

    IDirect3DDevice9* pDevice { nullptr };
    D3DPRESENT_PARAMETERS dParameters {};

    ID3DXEffect* pEffect { nullptr };
    IDirect3DVertexDeclaration9* pVertexDeclaration { nullptr };

    IDirect3DTexture9* pBackBufferTexture { nullptr };
    IDirect3DSurface9* pBackBufferSurface { nullptr };

    IDirect3DTexture9* pTempBufferTexture { nullptr };
    IDirect3DSurface9* pTempBufferSurface { nullptr };

    IDirect3DTexture9* pFrontBufferTexture { nullptr };
    IDirect3DSurface9* pFrontBufferSurface { nullptr };

};

using BlurEffectPtr = std::shared_ptr<BlurEffect>;
#define MakeBlurEffect std::make_shared<BlurEffect>
