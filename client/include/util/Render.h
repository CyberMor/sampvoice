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

#define GtaDirectPointer (*(IDirect3D9**)(0xC97C20))
#define GtaDevicePointer (*(IDirect3DDevice9**)(0xC97C28))
#define GtaParametersPointer ((D3DPRESENT_PARAMETERS*)(0xC9C040))
#define GtaDirect3DCreate9 ((IDirect3D9*(CALLBACK*)(UINT))(0x807C2B))

class Render {

    using DeviceInitHandlerType = std::function<void(IDirect3D9*, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)>;
    using BeforeResetHandlerType = std::function<void()>;
    using BeginSceneHandlerType = std::function<void()>;
    using RenderHandlerType = std::function<void()>;
    using EndSceneHandlerType = std::function<void()>;
    using AfterResetHandlerType = std::function<void(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)>;
    using DeviceFreeHandlerType = std::function<void()>;

public:

    static constexpr float BaseWidth = 640.f;
    static constexpr float BaseHeight = 480.f;

public:

    static bool Init(
        DeviceInitHandlerType&& deviceInitHandler,
        BeforeResetHandlerType&& beforeResetHandler,
        BeginSceneHandlerType&& beginSceneHandler,
        RenderHandlerType&& renderHandler,
        EndSceneHandlerType&& endSceneHandler,
        AfterResetHandlerType&& afterResetHandler,
        DeviceFreeHandlerType&& deviceFreeHandler
    ) noexcept;
    static void Free() noexcept;

    static bool GetWindowHandle(HWND& windowHandle) noexcept;
    static bool GetScreenSize(float& screenWidth, float& screenHeight) noexcept;
    static bool ConvertBaseXValueToScreenXValue(float baseValue, float& screenValue) noexcept;
    static bool ConvertBaseYValueToScreenYValue(float baseValue, float& screenValue) noexcept;
    static bool ConvertScreenXValueToBaseXValue(float screenValue, float& baseValue) noexcept;
    static bool ConvertScreenYValueToBaseYValue(float screenValue, float& baseValue) noexcept;

private:

    interface IDirect3DDevice9Hook : public IDirect3DDevice9 {

        explicit IDirect3DDevice9Hook(IDirect3DDevice9* pOrigInterface) noexcept
            : pOrigInterface(pOrigInterface)
        {
            this->pOrigInterface->AddRef();
        }

        ~IDirect3DDevice9Hook() noexcept
        {
            this->pOrigInterface->Release();
        }

        // IDirect3DDevice9 Interface methods...
        // ------------------------------------------------

        __declspec(nothrow) HRESULT __stdcall Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) override
        {
            if (this->pOrigInterface == Render::pDeviceInterface && !this->resetStatus)
                if (Render::renderHandler) Render::renderHandler();

            return this->pOrigInterface->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
        }

