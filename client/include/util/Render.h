/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cstdint>
#include <functional>
#include <mutex>
#include <vector>

#include <d3d9.h>
#include <d3dx9.h>

#include "Memory.hpp"

class Render {

    Render() = delete;
    ~Render() = delete;
    Render(const Render&) = delete;
    Render(Render&&) = delete;
    Render& operator=(const Render&) = delete;
    Render& operator=(Render&&) = delete;

private:

    using DeviceInitCallback = std::function<void(IDirect3D9*, IDirect3DDevice9*, const D3DPRESENT_PARAMETERS&)>;
    using BeforeResetCallback = std::function<void()>;
    using BeginSceneCallback = std::function<void()>;
    using RenderCallback = std::function<void()>;
    using EndSceneCallback = std::function<void()>;
    using AfterResetCallback = std::function<void(IDirect3DDevice9*, const D3DPRESENT_PARAMETERS&)>;
    using DeviceFreeCallback = std::function<void()>;

public:

    static constexpr float kBaseWidth = 640.f;
    static constexpr float kBaseHeight = 480.f;

public:

    static bool Init() noexcept;
    static void Free() noexcept;

    static bool GetWindowHandle(HWND& windowHandle) noexcept;
    static bool GetScreenSize(float& screenWidth, float& screenHeight) noexcept;
    static bool ConvertBaseXValueToScreenXValue(float baseValue, float& screenValue) noexcept;
    static bool ConvertBaseYValueToScreenYValue(float baseValue, float& screenValue) noexcept;
    static bool ConvertScreenXValueToBaseXValue(float screenValue, float& baseValue) noexcept;
    static bool ConvertScreenYValueToBaseYValue(float screenValue, float& baseValue) noexcept;

public:

    static std::size_t AddDeviceInitCallback(DeviceInitCallback callback) noexcept;
    static std::size_t AddBeforeResetCallback(BeforeResetCallback callback) noexcept;
    static std::size_t AddBeginSceneCallback(BeginSceneCallback callback) noexcept;
    static std::size_t AddRenderCallback(RenderCallback callback) noexcept;
    static std::size_t AddEndSceneCallback(EndSceneCallback callback) noexcept;
    static std::size_t AddAfterResetCallback(AfterResetCallback callback) noexcept;
    static std::size_t AddDeviceFreeCallback(DeviceFreeCallback callback) noexcept;

    static void RemoveDeviceInitCallback(std::size_t callback) noexcept;
    static void RemoveBeforeResetCallback(std::size_t callback) noexcept;
    static void RemoveBeginSceneCallback(std::size_t callback) noexcept;
    static void RemoveRenderCallback(std::size_t callback) noexcept;
    static void RemoveEndSceneCallback(std::size_t callback) noexcept;
    static void RemoveAfterResetCallback(std::size_t callback) noexcept;
    static void RemoveDeviceFreeCallback(std::size_t callback) noexcept;

private:

    interface IDirect3DDevice9Hook : public IDirect3DDevice9 {

        IDirect3DDevice9Hook() = delete;
        IDirect3DDevice9Hook(const IDirect3DDevice9Hook&) = delete;
        IDirect3DDevice9Hook(IDirect3DDevice9Hook&&) = delete;
        IDirect3DDevice9Hook& operator=(const IDirect3DDevice9Hook&) = delete;
        IDirect3DDevice9Hook& operator=(IDirect3DDevice9Hook&&) = delete;

    public:

        explicit IDirect3DDevice9Hook(IDirect3DDevice9* pOrigInterface) noexcept;

        ~IDirect3DDevice9Hook() noexcept;

    public:

