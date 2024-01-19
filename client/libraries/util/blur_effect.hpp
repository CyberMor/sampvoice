/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>
#include <algorithm>

#include <d3d9.h>
#include <d3dx9.h>

#include <execution/scope_exit.hpp>
#include <memory/view.hpp>

#include <game/RenderWare.h>

#include "logger.hpp"

struct BlurEffect {

    BlurEffect() noexcept = default;
    ~BlurEffect() noexcept
    {
        if (_device_backbuffer   != nullptr) _device_backbuffer->Release();
        if (_effect              != nullptr) _effect->Release();
        if (_vertex_declaration  != nullptr) _vertex_declaration->Release();
        if (_backbuffer_surface  != nullptr) _backbuffer_surface->Release();
        if (_backbuffer_texture  != nullptr) _backbuffer_texture->Release();
        if (_tempbuffer_surface  != nullptr) _tempbuffer_surface->Release();
        if (_tempbuffer_texture  != nullptr) _tempbuffer_texture->Release();
        if (_frontbuffer_surface != nullptr) _frontbuffer_surface->Release();
        if (_frontbuffer_texture != nullptr) _frontbuffer_texture->Release();
    }

    BlurEffect(const BlurEffect&) = delete;
    BlurEffect(BlurEffect&& object) noexcept
        : _device              { object._device }
        , _device_backbuffer   { object._device_backbuffer }
        , _backbuffer_width    { object._backbuffer_width }
        , _backbuffer_height   { object._backbuffer_height }
        , _effect              { object._effect }
        , _vertex_declaration  { object._vertex_declaration }
        , _backbuffer_texture  { object._backbuffer_texture }
        , _backbuffer_surface  { object._backbuffer_surface }
        , _tempbuffer_texture  { object._tempbuffer_texture }
        , _tempbuffer_surface  { object._tempbuffer_surface }
        , _frontbuffer_texture { object._frontbuffer_texture }
        , _frontbuffer_surface { object._frontbuffer_surface }
    {
        object._device              = nullptr;
        object._device_backbuffer   = nullptr;
        object._backbuffer_width    = 0;
        object._backbuffer_height   = 0;
        object._effect              = nullptr;
        object._vertex_declaration  = nullptr;
        object._backbuffer_texture  = nullptr;
        object._backbuffer_surface  = nullptr;
        object._tempbuffer_texture  = nullptr;
        object._tempbuffer_surface  = nullptr;
        object._frontbuffer_texture = nullptr;
        object._frontbuffer_surface = nullptr;
    }

    BlurEffect& operator=(const BlurEffect&) = delete;
    BlurEffect& operator=(BlurEffect&& object) noexcept
    {
        if (&object != this)
        {
            if (_device_backbuffer   != nullptr) _device_backbuffer->Release();
            if (_effect              != nullptr) _effect->Release();
            if (_vertex_declaration  != nullptr) _vertex_declaration->Release();
            if (_backbuffer_surface  != nullptr) _backbuffer_surface->Release();
            if (_backbuffer_texture  != nullptr) _backbuffer_texture->Release();
            if (_tempbuffer_surface  != nullptr) _tempbuffer_surface->Release();
            if (_tempbuffer_texture  != nullptr) _tempbuffer_texture->Release();
            if (_frontbuffer_surface != nullptr) _frontbuffer_surface->Release();
            if (_frontbuffer_texture != nullptr) _frontbuffer_texture->Release();

            _device              = object._device;
            _device_backbuffer   = object._device_backbuffer;
            _backbuffer_width    = object._backbuffer_width;
            _backbuffer_height   = object._backbuffer_height;
            _effect              = object._effect;
            _vertex_declaration  = object._vertex_declaration;
            _backbuffer_texture  = object._backbuffer_texture;
            _backbuffer_surface  = object._backbuffer_surface;
            _tempbuffer_texture  = object._tempbuffer_texture;
            _tempbuffer_surface  = object._tempbuffer_surface;
            _frontbuffer_texture = object._frontbuffer_texture;
            _frontbuffer_surface = object._frontbuffer_surface;

            object._device              = nullptr;
            object._device_backbuffer   = nullptr;
            object._backbuffer_width    = 0;
            object._backbuffer_height   = 0;
            object._effect              = nullptr;
            object._vertex_declaration  = nullptr;
            object._backbuffer_texture  = nullptr;
            object._backbuffer_surface  = nullptr;
            object._tempbuffer_texture  = nullptr;
            object._tempbuffer_surface  = nullptr;
            object._frontbuffer_texture = nullptr;
            object._frontbuffer_surface = nullptr;
        }

        return *this;
    }

public:

