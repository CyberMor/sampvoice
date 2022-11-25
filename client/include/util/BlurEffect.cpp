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

BlurEffect::~BlurEffect() noexcept
{
    if (_device_backbuffer    != nullptr) _device_backbuffer->Release();
    if (_effect               != nullptr) _effect->Release();
    if (_vertex_declaration   != nullptr) _vertex_declaration->Release();
    if (_backbuffer_surface   != nullptr) _backbuffer_surface->Release();
    if (_backbuffer_texture   != nullptr) _backbuffer_texture->Release();
    if (_temp_buffer_surface  != nullptr) _temp_buffer_surface->Release();
    if (_temp_buffer_texture  != nullptr) _temp_buffer_texture->Release();
    if (_front_buffer_surface != nullptr) _front_buffer_surface->Release();
    if (_front_buffer_texture != nullptr) _front_buffer_texture->Release();
}

BlurEffect::BlurEffect(BlurEffect&& object) noexcept
    : _device               { object._device }
    , _device_backbuffer    { object._device_backbuffer }
    , _backbuffer_width     { object._backbuffer_width }
    , _backbuffer_height    { object._backbuffer_height }
    , _effect               { object._effect }
    , _vertex_declaration   { object._vertex_declaration }
    , _backbuffer_texture   { object._backbuffer_texture }
    , _backbuffer_surface   { object._backbuffer_surface }
    , _temp_buffer_texture  { object._temp_buffer_texture }
    , _temp_buffer_surface  { object._temp_buffer_surface }
    , _front_buffer_texture { object._front_buffer_texture }
    , _front_buffer_surface { object._front_buffer_surface }
{
    object._device               = nullptr;
    object._device_backbuffer    = nullptr;
    object._backbuffer_width     = 0;
    object._backbuffer_height    = 0;
    object._effect               = nullptr;
    object._vertex_declaration   = nullptr;
    object._backbuffer_texture   = nullptr;
    object._backbuffer_surface   = nullptr;
    object._temp_buffer_texture  = nullptr;
    object._temp_buffer_surface  = nullptr;
    object._front_buffer_texture = nullptr;
    object._front_buffer_surface = nullptr;
}

BlurEffect& BlurEffect::operator=(BlurEffect&& object) noexcept
{
    if (&object != this)
    {
        if (_device_backbuffer    != nullptr) _device_backbuffer->Release();
        if (_effect               != nullptr) _effect->Release();
        if (_vertex_declaration   != nullptr) _vertex_declaration->Release();
        if (_backbuffer_surface   != nullptr) _backbuffer_surface->Release();
        if (_backbuffer_texture   != nullptr) _backbuffer_texture->Release();
        if (_temp_buffer_surface  != nullptr) _temp_buffer_surface->Release();
        if (_temp_buffer_texture  != nullptr) _temp_buffer_texture->Release();
        if (_front_buffer_surface != nullptr) _front_buffer_surface->Release();
        if (_front_buffer_texture != nullptr) _front_buffer_texture->Release();

        _device               = object._device;
        _device_backbuffer    = object._device_backbuffer;
        _backbuffer_width     = object._backbuffer_width;
        _backbuffer_height    = object._backbuffer_height;
        _effect               = object._effect;
        _vertex_declaration   = object._vertex_declaration;
        _backbuffer_texture   = object._backbuffer_texture;
        _backbuffer_surface   = object._backbuffer_surface;
        _temp_buffer_texture  = object._temp_buffer_texture;
        _temp_buffer_surface  = object._temp_buffer_surface;
        _front_buffer_texture = object._front_buffer_texture;
        _front_buffer_surface = object._front_buffer_surface;

        object._device               = nullptr;
        object._device_backbuffer    = nullptr;
        object._backbuffer_width     = 0;
        object._backbuffer_height    = 0;
        object._effect               = nullptr;
        object._vertex_declaration   = nullptr;
        object._backbuffer_texture   = nullptr;
        object._backbuffer_surface   = nullptr;
        object._temp_buffer_texture  = nullptr;
        object._temp_buffer_surface  = nullptr;
        object._front_buffer_texture = nullptr;
        object._front_buffer_surface = nullptr;
    }

    return *this;
}

