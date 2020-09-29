/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Texture.h"

#include <cassert>

#include "Logger.h"

Texture::Texture(IDirect3DDevice9* pDevice, const Resource& rTexture)
{
    assert(pDevice);

    if (const HRESULT hResult = D3DXCreateTextureFromFileInMemory(pDevice,
        rTexture.GetDataPtr(), rTexture.GetDataSize(), &this->pTexture); FAILED(hResult))
    {
        Logger::LogToFile("[err:texture] : failed to create texture (code:%ld)", hResult);
        throw std::exception();
    }

    if (const HRESULT hResult = this->pTexture->GetSurfaceLevel(0, &this->pSurface); FAILED(hResult))
    {
        Logger::LogToFile("[err:texture] : failed to get surface (code:%ld)", hResult);
        this->pTexture->Release();
        throw std::exception();
    }

    if (const HRESULT hResult = D3DXCreateSprite(pDevice, &this->pSprite); FAILED(hResult))
    {
        Logger::LogToFile("[err:texture] : failed to create sprite (code:%ld)", hResult);
        this->pSurface->Release();
        this->pTexture->Release();
        throw std::exception();
    }

    if (const HRESULT hResult = this->pTexture->GetLevelDesc(0, &this->textureInfo); FAILED(hResult))
    {
        Logger::LogToFile("[err:texture] : failed to get texture info (code:%ld)", hResult);
        this->pSurface->Release();
        this->pTexture->Release();
        this->pSprite->Release();
        throw std::exception();
    }
}

Texture::~Texture() noexcept
{
    this->pSurface->Release();
    this->pTexture->Release();
    this->pSprite->Release();
}

IDirect3DTexture9* Texture::GetTexture() const noexcept
{
    return this->pTexture;
}

IDirect3DSurface9* Texture::GetSurface() const noexcept
{
    return this->pSurface;
}

ID3DXSprite* Texture::GetSprite() const noexcept
{
    return this->pSprite;
}

int Texture::GetWidth() const noexcept
{
    return this->textureInfo.Width;
}

int Texture::GetHeight() const noexcept
{
    return this->textureInfo.Height;
}

void Texture::Draw(const float x, const float y,
                   const float width, const float height,
                   const D3DCOLOR color, const float angle) const noexcept
{
    D3DXMATRIX matrix;

    D3DXMatrixTransformation2D(
        &matrix, nullptr, 0.f, &D3DXVECTOR2(
            width / static_cast<float>(this->textureInfo.Width),
            height / static_cast<float>(this->textureInfo.Height)
        ), &D3DXVECTOR2(width / 2.f, height / 2.f),
        angle, &D3DXVECTOR2(x, y)
    );

    this->pSprite->SetTransform(&matrix);

    if (SUCCEEDED(this->pSprite->Begin(D3DXSPRITE_ALPHABLEND)))
    {
        this->pSprite->Draw(this->pTexture, nullptr, nullptr, nullptr, color);
        this->pSprite->End();
    }
}