    BlurEffect(IDirect3DDevice9* const device, const DataView<ubyte_t>& resource) noexcept
    {
        assert(device != nullptr);

        if (const HRESULT result = device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO,
            &_device_backbuffer); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:blureffect:constructor] failed to "
                "get device backbuffer (code:%ld)", result);
            return;
        }

        ScopeExit device_backbuffer_scope
        {
            [this]() noexcept -> void
            {
                _device_backbuffer->Release();
                _device_backbuffer = nullptr;
            }
        };

        {
            D3DSURFACE_DESC backbuffer_desc;

            if (const HRESULT result = _device_backbuffer->GetDesc(&backbuffer_desc); FAILED(result))
            {
                Logger::Instance().LogToFile("[err:blureffect:constructor] failed to "
                    "get backbuffer description (code:%ld)", result);
                return;
            }

            _backbuffer_width  = backbuffer_desc.Width;
            _backbuffer_height = backbuffer_desc.Height;
        }

        ScopeExit backbuffer_size_scope
        {
            [this]() noexcept -> void
            {
                _backbuffer_width  = 0;
                _backbuffer_height = 0;
            }
        };

        {
            ID3DXBuffer* error_buffer;

            if (const HRESULT result = D3DXCreateEffect(device, resource.Data(), resource.Size(),
                NULL, NULL, D3DXFX_DONOTSAVESTATE | D3DXFX_NOT_CLONEABLE | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY,
                NULL, &_effect, &error_buffer); FAILED(result))
            {
                Logger::Instance().LogToFile("[err:blureffect:constructor] failed to create effect (code:%ld) (%.*s)",
                    result, error_buffer->GetBufferSize() - 1, error_buffer->GetBufferPointer());
                error_buffer->Release();
                return;
            }
        }

        ScopeExit effect_scope
        {
            [this]() noexcept -> void
            {
                _effect->Release();
                _effect = nullptr;
            }
        };

        const float iResolution[]
        {
            static_cast<float>(_backbuffer_width),
            static_cast<float>(_backbuffer_height)
        };

        _effect->SetFloatArray(static_cast<D3DXHANDLE>("iResolution"), iResolution, std::size(iResolution));
        _effect->SetFloat(static_cast<D3DXHANDLE>("iLevel"), 0);

        const D3DVERTEXELEMENT9 vertex_elements[]
        {
            { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
            { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
            D3DDECL_END()
        };

        if (const HRESULT result = device->CreateVertexDeclaration(vertex_elements, &_vertex_declaration); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:blureffect:constructor] failed to "
                "create vertex declaration (code:%ld)", result);
            return;
        }

        ScopeExit vertex_declaration_scope
        {
            [this]() noexcept -> void
            {
                _vertex_declaration->Release();
                _vertex_declaration = nullptr;
            }
        };

        if (const HRESULT result = D3DXCreateTexture(device, _backbuffer_width,
            _backbuffer_height, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
            &_backbuffer_texture); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:blureffect:constructor] failed to "
                "create back-buffer texture (code:%ld)", result);
            return;
        }

        ScopeExit backbuffer_texture_scope
        {
            [this]() noexcept -> void
            {
                _backbuffer_texture->Release();
                _backbuffer_texture = nullptr;
            }
        };

        if (const HRESULT result = _backbuffer_texture->GetSurfaceLevel(0, &_backbuffer_surface); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:blureffect:constructor] failed to "
                "get back-buffer surface (code:%ld)", result);
            return;
        }

        ScopeExit backbuffer_surface_scope
        {
            [this]() noexcept -> void
            {
                _backbuffer_surface->Release();
                _backbuffer_surface = nullptr;
            }
        };

        if (const HRESULT result = D3DXCreateTexture(device, _backbuffer_width,
            _backbuffer_height, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP,
            D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &_tempbuffer_texture); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:blureffect:constructor] failed to "
                "create temp-buffer texture (code:%ld)", result);
            return;
        }

        ScopeExit tempbuffer_texture_scope
        {
            [this]() noexcept -> void
            {
                _tempbuffer_texture->Release();
                _tempbuffer_texture = nullptr;
            }
        };

        if (const HRESULT result = _tempbuffer_texture->GetSurfaceLevel(0, &_tempbuffer_surface); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:blureffect:constructor] failed to "
                "get temp-buffer surface (code:%ld)", result);
            return;
        }

        ScopeExit tempbuffer_surface_scope
        {
            [this]() noexcept -> void
            {
                _tempbuffer_surface->Release();
                _tempbuffer_surface = nullptr;
            }
        };

        if (const HRESULT result = D3DXCreateTexture(device, _backbuffer_width,
            _backbuffer_height, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP,
            D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &_frontbuffer_texture); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:blureffect:constructor] failed to "
                "create front-buffer texture (code:%ld)", result);
            return;
        }

        ScopeExit frontbuffer_texture_scope
        {
            [this]() noexcept -> void
            {
                _frontbuffer_texture->Release();
                _frontbuffer_texture = nullptr;
            }
        };

        if (const HRESULT result = _frontbuffer_texture->GetSurfaceLevel(0, &_frontbuffer_surface); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:blureffect:constructor] failed to "
                "get front-buffer surface (code:%ld)", result);
            return;
        }

        device_backbuffer_scope.Release();
        backbuffer_size_scope.Release();
        effect_scope.Release();
        vertex_declaration_scope.Release();
        backbuffer_texture_scope.Release();
        backbuffer_surface_scope.Release();
        tempbuffer_texture_scope.Release();
        tempbuffer_surface_scope.Release();
        frontbuffer_texture_scope.Release();

        _device = device;
    }