        __declspec(nothrow) HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj) override
        {
            *ppvObj = nullptr;

            const auto hResult = this->pOrigInterface->QueryInterface(riid, ppvObj);

            if (SUCCEEDED(hResult)) *ppvObj = this;

            return hResult;
        }

        __declspec(nothrow) ULONG __stdcall AddRef() override
        {
            return this->pOrigInterface->AddRef();
        }

        __declspec(nothrow) ULONG __stdcall Release() override
        {
            const auto count = this->pOrigInterface->Release();

            if (count <= 1)
            {
                if (this->pOrigInterface == Render::pDeviceInterface)
                {
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

        __declspec(nothrow) HRESULT __stdcall TestCooperativeLevel() override
        {
            return this->pOrigInterface->TestCooperativeLevel();
        }

        __declspec(nothrow) UINT __stdcall GetAvailableTextureMem() override
        {
            return this->pOrigInterface->GetAvailableTextureMem();
        }

        __declspec(nothrow) HRESULT __stdcall EvictManagedResources() override
        {
            return this->pOrigInterface->EvictManagedResources();
        }

        __declspec(nothrow) HRESULT __stdcall GetDirect3D(IDirect3D9** ppD3D9) override
        {
            return this->pOrigInterface->GetDirect3D(ppD3D9);
        }

        __declspec(nothrow) HRESULT __stdcall GetDeviceCaps(D3DCAPS9* pCaps) override
        {
            return this->pOrigInterface->GetDeviceCaps(pCaps);
        }

        __declspec(nothrow) HRESULT __stdcall GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) override
        {
            return this->pOrigInterface->GetDisplayMode(iSwapChain, pMode);
        }

        __declspec(nothrow) HRESULT __stdcall GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters) override
        {
            return this->pOrigInterface->GetCreationParameters(pParameters);
        }

        __declspec(nothrow) HRESULT __stdcall SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap) override
        {
            return this->pOrigInterface->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
        }

        __declspec(nothrow) VOID __stdcall SetCursorPosition(int X, int Y, DWORD Flags) override
        {
            this->pOrigInterface->SetCursorPosition(X, Y, Flags);
        }

        __declspec(nothrow) BOOL __stdcall ShowCursor(BOOL bShow) override
        {
            return this->pOrigInterface->ShowCursor(bShow);
        }

        __declspec(nothrow) HRESULT __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain) override
        {
            return this->pOrigInterface->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
        }

        __declspec(nothrow) HRESULT __stdcall GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain) override
        {
            return this->pOrigInterface->GetSwapChain(iSwapChain, pSwapChain);
        }

        __declspec(nothrow) UINT __stdcall GetNumberOfSwapChains() override
        {
            return this->pOrigInterface->GetNumberOfSwapChains();
        }

        __declspec(nothrow) HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) override
        {
            if (this->pOrigInterface == Render::pDeviceInterface && !this->resetStatus)
            {
                if (Render::beforeResetHandler) Render::beforeResetHandler();

                this->resetStatus = true;
            }

            const auto hResult = this->pOrigInterface->Reset(pPresentationParameters);

            if (this->pOrigInterface == Render::pDeviceInterface && SUCCEEDED(hResult))
            {
                Render::deviceMutex.lock();
                Render::deviceParameters = *pPresentationParameters;
                Render::deviceMutex.unlock();

                if (this->resetStatus)
                {
                    if (Render::afterResetHandler) Render::afterResetHandler(
                        Render::pDeviceInterface, &Render::deviceParameters);

                    this->resetStatus = false;
                }
            }

            return hResult;
        }

        __declspec(nothrow) HRESULT __stdcall GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) override
        {
            return this->pOrigInterface->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
        }

        __declspec(nothrow) HRESULT __stdcall GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus) override
        {
            return this->pOrigInterface->GetRasterStatus(iSwapChain, pRasterStatus);
        }

        __declspec(nothrow) HRESULT __stdcall SetDialogBoxMode(BOOL bEnableDialogs) override
        {
            return this->pOrigInterface->SetDialogBoxMode(bEnableDialogs);
        }

        __declspec(nothrow) VOID __stdcall SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp) override
        {
            this->pOrigInterface->SetGammaRamp(iSwapChain, Flags, pRamp);
        }

        __declspec(nothrow) VOID __stdcall GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) override
        {
            this->pOrigInterface->GetGammaRamp(iSwapChain, pRamp);
        }

        __declspec(nothrow) HRESULT __stdcall CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) override
        {
            return this->pOrigInterface->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
        }

        __declspec(nothrow) HRESULT __stdcall CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle) override
        {
            return this->pOrigInterface->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
        }

        __declspec(nothrow) HRESULT __stdcall CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle) override
        {
            return this->pOrigInterface->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
        }

        __declspec(nothrow) HRESULT __stdcall CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle) override
        {
            return this->pOrigInterface->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
        }

        __declspec(nothrow) HRESULT __stdcall CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle) override
        {
            return this->pOrigInterface->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
        }

        __declspec(nothrow) HRESULT __stdcall CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) override
        {
            return this->pOrigInterface->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
        }

        __declspec(nothrow) HRESULT __stdcall CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) override
        {
            return this->pOrigInterface->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
        }

        __declspec(nothrow) HRESULT __stdcall UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint) override
        {
            return this->pOrigInterface->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
        }

        __declspec(nothrow) HRESULT __stdcall UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture) override
        {
            return this->pOrigInterface->UpdateTexture(pSourceTexture, pDestinationTexture);
        }

        __declspec(nothrow) HRESULT __stdcall GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface) override
        {
            return this->pOrigInterface->GetRenderTargetData(pRenderTarget, pDestSurface);
        }

        __declspec(nothrow) HRESULT __stdcall GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface) override
        {
            return this->pOrigInterface->GetFrontBufferData(iSwapChain, pDestSurface);
        }

        __declspec(nothrow) HRESULT __stdcall StretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) override
        {
            return this->pOrigInterface->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
        }

        __declspec(nothrow) HRESULT __stdcall ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color) override
        {
            return this->pOrigInterface->ColorFill(pSurface, pRect, color);
        }

        __declspec(nothrow) HRESULT __stdcall CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) override
        {
            return this->pOrigInterface->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
        }

        __declspec(nothrow) HRESULT __stdcall SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) override
        {
            return this->pOrigInterface->SetRenderTarget(RenderTargetIndex, pRenderTarget);
        }

        __declspec(nothrow) HRESULT __stdcall GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget) override
        {
            return this->pOrigInterface->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
        }

        __declspec(nothrow) HRESULT __stdcall SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) override
        {
            return this->pOrigInterface->SetDepthStencilSurface(pNewZStencil);
        }

        __declspec(nothrow) HRESULT __stdcall GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface) override
        {
            return this->pOrigInterface->GetDepthStencilSurface(ppZStencilSurface);
        }

        __declspec(nothrow) HRESULT __stdcall BeginScene() override
        {
            const auto hResult = this->pOrigInterface->BeginScene();

            if (this->pOrigInterface == Render::pDeviceInterface && SUCCEEDED(hResult) &&
                !this->resetStatus && !this->sceneStatus)
            {
                if (Render::beginSceneHandler) Render::beginSceneHandler();

                this->sceneStatus = true;
            }

            return hResult;
        }

        __declspec(nothrow) HRESULT __stdcall EndScene() override
        {
            if (this->pOrigInterface == Render::pDeviceInterface &&
                !this->resetStatus && this->sceneStatus)
            {
                if (Render::endSceneHandler) Render::endSceneHandler();

                this->sceneStatus = false;
            }

            return this->pOrigInterface->EndScene();
        }

        __declspec(nothrow) HRESULT __stdcall Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) override
        {
            return this->pOrigInterface->Clear(Count, pRects, Flags, Color, Z, Stencil);
        }

        __declspec(nothrow) HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* mat) override
        {
            return this->pOrigInterface->SetTransform(State, mat);
        }

        __declspec(nothrow) HRESULT __stdcall GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* mat) override
        {
            return this->pOrigInterface->GetTransform(State, mat);
        }

        __declspec(nothrow) HRESULT __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* mat) override
        {
            return this->pOrigInterface->MultiplyTransform(State, mat);
        }

        __declspec(nothrow) HRESULT __stdcall SetViewport(CONST D3DVIEWPORT9* pViewport) override
        {
            return this->pOrigInterface->SetViewport(pViewport);
        }

        __declspec(nothrow) HRESULT __stdcall GetViewport(D3DVIEWPORT9* pViewport) override
        {
            return this->pOrigInterface->GetViewport(pViewport);
        }

        __declspec(nothrow) HRESULT __stdcall SetMaterial(CONST D3DMATERIAL9* pMaterial) override
        {
            return this->pOrigInterface->SetMaterial(pMaterial);
        }

        __declspec(nothrow) HRESULT __stdcall GetMaterial(D3DMATERIAL9* pMaterial) override
        {
            return this->pOrigInterface->GetMaterial(pMaterial);
        }

        __declspec(nothrow) HRESULT __stdcall SetLight(DWORD Index, CONST D3DLIGHT9* pLight) override
        {
            return this->pOrigInterface->SetLight(Index, pLight);
        }

        __declspec(nothrow) HRESULT __stdcall GetLight(DWORD Index, D3DLIGHT9* pLight) override
        {
            return this->pOrigInterface->GetLight(Index, pLight);
        }

        __declspec(nothrow) HRESULT __stdcall LightEnable(DWORD Index, BOOL Enable) override
        {
            return this->pOrigInterface->LightEnable(Index, Enable);
        }

        __declspec(nothrow) HRESULT __stdcall GetLightEnable(DWORD Index, BOOL* pEnable) override
        {
            return this->pOrigInterface->GetLightEnable(Index, pEnable);
        }

        __declspec(nothrow) HRESULT __stdcall SetClipPlane(DWORD Index, CONST float* pPlane) override
        {
            return this->pOrigInterface->SetClipPlane(Index, pPlane);
        }

        __declspec(nothrow) HRESULT __stdcall GetClipPlane(DWORD Index, float* pPlane) override
        {
            return this->pOrigInterface->GetClipPlane(Index, pPlane);
        }

        __declspec(nothrow) HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) override
        {
            return this->pOrigInterface->SetRenderState(State, Value);
        }

        __declspec(nothrow) HRESULT __stdcall GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) override
        {
            return this->pOrigInterface->GetRenderState(State, pValue);
        }

        __declspec(nothrow) HRESULT __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB) override
        {
            return this->pOrigInterface->CreateStateBlock(Type, ppSB);
        }

        __declspec(nothrow) HRESULT __stdcall BeginStateBlock() override
        {
            return this->pOrigInterface->BeginStateBlock();
        }

        __declspec(nothrow) HRESULT __stdcall EndStateBlock(IDirect3DStateBlock9** ppSB) override
        {
            return this->pOrigInterface->EndStateBlock(ppSB);
        }

        __declspec(nothrow) HRESULT __stdcall SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus) override
        {
            return this->pOrigInterface->SetClipStatus(pClipStatus);
        }

        __declspec(nothrow) HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS9* pClipStatus) override
        {
            return this->pOrigInterface->GetClipStatus(pClipStatus);
        }

        __declspec(nothrow) HRESULT __stdcall GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture) override
        {
            return this->pOrigInterface->GetTexture(Stage, ppTexture);
        }

        __declspec(nothrow) HRESULT __stdcall SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture) override
        {
            return this->pOrigInterface->SetTexture(Stage, pTexture);
        }

        __declspec(nothrow) HRESULT __stdcall GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) override
        {
            return this->pOrigInterface->GetTextureStageState(Stage, Type, pValue);
        }

        __declspec(nothrow) HRESULT __stdcall SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) override
        {
            return this->pOrigInterface->SetTextureStageState(Stage, Type, Value);
        }

        __declspec(nothrow) HRESULT __stdcall GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue) override
        {
            return this->pOrigInterface->GetSamplerState(Sampler, Type, pValue);
        }

        __declspec(nothrow) HRESULT __stdcall SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) override
        {
            return this->pOrigInterface->SetSamplerState(Sampler, Type, Value);
        }

        __declspec(nothrow) HRESULT __stdcall ValidateDevice(DWORD* pNumPasses) override
        {
            return this->pOrigInterface->ValidateDevice(pNumPasses);
        }

        __declspec(nothrow) HRESULT __stdcall SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY* pEntries) override
        {
            return this->pOrigInterface->SetPaletteEntries(PaletteNumber, pEntries);
        }

        __declspec(nothrow) HRESULT __stdcall GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) override
        {
            return this->pOrigInterface->GetPaletteEntries(PaletteNumber, pEntries);
        }

        __declspec(nothrow) HRESULT __stdcall SetCurrentTexturePalette(UINT PaletteNumber) override
        {
            return this->pOrigInterface->SetCurrentTexturePalette(PaletteNumber);
        }

        __declspec(nothrow) HRESULT __stdcall GetCurrentTexturePalette(UINT* PaletteNumber) override
        {
            return this->pOrigInterface->GetCurrentTexturePalette(PaletteNumber);
        }

        __declspec(nothrow) HRESULT __stdcall SetScissorRect(CONST RECT* pRect) override
        {
            return this->pOrigInterface->SetScissorRect(pRect);
        }

        __declspec(nothrow) HRESULT __stdcall GetScissorRect(RECT* pRect) override
        {
            return this->pOrigInterface->GetScissorRect(pRect);
        }

        __declspec(nothrow) HRESULT __stdcall SetSoftwareVertexProcessing(BOOL bSoftware) override
        {
            return this->pOrigInterface->SetSoftwareVertexProcessing(bSoftware);
        }

        __declspec(nothrow) BOOL __stdcall GetSoftwareVertexProcessing() override
        {
            return this->pOrigInterface->GetSoftwareVertexProcessing();
        }

        __declspec(nothrow) HRESULT __stdcall SetNPatchMode(float nSegments) override
        {
            return this->pOrigInterface->SetNPatchMode(nSegments);
        }

        __declspec(nothrow) FLOAT __stdcall GetNPatchMode() override
        {
            return this->pOrigInterface->GetNPatchMode();
        }

        __declspec(nothrow) HRESULT __stdcall DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) override
        {
            return this->pOrigInterface->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
        }

        __declspec(nothrow) HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) override
        {
            return this->pOrigInterface->DrawIndexedPrimitive(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
        }

        __declspec(nothrow) HRESULT __stdcall DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) override
        {
            return this->pOrigInterface->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
        }

        __declspec(nothrow) HRESULT __stdcall DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) override
        {
            return this->pOrigInterface->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
        }

        __declspec(nothrow) HRESULT __stdcall ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags) override
        {
            return this->pOrigInterface->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
        }

        __declspec(nothrow) HRESULT __stdcall CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl) override
        {
            return this->pOrigInterface->CreateVertexDeclaration(pVertexElements, ppDecl);
        }

        __declspec(nothrow) HRESULT __stdcall SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) override
        {
            return this->pOrigInterface->SetVertexDeclaration(pDecl);
        }

        __declspec(nothrow) HRESULT __stdcall GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) override
        {
            return this->pOrigInterface->GetVertexDeclaration(ppDecl);
        }

        __declspec(nothrow) HRESULT __stdcall SetFVF(DWORD FVF) override
        {
            return this->pOrigInterface->SetFVF(FVF);
        }

        __declspec(nothrow) HRESULT __stdcall GetFVF(DWORD* pFVF) override
        {
            return this->pOrigInterface->GetFVF(pFVF);
        }

        __declspec(nothrow) HRESULT __stdcall CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader) override
        {
            return this->pOrigInterface->CreateVertexShader(pFunction, ppShader);
        }

        __declspec(nothrow) HRESULT __stdcall SetVertexShader(IDirect3DVertexShader9* pShader) override
        {
            return this->pOrigInterface->SetVertexShader(pShader);
        }

        __declspec(nothrow) HRESULT __stdcall GetVertexShader(IDirect3DVertexShader9** ppShader) override
        {
            return this->pOrigInterface->GetVertexShader(ppShader);
        }

        __declspec(nothrow) HRESULT __stdcall SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) override
        {
            return this->pOrigInterface->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
        }

        __declspec(nothrow) HRESULT __stdcall GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) override
        {
            return this->pOrigInterface->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
        }

        __declspec(nothrow) HRESULT __stdcall SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) override
        {
            return this->pOrigInterface->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
        }

        __declspec(nothrow) HRESULT __stdcall GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) override
        {
            return this->pOrigInterface->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
        }

        __declspec(nothrow) HRESULT __stdcall SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount) override
        {
            return this->pOrigInterface->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
        }

        __declspec(nothrow) HRESULT __stdcall GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) override
        {
            return this->pOrigInterface->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
        }

        __declspec(nothrow) HRESULT __stdcall SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride) override
        {
            return this->pOrigInterface->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
        }

        __declspec(nothrow) HRESULT __stdcall GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* OffsetInBytes, UINT* pStride) override
        {
            return this->pOrigInterface->GetStreamSource(StreamNumber, ppStreamData, OffsetInBytes, pStride);
        }

        __declspec(nothrow) HRESULT __stdcall SetStreamSourceFreq(UINT StreamNumber, UINT Divider) override
        {
            return this->pOrigInterface->SetStreamSourceFreq(StreamNumber, Divider);
        }

        __declspec(nothrow) HRESULT __stdcall GetStreamSourceFreq(UINT StreamNumber, UINT* Divider) override
        {
            return this->pOrigInterface->GetStreamSourceFreq(StreamNumber, Divider);
        }

        __declspec(nothrow) HRESULT __stdcall SetIndices(IDirect3DIndexBuffer9* pIndexData) override
        {
            return this->pOrigInterface->SetIndices(pIndexData);
        }

        __declspec(nothrow) HRESULT __stdcall GetIndices(IDirect3DIndexBuffer9** ppIndexData) override
        {
            return this->pOrigInterface->GetIndices(ppIndexData);
        }

        __declspec(nothrow) HRESULT __stdcall CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader) override
        {
            return this->pOrigInterface->CreatePixelShader(pFunction, ppShader);
        }

        __declspec(nothrow) HRESULT __stdcall SetPixelShader(IDirect3DPixelShader9* pShader) override
        {
            return this->pOrigInterface->SetPixelShader(pShader);
        }

        __declspec(nothrow) HRESULT __stdcall GetPixelShader(IDirect3DPixelShader9** ppShader) override
        {
            return this->pOrigInterface->GetPixelShader(ppShader);
        }

        __declspec(nothrow) HRESULT __stdcall SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) override
        {
            return this->pOrigInterface->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
        }

        __declspec(nothrow) HRESULT __stdcall GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) override
        {
            return this->pOrigInterface->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
        }

        __declspec(nothrow) HRESULT __stdcall SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) override
        {
            return this->pOrigInterface->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
        }

        __declspec(nothrow) HRESULT __stdcall GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) override
        {
            return this->pOrigInterface->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
        }

        __declspec(nothrow) HRESULT __stdcall SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount) override
        {
            return this->pOrigInterface->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
        }

        __declspec(nothrow) HRESULT __stdcall GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) override
        {
            return this->pOrigInterface->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
        }

        __declspec(nothrow) HRESULT __stdcall DrawRectPatch(UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo) override
        {
            return this->pOrigInterface->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
        }

        __declspec(nothrow) HRESULT __stdcall DrawTriPatch(UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo) override
        {
            return this->pOrigInterface->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
        }

        __declspec(nothrow) HRESULT __stdcall DeletePatch(UINT Handle) override
        {
            return this->pOrigInterface->DeletePatch(Handle);
        }

        __declspec(nothrow) HRESULT __stdcall CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) override
        {
            return this->pOrigInterface->CreateQuery(Type, ppQuery);
        }

    private:

        IDirect3DDevice9* pOrigInterface { nullptr };

        bool resetStatus { false };
        bool sceneStatus { false };

    };

    interface IDirect3D9Hook : public IDirect3D9 {

        explicit IDirect3D9Hook(IDirect3D9* pOrigInterface) noexcept
            : pOrigInterface(pOrigInterface)
        {
            this->pOrigInterface->AddRef();
        }

        ~IDirect3D9Hook() noexcept
        {
            this->pOrigInterface->Release();
        }

        // IDirect3D9 Interface methods...
        // ------------------------------------------------

        __declspec(nothrow) HRESULT __stdcall QueryInterface(THIS_ REFIID riid, void** ppvObj) override
        {
            *ppvObj = nullptr;

            const auto hResult = this->pOrigInterface->QueryInterface(riid, ppvObj);

            if (SUCCEEDED(hResult)) *ppvObj = this;

            return hResult;
        }

        __declspec(nothrow) ULONG __stdcall AddRef() override
        {
            return this->pOrigInterface->AddRef();
        }

        __declspec(nothrow) ULONG __stdcall Release() override
        {
            const auto count = this->pOrigInterface->Release();

            if (count <= 1)
            {
                delete this;
                return 0;
            }

            return count;
        }

        __declspec(nothrow) HRESULT __stdcall RegisterSoftwareDevice(THIS_ void* pInitializeFunction) override
        {
            return this->pOrigInterface->RegisterSoftwareDevice(pInitializeFunction);
        }

        __declspec(nothrow) UINT __stdcall GetAdapterCount() override
        {
            return this->pOrigInterface->GetAdapterCount();
        }

        __declspec(nothrow) HRESULT __stdcall GetAdapterIdentifier(THIS_ UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier) override
        {
            return this->pOrigInterface->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
        }

        __declspec(nothrow) UINT __stdcall GetAdapterModeCount(THIS_ UINT Adapter, D3DFORMAT Format) override
        {
            return this->pOrigInterface->GetAdapterModeCount(Adapter, Format);
        }

        __declspec(nothrow) HRESULT __stdcall EnumAdapterModes(THIS_ UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode) override
        {
            return this->pOrigInterface->EnumAdapterModes(Adapter, Format, Mode, pMode);
        }

        __declspec(nothrow) HRESULT __stdcall GetAdapterDisplayMode(THIS_ UINT Adapter, D3DDISPLAYMODE* pMode) override
        {
            return this->pOrigInterface->GetAdapterDisplayMode(Adapter, pMode);
        }

        __declspec(nothrow) HRESULT __stdcall CheckDeviceType(THIS_ UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) override
        {
            return this->pOrigInterface->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
        }

        __declspec(nothrow) HRESULT __stdcall CheckDeviceFormat(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) override
        {
            return this->pOrigInterface->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
        }

        __declspec(nothrow) HRESULT __stdcall CheckDeviceMultiSampleType(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) override
        {
            return this->pOrigInterface->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
        }

        __declspec(nothrow) HRESULT __stdcall CheckDepthStencilMatch(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) override
        {
            return this->pOrigInterface->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
        }

        __declspec(nothrow) HRESULT __stdcall CheckDeviceFormatConversion(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) override
        {
            return this->pOrigInterface->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
        }

        __declspec(nothrow) HRESULT __stdcall GetDeviceCaps(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps) override
        {
            return this->pOrigInterface->GetDeviceCaps(Adapter, DeviceType, pCaps);
        }

        __declspec(nothrow) HMONITOR __stdcall GetAdapterMonitor(THIS_ UINT Adapter) override
        {
            return this->pOrigInterface->GetAdapterMonitor(Adapter);
        }

        __declspec(nothrow) HRESULT __stdcall CreateDevice(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface) override
        {
            const auto hResult = this->pOrigInterface->CreateDevice(
                Adapter, DeviceType, hFocusWindow,
                BehaviorFlags, pPresentationParameters,
                ppReturnedDeviceInterface
            );

            if (FAILED(hResult) || (this != GtaDirectPointer) ||
                (ppReturnedDeviceInterface != &GtaDevicePointer)) return hResult;

            Logger::LogToFile(
                "[dbg:render:direct_%p] : device interface (ptr:%p) success created with hwnd (value:%u)"
                " windowed (value:%d) screenwidth (value:%u) screenheight (value:%u)",
                this, *ppReturnedDeviceInterface, hFocusWindow, pPresentationParameters->Windowed,
                pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight
            );

            IDirect3DDevice9Hook* pHookDevice;

            if (!(pHookDevice = new IDirect3DDevice9Hook(*ppReturnedDeviceInterface)))
            {
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

    private:

        IDirect3D9* pOrigInterface { nullptr };

    };

private:

    static IDirect3D9* CALLBACK HookFuncDirect3DCreate9(UINT SDKVersion) noexcept;

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

    static Memory::JumpHookPtr hookDirect3DCreate9;

};
