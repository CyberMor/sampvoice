/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "BlurEffect.h"

#include <cassert>
#include <algorithm>

#include <game/RenderWare.h>

#include "Memory.hpp"
#include "Logger.h"

BlurEffect::BlurEffect(IDirect3DDevice9* const pDevice, const Resource& rEffect) : pDevice(pDevice)
{
    assert(pDevice != nullptr);

    if (const auto hResult = pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &this->pDeviceBackBuffer); FAILED(hResult))
    {
        Logger::LogToFile("[err:blureffect] : failed to get device back buffer (code:%ld)", hResult);
        throw std::exception();
    }

    {
        D3DSURFACE_DESC backBufferDesc {};

        if (const auto hResult = this->pDeviceBackBuffer->GetDesc(&backBufferDesc); FAILED(hResult))
        {
            Logger::LogToFile("[err:blureffect] : failed to get back buffer description (code:%ld)", hResult);
            this->pDeviceBackBuffer->Release();
            throw std::exception();
        }

        this->backBufferWidth = backBufferDesc.Width;
        this->backBufferHeight = backBufferDesc.Height;
    }

    {
        ID3DXBuffer* pErrorBuffer { nullptr };

        if (const auto hResult = D3DXCreateEffect(pDevice, rEffect.GetDataPtr(), rEffect.GetDataSize(),
            NULL, NULL, D3DXFX_DONOTSAVESTATE | D3DXFX_NOT_CLONEABLE | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY,
            NULL, &this->pEffect, &pErrorBuffer); FAILED(hResult))
        {
            Logger::LogToFile("[err:blureffect] : failed to create effect (code:%ld) (%.*s)",
                hResult, pErrorBuffer->GetBufferSize() - 1, pErrorBuffer->GetBufferPointer());
            this->pDeviceBackBuffer->Release();
            pErrorBuffer->Release();
            throw std::exception();
        }
    }

    const float iResolution[] = { this->backBufferWidth, this->backBufferHeight };

    this->pEffect->SetFloatArray(static_cast<D3DXHANDLE>("iResolution"), iResolution, SizeOfArray(iResolution));
    this->pEffect->SetFloat(static_cast<D3DXHANDLE>("iLevel"), 0.f);

    const D3DVERTEXELEMENT9 vertexElements[] =
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };

    if (const auto hResult = pDevice->CreateVertexDeclaration(vertexElements, &this->pVertexDeclaration); FAILED(hResult))
    {
        Logger::LogToFile("[err:blureffect] : failed to create vertex declaration (code:%ld)", hResult);
        this->pDeviceBackBuffer->Release();
        this->pEffect->Release();
        throw std::exception();
    }

    if (const auto hResult = D3DXCreateTexture(pDevice,
        this->backBufferWidth, this->backBufferHeight, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET,
        D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &this->pBackBufferTexture); FAILED(hResult))
    {
        Logger::LogToFile("[err:blureffect] : failed to create back-buffer texture (code:%ld)", hResult);
        this->pDeviceBackBuffer->Release();
        this->pEffect->Release();
        this->pVertexDeclaration->Release();
        throw std::exception();
    }

    if (const auto hResult = this->pBackBufferTexture->GetSurfaceLevel(0, &this->pBackBufferSurface); FAILED(hResult))
    {
        Logger::LogToFile("[err:blureffect] : failed to get back-buffer surface (code:%ld)", hResult);
        this->pDeviceBackBuffer->Release();
        this->pEffect->Release();
        this->pVertexDeclaration->Release();
        this->pBackBufferTexture->Release();
        throw std::exception();
    }

    if (const auto hResult = D3DXCreateTexture(pDevice, this->backBufferWidth,
        this->backBufferHeight, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP,
        D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &this->pTempBufferTexture); FAILED(hResult))
    {
        Logger::LogToFile("[err:blureffect] : failed to create temp-buffer texture (code:%ld)", hResult);
        this->pDeviceBackBuffer->Release();
        this->pEffect->Release();
        this->pVertexDeclaration->Release();
        this->pBackBufferSurface->Release();
        this->pBackBufferTexture->Release();
        throw std::exception();
    }

    if (const auto hResult = this->pTempBufferTexture->GetSurfaceLevel(0, &this->pTempBufferSurface); FAILED(hResult))
    {
        Logger::LogToFile("[err:blureffect] : failed to get temp-buffer surface (code:%ld)", hResult);
        this->pDeviceBackBuffer->Release();
        this->pEffect->Release();
        this->pVertexDeclaration->Release();
        this->pBackBufferSurface->Release();
        this->pBackBufferTexture->Release();
        this->pTempBufferTexture->Release();
        throw std::exception();
    }

    if (const auto hResult = D3DXCreateTexture(pDevice, this->backBufferWidth,
        this->backBufferHeight, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP,
        D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &this->pFrontBufferTexture); FAILED(hResult))
    {
        Logger::LogToFile("[err:blureffect] : failed to create front-buffer texture (code:%ld)", hResult);
        this->pDeviceBackBuffer->Release();
        this->pEffect->Release();
        this->pVertexDeclaration->Release();
        this->pBackBufferSurface->Release();
        this->pBackBufferTexture->Release();
        this->pTempBufferSurface->Release();
        this->pTempBufferTexture->Release();
        throw std::exception();
    }

    if (const auto hResult = this->pFrontBufferTexture->GetSurfaceLevel(0, &this->pFrontBufferSurface); FAILED(hResult))
    {
        Logger::LogToFile("[err:blureffect] : failed to get front-buffer surface (code:%ld)", hResult);
        this->pDeviceBackBuffer->Release();
        this->pEffect->Release();
        this->pVertexDeclaration->Release();
        this->pBackBufferSurface->Release();
        this->pBackBufferTexture->Release();
        this->pTempBufferSurface->Release();
        this->pTempBufferTexture->Release();
        this->pFrontBufferTexture->Release();
        throw std::exception();
    }
}