        HRESULT __stdcall Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) noexcept override;
        HRESULT __stdcall QueryInterface(REFIID riid, VOID** ppvObj) noexcept override;
        ULONG __stdcall AddRef() noexcept override;
        ULONG __stdcall Release() noexcept override;
        HRESULT __stdcall TestCooperativeLevel() noexcept override;
        UINT __stdcall GetAvailableTextureMem() noexcept override;
        HRESULT __stdcall EvictManagedResources() noexcept override;
        HRESULT __stdcall GetDirect3D(IDirect3D9** ppD3D9) noexcept override;
        HRESULT __stdcall GetDeviceCaps(D3DCAPS9* pCaps) noexcept override;
        HRESULT __stdcall GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) noexcept override;
        HRESULT __stdcall GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters) noexcept override;
        HRESULT __stdcall SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap) noexcept override;
        VOID __stdcall SetCursorPosition(INT X, INT Y, DWORD Flags) noexcept override;
        BOOL __stdcall ShowCursor(BOOL bShow) noexcept override;
        HRESULT __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain) noexcept override;
        HRESULT __stdcall GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain) noexcept override;
        UINT __stdcall GetNumberOfSwapChains() noexcept override;
        HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) noexcept override;
        HRESULT __stdcall GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) noexcept override;
        HRESULT __stdcall GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus) noexcept override;
        HRESULT __stdcall SetDialogBoxMode(BOOL bEnableDialogs) noexcept override;
        VOID __stdcall SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp) noexcept override;
        VOID __stdcall GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) noexcept override;
        HRESULT __stdcall CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) noexcept override;
        HRESULT __stdcall CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle) noexcept override;
        HRESULT __stdcall CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle) noexcept override;
        HRESULT __stdcall CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle) noexcept override;
        HRESULT __stdcall CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle) noexcept override;
        HRESULT __stdcall CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) noexcept override;
        HRESULT __stdcall CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) noexcept override;
        HRESULT __stdcall UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint) noexcept override;
        HRESULT __stdcall UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture) noexcept override;
        HRESULT __stdcall GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface) noexcept override;
        HRESULT __stdcall GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface) noexcept override;
        HRESULT __stdcall StretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) noexcept override;
        HRESULT __stdcall ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color) noexcept override;
        HRESULT __stdcall CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) noexcept override;
        HRESULT __stdcall SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) noexcept override;
        HRESULT __stdcall GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget) noexcept override;
        HRESULT __stdcall SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) noexcept override;
        HRESULT __stdcall GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface) noexcept override;
        HRESULT __stdcall BeginScene() noexcept override;
        HRESULT __stdcall EndScene() noexcept override;
        HRESULT __stdcall Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, FLOAT Z, DWORD Stencil) noexcept override;
        HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* mat) noexcept override;
        HRESULT __stdcall GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* mat) noexcept override;
        HRESULT __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* mat) noexcept override;
        HRESULT __stdcall SetViewport(CONST D3DVIEWPORT9* pViewport) noexcept override;
        HRESULT __stdcall GetViewport(D3DVIEWPORT9* pViewport) noexcept override;
        HRESULT __stdcall SetMaterial(CONST D3DMATERIAL9* pMaterial) noexcept override;
        HRESULT __stdcall GetMaterial(D3DMATERIAL9* pMaterial) noexcept override;
        HRESULT __stdcall SetLight(DWORD Index, CONST D3DLIGHT9* pLight) noexcept override;
        HRESULT __stdcall GetLight(DWORD Index, D3DLIGHT9* pLight) noexcept override;
        HRESULT __stdcall LightEnable(DWORD Index, BOOL Enable) noexcept override;
        HRESULT __stdcall GetLightEnable(DWORD Index, BOOL* pEnable) noexcept override;
        HRESULT __stdcall SetClipPlane(DWORD Index, CONST FLOAT* pPlane) noexcept override;
        HRESULT __stdcall GetClipPlane(DWORD Index, FLOAT* pPlane) noexcept override;
        HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) noexcept override;
        HRESULT __stdcall GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) noexcept override;
        HRESULT __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB) noexcept override;
        HRESULT __stdcall BeginStateBlock() noexcept override;
        HRESULT __stdcall EndStateBlock(IDirect3DStateBlock9** ppSB) noexcept override;
        HRESULT __stdcall SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus) noexcept override;
        HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS9* pClipStatus) noexcept override;
        HRESULT __stdcall GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture) noexcept override;
        HRESULT __stdcall SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture) noexcept override;
        HRESULT __stdcall GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) noexcept override;
        HRESULT __stdcall SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) noexcept override;
        HRESULT __stdcall GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue) noexcept override;
        HRESULT __stdcall SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) noexcept override;
        HRESULT __stdcall ValidateDevice(DWORD* pNumPasses) noexcept override;
        HRESULT __stdcall SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY* pEntries) noexcept override;
        HRESULT __stdcall GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) noexcept override;
        HRESULT __stdcall SetCurrentTexturePalette(UINT PaletteNumber) noexcept override;
        HRESULT __stdcall GetCurrentTexturePalette(UINT* PaletteNumber) noexcept override;
        HRESULT __stdcall SetScissorRect(CONST RECT* pRect) noexcept override;
        HRESULT __stdcall GetScissorRect(RECT* pRect) noexcept override;
        HRESULT __stdcall SetSoftwareVertexProcessing(BOOL bSoftware) noexcept override;
        BOOL __stdcall GetSoftwareVertexProcessing() noexcept override;
        HRESULT __stdcall SetNPatchMode(FLOAT nSegments) noexcept override;
        FLOAT __stdcall GetNPatchMode() noexcept override;
        HRESULT __stdcall DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) noexcept override;
        HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) noexcept override;
        HRESULT __stdcall DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST VOID* pVertexStreamZeroData, UINT VertexStreamZeroStride) noexcept override;
        HRESULT __stdcall DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST VOID* pIndexData, D3DFORMAT IndexDataFormat, CONST VOID* pVertexStreamZeroData, UINT VertexStreamZeroStride) noexcept override;
        HRESULT __stdcall ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags) noexcept override;
        HRESULT __stdcall CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl) noexcept override;
        HRESULT __stdcall SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) noexcept override;
        HRESULT __stdcall GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) noexcept override;
        HRESULT __stdcall SetFVF(DWORD FVF) noexcept override;
        HRESULT __stdcall GetFVF(DWORD* pFVF) noexcept override;
        HRESULT __stdcall CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader) noexcept override;
        HRESULT __stdcall SetVertexShader(IDirect3DVertexShader9* pShader) noexcept override;
        HRESULT __stdcall GetVertexShader(IDirect3DVertexShader9** ppShader) noexcept override;
        HRESULT __stdcall SetVertexShaderConstantF(UINT StartRegister, CONST FLOAT* pConstantData, UINT Vector4fCount) noexcept override;
        HRESULT __stdcall GetVertexShaderConstantF(UINT StartRegister, FLOAT* pConstantData, UINT Vector4fCount) noexcept override;
        HRESULT __stdcall SetVertexShaderConstantI(UINT StartRegister, CONST INT* pConstantData, UINT Vector4iCount) noexcept override;
        HRESULT __stdcall GetVertexShaderConstantI(UINT StartRegister, INT* pConstantData, UINT Vector4iCount) noexcept override;
        HRESULT __stdcall SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount) noexcept override;
        HRESULT __stdcall GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) noexcept override;
        HRESULT __stdcall SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride) noexcept override;
        HRESULT __stdcall GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* OffsetInBytes, UINT* pStride) noexcept override;
        HRESULT __stdcall SetStreamSourceFreq(UINT StreamNumber, UINT Divider) noexcept override;
        HRESULT __stdcall GetStreamSourceFreq(UINT StreamNumber, UINT* Divider) noexcept override;
        HRESULT __stdcall SetIndices(IDirect3DIndexBuffer9* pIndexData) noexcept override;
        HRESULT __stdcall GetIndices(IDirect3DIndexBuffer9** ppIndexData) noexcept override;
        HRESULT __stdcall CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader) noexcept override;
        HRESULT __stdcall SetPixelShader(IDirect3DPixelShader9* pShader) noexcept override;
        HRESULT __stdcall GetPixelShader(IDirect3DPixelShader9** ppShader) noexcept override;
        HRESULT __stdcall SetPixelShaderConstantF(UINT StartRegister, CONST FLOAT* pConstantData, UINT Vector4fCount) noexcept override;
        HRESULT __stdcall GetPixelShaderConstantF(UINT StartRegister, FLOAT* pConstantData, UINT Vector4fCount) noexcept override;
        HRESULT __stdcall SetPixelShaderConstantI(UINT StartRegister, CONST INT* pConstantData, UINT Vector4iCount) noexcept override;
        HRESULT __stdcall GetPixelShaderConstantI(UINT StartRegister, INT* pConstantData, UINT Vector4iCount) noexcept override;
        HRESULT __stdcall SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount) noexcept override;
        HRESULT __stdcall GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) noexcept override;
        HRESULT __stdcall DrawRectPatch(UINT Handle, CONST FLOAT* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo) noexcept override;
        HRESULT __stdcall DrawTriPatch(UINT Handle, CONST FLOAT* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo) noexcept override;
        HRESULT __stdcall DeletePatch(UINT Handle) noexcept override;
        HRESULT __stdcall CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) noexcept override;

    private:

        IDirect3DDevice9* const pOrigInterface;

        bool resetStatus { false };
        bool sceneStatus { false };

    };

    interface IDirect3D9Hook : public IDirect3D9 {

        IDirect3D9Hook() = delete;
        IDirect3D9Hook(const IDirect3D9Hook&) = delete;
        IDirect3D9Hook(IDirect3D9Hook&&) = delete;
        IDirect3D9Hook& operator=(const IDirect3D9Hook&) = delete;
        IDirect3D9Hook& operator=(IDirect3D9Hook&&) = delete;

    public:

        explicit IDirect3D9Hook(IDirect3D9* pOrigInterface) noexcept;

        ~IDirect3D9Hook() noexcept;

    public:
        
        HRESULT __stdcall QueryInterface(REFIID riid, VOID** ppvObj) noexcept;
        ULONG __stdcall AddRef() noexcept;
        ULONG __stdcall Release() noexcept;
        HRESULT __stdcall RegisterSoftwareDevice(VOID* pInitializeFunction) noexcept;
        UINT __stdcall GetAdapterCount() noexcept;
        HRESULT __stdcall GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier) noexcept;
        UINT __stdcall GetAdapterModeCount(UINT Adapter, D3DFORMAT Format) noexcept;
        HRESULT __stdcall EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode) noexcept;
        HRESULT __stdcall GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode) noexcept;
        HRESULT __stdcall CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) noexcept;
        HRESULT __stdcall CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) noexcept;
        HRESULT __stdcall CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) noexcept;
        HRESULT __stdcall CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) noexcept;
        HRESULT __stdcall CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) noexcept;
        HRESULT __stdcall GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps) noexcept;
        HMONITOR __stdcall GetAdapterMonitor(UINT Adapter) noexcept;
        HRESULT __stdcall CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface) noexcept;

    private:

        IDirect3D9* const pOrigInterface;

    };

private:

    static IDirect3D9* CALLBACK HookDirect3DCreate9(UINT SDKVersion) noexcept;

private:

    static bool initStatus;

    static std::mutex deviceMutex;
    static IDirect3D9* pDirectInterface;
    static IDirect3DDevice9* pDeviceInterface;
    static D3DPRESENT_PARAMETERS deviceParameters;

    static std::vector<DeviceInitCallback> deviceInitCallbacks;
    static std::vector<BeforeResetCallback> beforeResetCallbacks;
    static std::vector<BeginSceneCallback> beginSceneCallbacks;
    static std::vector<RenderCallback> renderCallbacks;
    static std::vector<EndSceneCallback> endSceneCallbacks;
    static std::vector<AfterResetCallback> afterResetCallbacks;
    static std::vector<DeviceFreeCallback> deviceFreeCallbacks;

    static Memory::JumpHookPtr hookDirect3DCreate9;

};