BlurEffect::BlurEffect(IDirect3DDevice9* const device, const Resource& resource) noexcept
{
    assert(device != nullptr);

    if (const auto result = device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO,
        &_device_backbuffer); FAILED(result))
    {
        Logger::LogToFile("[err:blureffect] : failed to get device backbuffer (code:%ld)", result);
        return;
    }

    {
        D3DSURFACE_DESC backbuffer_desc;

        if (const auto result = _device_backbuffer->GetDesc(&backbuffer_desc); FAILED(result))
        {
            Logger::LogToFile("[err:blureffect] : failed to get backbuffer description (code:%ld)", result);
            _device_backbuffer->Release();
            _device_backbuffer = nullptr;
            return;
        }

        _backbuffer_width  = backbuffer_desc.Width;
        _backbuffer_height = backbuffer_desc.Height;
    }

    {
        ID3DXBuffer* error_buffer;

        if (const auto result = D3DXCreateEffect(device, resource.GetData(), resource.GetSize(),
            NULL, NULL, D3DXFX_DONOTSAVESTATE | D3DXFX_NOT_CLONEABLE | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY,
            NULL, &_effect, &error_buffer); FAILED(result))
        {
            Logger::LogToFile("[err:blureffect] : failed to create effect (code:%ld) (%.*s)",
                result, error_buffer->GetBufferSize() - 1, error_buffer->GetBufferPointer());
            _device_backbuffer->Release();
            _device_backbuffer = nullptr;
            _backbuffer_width = 0;
            _backbuffer_height = 0;
            error_buffer->Release();
            return;
        }
    }

    const float iResolution[]
    {
        static_cast<float>(_backbuffer_width),
        static_cast<float>(_backbuffer_height)
    };

    _effect->SetFloatArray(static_cast<D3DXHANDLE>("iResolution"), iResolution, GetArraySize(iResolution));
    _effect->SetFloat(static_cast<D3DXHANDLE>("iLevel"), 0.f);

    const D3DVERTEXELEMENT9 vertex_elements[]
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };

    if (const auto result = device->CreateVertexDeclaration(vertex_elements,
        &_vertex_declaration); FAILED(result))
    {
        Logger::LogToFile("[err:blureffect] : failed to create vertex declaration (code:%ld)", result);
        _device_backbuffer->Release();
        _device_backbuffer = nullptr;
        _backbuffer_width = 0;
        _backbuffer_height = 0;
        _effect->Release();
        _effect = nullptr;
        return;
    }

    if (const auto result = D3DXCreateTexture(device, _backbuffer_width, _backbuffer_height,
        D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
        &_backbuffer_texture); FAILED(result))
    {
        Logger::LogToFile("[err:blureffect] : failed to create back-buffer texture (code:%ld)", result);
        _device_backbuffer->Release();
        _device_backbuffer = nullptr;
        _backbuffer_width = 0;
        _backbuffer_height = 0;
        _effect->Release();
        _effect = nullptr;
        _vertex_declaration->Release();
        _vertex_declaration = nullptr;
        return;
    }

    if (const auto result = _backbuffer_texture->GetSurfaceLevel(0,
        &_backbuffer_surface); FAILED(result))
    {
        Logger::LogToFile("[err:blureffect] : failed to get back-buffer surface (code:%ld)", result);
        _device_backbuffer->Release();
        _device_backbuffer = nullptr;
        _backbuffer_width = 0;
        _backbuffer_height = 0;
        _effect->Release();
        _effect = nullptr;
        _vertex_declaration->Release();
        _vertex_declaration = nullptr;
        _backbuffer_texture->Release();
        _backbuffer_texture = nullptr;
        return;
    }

    if (const auto result = D3DXCreateTexture(device, _backbuffer_width,
        _backbuffer_height, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP,
        D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &_temp_buffer_texture); FAILED(result))
    {
        Logger::LogToFile("[err:blureffect] : failed to create temp-buffer texture (code:%ld)", result);
        _device_backbuffer->Release();
        _device_backbuffer = nullptr;
        _backbuffer_width = 0;
        _backbuffer_height = 0;
        _effect->Release();
        _effect = nullptr;
        _vertex_declaration->Release();
        _vertex_declaration = nullptr;
        _backbuffer_texture->Release();
        _backbuffer_texture = nullptr;
        _backbuffer_surface->Release();
        _backbuffer_surface = nullptr;
        return;
    }

    if (const auto result = _temp_buffer_texture->GetSurfaceLevel(0,
        &_temp_buffer_surface); FAILED(result))
    {
        Logger::LogToFile("[err:blureffect] : failed to get temp-buffer surface (code:%ld)", result);
        _device_backbuffer->Release();
        _device_backbuffer = nullptr;
        _backbuffer_width = 0;
        _backbuffer_height = 0;
        _effect->Release();
        _effect = nullptr;
        _vertex_declaration->Release();
        _vertex_declaration = nullptr;
        _backbuffer_texture->Release();
        _backbuffer_texture = nullptr;
        _backbuffer_surface->Release();
        _backbuffer_surface = nullptr;
        _temp_buffer_texture->Release();
        _temp_buffer_texture = nullptr;
        return;
    }

    if (const auto result = D3DXCreateTexture(device, _backbuffer_width,
        _backbuffer_height, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP,
        D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &_front_buffer_texture); FAILED(result))
    {
        Logger::LogToFile("[err:blureffect] : failed to create front-buffer texture (code:%ld)", result);
        _device_backbuffer->Release();
        _device_backbuffer = nullptr;
        _backbuffer_width = 0;
        _backbuffer_height = 0;
        _effect->Release();
        _effect = nullptr;
        _vertex_declaration->Release();
        _vertex_declaration = nullptr;
        _backbuffer_texture->Release();
        _backbuffer_texture = nullptr;
        _backbuffer_surface->Release();
        _backbuffer_surface = nullptr;
        _temp_buffer_texture->Release();
        _temp_buffer_texture = nullptr;
        _temp_buffer_surface->Release();
        _temp_buffer_surface = nullptr;
        return;
    }

    if (const auto result = _front_buffer_texture->GetSurfaceLevel(0,
        &_front_buffer_surface); FAILED(result))
    {
        Logger::LogToFile("[err:blureffect] : failed to get front-buffer surface (code:%ld)", result);
        _device_backbuffer->Release();
        _device_backbuffer = nullptr;
        _backbuffer_width = 0;
        _backbuffer_height = 0;
        _effect->Release();
        _effect = nullptr;
        _vertex_declaration->Release();
        _vertex_declaration = nullptr;
        _backbuffer_texture->Release();
        _backbuffer_texture = nullptr;
        _backbuffer_surface->Release();
        _backbuffer_surface = nullptr;
        _temp_buffer_texture->Release();
        _temp_buffer_texture = nullptr;
        _temp_buffer_surface->Release();
        _temp_buffer_surface = nullptr;
        _front_buffer_texture->Release();
        _front_buffer_texture = nullptr;
        return;
    }

    _device = device;
}

