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
private:

	IDirect3DDevice9* pDevice = nullptr;
	D3DPRESENT_PARAMETERS dParameters = {};

	ID3DXEffect* pEffect = nullptr;
	IDirect3DVertexDeclaration9* pVertexDeclaration = nullptr;

	IDirect3DTexture9* pBackBufferTexture = nullptr;
	IDirect3DSurface9* pBackBufferSurface = nullptr;

	IDirect3DTexture9* pTempBufferTexture = nullptr;
	IDirect3DSurface9* pTempBufferSurface = nullptr;

	IDirect3DTexture9* pFrontBufferTexture = nullptr;
	IDirect3DSurface9* pFrontBufferSurface = nullptr;

private:

	void Draw() const;

public:

	BlurEffect() = delete;
	BlurEffect(const BlurEffect& object) = delete;
	BlurEffect(BlurEffect&& object) = delete;

	BlurEffect& operator=(const BlurEffect& object) = delete;
	BlurEffect& operator=(BlurEffect&& object) = delete;

	BlurEffect(
		IDirect3DDevice9* pDevice,
		D3DPRESENT_PARAMETERS* pParameters,
		const Resource& rEffect
	);

	void Render(float level) const;

	~BlurEffect();

};

typedef std::shared_ptr<BlurEffect> BlurEffectPtr;
#define MakeBlurEffect std::make_shared<BlurEffect>