public:

    bool Valid() const noexcept
    {
        return _device              != nullptr &&
               _device_backbuffer   != nullptr &&
               _backbuffer_width    != 0       &&
               _backbuffer_height   != 0       &&
               _effect              != nullptr &&
               _vertex_declaration  != nullptr &&
               _backbuffer_texture  != nullptr &&
               _backbuffer_surface  != nullptr &&
               _tempbuffer_texture  != nullptr &&
               _tempbuffer_surface  != nullptr &&
               _frontbuffer_texture != nullptr &&
               _frontbuffer_surface != nullptr ;
    }

    bool Invalid() const noexcept
    {
        return _device              == nullptr ||
               _device_backbuffer   == nullptr ||
               _backbuffer_width    == 0       ||
               _backbuffer_height   == 0       ||
               _effect              == nullptr ||
               _vertex_declaration  == nullptr ||
               _backbuffer_texture  == nullptr ||
               _backbuffer_surface  == nullptr ||
               _tempbuffer_texture  == nullptr ||
               _tempbuffer_surface  == nullptr ||
               _frontbuffer_texture == nullptr ||
               _frontbuffer_surface == nullptr ;
    }

public:

    bool Initialize(IDirect3DDevice9* const device, const DataView<ubyte_t>& resource) noexcept
    {
        if (_device_backbuffer   != nullptr) _device_backbuffer->Release();
        if (_effect              != nullptr) _effect->Release();
        if (_vertex_declaration  != nullptr) _vertex_declaration->Release();
        if (_backbuffer_surface  != nullptr) _backbuffer_surface->Release();
        if (_backbuffer_texture  != nullptr) _backbuffer_texture->Release();
        if (_tempbuffer_surface  != nullptr) _tempbuffer_surface->Release();
        if (_tempbuffer_texture  != nullptr) _tempbuffer_texture->Release();
        if (_frontbuffer_surface != nullptr) _frontbuffer_surface->Release();
        if (_frontbuffer_texture != nullptr) _frontbuffer_texture->Release();

        _device              = nullptr;
        _device_backbuffer   = nullptr;
        _backbuffer_width    = 0;
        _backbuffer_height   = 0;
        _effect              = nullptr;
        _vertex_declaration  = nullptr;
        _backbuffer_texture  = nullptr;
        _backbuffer_surface  = nullptr;
        _tempbuffer_texture  = nullptr;
        _tempbuffer_surface  = nullptr;
        _frontbuffer_texture = nullptr;
        _frontbuffer_surface = nullptr;

        assert(device != nullptr);

        if (const HRESULT result = device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO,
            &_device_backbuffer); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:blureffect:initialize] failed to "
                "get device backbuffer (code:%ld)", result);
            return false;
        }

        ScopeExit device_backbuffer_scope
        {
            [this]() noexcept -> void
            {
                _device_backbuffer->Release();
                _device_backbuffer = nullptr;
            }
        };

        {
            D3DSURFACE_DESC backbuffer_desc;

            if (const HRESULT result = _device_backbuffer->GetDesc(&backbuffer_desc); FAILED(result))
            {
                Logger::Instance().LogToFile("[err:blureffect:initialize] failed to "
                    "get backbuffer description (code:%ld)", result);
                return false;
            }

            _backbuffer_width  = backbuffer_desc.Width;
            _backbuffer_height = backbuffer_desc.Height;
        }

        ScopeExit backbuffer_size_scope
        {
            [this]() noexcept -> void
            {
                _backbuffer_width  = 0;
                _backbuffer_height = 0;
            }
        };

        {
            ID3DXBuffer* error_buffer;

            if (const HRESULT result = D3DXCreateEffect(device, resource.Data(), resource.Size(),
                NULL, NULL, D3DXFX_DONOTSAVESTATE | D3DXFX_NOT_CLONEABLE | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY,
                NULL, &_effect, &error_buffer); FAILED(result))
            {
                Logger::Instance().LogToFile("[err:blureffect:initialize] failed to create effect (code:%ld) (%.*s)",
                    result, error_buffer->GetBufferSize() - 1, error_buffer->GetBufferPointer());
                error_buffer->Release();
                return false;
            }
        }

        ScopeExit effect_scope
        {
            [this]() noexcept -> void
            {
                _effect->Release();
                _effect = nullptr;
            }
        };

        const float iResolution[]
        {
            static_cast<float>(_backbuffer_width),
            static_cast<float>(_backbuffer_height)
        };

        _effect->SetFloatArray(static_cast<D3DXHANDLE>("iResolution"), iResolution, std::size(iResolution));
        _effect->SetFloat(static_cast<D3DXHANDLE>("iLevel"), 0);

        const D3DVERTEXELEMENT9 vertex_elements[]
        {
            { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
            { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
            D3DDECL_END()
        };

        if (const HRESULT result = device->CreateVertexDeclaration(vertex_elements, &_vertex_declaration); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:blureffect:initialize] failed to "
                "create vertex declaration (code:%ld)", result);
            return false;
        }

        ScopeExit vertex_declaration_scope
        {
            [this]() noexcept -> void
            {
                _vertex_declaration->Release();
                _vertex_declaration = nullptr;
            }
        };

        if (const HRESULT result = D3DXCreateTexture(device, _backbuffer_width,
            _backbuffer_height, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
            &_backbuffer_texture); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:blureffect:initialize] failed to "
                "create back-buffer texture (code:%ld)", result);
            return false;
        }

        ScopeExit backbuffer_texture_scope
        {
            [this]() noexcept -> void
            {
                _backbuffer_texture->Release();
                _backbuffer_texture = nullptr;
            }
        };

        if (const HRESULT result = _backbuffer_texture->GetSurfaceLevel(0, &_backbuffer_surface); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:blureffect:initialize] failed to "
                "get back-buffer surface (code:%ld)", result);
            return false;
        }

        ScopeExit backbuffer_surface_scope
        {
            [this]() noexcept -> void
            {
                _backbuffer_surface->Release();
                _backbuffer_surface = nullptr;
            }
        };

        if (const HRESULT result = D3DXCreateTexture(device, _backbuffer_width,
            _backbuffer_height, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP,
            D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &_tempbuffer_texture); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:blureffect:initialize] failed to "
                "create temp-buffer texture (code:%ld)", result);
            return false;
        }

        ScopeExit tempbuffer_texture_scope
        {
            [this]() noexcept -> void
            {
                _tempbuffer_texture->Release();
                _tempbuffer_texture = nullptr;
            }
        };

        if (const HRESULT result = _tempbuffer_texture->GetSurfaceLevel(0, &_tempbuffer_surface); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:blureffect:initialize] failed to "
                "get temp-buffer surface (code:%ld)", result);
            return false;
        }

        ScopeExit tempbuffer_surface_scope
        {
            [this]() noexcept -> void
            {
                _tempbuffer_surface->Release();
                _tempbuffer_surface = nullptr;
            }
        };

        if (const HRESULT result = D3DXCreateTexture(device, _backbuffer_width,
            _backbuffer_height, D3DX_DEFAULT, D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP,
            D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &_frontbuffer_texture); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:blureffect:initialize] failed to "
                "create front-buffer texture (code:%ld)", result);
            return false;
        }

        ScopeExit frontbuffer_texture_scope
        {
            [this]() noexcept -> void
            {
                _frontbuffer_texture->Release();
                _frontbuffer_texture = nullptr;
            }
        };

        if (const HRESULT result = _frontbuffer_texture->GetSurfaceLevel(0, &_frontbuffer_surface); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:blureffect:initialize] failed to "
                "get front-buffer surface (code:%ld)", result);
            return false;
        }

        device_backbuffer_scope.Release();
        backbuffer_size_scope.Release();
        effect_scope.Release();
        vertex_declaration_scope.Release();
        backbuffer_texture_scope.Release();
        backbuffer_surface_scope.Release();
        tempbuffer_texture_scope.Release();
        tempbuffer_surface_scope.Release();
        frontbuffer_texture_scope.Release();

        _device = device;

        return true;
    }

    void Deinitialize() noexcept
    {
        if (_device_backbuffer   != nullptr) _device_backbuffer->Release();
        if (_effect              != nullptr) _effect->Release();
        if (_vertex_declaration  != nullptr) _vertex_declaration->Release();
        if (_backbuffer_surface  != nullptr) _backbuffer_surface->Release();
        if (_backbuffer_texture  != nullptr) _backbuffer_texture->Release();
        if (_tempbuffer_surface  != nullptr) _tempbuffer_surface->Release();
        if (_tempbuffer_texture  != nullptr) _tempbuffer_texture->Release();
        if (_frontbuffer_surface != nullptr) _frontbuffer_surface->Release();
        if (_frontbuffer_texture != nullptr) _frontbuffer_texture->Release();

        _device              = nullptr;
        _device_backbuffer   = nullptr;
        _backbuffer_width    = 0;
        _backbuffer_height   = 0;
        _effect              = nullptr;
        _vertex_declaration  = nullptr;
        _backbuffer_texture  = nullptr;
        _backbuffer_surface  = nullptr;
        _tempbuffer_texture  = nullptr;
        _tempbuffer_surface  = nullptr;
        _frontbuffer_texture = nullptr;
        _frontbuffer_surface = nullptr;
    }

    void Release() noexcept
    {
        _device              = nullptr;
        _device_backbuffer   = nullptr;
        _backbuffer_width    = 0;
        _backbuffer_height   = 0;
        _effect              = nullptr;
        _vertex_declaration  = nullptr;
        _backbuffer_texture  = nullptr;
        _backbuffer_surface  = nullptr;
        _tempbuffer_texture  = nullptr;
        _tempbuffer_surface  = nullptr;
        _frontbuffer_texture = nullptr;
        _frontbuffer_surface = nullptr;
    }

