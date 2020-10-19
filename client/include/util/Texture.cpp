/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Texture.h"

#include "Logger.h"

Texture::Texture(IDirect3DDevice9* const pDevice, const Resource& rTexture)
{
    if (const auto hResult = D3DXCreateTextureFromFileInMemory(pDevice,
        rTexture.GetDataPtr(), rTexture.GetDataSize(), &this->pTexture); FAILED(hResult))
    {
        Logger::LogToFile("[err:texture] : failed to create texture (code:%ld)", hResult);
        throw std::exception();
    }

    if (const auto hResult = D3DXCreateSprite(pDevice, &this->pSprite); FAILED(hResult))
    {
        Logger::LogToFile("[err:texture] : failed to create sprite (code:%ld)", hResult);
        this->pTexture->Release();
        throw std::exception();
    }

    D3DSURFACE_DESC textureInfo {};

    if (const auto hResult = this->pTexture->GetLevelDesc(0, &textureInfo); FAILED(hResult))
    {
        Logger::LogToFile("[err:texture] : failed to get texture info (code:%ld)", hResult);
        this->pTexture->Release();
        this->pSprite->Release();
        throw std::exception();
    }

    this->textureWidth = textureInfo.Width;
    this->textureHeight = textureInfo.Height;
}

Texture::~Texture() noexcept
{
    this->pTexture->Release();
    this->pSprite->Release();
}

IDirect3DTexture9* Texture::GetTexture() const noexcept
{
    return this->pTexture;
}

ID3DXSprite* Texture::GetSprite() const noexcept
{
    return this->pSprite;
}

UINT Texture::GetWidth() const noexcept
{
    return this->textureWidth;
}

UINT Texture::GetHeight() const noexcept
{
    return this->textureHeight;
}

void Texture::Draw(const float x, const float y, const float width, const float height,
                   const D3DCOLOR color, const float angle) const noexcept
{
    D3DXMATRIX matrix;

    D3DXMatrixTransformation2D(&matrix, nullptr, 0.f,
        &D3DXVECTOR2(width / static_cast<float>(this->textureWidth),
                     height / static_cast<float>(this->textureHeight)),
        &D3DXVECTOR2(width / 2.f, height / 2.f), angle, &D3DXVECTOR2(x, y));

    if (SUCCEEDED(this->pSprite->SetTransform(&matrix)) &&
        SUCCEEDED(this->pSprite->Begin(D3DXSPRITE_ALPHABLEND)))
    {
        this->pSprite->Draw(this->pTexture, nullptr, nullptr, nullptr, color);
        this->pSprite->End();
    }
}
