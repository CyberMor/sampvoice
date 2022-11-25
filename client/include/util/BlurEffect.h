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

struct BlurEffect {

    BlurEffect() noexcept = default;
    ~BlurEffect() noexcept;
    BlurEffect(const BlurEffect&) = delete;
    BlurEffect(BlurEffect&& object) noexcept;
    BlurEffect& operator=(const BlurEffect&) = delete;
    BlurEffect& operator=(BlurEffect&& object) noexcept;

public:

    BlurEffect(IDirect3DDevice9* device, const Resource& resource) noexcept;

public:

    bool Valid() const noexcept;

public:

    void Render(float level) noexcept;

private:

    void Draw() noexcept;

private:

    IDirect3DDevice9*            _device               = nullptr;
    IDirect3DSurface9*           _device_backbuffer    = nullptr;

    UINT                         _backbuffer_width     = 0;
    UINT                         _backbuffer_height    = 0;

    ID3DXEffect*                 _effect               = nullptr;
    IDirect3DVertexDeclaration9* _vertex_declaration   = nullptr;

    IDirect3DTexture9*           _backbuffer_texture   = nullptr;
    IDirect3DSurface9*           _backbuffer_surface   = nullptr;
    IDirect3DTexture9*           _temp_buffer_texture  = nullptr;
    IDirect3DSurface9*           _temp_buffer_surface  = nullptr;
    IDirect3DTexture9*           _front_buffer_texture = nullptr;
    IDirect3DSurface9*           _front_buffer_surface = nullptr;

};
