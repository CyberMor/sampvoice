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
private:

	IDirect3DTexture9* pTexture = nullptr;
	IDirect3DSurface9* pSurface = nullptr;
	ID3DXSprite* pSprite = nullptr;

	D3DSURFACE_DESC textureInfo = {};

public:

	Texture() = delete;
	Texture(const Texture& object) = delete;
	Texture(Texture&& object) = delete;

	Texture& operator=(const Texture& object) = delete;
	Texture& operator=(Texture&& object) = delete;

	Texture(
		IDirect3DDevice9* pDevice,
		const Resource& rTexture
	);

	IDirect3DTexture9* GetTexture() const;
	IDirect3DSurface9* GetSurface() const;
	ID3DXSprite* GetSprite() const;

	int GetWidth() const;
	int GetHeight() const;

	void Draw(
		const float x,
		const float y,
		const float width,
		const float height,
		const D3DCOLOR color,
		const float angle
	) const;

	~Texture();

};

typedef std::shared_ptr<Texture> TexturePtr;
#define MakeTexture std::make_shared<Texture>
