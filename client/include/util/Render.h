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

#define GtaDirectPointer *reinterpret_cast<IDirect3D9**>(0xC97C20)
#define GtaDevicePointer *reinterpret_cast<IDirect3DDevice9**>(0xC97C28)
#define GtaParametersPointer reinterpret_cast<D3DPRESENT_PARAMETERS*>(0xC9C040)
#define GtaDirect3DCreate9 reinterpret_cast<IDirect3D9*(CALLBACK*)(UINT)>(0x807C2B)

class Render {

    Render() = delete;
    ~Render() = delete;
    Render(const Render&) = delete;
    Render(Render&&) = delete;
    Render& operator=(const Render&) = delete;
    Render& operator=(Render&&) = delete;

private:

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

    static bool Init(DeviceInitHandlerType deviceInitHandler,
                     BeforeResetHandlerType beforeResetHandler,
                     BeginSceneHandlerType beginSceneHandler,
                     RenderHandlerType renderHandler,
                     EndSceneHandlerType endSceneHandler,
                     AfterResetHandlerType afterResetHandler,
                     DeviceFreeHandlerType deviceFreeHandler) noexcept;
    static void Free() noexcept;

    static bool GetWindowHandle(HWND& windowHandle) noexcept;
    static bool GetScreenSize(float& screenWidth, float& screenHeight) noexcept;
    static bool ConvertBaseXValueToScreenXValue(float baseValue, float& screenValue) noexcept;
    static bool ConvertBaseYValueToScreenYValue(float baseValue, float& screenValue) noexcept;
    static bool ConvertScreenXValueToBaseXValue(float screenValue, float& baseValue) noexcept;
    static bool ConvertScreenYValueToBaseYValue(float screenValue, float& baseValue) noexcept;

private:

    interface IDirect3DDevice9Hook : public IDirect3DDevice9 {

        explicit IDirect3DDevice9Hook(IDirect3DDevice9* const pOrigInterface) noexcept
            : pOrigInterface(pOrigInterface) { this->pOrigInterface->AddRef(); }

        ~IDirect3DDevice9Hook() noexcept { this->pOrigInterface->Release(); }

        // IDirect3DDevice9 Interface methods...
        // ------------------------------------------------

        HRESULT __stdcall Present(CONST RECT* const pSourceRect,
                                  CONST RECT* const pDestRect,
                                  const HWND hDestWindowOverride,
                                  CONST RGNDATA* const pDirtyRegion) noexcept override
        {
            if (this->pOrigInterface == Render::pDeviceInterface && !this->resetStatus)
                if (Render::renderHandler) Render::renderHandler();

            return this->pOrigInterface->Present(pSourceRect, pDestRect,
                hDestWindowOverride, pDirtyRegion);
        }

        HRESULT __stdcall QueryInterface(REFIID const riid,
                                         VOID** const ppvObj) noexcept override
        {
            *ppvObj = nullptr;

            const auto hResult = this->pOrigInterface->QueryInterface(riid, ppvObj);

            if (SUCCEEDED(hResult)) *ppvObj = this;

            return hResult;
        }

        ULONG __stdcall AddRef() noexcept override
        {
            return this->pOrigInterface->AddRef();
        }

        ULONG __stdcall Release() noexcept override
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

        HRESULT __stdcall TestCooperativeLevel() noexcept override
        {
            return this->pOrigInterface->TestCooperativeLevel();
        }

        UINT __stdcall GetAvailableTextureMem() noexcept override
        {
            return this->pOrigInterface->GetAvailableTextureMem();
        }

        HRESULT __stdcall EvictManagedResources() noexcept override
        {
            return this->pOrigInterface->EvictManagedResources();
        }

        HRESULT __stdcall GetDirect3D(IDirect3D9** const ppD3D9) noexcept override
        {
            return this->pOrigInterface->GetDirect3D(ppD3D9);
        }

        HRESULT __stdcall GetDeviceCaps(D3DCAPS9* const pCaps) noexcept override
        {
            return this->pOrigInterface->GetDeviceCaps(pCaps);
        }

        HRESULT __stdcall GetDisplayMode(const UINT iSwapChain,
                                         D3DDISPLAYMODE* const pMode) noexcept override
        {
            return this->pOrigInterface->GetDisplayMode(iSwapChain, pMode);
        }

        HRESULT __stdcall GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* const pParameters) noexcept override
        {
            return this->pOrigInterface->GetCreationParameters(pParameters);
        }

        HRESULT __stdcall SetCursorProperties(const UINT XHotSpot, const UINT YHotSpot,
                                              IDirect3DSurface9* const pCursorBitmap) noexcept override
        {
            return this->pOrigInterface->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
        }

