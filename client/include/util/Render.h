/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <mutex>
#include <functional>

#include <d3d9.h>
#include <d3dx9.h>

#include "Memory.hpp"
#include "Logger.h"

#define GtaDirectPointer     (*(IDirect3D9**)(0xC97C20))
#define GtaDevicePointer     (*(IDirect3DDevice9**)(0xC97C28))
#define GtaParametersPointer ((D3DPRESENT_PARAMETERS*)(0xC9C040))
#define GtaDirect3DCreate9   ((IDirect3D9*(CALLBACK*)(UINT))(0x807C2B))

class Render {
public:

	using DeviceInitHandlerType  = std::function<void(IDirect3D9*, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)>;
	using BeforeResetHandlerType = std::function<void()>;
	using BeginSceneHandlerType  = std::function<void()>;
	using RenderHandlerType      = std::function<void()>;
	using EndSceneHandlerType    = std::function<void()>;
	using AfterResetHandlerType  = std::function<void(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)>;
	using DeviceFreeHandlerType  = std::function<void()>;

	static constexpr float BaseWidth = 640;
	static constexpr float BaseHeight = 480;

private:

	static bool initStatus;

	static std::mutex deviceMutex;

	static IDirect3D9* pDirectInterface;
	static IDirect3DDevice9* pDeviceInterface;
	static D3DPRESENT_PARAMETERS deviceParameters;

	static DeviceInitHandlerType deviceInitHandler;
	static BeforeResetHandlerType beforeResetHandler;
	static BeginSceneHandlerType beginSceneHandler;
	static RenderHandlerType renderHandler;
	static EndSceneHandlerType endSceneHandler;
	static AfterResetHandlerType afterResetHandler;
	static DeviceFreeHandlerType deviceFreeHandler;

public:

	interface IDirect3DDevice9Hook : public IDirect3DDevice9 {
	private:

		IDirect3DDevice9* const pOrigInterface;

		bool resetStatus = false;
		bool sceneStatus = false;

	public:

		IDirect3DDevice9Hook(IDirect3DDevice9* pOrigInterface) :
			pOrigInterface(pOrigInterface)
		{

			this->pOrigInterface->AddRef();

		}

		~IDirect3DDevice9Hook() {

			this->pOrigInterface->Release();

		}

		// IDirect3DDevice9 Interface methods...
		// ------------------------------------------------