bool BlurEffect::Valid() const noexcept
{
    return _device               != nullptr &&
           _device_backbuffer    != nullptr &&
           _backbuffer_width     != 0       &&
           _backbuffer_height    != 0       &&
           _effect               != nullptr &&
           _vertex_declaration   != nullptr &&
           _backbuffer_texture   != nullptr &&
           _backbuffer_surface   != nullptr &&
           _temp_buffer_texture  != nullptr &&
           _temp_buffer_surface  != nullptr &&
           _front_buffer_texture != nullptr &&
           _front_buffer_surface != nullptr ;
}

void BlurEffect::Render(const float level) noexcept
{
    assert(_device               != nullptr);
    assert(_device_backbuffer    != nullptr);
    assert(_effect               != nullptr);
    assert(_backbuffer_texture   != nullptr);
    assert(_backbuffer_surface   != nullptr);
    assert(_temp_buffer_texture  != nullptr);
    assert(_temp_buffer_surface  != nullptr);
    assert(_front_buffer_texture != nullptr);
    assert(_front_buffer_surface != nullptr);

    if (SUCCEEDED(_device->StretchRect(_device_backbuffer, NULL, _backbuffer_surface, NULL, D3DTEXF_POINT)) &&
        SUCCEEDED(_effect->SetFloat(static_cast<D3DXHANDLE>("iLevel"), std::clamp(level, 0.f, 100.f))) &&
        SUCCEEDED(_effect->Begin(nullptr, NULL)))
    {
        if (SUCCEEDED(_effect->SetTexture(static_cast<D3DXHANDLE>("iFrameTexture"), _backbuffer_texture)) &&
            SUCCEEDED(_device->SetRenderTarget(0, _temp_buffer_surface)) &&
            SUCCEEDED(_effect->BeginPass(0)))
        {
            Draw(); _effect->EndPass();
        }

        if (SUCCEEDED(_effect->SetTexture(static_cast<D3DXHANDLE>("iFrameTexture"), _temp_buffer_texture)) &&
            SUCCEEDED(_device->SetRenderTarget(0, _front_buffer_surface)) &&
            SUCCEEDED(_effect->BeginPass(1)))
        {
            Draw(); _effect->EndPass();
        }

        if (SUCCEEDED(_device->SetRenderTarget(0, _device_backbuffer)) &&
            SUCCEEDED(_effect->SetTexture(static_cast<D3DXHANDLE>("iBackBuffer"), _backbuffer_texture)) &&
            SUCCEEDED(_effect->SetTexture(static_cast<D3DXHANDLE>("iFrameTexture"), _front_buffer_texture)) &&
            SUCCEEDED(_effect->BeginPass(2)))
        {
            Draw(); _effect->EndPass();
        }

        _effect->End();
    }
}

void BlurEffect::Draw() noexcept
{
    assert(_device             != nullptr);
    assert(_backbuffer_width   != 0);
    assert(_backbuffer_height  != 0);
    assert(_vertex_declaration != nullptr);

    struct EffectVertex { float x, y, z, tx0, tx1; };

    if (SUCCEEDED(_device->SetVertexDeclaration(_vertex_declaration)))
    {
        const float screen_width  = _backbuffer_width;
        const float screen_height = _backbuffer_height;

        const EffectVertex quad[]
        {
            { 0.f,          0.f,           0.5f, 0.f, 0.f },
            { screen_width, 0.f,           0.5f, 1.f, 0.f },
            { 0.f,          screen_height, 0.5f, 0.f, 1.f },
            { screen_width, screen_height, 0.5f, 1.f, 1.f }
        };

        _rwD3D9DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(*quad));
    }
}