BlurEffect::~BlurEffect() noexcept
{
    this->pDeviceBackBuffer->Release();
    this->pEffect->Release();
    this->pVertexDeclaration->Release();
    this->pBackBufferSurface->Release();
    this->pBackBufferTexture->Release();
    this->pTempBufferSurface->Release();
    this->pTempBufferTexture->Release();
    this->pFrontBufferSurface->Release();
    this->pFrontBufferTexture->Release();
}

void BlurEffect::Render(const float level) const noexcept
{
    if (SUCCEEDED(this->pDevice->StretchRect(this->pDeviceBackBuffer, NULL, this->pBackBufferSurface, NULL, D3DTEXF_POINT)) &&
        SUCCEEDED(this->pEffect->SetFloat(static_cast<D3DXHANDLE>("iLevel"), std::clamp(level, 0.f, 100.f))) &&
        SUCCEEDED(this->pEffect->Begin(nullptr, NULL)))
    {
        if (SUCCEEDED(this->pEffect->SetTexture(static_cast<D3DXHANDLE>("iFrameTexture"), this->pBackBufferTexture)) &&
            SUCCEEDED(this->pDevice->SetRenderTarget(0, this->pTempBufferSurface)) &&
            SUCCEEDED(this->pEffect->BeginPass(0)))
        {
            this->Draw();
            this->pEffect->EndPass();
        }

        if (SUCCEEDED(this->pEffect->SetTexture(static_cast<D3DXHANDLE>("iFrameTexture"), this->pTempBufferTexture)) &&
            SUCCEEDED(this->pDevice->SetRenderTarget(0, this->pFrontBufferSurface)) &&
            SUCCEEDED(this->pEffect->BeginPass(1)))
        {
            this->Draw();
            this->pEffect->EndPass();
        }

        if (SUCCEEDED(this->pDevice->SetRenderTarget(0, this->pDeviceBackBuffer)) &&
            SUCCEEDED(this->pEffect->SetTexture(static_cast<D3DXHANDLE>("iBackBuffer"), this->pBackBufferTexture)) &&
            SUCCEEDED(this->pEffect->SetTexture(static_cast<D3DXHANDLE>("iFrameTexture"), this->pFrontBufferTexture)) &&
            SUCCEEDED(this->pEffect->BeginPass(2)))
        {
            this->Draw();
            this->pEffect->EndPass();
        }

        this->pEffect->End();
    }
}

void BlurEffect::Draw() const noexcept
{
    struct EffectVertex
    {
        float x, y, z;
        float tx0, tx1;
    };

    if (SUCCEEDED(this->pDevice->SetVertexDeclaration(this->pVertexDeclaration)))
    {
        const float screenWidth = this->backBufferWidth;
        const float screenHeight = this->backBufferHeight;

        const EffectVertex quad[] =
        {
            { 0.f,         0.f,          0.5f, 0.f, 0.f },
            { screenWidth, 0.f,          0.5f, 1.f, 0.f },
            { 0.f,         screenHeight, 0.5f, 0.f, 1.f },
            { screenWidth, screenHeight, 0.5f, 1.f, 1.f }
        };

        _rwD3D9DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(*quad));
    }
}