public:

    void Render(const float level) noexcept
    {
        assert(_device              != nullptr);
        assert(_device_backbuffer   != nullptr);
        assert(_effect              != nullptr);
        assert(_backbuffer_texture  != nullptr);
        assert(_backbuffer_surface  != nullptr);
        assert(_tempbuffer_texture  != nullptr);
        assert(_tempbuffer_surface  != nullptr);
        assert(_frontbuffer_texture != nullptr);
        assert(_frontbuffer_surface != nullptr);

        if (SUCCEEDED(_device->StretchRect(_device_backbuffer, NULL, _backbuffer_surface, NULL, D3DTEXF_POINT)) &&
            SUCCEEDED(_effect->SetFloat(static_cast<D3DXHANDLE>("iLevel"), std::clamp(level, 0.0F, 100.0F))) &&
            SUCCEEDED(_effect->Begin(nullptr, NULL)))
        {
            if (SUCCEEDED(_effect->SetTexture(static_cast<D3DXHANDLE>("iFrameTexture"), _backbuffer_texture)) &&
                SUCCEEDED(_device->SetRenderTarget(0, _tempbuffer_surface)) &&
                SUCCEEDED(_effect->BeginPass(0)))
            {
                Draw(); _effect->EndPass();
            }

            if (SUCCEEDED(_effect->SetTexture(static_cast<D3DXHANDLE>("iFrameTexture"), _tempbuffer_texture)) &&
                SUCCEEDED(_device->SetRenderTarget(0, _frontbuffer_surface)) &&
                SUCCEEDED(_effect->BeginPass(1)))
            {
                Draw(); _effect->EndPass();
            }

            if (SUCCEEDED(_device->SetRenderTarget(0, _device_backbuffer)) &&
                SUCCEEDED(_effect->SetTexture(static_cast<D3DXHANDLE>("iBackBuffer"), _backbuffer_texture)) &&
                SUCCEEDED(_effect->SetTexture(static_cast<D3DXHANDLE>("iFrameTexture"), _frontbuffer_texture)) &&
                SUCCEEDED(_effect->BeginPass(2)))
            {
                Draw(); _effect->EndPass();
            }

            _effect->End();
        }
    }

private:

    void Draw() noexcept
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
                { 0.0F,         0.0F,          0.5F, 0.0F, 0.0F },
                { screen_width, 0.0F,          0.5F, 1.0F, 0.0F },
                { 0.0F,         screen_height, 0.5F, 0.0F, 1.0F },
                { screen_width, screen_height, 0.5F, 1.0F, 1.0F }
            };

            _rwD3D9DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(*quad));
        }
    }

private:

    IDirect3DDevice9*            _device              = nullptr;
    IDirect3DSurface9*           _device_backbuffer   = nullptr;

    UINT                         _backbuffer_width    = 0;
    UINT                         _backbuffer_height   = 0;

    ID3DXEffect*                 _effect              = nullptr;
    IDirect3DVertexDeclaration9* _vertex_declaration  = nullptr;

    IDirect3DTexture9*           _backbuffer_texture  = nullptr;
    IDirect3DSurface9*           _backbuffer_surface  = nullptr;

    IDirect3DTexture9*           _tempbuffer_texture  = nullptr;
    IDirect3DSurface9*           _tempbuffer_surface  = nullptr;

    IDirect3DTexture9*           _frontbuffer_texture = nullptr;
    IDirect3DSurface9*           _frontbuffer_surface = nullptr;

};