        VOID __stdcall SetCursorPosition(const int X, const int Y, const DWORD Flags) noexcept override
        {
            this->pOrigInterface->SetCursorPosition(X, Y, Flags);
        }

        BOOL __stdcall ShowCursor(const BOOL bShow) noexcept override
        {
            return this->pOrigInterface->ShowCursor(bShow);
        }

        HRESULT __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* const pPresentationParameters,
                                                    IDirect3DSwapChain9** const pSwapChain) noexcept override
        {
            return this->pOrigInterface->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
        }

        HRESULT __stdcall GetSwapChain(const UINT iSwapChain,
                                       IDirect3DSwapChain9** const pSwapChain) noexcept override
        {
            return this->pOrigInterface->GetSwapChain(iSwapChain, pSwapChain);
        }

        UINT __stdcall GetNumberOfSwapChains() noexcept override
        {
            return this->pOrigInterface->GetNumberOfSwapChains();
        }

        HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS* const pPresentationParameters) noexcept override
        {
            if (this->pOrigInterface == Render::pDeviceInterface && !this->resetStatus)
            {
                if (Render::beforeResetHandler)
                    Render::beforeResetHandler();

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
                    if (Render::afterResetHandler)
                        Render::afterResetHandler(Render::pDeviceInterface,
                            &Render::deviceParameters);

                    this->resetStatus = false;
                }
            }

            return hResult;
        }

        HRESULT __stdcall GetBackBuffer(const UINT iSwapChain,
                                        const UINT iBackBuffer,
                                        const D3DBACKBUFFER_TYPE Type,
                                        IDirect3DSurface9** const ppBackBuffer) noexcept override
        {
            return this->pOrigInterface->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
        }

        HRESULT __stdcall GetRasterStatus(const UINT iSwapChain,
                                          D3DRASTER_STATUS* const pRasterStatus) noexcept override
        {
            return this->pOrigInterface->GetRasterStatus(iSwapChain, pRasterStatus);
        }

        HRESULT __stdcall SetDialogBoxMode(const BOOL bEnableDialogs) noexcept override
        {
            return this->pOrigInterface->SetDialogBoxMode(bEnableDialogs);
        }

        VOID __stdcall SetGammaRamp(const UINT iSwapChain, const DWORD Flags,
                                    CONST D3DGAMMARAMP* const pRamp) noexcept override
        {
            this->pOrigInterface->SetGammaRamp(iSwapChain, Flags, pRamp);
        }

        VOID __stdcall GetGammaRamp(const UINT iSwapChain, D3DGAMMARAMP* const pRamp) noexcept override
        {
            this->pOrigInterface->GetGammaRamp(iSwapChain, pRamp);
        }

        HRESULT __stdcall CreateTexture(const UINT Width, const UINT Height,
                                        const UINT Levels, const DWORD Usage,
                                        const D3DFORMAT Format, const D3DPOOL Pool,
                                        IDirect3DTexture9** const ppTexture,
                                        HANDLE* const pSharedHandle) noexcept override
        {
            return this->pOrigInterface->CreateTexture(Width, Height, Levels,
                Usage, Format, Pool, ppTexture, pSharedHandle);
        }

        HRESULT __stdcall CreateVolumeTexture(const UINT Width, const UINT Height,
                                              const UINT Depth, const UINT Levels, const DWORD Usage,
                                              const D3DFORMAT Format, const D3DPOOL Pool,
                                              IDirect3DVolumeTexture9** const ppVolumeTexture,
                                              HANDLE* const pSharedHandle) noexcept override
        {
            return this->pOrigInterface->CreateVolumeTexture(Width, Height, Depth,
                Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
        }

        HRESULT __stdcall CreateCubeTexture(const UINT EdgeLength, const UINT Levels,
                                            const DWORD Usage, const D3DFORMAT Format, const D3DPOOL Pool,
                                            IDirect3DCubeTexture9** const ppCubeTexture,
                                            HANDLE* const pSharedHandle) noexcept override
        {
            return this->pOrigInterface->CreateCubeTexture(EdgeLength, Levels,
                Usage, Format, Pool, ppCubeTexture, pSharedHandle);
        }

        HRESULT __stdcall CreateVertexBuffer(const UINT Length, const DWORD Usage,
                                             const DWORD FVF, const D3DPOOL Pool,
                                             IDirect3DVertexBuffer9** const ppVertexBuffer,
                                             HANDLE* const pSharedHandle) noexcept override
        {
            return this->pOrigInterface->CreateVertexBuffer(Length, Usage,
                FVF, Pool, ppVertexBuffer, pSharedHandle);
        }

        HRESULT __stdcall CreateIndexBuffer(const UINT Length, const DWORD Usage,
                                            const D3DFORMAT Format, const D3DPOOL Pool,
                                            IDirect3DIndexBuffer9** const ppIndexBuffer,
                                            HANDLE* const pSharedHandle) noexcept override
        {
            return this->pOrigInterface->CreateIndexBuffer(Length, Usage,
                Format, Pool, ppIndexBuffer, pSharedHandle);
        }

        HRESULT __stdcall CreateRenderTarget(const UINT Width, const UINT Height,
                                             const D3DFORMAT Format, const D3DMULTISAMPLE_TYPE MultiSample,
                                             const DWORD MultisampleQuality, const BOOL Lockable,
                                             IDirect3DSurface9** const ppSurface,
                                             HANDLE* const pSharedHandle) noexcept override
        {
            return this->pOrigInterface->CreateRenderTarget(Width, Height,
                Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
        }

        HRESULT __stdcall CreateDepthStencilSurface(const UINT Width, const UINT Height,
                                                    const D3DFORMAT Format, const D3DMULTISAMPLE_TYPE MultiSample,
                                                    const DWORD MultisampleQuality, const BOOL Discard,
                                                    IDirect3DSurface9** const ppSurface,
                                                    HANDLE* const pSharedHandle) noexcept override
        {
            return this->pOrigInterface->CreateDepthStencilSurface(Width, Height,
                Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
        }

        HRESULT __stdcall UpdateSurface(IDirect3DSurface9* const pSourceSurface,
                                        CONST RECT* const pSourceRect,
                                        IDirect3DSurface9* const pDestinationSurface,
                                        CONST POINT* const pDestPoint) noexcept override
        {
            return this->pOrigInterface->UpdateSurface(pSourceSurface, pSourceRect,
                pDestinationSurface, pDestPoint);
        }

        HRESULT __stdcall UpdateTexture(IDirect3DBaseTexture9* const pSourceTexture,
                                        IDirect3DBaseTexture9* const pDestinationTexture) noexcept override
        {
            return this->pOrigInterface->UpdateTexture(pSourceTexture, pDestinationTexture);
        }

        HRESULT __stdcall GetRenderTargetData(IDirect3DSurface9* const pRenderTarget,
                                              IDirect3DSurface9* const pDestSurface) noexcept override
        {
            return this->pOrigInterface->GetRenderTargetData(pRenderTarget, pDestSurface);
        }

        HRESULT __stdcall GetFrontBufferData(const UINT iSwapChain,
                                             IDirect3DSurface9* const pDestSurface) noexcept override
        {
            return this->pOrigInterface->GetFrontBufferData(iSwapChain, pDestSurface);
        }

        HRESULT __stdcall StretchRect(IDirect3DSurface9* const pSourceSurface,
                                      CONST RECT* const pSourceRect,
                                      IDirect3DSurface9* const pDestSurface,
                                      CONST RECT* const pDestRect,
                                      const D3DTEXTUREFILTERTYPE Filter) noexcept override
        {
            return this->pOrigInterface->StretchRect(pSourceSurface, pSourceRect,
                pDestSurface, pDestRect, Filter);
        }

        HRESULT __stdcall ColorFill(IDirect3DSurface9* const pSurface,
                                    CONST RECT* const pRect, const D3DCOLOR color) noexcept override
        {
            return this->pOrigInterface->ColorFill(pSurface, pRect, color);
        }

        HRESULT __stdcall CreateOffscreenPlainSurface(const UINT Width, const UINT Height,
                                                      const D3DFORMAT Format, const D3DPOOL Pool,
                                                      IDirect3DSurface9** const ppSurface,
                                                      HANDLE* const pSharedHandle) noexcept override
        {
            return this->pOrigInterface->CreateOffscreenPlainSurface(Width, Height,
                Format, Pool, ppSurface, pSharedHandle);
        }

        HRESULT __stdcall SetRenderTarget(const DWORD RenderTargetIndex,
                                          IDirect3DSurface9* const pRenderTarget) noexcept override
        {
            return this->pOrigInterface->SetRenderTarget(RenderTargetIndex, pRenderTarget);
        }

        HRESULT __stdcall GetRenderTarget(const DWORD RenderTargetIndex,
                                          IDirect3DSurface9** const ppRenderTarget) noexcept override
        {
            return this->pOrigInterface->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
        }

        HRESULT __stdcall SetDepthStencilSurface(IDirect3DSurface9* const pNewZStencil) noexcept override
        {
            return this->pOrigInterface->SetDepthStencilSurface(pNewZStencil);
        }

        HRESULT __stdcall GetDepthStencilSurface(IDirect3DSurface9** const ppZStencilSurface) noexcept override
        {
            return this->pOrigInterface->GetDepthStencilSurface(ppZStencilSurface);
        }

        HRESULT __stdcall BeginScene() noexcept override
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

        HRESULT __stdcall EndScene() noexcept override
        {
            if (this->pOrigInterface == Render::pDeviceInterface &&
                !this->resetStatus && this->sceneStatus)
            {
                if (Render::endSceneHandler) Render::endSceneHandler();

                this->sceneStatus = false;
            }

            return this->pOrigInterface->EndScene();
        }

        HRESULT __stdcall Clear(const DWORD Count, CONST D3DRECT* const pRects,
                                const DWORD Flags, const D3DCOLOR Color,
                                const float Z, const DWORD Stencil) noexcept override
        {
            return this->pOrigInterface->Clear(Count, pRects, Flags, Color, Z, Stencil);
        }

        HRESULT __stdcall SetTransform(const D3DTRANSFORMSTATETYPE State,
                                       CONST D3DMATRIX* const mat) noexcept override
        {
            return this->pOrigInterface->SetTransform(State, mat);
        }

        HRESULT __stdcall GetTransform(const D3DTRANSFORMSTATETYPE State,
                                       D3DMATRIX* const mat) noexcept override
        {
            return this->pOrigInterface->GetTransform(State, mat);
        }

        HRESULT __stdcall MultiplyTransform(const D3DTRANSFORMSTATETYPE State,
                                            CONST D3DMATRIX* const mat) noexcept override
        {
            return this->pOrigInterface->MultiplyTransform(State, mat);
        }

        HRESULT __stdcall SetViewport(CONST D3DVIEWPORT9* const pViewport) noexcept override
        {
            return this->pOrigInterface->SetViewport(pViewport);
        }

        HRESULT __stdcall GetViewport(D3DVIEWPORT9* const pViewport) noexcept override
        {
            return this->pOrigInterface->GetViewport(pViewport);
        }

        HRESULT __stdcall SetMaterial(CONST D3DMATERIAL9* const pMaterial) noexcept override
        {
            return this->pOrigInterface->SetMaterial(pMaterial);
        }

        HRESULT __stdcall GetMaterial(D3DMATERIAL9* const pMaterial) noexcept override
        {
            return this->pOrigInterface->GetMaterial(pMaterial);
        }

        HRESULT __stdcall SetLight(const DWORD Index,
                                   CONST D3DLIGHT9* const pLight) noexcept override
        {
            return this->pOrigInterface->SetLight(Index, pLight);
        }

        HRESULT __stdcall GetLight(const DWORD Index,
                                   D3DLIGHT9* const pLight) noexcept override
        {
            return this->pOrigInterface->GetLight(Index, pLight);
        }

        HRESULT __stdcall LightEnable(const DWORD Index, const BOOL Enable) noexcept override
        {
            return this->pOrigInterface->LightEnable(Index, Enable);
        }

        HRESULT __stdcall GetLightEnable(const DWORD Index, BOOL* const pEnable) noexcept override
        {
            return this->pOrigInterface->GetLightEnable(Index, pEnable);
        }

        HRESULT __stdcall SetClipPlane(const DWORD Index,
                                       CONST float* const pPlane) noexcept override
        {
            return this->pOrigInterface->SetClipPlane(Index, pPlane);
        }

        HRESULT __stdcall GetClipPlane(const DWORD Index, float* const pPlane) noexcept override
        {
            return this->pOrigInterface->GetClipPlane(Index, pPlane);
        }

        HRESULT __stdcall SetRenderState(const D3DRENDERSTATETYPE State,
                                         const DWORD Value) noexcept override
        {
            return this->pOrigInterface->SetRenderState(State, Value);
        }

        HRESULT __stdcall GetRenderState(const D3DRENDERSTATETYPE State,
                                         DWORD* const pValue) noexcept override
        {
            return this->pOrigInterface->GetRenderState(State, pValue);
        }

        HRESULT __stdcall CreateStateBlock(const D3DSTATEBLOCKTYPE Type,
                                           IDirect3DStateBlock9** const ppSB) noexcept override
        {
            return this->pOrigInterface->CreateStateBlock(Type, ppSB);
        }

        HRESULT __stdcall BeginStateBlock() noexcept override
        {
            return this->pOrigInterface->BeginStateBlock();
        }

        HRESULT __stdcall EndStateBlock(IDirect3DStateBlock9** const ppSB) noexcept override
        {
            return this->pOrigInterface->EndStateBlock(ppSB);
        }

        HRESULT __stdcall SetClipStatus(CONST D3DCLIPSTATUS9* const pClipStatus) noexcept override
        {
            return this->pOrigInterface->SetClipStatus(pClipStatus);
        }

        HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS9* const pClipStatus) noexcept override
        {
            return this->pOrigInterface->GetClipStatus(pClipStatus);
        }

        HRESULT __stdcall GetTexture(const DWORD Stage,
                                     IDirect3DBaseTexture9** const ppTexture) noexcept override
        {
            return this->pOrigInterface->GetTexture(Stage, ppTexture);
        }

        HRESULT __stdcall SetTexture(const DWORD Stage,
                                     IDirect3DBaseTexture9* const pTexture) noexcept override
        {
            return this->pOrigInterface->SetTexture(Stage, pTexture);
        }

        HRESULT __stdcall GetTextureStageState(const DWORD Stage,
                                               const D3DTEXTURESTAGESTATETYPE Type,
                                               DWORD* const pValue) noexcept override
        {
            return this->pOrigInterface->GetTextureStageState(Stage, Type, pValue);
        }

        HRESULT __stdcall SetTextureStageState(const DWORD Stage,
                                               const D3DTEXTURESTAGESTATETYPE Type,
                                               const DWORD Value) noexcept override
        {
            return this->pOrigInterface->SetTextureStageState(Stage, Type, Value);
        }

        HRESULT __stdcall GetSamplerState(const DWORD Sampler,
                                          const D3DSAMPLERSTATETYPE Type,
                                          DWORD* const pValue) noexcept override
        {
            return this->pOrigInterface->GetSamplerState(Sampler, Type, pValue);
        }

        HRESULT __stdcall SetSamplerState(const DWORD Sampler,
                                          const D3DSAMPLERSTATETYPE Type,
                                          const DWORD Value) noexcept override
        {
            return this->pOrigInterface->SetSamplerState(Sampler, Type, Value);
        }

        HRESULT __stdcall ValidateDevice(DWORD* const pNumPasses) noexcept override
        {
            return this->pOrigInterface->ValidateDevice(pNumPasses);
        }

        HRESULT __stdcall SetPaletteEntries(const UINT PaletteNumber,
                                            CONST PALETTEENTRY* const pEntries) noexcept override
        {
            return this->pOrigInterface->SetPaletteEntries(PaletteNumber, pEntries);
        }

        HRESULT __stdcall GetPaletteEntries(const UINT PaletteNumber,
                                            PALETTEENTRY* const pEntries) noexcept override
        {
            return this->pOrigInterface->GetPaletteEntries(PaletteNumber, pEntries);
        }

        HRESULT __stdcall SetCurrentTexturePalette(const UINT PaletteNumber) noexcept override
        {
            return this->pOrigInterface->SetCurrentTexturePalette(PaletteNumber);
        }

        HRESULT __stdcall GetCurrentTexturePalette(UINT* const PaletteNumber) noexcept override
        {
            return this->pOrigInterface->GetCurrentTexturePalette(PaletteNumber);
        }

        HRESULT __stdcall SetScissorRect(CONST RECT* const pRect) noexcept override
        {
            return this->pOrigInterface->SetScissorRect(pRect);
        }

        HRESULT __stdcall GetScissorRect(RECT* const pRect) noexcept override
        {
            return this->pOrigInterface->GetScissorRect(pRect);
        }

        HRESULT __stdcall SetSoftwareVertexProcessing(const BOOL bSoftware) noexcept override
        {
            return this->pOrigInterface->SetSoftwareVertexProcessing(bSoftware);
        }

        BOOL __stdcall GetSoftwareVertexProcessing() noexcept override
        {
            return this->pOrigInterface->GetSoftwareVertexProcessing();
        }

        HRESULT __stdcall SetNPatchMode(const float nSegments) noexcept override
        {
            return this->pOrigInterface->SetNPatchMode(nSegments);
        }

        FLOAT __stdcall GetNPatchMode() noexcept override
        {
            return this->pOrigInterface->GetNPatchMode();
        }

        HRESULT __stdcall DrawPrimitive(const D3DPRIMITIVETYPE PrimitiveType,
                                        const UINT StartVertex,
                                        const UINT PrimitiveCount) noexcept override
        {
            return this->pOrigInterface->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
        }

        HRESULT __stdcall DrawIndexedPrimitive(const D3DPRIMITIVETYPE Type,
                                               const INT BaseVertexIndex,
                                               const UINT MinVertexIndex,
                                               const UINT NumVertices,
                                               const UINT startIndex,
                                               const UINT primCount) noexcept override
        {
            return this->pOrigInterface->DrawIndexedPrimitive(Type, BaseVertexIndex,
                MinVertexIndex, NumVertices, startIndex, primCount);
        }

        HRESULT __stdcall DrawPrimitiveUP(const D3DPRIMITIVETYPE PrimitiveType,
                                          const UINT PrimitiveCount,
                                          CONST void* const pVertexStreamZeroData,
                                          const UINT VertexStreamZeroStride) noexcept override
        {
            return this->pOrigInterface->DrawPrimitiveUP(PrimitiveType,
                PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
        }

        HRESULT __stdcall DrawIndexedPrimitiveUP(const D3DPRIMITIVETYPE PrimitiveType,
                                                 const UINT MinVertexIndex,
                                                 const UINT NumVertices,
                                                 const UINT PrimitiveCount,
                                                 CONST void* const pIndexData,
                                                 const D3DFORMAT IndexDataFormat,
                                                 CONST void* const pVertexStreamZeroData,
                                                 const UINT VertexStreamZeroStride) noexcept override
        {
            return this->pOrigInterface->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex,
                NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
        }

        HRESULT __stdcall ProcessVertices(const UINT SrcStartIndex, const UINT DestIndex,
                                          const UINT VertexCount, IDirect3DVertexBuffer9* const pDestBuffer,
                                          IDirect3DVertexDeclaration9* const pVertexDecl, const DWORD Flags) noexcept override
        {
            return this->pOrigInterface->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
        }

        HRESULT __stdcall CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* const pVertexElements,
                                                  IDirect3DVertexDeclaration9** const ppDecl) noexcept override
        {
            return this->pOrigInterface->CreateVertexDeclaration(pVertexElements, ppDecl);
        }

        HRESULT __stdcall SetVertexDeclaration(IDirect3DVertexDeclaration9* const pDecl) noexcept override
        {
            return this->pOrigInterface->SetVertexDeclaration(pDecl);
        }

        HRESULT __stdcall GetVertexDeclaration(IDirect3DVertexDeclaration9** const ppDecl) noexcept override
        {
            return this->pOrigInterface->GetVertexDeclaration(ppDecl);
        }

        HRESULT __stdcall SetFVF(const DWORD FVF) noexcept override
        {
            return this->pOrigInterface->SetFVF(FVF);
        }

        HRESULT __stdcall GetFVF(DWORD* const pFVF) noexcept override
        {
            return this->pOrigInterface->GetFVF(pFVF);
        }

        HRESULT __stdcall CreateVertexShader(CONST DWORD* const pFunction,
                                             IDirect3DVertexShader9** const ppShader) noexcept override
        {
            return this->pOrigInterface->CreateVertexShader(pFunction, ppShader);
        }

        HRESULT __stdcall SetVertexShader(IDirect3DVertexShader9* const pShader) noexcept override
        {
            return this->pOrigInterface->SetVertexShader(pShader);
        }

        HRESULT __stdcall GetVertexShader(IDirect3DVertexShader9** const ppShader) noexcept override
        {
            return this->pOrigInterface->GetVertexShader(ppShader);
        }

        HRESULT __stdcall SetVertexShaderConstantF(const UINT StartRegister,
                                                   CONST float* const pConstantData,
                                                   const UINT Vector4fCount) noexcept override
        {
            return this->pOrigInterface->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
        }

        HRESULT __stdcall GetVertexShaderConstantF(const UINT StartRegister,
                                                   float* const pConstantData,
                                                   const UINT Vector4fCount) noexcept override
        {
            return this->pOrigInterface->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
        }

        HRESULT __stdcall SetVertexShaderConstantI(const UINT StartRegister,
                                                   CONST int* const pConstantData,
                                                   const UINT Vector4iCount) noexcept override
        {
            return this->pOrigInterface->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
        }

        HRESULT __stdcall GetVertexShaderConstantI(const UINT StartRegister,
                                                   int* const pConstantData,
                                                   const UINT Vector4iCount) noexcept override
        {
            return this->pOrigInterface->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
        }

        HRESULT __stdcall SetVertexShaderConstantB(const UINT StartRegister,
                                                   CONST BOOL* const pConstantData,
                                                   const UINT BoolCount) noexcept override
        {
            return this->pOrigInterface->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
        }

        HRESULT __stdcall GetVertexShaderConstantB(const UINT StartRegister,
                                                   BOOL* const pConstantData,
                                                   const UINT BoolCount) noexcept override
        {
            return this->pOrigInterface->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
        }

        HRESULT __stdcall SetStreamSource(const UINT StreamNumber,
                                          IDirect3DVertexBuffer9* const pStreamData,
                                          const UINT OffsetInBytes,
                                          const UINT Stride) noexcept override
        {
            return this->pOrigInterface->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
        }

        HRESULT __stdcall GetStreamSource(const UINT StreamNumber,
                                          IDirect3DVertexBuffer9** const ppStreamData,
                                          UINT* const OffsetInBytes,
                                          UINT* const pStride) noexcept override
        {
            return this->pOrigInterface->GetStreamSource(StreamNumber, ppStreamData, OffsetInBytes, pStride);
        }

        HRESULT __stdcall SetStreamSourceFreq(const UINT StreamNumber,
                                              const UINT Divider) noexcept override
        {
            return this->pOrigInterface->SetStreamSourceFreq(StreamNumber, Divider);
        }

        HRESULT __stdcall GetStreamSourceFreq(const UINT StreamNumber,
                                              UINT* const Divider) noexcept override
        {
            return this->pOrigInterface->GetStreamSourceFreq(StreamNumber, Divider);
        }

        HRESULT __stdcall SetIndices(IDirect3DIndexBuffer9* const pIndexData) noexcept override
        {
            return this->pOrigInterface->SetIndices(pIndexData);
        }

        HRESULT __stdcall GetIndices(IDirect3DIndexBuffer9** const ppIndexData) noexcept override
        {
            return this->pOrigInterface->GetIndices(ppIndexData);
        }

        HRESULT __stdcall CreatePixelShader(CONST DWORD* const pFunction,
                                            IDirect3DPixelShader9** const ppShader) noexcept override
        {
            return this->pOrigInterface->CreatePixelShader(pFunction, ppShader);
        }

        HRESULT __stdcall SetPixelShader(IDirect3DPixelShader9* const pShader) noexcept override
        {
            return this->pOrigInterface->SetPixelShader(pShader);
        }

        HRESULT __stdcall GetPixelShader(IDirect3DPixelShader9** const ppShader) noexcept override
        {
            return this->pOrigInterface->GetPixelShader(ppShader);
        }

        HRESULT __stdcall SetPixelShaderConstantF(const UINT StartRegister,
                                                  CONST float* const pConstantData,
                                                  const UINT Vector4fCount) noexcept override
        {
            return this->pOrigInterface->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
        }

        HRESULT __stdcall GetPixelShaderConstantF(const UINT StartRegister,
                                                  float* const pConstantData,
                                                  const UINT Vector4fCount) noexcept override
        {
            return this->pOrigInterface->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
        }

        HRESULT __stdcall SetPixelShaderConstantI(const UINT StartRegister,
                                                  CONST int* const pConstantData,
                                                  const UINT Vector4iCount) noexcept override
        {
            return this->pOrigInterface->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
        }

        HRESULT __stdcall GetPixelShaderConstantI(const UINT StartRegister,
                                                  int* const pConstantData,
                                                  const UINT Vector4iCount) noexcept override
        {
            return this->pOrigInterface->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
        }

        HRESULT __stdcall SetPixelShaderConstantB(const UINT StartRegister,
                                                  CONST BOOL* const pConstantData,
                                                  const UINT BoolCount) noexcept override
        {
            return this->pOrigInterface->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
        }

        HRESULT __stdcall GetPixelShaderConstantB(const UINT StartRegister,
                                                  BOOL* const pConstantData,
                                                  const UINT BoolCount) noexcept override
        {
            return this->pOrigInterface->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
        }

        HRESULT __stdcall DrawRectPatch(const UINT Handle, CONST float* const pNumSegs,
                                        CONST D3DRECTPATCH_INFO* const pRectPatchInfo) noexcept override
        {
            return this->pOrigInterface->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
        }

        HRESULT __stdcall DrawTriPatch(const UINT Handle, CONST float* const pNumSegs,
                                       CONST D3DTRIPATCH_INFO* const pTriPatchInfo) noexcept override
        {
            return this->pOrigInterface->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
        }

        HRESULT __stdcall DeletePatch(const UINT Handle) noexcept override
        {
            return this->pOrigInterface->DeletePatch(Handle);
        }

        HRESULT __stdcall CreateQuery(const D3DQUERYTYPE Type,
                                      IDirect3DQuery9** const ppQuery) noexcept override
        {
            return this->pOrigInterface->CreateQuery(Type, ppQuery);
        }

    private:

        IDirect3DDevice9* pOrigInterface { nullptr };

        bool resetStatus { false };
        bool sceneStatus { false };

    };

    interface IDirect3D9Hook : public IDirect3D9 {

        explicit IDirect3D9Hook(IDirect3D9* const pOrigInterface) noexcept
            : pOrigInterface(pOrigInterface) { this->pOrigInterface->AddRef(); }

        ~IDirect3D9Hook() noexcept { this->pOrigInterface->Release(); }

        // IDirect3D9 Interface methods...
        // ------------------------------------------------

        HRESULT __stdcall QueryInterface(THIS_ REFIID const riid,
                                         VOID** const ppvObj) noexcept override
        {
            *ppvObj = nullptr;

            const auto hResult = this->pOrigInterface->QueryInterface(riid, ppvObj);

            if (SUCCEEDED(hResult)) *ppvObj = this;

            return hResult;
        }

        ULONG __stdcall AddRef() noexcept override
        {
            return this->pOrigInterface->AddRef();
        }

        ULONG __stdcall Release() noexcept override
        {
            const auto count = this->pOrigInterface->Release();

            if (count <= 1)
            {
                delete this;
                return 0;
            }

            return count;
        }

        HRESULT __stdcall RegisterSoftwareDevice(THIS_ void* const pInitializeFunction) noexcept override
        {
            return this->pOrigInterface->RegisterSoftwareDevice(pInitializeFunction);
        }

        UINT __stdcall GetAdapterCount() noexcept override
        {
            return this->pOrigInterface->GetAdapterCount();
        }

        HRESULT __stdcall GetAdapterIdentifier(const THIS_ UINT Adapter, const DWORD Flags,
                                               D3DADAPTER_IDENTIFIER9* const pIdentifier) noexcept override
        {
            return this->pOrigInterface->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
        }

        UINT __stdcall GetAdapterModeCount(const THIS_ UINT Adapter, const D3DFORMAT Format) noexcept override
        {
            return this->pOrigInterface->GetAdapterModeCount(Adapter, Format);
        }

        HRESULT __stdcall EnumAdapterModes(const THIS_ UINT Adapter, const D3DFORMAT Format,
                                           const UINT Mode, D3DDISPLAYMODE* const pMode) noexcept override
        {
            return this->pOrigInterface->EnumAdapterModes(Adapter, Format, Mode, pMode);
        }

        HRESULT __stdcall GetAdapterDisplayMode(const THIS_ UINT Adapter,
                                                D3DDISPLAYMODE* const pMode) noexcept override
        {
            return this->pOrigInterface->GetAdapterDisplayMode(Adapter, pMode);
        }

        HRESULT __stdcall CheckDeviceType(const THIS_ UINT Adapter, const D3DDEVTYPE DevType,
                                          const D3DFORMAT AdapterFormat, const D3DFORMAT BackBufferFormat,
                                          const BOOL bWindowed) noexcept override
        {
            return this->pOrigInterface->CheckDeviceType(Adapter, DevType,
                AdapterFormat, BackBufferFormat, bWindowed);
        }

        HRESULT __stdcall CheckDeviceFormat(const THIS_ UINT Adapter, const D3DDEVTYPE DeviceType,
                                            const D3DFORMAT AdapterFormat, const DWORD Usage,
                                            const D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) noexcept override
        {
            return this->pOrigInterface->CheckDeviceFormat(Adapter, DeviceType,
                AdapterFormat, Usage, RType, CheckFormat);
        }

        HRESULT __stdcall CheckDeviceMultiSampleType(const THIS_ UINT Adapter, const D3DDEVTYPE DeviceType,
                                                     const D3DFORMAT SurfaceFormat, const BOOL Windowed,
                                                     const D3DMULTISAMPLE_TYPE MultiSampleType,
                                                     DWORD* const pQualityLevels) noexcept override
        {
            return this->pOrigInterface->CheckDeviceMultiSampleType(Adapter, DeviceType,
                SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
        }

        HRESULT __stdcall CheckDepthStencilMatch(const THIS_ UINT Adapter, const D3DDEVTYPE DeviceType,
                                                 const D3DFORMAT AdapterFormat, const D3DFORMAT RenderTargetFormat,
                                                 const D3DFORMAT DepthStencilFormat) noexcept override
        {
            return this->pOrigInterface->CheckDepthStencilMatch(Adapter, DeviceType,
                AdapterFormat, RenderTargetFormat, DepthStencilFormat);
        }

        HRESULT __stdcall CheckDeviceFormatConversion(const THIS_ UINT Adapter, const D3DDEVTYPE DeviceType,
                                                      const D3DFORMAT SourceFormat, const D3DFORMAT TargetFormat) noexcept override
        {
            return this->pOrigInterface->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
        }

        HRESULT __stdcall GetDeviceCaps(const THIS_ UINT Adapter, const D3DDEVTYPE DeviceType,
                                        D3DCAPS9* const pCaps) noexcept override
        {
            return this->pOrigInterface->GetDeviceCaps(Adapter, DeviceType, pCaps);
        }

        HMONITOR __stdcall GetAdapterMonitor(const THIS_ UINT Adapter) noexcept override
        {
            return this->pOrigInterface->GetAdapterMonitor(Adapter);
        }

        HRESULT __stdcall CreateDevice(const THIS_ UINT Adapter, const D3DDEVTYPE DeviceType,
                                       const HWND hFocusWindow, const DWORD BehaviorFlags,
                                       D3DPRESENT_PARAMETERS* const pPresentationParameters,
                                       IDirect3DDevice9** const ppReturnedDeviceInterface) noexcept override
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
