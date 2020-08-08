/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "BlurEffect.h"

#include <assert.h>

#include <game/RenderWare.h>

#include "Logger.h"

void BlurEffect::Draw() const {

	struct EffectVertex {

		float x, y, z;
		float tx0, tx1;

	};

	this->pDevice->SetVertexDeclaration(this->pVertexDeclaration);

	const float screenWidth = this->dParameters.BackBufferWidth;
	const float screenHeight = this->dParameters.BackBufferHeight;

	const EffectVertex quad[] = {

		{ 0.f,         0.f,          0.5f, 0.f, 0.f },
		{ screenWidth, 0.f,          0.5f, 1.f, 0.f },
		{ 0.f,         screenHeight, 0.5f, 0.f, 1.f },
		{ screenWidth, screenHeight, 0.5f, 1.f, 1.f }

	};

	_rwD3D9DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(*quad));

}

BlurEffect::BlurEffect(
	IDirect3DDevice9* pDevice,
	D3DPRESENT_PARAMETERS* pParameters,
	const Resource& rEffect
) {

	assert(pDevice);
	assert(pParameters);

	this->pDevice = pDevice;
	this->dParameters = *pParameters;

	ID3DXBuffer* pErrorBuffer = nullptr;

	if (const HRESULT hResult = D3DXCreateEffect(
		pDevice, rEffect.GetDataPtr(), rEffect.GetDataSize(), NULL, NULL,
		D3DXFX_DONOTSAVESTATE | D3DXFX_NOT_CLONEABLE | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY,
		NULL, &this->pEffect, &pErrorBuffer
	); FAILED(hResult)) {

		Logger::LogToFile(
			"[err:blureffect_%p] : failed to create effect (code:%d) (%.*s)",
			this, hResult, pErrorBuffer->GetBufferSize() - 1,
			pErrorBuffer->GetBufferPointer()
		);

		pErrorBuffer->Release();

		throw std::bad_alloc();

	}

	const float iResolution[] = {

		this->dParameters.BackBufferWidth,
		this->dParameters.BackBufferHeight

	};

	this->pEffect->SetFloatArray((D3DXHANDLE)("iResolution"), iResolution, ARRAYSIZE(iResolution));
	this->pEffect->SetFloat((D3DXHANDLE)("iLevel"), 0.f);

	const D3DVERTEXELEMENT9 vertexElements[] = {

		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0 },

		D3DDECL_END()

	};

	if (const HRESULT hResult = pDevice->CreateVertexDeclaration(vertexElements, &this->pVertexDeclaration); FAILED(hResult)) {

		Logger::LogToFile("[err:blureffect_%p] : failed to create vertex declaration (code:%d)", this, hResult);

		this->pEffect->Release();

		throw std::bad_alloc();

	}

	if (const HRESULT hResult = D3DXCreateTexture(
		pDevice, this->dParameters.BackBufferWidth, this->dParameters.BackBufferHeight, D3DX_DEFAULT,
		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &this->pBackBufferTexture
	); FAILED(hResult)) {

		Logger::LogToFile("[err:blureffect_%p] : failed to create back-buffer texture (code:%d)", this, hResult);

		this->pEffect->Release();
		this->pVertexDeclaration->Release();

		throw std::bad_alloc();

	}

	if (const HRESULT hResult = this->pBackBufferTexture->GetSurfaceLevel(0, &this->pBackBufferSurface); FAILED(hResult)) {

		Logger::LogToFile("[err:blureffect_%p] : failed to get back-buffer surface (code:%d)", this, hResult);

		this->pEffect->Release();
		this->pVertexDeclaration->Release();

		this->pBackBufferTexture->Release();

		throw std::bad_alloc();

	}

	if (const HRESULT hResult = D3DXCreateTexture(
		pDevice, this->dParameters.BackBufferWidth, this->dParameters.BackBufferHeight, D3DX_DEFAULT,
		D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &this->pTempBufferTexture
	); FAILED(hResult)) {

		Logger::LogToFile("[err:blureffect_%p] : failed to create temp-buffer texture (code:%d)", this, hResult);

		this->pEffect->Release();
		this->pVertexDeclaration->Release();

		this->pBackBufferSurface->Release();
		this->pBackBufferTexture->Release();

		throw std::bad_alloc();

	}

	if (const HRESULT hResult = this->pTempBufferTexture->GetSurfaceLevel(0, &this->pTempBufferSurface); FAILED(hResult)) {

		Logger::LogToFile("[err:blureffect_%p] : failed to get temp-buffer surface (code:%d)", this, hResult);

		this->pEffect->Release();
		this->pVertexDeclaration->Release();

		this->pBackBufferSurface->Release();
		this->pBackBufferTexture->Release();

		this->pTempBufferTexture->Release();

		throw std::bad_alloc();

	}

	if (const HRESULT hResult = D3DXCreateTexture(
		pDevice, this->dParameters.BackBufferWidth, this->dParameters.BackBufferHeight, D3DX_DEFAULT,
		D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &this->pFrontBufferTexture
	); FAILED(hResult)) {

		Logger::LogToFile("[err:blureffect_%p] : failed to create front-buffer texture (code:%d)", this, hResult);

		this->pEffect->Release();
		this->pVertexDeclaration->Release();

		this->pBackBufferSurface->Release();
		this->pBackBufferTexture->Release();

		this->pTempBufferSurface->Release();
		this->pTempBufferTexture->Release();

		throw std::bad_alloc();

	}

	if (const HRESULT hResult = this->pFrontBufferTexture->GetSurfaceLevel(0, &this->pFrontBufferSurface); FAILED(hResult)) {

		Logger::LogToFile("[err:blureffect_%p] : failed to get front-buffer surface (code:%d)", this, hResult);

		this->pEffect->Release();
		this->pVertexDeclaration->Release();

		this->pBackBufferSurface->Release();
		this->pBackBufferTexture->Release();

		this->pTempBufferSurface->Release();
		this->pTempBufferTexture->Release();

		this->pFrontBufferTexture->Release();

		throw std::bad_alloc();

	}

}

