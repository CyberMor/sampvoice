/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>

#include <d3d9.h>
#include <d3dx9.h>

#include <execution/scope_exit.hpp>
#include <memory/view.hpp>

#include "logger.hpp"

struct Texture {

    Texture() noexcept = default;
    ~Texture() noexcept
    {
        if (_texture != nullptr) _texture->Release();
        if (_sprite  != nullptr) _sprite->Release();
    }

    Texture(const Texture&) = delete;
    Texture(Texture&& object) noexcept
        : _texture { object._texture }
        , _sprite  { object._sprite }
        , _width   { object._width }
        , _height  { object._height }
    {
        object._texture = nullptr;
        object._sprite  = nullptr;
        object._width   = 0;
        object._height  = 0;
    }

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&& object) noexcept
    {
        if (&object != this)
        {
            if (_texture != nullptr) _texture->Release();
            if (_sprite  != nullptr) _sprite->Release();

            _texture = object._texture;
            _sprite  = object._sprite;
            _width   = object._width;
            _height  = object._height;

            object._texture = nullptr;
            object._sprite  = nullptr;
            object._width   = 0;
            object._height  = 0;
        }

        return *this;
    }

public:

    Texture(IDirect3DDevice9* const device, const DataView<ubyte_t>& resource) noexcept
    {
        assert(device != nullptr);

        if (const HRESULT result = D3DXCreateTextureFromFileInMemory
            (device, resource.Data(), resource.Size(), &_texture); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:texture:constructor] failed to create texture (code:%ld)", result);
            return;
        }

        ScopeExit texture_scope
        {
            [this]() noexcept -> void
            {
                _texture->Release();
                _texture = nullptr;
            }
        };

        if (const HRESULT result = D3DXCreateSprite(device, &_sprite); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:texture:constructor] failed to create sprite (code:%ld)", result);
            return;
        }

        ScopeExit sprite_scope
        {
            [this]() noexcept -> void
            {
                _sprite->Release();
                _sprite = nullptr;
            }
        };

        {
            D3DSURFACE_DESC texture_info;

            if (const HRESULT result = _texture->GetLevelDesc(0, &texture_info); FAILED(result))
            {
                Logger::Instance().LogToFile("[err:texture:constructor] failed to get texture info (code:%ld)", result);
                return;
            }

            _width  = texture_info.Width;
            _height = texture_info.Height;
        }

        texture_scope.Release();
        sprite_scope.Release();
    }

public:

    bool Valid() const noexcept
    {
        return _texture != nullptr &&
               _sprite  != nullptr &&
               _width   != 0       &&
               _height  != 0       ;
    }

    bool Invalid() const noexcept
    {
        return _texture == nullptr ||
               _sprite  == nullptr ||
               _width   == 0       ||
               _height  == 0       ;
    }

public:

    bool Initialize(IDirect3DDevice9* const device, const DataView<ubyte_t>& resource) noexcept
    {
        if (_texture != nullptr) _texture->Release();
        if (_sprite  != nullptr) _sprite->Release();

        _texture = nullptr;
        _sprite  = nullptr;
        _width   = 0;
        _height  = 0;

        assert(device != nullptr);

        if (const HRESULT result = D3DXCreateTextureFromFileInMemory
            (device, resource.Data(), resource.Size(), &_texture); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:texture:initialize] failed to create texture (code:%ld)", result);
            return false;
        }

        ScopeExit texture_scope
        {
            [this]() noexcept -> void
            {
                _texture->Release();
                _texture = nullptr;
            }
        };

        if (const HRESULT result = D3DXCreateSprite(device, &_sprite); FAILED(result))
        {
            Logger::Instance().LogToFile("[err:texture:initialize] failed to create sprite (code:%ld)", result);
            return false;
        }

        ScopeExit sprite_scope
        {
            [this]() noexcept -> void
            {
                _sprite->Release();
                _sprite = nullptr;
            }
        };

        {
            D3DSURFACE_DESC texture_info;

            if (const HRESULT result = _texture->GetLevelDesc(0, &texture_info); FAILED(result))
            {
                Logger::Instance().LogToFile("[err:texture:initialize] failed to get texture info (code:%ld)", result);
                return false;
            }

            _width  = texture_info.Width;
            _height = texture_info.Height;
        }

        texture_scope.Release();
        sprite_scope.Release();

        return true;
    }

    void Deinitialize() noexcept
    {
        if (_texture != nullptr) _texture->Release();
        if (_sprite  != nullptr) _sprite->Release();

        _texture = nullptr;
        _sprite  = nullptr;
        _width   = 0;
        _height  = 0;
    }

    void Release() noexcept
    {
        _texture = nullptr;
        _sprite  = nullptr;
        _width   = 0;
        _height  = 0;
    }

public:

    IDirect3DTexture9* Handle() const noexcept
    {
        assert(_texture != nullptr);
        return _texture;
    }

    ID3DXSprite* Sprite() const noexcept
    {
        assert(_sprite != nullptr);
        return _sprite;
    }

    UINT Width() const noexcept
    {
        assert(_width != 0);
        return _width;
    }

    UINT Height() const noexcept
    {
        assert(_height != 0);
        return _height;
    }

public:

    void Draw(const float x, const float y, const float width, const float height,
        const D3DCOLOR color = -1, const float angle = 0) noexcept
    {
        assert(_texture != nullptr);
        assert(_sprite  != nullptr);
        assert(_width   != 0);
        assert(_height  != 0);

        D3DXMATRIX matrix;

        D3DXMatrixTransformation2D(&matrix, NULL, 0,
            &D3DXVECTOR2(width / _width, height / _height),
            &D3DXVECTOR2(width / 2, height / 2), angle, &D3DXVECTOR2(x, y));

        if (SUCCEEDED(_sprite->SetTransform(&matrix)) &&
            SUCCEEDED(_sprite->Begin(D3DXSPRITE_ALPHABLEND)))
        {
            _sprite->Draw(_texture, NULL, NULL, NULL, color);
            _sprite->End();
        }
    }

private:

    IDirect3DTexture9* _texture = nullptr;
    ID3DXSprite*       _sprite  = nullptr;

    UINT               _width   = 0;
    UINT               _height  = 0;

};