		HRESULT __stdcall Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) {

			if (this->pOrigInterface == Render::pDeviceInterface && !this->resetStatus)
				if (Render::renderHandler) Render::renderHandler();

			return this->pOrigInterface->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);

		}

		HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj) {

			*ppvObj = nullptr;

			const HRESULT hResult = this->pOrigInterface->QueryInterface(riid, ppvObj);
			if (SUCCEEDED(hResult)) *ppvObj = this;
			return hResult;

		}

		ULONG __stdcall AddRef() {
			return this->pOrigInterface->AddRef();
		}

		ULONG __stdcall Release() {

			const ULONG count = this->pOrigInterface->Release();

			if (count <= 1) {

				if (this->pOrigInterface == Render::pDeviceInterface) {

					if (Render::deviceFreeHandler)
						Render::deviceFreeHandler();

					Render::deviceMutex.lock();
					Render::pDirectInterface = nullptr;
					Render::pDeviceInterface = nullptr;
					Render::deviceMutex.unlock();

				}

				delete this;
				return 0;

			}

			return count;

		}

		HRESULT __stdcall TestCooperativeLevel() {
			return this->pOrigInterface->TestCooperativeLevel();
		}

		UINT __stdcall GetAvailableTextureMem() {
			return this->pOrigInterface->GetAvailableTextureMem();
		}

		HRESULT __stdcall EvictManagedResources() {
			return this->pOrigInterface->EvictManagedResources();
		}

		HRESULT __stdcall GetDirect3D(IDirect3D9** ppD3D9) {
			return this->pOrigInterface->GetDirect3D(ppD3D9);
		}

		HRESULT __stdcall GetDeviceCaps(D3DCAPS9* pCaps) {
			return this->pOrigInterface->GetDeviceCaps(pCaps);
		}

		HRESULT __stdcall GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) {
			return this->pOrigInterface->GetDisplayMode(iSwapChain, pMode);
		}

		HRESULT __stdcall GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters) {
			return this->pOrigInterface->GetCreationParameters(pParameters);
		}

		HRESULT __stdcall SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap) {
			return this->pOrigInterface->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
		}

		void __stdcall SetCursorPosition(int X, int Y, DWORD Flags) {
			this->pOrigInterface->SetCursorPosition(X, Y, Flags);
		}

		BOOL __stdcall ShowCursor(BOOL bShow) {
			return this->pOrigInterface->ShowCursor(bShow);
		}

		HRESULT __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain) {
			return this->pOrigInterface->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
		}

		HRESULT __stdcall GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain) {
			return this->pOrigInterface->GetSwapChain(iSwapChain, pSwapChain);
		}

		UINT __stdcall GetNumberOfSwapChains() {
			return this->pOrigInterface->GetNumberOfSwapChains();
		}

		HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) {

			if (this->pOrigInterface == Render::pDeviceInterface && !this->resetStatus) {

				if (Render::beforeResetHandler) Render::beforeResetHandler();

				this->resetStatus = true;

			}

			const HRESULT hResult = this->pOrigInterface->Reset(pPresentationParameters);

			if (this->pOrigInterface == Render::pDeviceInterface && SUCCEEDED(hResult)) {

				Render::deviceMutex.lock();
				Render::deviceParameters = *pPresentationParameters;
				Render::deviceMutex.unlock();

				if (this->resetStatus) {

					if (Render::afterResetHandler)
						Render::afterResetHandler(
							Render::pDeviceInterface,
							&Render::deviceParameters
						);

					this->resetStatus = false;

				}

			}

			return hResult;

		}

		HRESULT __stdcall GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) {
			return this->pOrigInterface->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
		}

		HRESULT __stdcall GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus) {
			return this->pOrigInterface->GetRasterStatus(iSwapChain, pRasterStatus);
		}

		HRESULT __stdcall SetDialogBoxMode(BOOL bEnableDialogs) {
			return this->pOrigInterface->SetDialogBoxMode(bEnableDialogs);
		}

		void __stdcall SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp) {
			this->pOrigInterface->SetGammaRamp(iSwapChain, Flags, pRamp);
		}

		void __stdcall GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) {
			this->pOrigInterface->GetGammaRamp(iSwapChain, pRamp);
		}

		HRESULT __stdcall CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) {
			return this->pOrigInterface->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
		}

		HRESULT __stdcall CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle) {
			return this->pOrigInterface->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
		}

		HRESULT __stdcall CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle) {
			return this->pOrigInterface->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
		}

		HRESULT __stdcall CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle) {
			return this->pOrigInterface->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
		}

		HRESULT __stdcall CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle) {
			return this->pOrigInterface->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
		}

		HRESULT __stdcall CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
			return this->pOrigInterface->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
		}

		HRESULT __stdcall CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
			return this->pOrigInterface->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
		}

		HRESULT __stdcall UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint) {
			return this->pOrigInterface->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
		}

		HRESULT __stdcall UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture) {
			return this->pOrigInterface->UpdateTexture(pSourceTexture, pDestinationTexture);
		}

		HRESULT __stdcall GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface) {
			return this->pOrigInterface->GetRenderTargetData(pRenderTarget, pDestSurface);
		}

		HRESULT __stdcall GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface) {
			return this->pOrigInterface->GetFrontBufferData(iSwapChain, pDestSurface);
		}

		HRESULT __stdcall StretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) {
			return this->pOrigInterface->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
		}

		HRESULT __stdcall ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color) {
			return this->pOrigInterface->ColorFill(pSurface, pRect, color);
		}

		HRESULT __stdcall CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
			return this->pOrigInterface->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
		}

		HRESULT __stdcall SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) {
			return this->pOrigInterface->SetRenderTarget(RenderTargetIndex, pRenderTarget);
		}

		HRESULT __stdcall GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget) {
			return this->pOrigInterface->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
		}

		HRESULT __stdcall SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) {
			return this->pOrigInterface->SetDepthStencilSurface(pNewZStencil);
		}

		HRESULT __stdcall GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface) {
			return this->pOrigInterface->GetDepthStencilSurface(ppZStencilSurface);
		}

		HRESULT __stdcall BeginScene() {

			const HRESULT hResult = this->pOrigInterface->BeginScene();

			if (this->pOrigInterface == Render::pDeviceInterface && SUCCEEDED(hResult))
				if (!this->resetStatus && !this->sceneStatus) {

					if (Render::beginSceneHandler)
						Render::beginSceneHandler();

					this->sceneStatus = true;

				}

			return hResult;

		}

		HRESULT __stdcall EndScene() {

			if (this->pOrigInterface == Render::pDeviceInterface)
				if (!this->resetStatus && this->sceneStatus) {

					if (Render::endSceneHandler)
						Render::endSceneHandler();

					this->sceneStatus = false;

				}

			return this->pOrigInterface->EndScene();

		}

		HRESULT __stdcall Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) {
			return this->pOrigInterface->Clear(Count, pRects, Flags, Color, Z, Stencil);
		}

		HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* mat) {
			return this->pOrigInterface->SetTransform(State, mat);
		}

		HRESULT __stdcall GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* mat) {
			return this->pOrigInterface->GetTransform(State, mat);
		}

		HRESULT __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* mat) {
			return this->pOrigInterface->MultiplyTransform(State, mat);
		}

		HRESULT __stdcall SetViewport(CONST D3DVIEWPORT9* pViewport) {
			return this->pOrigInterface->SetViewport(pViewport);
		}

		HRESULT __stdcall GetViewport(D3DVIEWPORT9* pViewport) {
			return this->pOrigInterface->GetViewport(pViewport);
		}

		HRESULT __stdcall SetMaterial(CONST D3DMATERIAL9* pMaterial) {
			return this->pOrigInterface->SetMaterial(pMaterial);
		}

		HRESULT __stdcall GetMaterial(D3DMATERIAL9* pMaterial) {
			return this->pOrigInterface->GetMaterial(pMaterial);
		}

		HRESULT __stdcall SetLight(DWORD Index, CONST D3DLIGHT9* pLight) {
			return this->pOrigInterface->SetLight(Index, pLight);
		}

		HRESULT __stdcall GetLight(DWORD Index, D3DLIGHT9* pLight) {
			return this->pOrigInterface->GetLight(Index, pLight);
		}

		HRESULT __stdcall LightEnable(DWORD Index, BOOL Enable) {
			return this->pOrigInterface->LightEnable(Index, Enable);
		}

		HRESULT __stdcall GetLightEnable(DWORD Index, BOOL* pEnable) {
			return this->pOrigInterface->GetLightEnable(Index, pEnable);
		}

		HRESULT __stdcall SetClipPlane(DWORD Index, CONST float* pPlane) {
			return this->pOrigInterface->SetClipPlane(Index, pPlane);
		}

		HRESULT __stdcall GetClipPlane(DWORD Index, float* pPlane) {
			return this->pOrigInterface->GetClipPlane(Index, pPlane);
		}

		HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) {
			return this->pOrigInterface->SetRenderState(State, Value);
		}

		HRESULT __stdcall GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) {
			return this->pOrigInterface->GetRenderState(State, pValue);
		}

		HRESULT __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB) {
			return this->pOrigInterface->CreateStateBlock(Type, ppSB);
		}

		HRESULT __stdcall BeginStateBlock() {
			return this->pOrigInterface->BeginStateBlock();
		}

		HRESULT __stdcall EndStateBlock(IDirect3DStateBlock9** ppSB) {
			return this->pOrigInterface->EndStateBlock(ppSB);
		}

		HRESULT __stdcall SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus) {
			return this->pOrigInterface->SetClipStatus(pClipStatus);
		}

		HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS9* pClipStatus) {
			return this->pOrigInterface->GetClipStatus(pClipStatus);
		}

		HRESULT __stdcall GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture) {
			return this->pOrigInterface->GetTexture(Stage, ppTexture);
		}

		HRESULT __stdcall SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture) {
			return this->pOrigInterface->SetTexture(Stage, pTexture);
		}

		HRESULT __stdcall GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) {
			return this->pOrigInterface->GetTextureStageState(Stage, Type, pValue);
		}

		HRESULT __stdcall SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) {
			return this->pOrigInterface->SetTextureStageState(Stage, Type, Value);
		}

		HRESULT __stdcall GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue) {
			return this->pOrigInterface->GetSamplerState(Sampler, Type, pValue);
		}

		HRESULT __stdcall SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) {
			return this->pOrigInterface->SetSamplerState(Sampler, Type, Value);
		}

		HRESULT __stdcall ValidateDevice(DWORD* pNumPasses) {
			return this->pOrigInterface->ValidateDevice(pNumPasses);
		}

		HRESULT __stdcall SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY* pEntries) {
			return this->pOrigInterface->SetPaletteEntries(PaletteNumber, pEntries);
		}

		HRESULT __stdcall GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) {
			return this->pOrigInterface->GetPaletteEntries(PaletteNumber, pEntries);
		}

		HRESULT __stdcall SetCurrentTexturePalette(UINT PaletteNumber) {
			return this->pOrigInterface->SetCurrentTexturePalette(PaletteNumber);
		}

		HRESULT __stdcall GetCurrentTexturePalette(UINT* PaletteNumber) {
			return this->pOrigInterface->GetCurrentTexturePalette(PaletteNumber);
		}

		HRESULT __stdcall SetScissorRect(CONST RECT* pRect) {
			return this->pOrigInterface->SetScissorRect(pRect);
		}

		HRESULT __stdcall GetScissorRect(RECT* pRect) {
			return this->pOrigInterface->GetScissorRect(pRect);
		}

		HRESULT __stdcall SetSoftwareVertexProcessing(BOOL bSoftware) {
			return this->pOrigInterface->SetSoftwareVertexProcessing(bSoftware);
		}

		BOOL __stdcall GetSoftwareVertexProcessing() {
			return this->pOrigInterface->GetSoftwareVertexProcessing();
		}

		HRESULT __stdcall SetNPatchMode(float nSegments) {
			return this->pOrigInterface->SetNPatchMode(nSegments);
		}

		float __stdcall GetNPatchMode() {
			return this->pOrigInterface->GetNPatchMode();
		}

		HRESULT __stdcall DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) {
			return this->pOrigInterface->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
		}

		HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) {
			return this->pOrigInterface->DrawIndexedPrimitive(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
		}

		HRESULT __stdcall DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
			return this->pOrigInterface->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
		}

		HRESULT __stdcall DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
			return this->pOrigInterface->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
		}

		HRESULT __stdcall ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags) {
			return this->pOrigInterface->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
		}

		HRESULT __stdcall CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl) {
			return this->pOrigInterface->CreateVertexDeclaration(pVertexElements, ppDecl);
		}

		HRESULT __stdcall SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) {
			return this->pOrigInterface->SetVertexDeclaration(pDecl);
		}

		HRESULT __stdcall GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) {
			return this->pOrigInterface->GetVertexDeclaration(ppDecl);
		}

		HRESULT __stdcall SetFVF(DWORD FVF) {
			return this->pOrigInterface->SetFVF(FVF);
		}

		HRESULT __stdcall GetFVF(DWORD* pFVF) {
			return this->pOrigInterface->GetFVF(pFVF);
		}

		HRESULT __stdcall CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader) {
			return this->pOrigInterface->CreateVertexShader(pFunction, ppShader);
		}

		HRESULT __stdcall SetVertexShader(IDirect3DVertexShader9* pShader) {
			return this->pOrigInterface->SetVertexShader(pShader);
		}

		HRESULT __stdcall GetVertexShader(IDirect3DVertexShader9** ppShader) {
			return this->pOrigInterface->GetVertexShader(ppShader);
		}

		HRESULT __stdcall SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
			return this->pOrigInterface->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
		}

		HRESULT __stdcall GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) {
			return this->pOrigInterface->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
		}

		HRESULT __stdcall SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) {
			return this->pOrigInterface->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
		}

		HRESULT __stdcall GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) {
			return this->pOrigInterface->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
		}

		HRESULT __stdcall SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount) {
			return this->pOrigInterface->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
		}

		HRESULT __stdcall GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) {
			return this->pOrigInterface->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
		}

		HRESULT __stdcall SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride) {
			return this->pOrigInterface->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
		}

		HRESULT __stdcall GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* OffsetInBytes, UINT* pStride) {
			return this->pOrigInterface->GetStreamSource(StreamNumber, ppStreamData, OffsetInBytes, pStride);
		}

		HRESULT __stdcall SetStreamSourceFreq(UINT StreamNumber, UINT Divider) {
			return this->pOrigInterface->SetStreamSourceFreq(StreamNumber, Divider);
		}

		HRESULT __stdcall GetStreamSourceFreq(UINT StreamNumber, UINT* Divider) {
			return this->pOrigInterface->GetStreamSourceFreq(StreamNumber, Divider);
		}

		HRESULT __stdcall SetIndices(IDirect3DIndexBuffer9* pIndexData) {
			return this->pOrigInterface->SetIndices(pIndexData);
		}

		HRESULT __stdcall GetIndices(IDirect3DIndexBuffer9** ppIndexData) {
			return this->pOrigInterface->GetIndices(ppIndexData);
		}

		HRESULT __stdcall CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader) {
			return this->pOrigInterface->CreatePixelShader(pFunction, ppShader);
		}

		HRESULT __stdcall SetPixelShader(IDirect3DPixelShader9* pShader) {
			return this->pOrigInterface->SetPixelShader(pShader);
		}

		HRESULT __stdcall GetPixelShader(IDirect3DPixelShader9** ppShader) {
			return this->pOrigInterface->GetPixelShader(ppShader);
		}

		HRESULT __stdcall SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
			return this->pOrigInterface->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
		}

		HRESULT __stdcall GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) {
			return this->pOrigInterface->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
		}

		HRESULT __stdcall SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) {
			return this->pOrigInterface->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
		}

		HRESULT __stdcall GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) {
			return this->pOrigInterface->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
		}

		HRESULT __stdcall SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount) {
			return this->pOrigInterface->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
		}

		HRESULT __stdcall GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) {
			return this->pOrigInterface->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
		}

		HRESULT __stdcall DrawRectPatch(UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo) {
			return this->pOrigInterface->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
		}

		HRESULT __stdcall DrawTriPatch(UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo) {
			return this->pOrigInterface->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
		}

		HRESULT __stdcall DeletePatch(UINT Handle) {
			return this->pOrigInterface->DeletePatch(Handle);
		}

		HRESULT __stdcall CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) {
			return this->pOrigInterface->CreateQuery(Type, ppQuery);
		}

	};

	interface IDirect3D9Hook : public IDirect3D9 {
	private:

		IDirect3D9* const pOrigInterface;

	public:

		IDirect3D9Hook(IDirect3D9* pOrigInterface) :
			pOrigInterface(pOrigInterface)
		{

			this->pOrigInterface->AddRef();

		}

		~IDirect3D9Hook() {

			this->pOrigInterface->Release();

		}

		// IDirect3D9 Interface methods...
		// ------------------------------------------------

		__declspec(nothrow) HRESULT __stdcall QueryInterface(THIS_ REFIID riid, void** ppvObj) {

			*ppvObj = nullptr;

			const HRESULT hResult = this->pOrigInterface->QueryInterface(riid, ppvObj);
			if (SUCCEEDED(hResult)) *ppvObj = this;
			return hResult;

		}

		__declspec(nothrow) ULONG __stdcall AddRef() {
			return this->pOrigInterface->AddRef();
		}

		__declspec(nothrow) ULONG __stdcall Release() {

			const ULONG count = this->pOrigInterface->Release();

			if (count <= 1) {

				delete this;
				return 0;

			}

			return count;

		}

		__declspec(nothrow) HRESULT __stdcall RegisterSoftwareDevice(THIS_ void* pInitializeFunction) {
			return this->pOrigInterface->RegisterSoftwareDevice(pInitializeFunction);
		}

		__declspec(nothrow) UINT __stdcall GetAdapterCount() {
			return this->pOrigInterface->GetAdapterCount();
		}

		__declspec(nothrow) HRESULT __stdcall GetAdapterIdentifier(THIS_ UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier) {
			return this->pOrigInterface->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
		}

		__declspec(nothrow) UINT __stdcall GetAdapterModeCount(THIS_ UINT Adapter, D3DFORMAT Format) {
			return this->pOrigInterface->GetAdapterModeCount(Adapter, Format);
		}

		__declspec(nothrow) HRESULT __stdcall EnumAdapterModes(THIS_ UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode) {
			return this->pOrigInterface->EnumAdapterModes(Adapter, Format, Mode, pMode);
		}

		__declspec(nothrow) HRESULT __stdcall GetAdapterDisplayMode(THIS_ UINT Adapter, D3DDISPLAYMODE* pMode) {
			return this->pOrigInterface->GetAdapterDisplayMode(Adapter, pMode);
		}

		__declspec(nothrow) HRESULT __stdcall CheckDeviceType(THIS_ UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) {
			return this->pOrigInterface->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
		}

		__declspec(nothrow) HRESULT __stdcall CheckDeviceFormat(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) {
			return this->pOrigInterface->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
		}

		__declspec(nothrow) HRESULT __stdcall CheckDeviceMultiSampleType(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) {
			return this->pOrigInterface->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
		}

		__declspec(nothrow) HRESULT __stdcall CheckDepthStencilMatch(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) {
			return this->pOrigInterface->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
		}

		__declspec(nothrow) HRESULT __stdcall CheckDeviceFormatConversion(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) {
			return this->pOrigInterface->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
		}

		__declspec(nothrow) HRESULT __stdcall GetDeviceCaps(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps) {
			return this->pOrigInterface->GetDeviceCaps(Adapter, DeviceType, pCaps);
		}

		__declspec(nothrow) HMONITOR __stdcall GetAdapterMonitor(THIS_ UINT Adapter) {
			return this->pOrigInterface->GetAdapterMonitor(Adapter);
		}

		__declspec(nothrow) HRESULT __stdcall CreateDevice(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface) {

			const HRESULT hResult = this->pOrigInterface->CreateDevice(
				Adapter, DeviceType, hFocusWindow,
				BehaviorFlags, pPresentationParameters,
				ppReturnedDeviceInterface
			);

			if (FAILED(hResult) || (this != GtaDirectPointer) || (ppReturnedDeviceInterface != &GtaDevicePointer))
				return hResult;

			Logger::LogToFile(
				"[dbg:render:direct_%p] : device interface (ptr:%p) success created with hwnd (value:%u)"
				" windowed (value:%d) screenwidth (value:%u) screenheight (value:%u)",
				this, *ppReturnedDeviceInterface, hFocusWindow, pPresentationParameters->Windowed,
				pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight
			);

			IDirect3DDevice9Hook* pHookDevice = nullptr;

			if (!(pHookDevice = new IDirect3DDevice9Hook(*ppReturnedDeviceInterface))) {

				Logger::LogToFile("[err:render:direct_%p] : failed to allocate memory for hook interface", this);
				return hResult;

			}

			Logger::LogToFile(
				"[dbg:render:direct_%p] : pointer successfully replaced from orig (ptr:%p) to hook (ptr:%p)",
				this, *ppReturnedDeviceInterface, pHookDevice
			);

			if (Render::pDirectInterface && Render::deviceFreeHandler)
				Render::deviceFreeHandler();

			Render::deviceMutex.lock();
			Render::pDirectInterface = this->pOrigInterface;
			Render::pDeviceInterface = *ppReturnedDeviceInterface;
			Render::deviceParameters = *pPresentationParameters;
			Render::deviceMutex.unlock();

			if (Render::deviceInitHandler) Render::deviceInitHandler(
				Render::pDirectInterface, Render::pDeviceInterface,
				&Render::deviceParameters
			);

			*ppReturnedDeviceInterface = pHookDevice;

			Logger::LogToFile("[dbg:render:direct_%p] : hook success created", this);

			return hResult;

		}

	};

private:

	static Memory::JumpHookPtr hookDirect3DCreate9;

	static IDirect3D9* CALLBACK HookFuncDirect3DCreate9(UINT SDKVersion);

public:

	static bool Init(
		const DeviceInitHandlerType& deviceInitHandler,
		const BeforeResetHandlerType& beforeResetHandler,
		const BeginSceneHandlerType& beginSceneHandler,
		const RenderHandlerType& renderHandler,
		const EndSceneHandlerType& endSceneHandler,
		const AfterResetHandlerType& afterResetHandler,
		const DeviceFreeHandlerType& deviceFreeHandler
	);

	static bool GetWindowHandle(HWND& windowHandle);

	static bool GetScreenSize(float& screenWidth, float& screenHeight);

	static bool ConvertBaseXValueToScreenXValue(const float baseValue, float& screenValue);
	static bool ConvertBaseYValueToScreenYValue(const float baseValue, float& screenValue);

	static bool ConvertScreenXValueToBaseXValue(const float screenValue, float& baseValue);
	static bool ConvertScreenYValueToBaseYValue(const float screenValue, float& baseValue);

	static void Free();

};