void BlurEffect::Render(float level) const {

	IDirect3DSurface9* pDeviceBackBuffer = nullptr;

	if (FAILED(this->pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pDeviceBackBuffer))) return;

	this->pDevice->StretchRect(pDeviceBackBuffer, NULL, this->pBackBufferSurface, NULL, D3DTEXF_POINT);

	if (level > 100.f) level = 100.f;
	if (level < 0.f) level = 0.f;

	this->pEffect->SetFloat((D3DXHANDLE)("iLevel"), level);

	UINT passCount = NULL;

	if (SUCCEEDED(this->pEffect->Begin(&passCount, NULL))) {

		this->pEffect->SetTexture((D3DXHANDLE)("iFrameTexture"), this->pBackBufferTexture);
		this->pDevice->SetRenderTarget(0, this->pTempBufferSurface);

		if (SUCCEEDED(this->pEffect->BeginPass(0))) {

			this->Draw();
			this->pEffect->EndPass();

		}

		this->pEffect->SetTexture((D3DXHANDLE)("iFrameTexture"), this->pTempBufferTexture);
		this->pDevice->SetRenderTarget(0, this->pFrontBufferSurface);

		if (SUCCEEDED(this->pEffect->BeginPass(1))) {

			this->Draw();
			this->pEffect->EndPass();

		}

		this->pEffect->End();

	}

	if (SUCCEEDED(this->pEffect->Begin(&passCount, NULL))) {

		this->pDevice->SetRenderTarget(0, pDeviceBackBuffer);
		this->pEffect->SetTexture((D3DXHANDLE)("iBackBuffer"), this->pBackBufferTexture);
		this->pEffect->SetTexture((D3DXHANDLE)("iFrameTexture"), this->pFrontBufferTexture);

		if (SUCCEEDED(this->pEffect->BeginPass(2))) {

			this->Draw();
			this->pEffect->EndPass();

		}

		this->pEffect->End();

	}

	pDeviceBackBuffer->Release();

}

BlurEffect::~BlurEffect() {

	this->pEffect->Release();
	this->pVertexDeclaration->Release();

	this->pBackBufferSurface->Release();
	this->pBackBufferTexture->Release();

	this->pTempBufferSurface->Release();
	this->pTempBufferTexture->Release();

	this->pFrontBufferSurface->Release();
	this->pFrontBufferTexture->Release();

}
