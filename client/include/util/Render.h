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

struct Render {

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

    static constexpr float kBaseWidth  = 640.f;
    static constexpr float kBaseHeight = 480.f;

public:

    static bool Init() noexcept;
    static void Free() noexcept;

    static bool GetWindowHandle(HWND& window_handle) noexcept;
    static bool GetScreenSize(float& screen_width, float& screen_height) noexcept;
    static bool ConvertBaseXValueToScreenXValue(float base_value, float& screen_value) noexcept;
    static bool ConvertBaseYValueToScreenYValue(float base_value, float& screen_value) noexcept;
    static bool ConvertScreenXValueToBaseXValue(float screen_value, float& base_value) noexcept;
    static bool ConvertScreenYValueToBaseYValue(float screen_value, float& base_value) noexcept;

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
        ~IDirect3DDevice9Hook() noexcept;
        IDirect3DDevice9Hook(const IDirect3DDevice9Hook&) = delete;
        IDirect3DDevice9Hook(IDirect3DDevice9Hook&&) = delete;
        IDirect3DDevice9Hook& operator=(const IDirect3DDevice9Hook&) = delete;
        IDirect3DDevice9Hook& operator=(IDirect3DDevice9Hook&&) = delete;

    public:

        IDirect3DDevice9Hook(IDirect3DDevice9* orig_interface) noexcept;

    public:

        HRESULT __stdcall Present(CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*) noexcept override;
        HRESULT __stdcall QueryInterface(REFIID, VOID**) noexcept override;
        ULONG __stdcall AddRef() noexcept override;
        ULONG __stdcall Release() noexcept override;
        HRESULT __stdcall TestCooperativeLevel() noexcept override;
        UINT __stdcall GetAvailableTextureMem() noexcept override;
        HRESULT __stdcall EvictManagedResources() noexcept override;
        HRESULT __stdcall GetDirect3D(IDirect3D9**) noexcept override;
        HRESULT __stdcall GetDeviceCaps(D3DCAPS9*) noexcept override;
        HRESULT __stdcall GetDisplayMode(UINT, D3DDISPLAYMODE*) noexcept override;
        HRESULT __stdcall GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS*) noexcept override;
        HRESULT __stdcall SetCursorProperties(UINT, UINT, IDirect3DSurface9*) noexcept override;
        VOID __stdcall SetCursorPosition(INT, INT, DWORD) noexcept override;
        BOOL __stdcall ShowCursor(BOOL) noexcept override;
        HRESULT __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS*, IDirect3DSwapChain9**) noexcept override;
        HRESULT __stdcall GetSwapChain(UINT, IDirect3DSwapChain9**) noexcept override;
        UINT __stdcall GetNumberOfSwapChains() noexcept override;
        HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS*) noexcept override;
        HRESULT __stdcall GetBackBuffer(UINT, UINT, D3DBACKBUFFER_TYPE, IDirect3DSurface9**) noexcept override;
        HRESULT __stdcall GetRasterStatus(UINT, D3DRASTER_STATUS*) noexcept override;
        HRESULT __stdcall SetDialogBoxMode(BOOL) noexcept override;
        VOID __stdcall SetGammaRamp(UINT, DWORD, CONST D3DGAMMARAMP*) noexcept override;
        VOID __stdcall GetGammaRamp(UINT, D3DGAMMARAMP*) noexcept override;
        HRESULT __stdcall CreateTexture(UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DTexture9**, HANDLE*) noexcept override;
        HRESULT __stdcall CreateVolumeTexture(UINT, UINT, UINT, UINT, DWORD Usage, D3DFORMAT, D3DPOOL, IDirect3DVolumeTexture9**, HANDLE*) noexcept override;
        HRESULT __stdcall CreateCubeTexture(UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DCubeTexture9**, HANDLE*) noexcept override;
        HRESULT __stdcall CreateVertexBuffer(UINT, DWORD, DWORD, D3DPOOL, IDirect3DVertexBuffer9**, HANDLE*) noexcept override;
        HRESULT __stdcall CreateIndexBuffer(UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DIndexBuffer9**, HANDLE*) noexcept override;
        HRESULT __stdcall CreateRenderTarget(UINT, UINT, D3DFORMAT, D3DMULTISAMPLE_TYPE, DWORD, BOOL, IDirect3DSurface9**, HANDLE*) noexcept override;
        HRESULT __stdcall CreateDepthStencilSurface(UINT, UINT, D3DFORMAT, D3DMULTISAMPLE_TYPE, DWORD, BOOL, IDirect3DSurface9**, HANDLE*) noexcept override;
        HRESULT __stdcall UpdateSurface(IDirect3DSurface9*, CONST RECT*, IDirect3DSurface9*, CONST POINT*) noexcept override;
        HRESULT __stdcall UpdateTexture(IDirect3DBaseTexture9*, IDirect3DBaseTexture9*) noexcept override;
        HRESULT __stdcall GetRenderTargetData(IDirect3DSurface9*, IDirect3DSurface9*) noexcept override;
        HRESULT __stdcall GetFrontBufferData(UINT, IDirect3DSurface9*) noexcept override;
        HRESULT __stdcall StretchRect(IDirect3DSurface9*, CONST RECT*, IDirect3DSurface9*, CONST RECT*, D3DTEXTUREFILTERTYPE) noexcept override;
        HRESULT __stdcall ColorFill(IDirect3DSurface9*, CONST RECT*, D3DCOLOR) noexcept override;
        HRESULT __stdcall CreateOffscreenPlainSurface(UINT, UINT, D3DFORMAT, D3DPOOL, IDirect3DSurface9**, HANDLE*) noexcept override;
        HRESULT __stdcall SetRenderTarget(DWORD, IDirect3DSurface9*) noexcept override;
        HRESULT __stdcall GetRenderTarget(DWORD, IDirect3DSurface9**) noexcept override;
        HRESULT __stdcall SetDepthStencilSurface(IDirect3DSurface9*) noexcept override;
        HRESULT __stdcall GetDepthStencilSurface(IDirect3DSurface9**) noexcept override;
        HRESULT __stdcall BeginScene() noexcept override;
        HRESULT __stdcall EndScene() noexcept override;
        HRESULT __stdcall Clear(DWORD, CONST D3DRECT*, DWORD, D3DCOLOR, FLOAT, DWORD) noexcept override;
        HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*) noexcept override;
        HRESULT __stdcall GetTransform(D3DTRANSFORMSTATETYPE, D3DMATRIX*) noexcept override;
        HRESULT __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*) noexcept override;
        HRESULT __stdcall SetViewport(CONST D3DVIEWPORT9*) noexcept override;
        HRESULT __stdcall GetViewport(D3DVIEWPORT9*) noexcept override;
        HRESULT __stdcall SetMaterial(CONST D3DMATERIAL9*) noexcept override;
        HRESULT __stdcall GetMaterial(D3DMATERIAL9*) noexcept override;
        HRESULT __stdcall SetLight(DWORD, CONST D3DLIGHT9*) noexcept override;
        HRESULT __stdcall GetLight(DWORD, D3DLIGHT9*) noexcept override;
        HRESULT __stdcall LightEnable(DWORD, BOOL) noexcept override;
        HRESULT __stdcall GetLightEnable(DWORD, BOOL*) noexcept override;
        HRESULT __stdcall SetClipPlane(DWORD, CONST FLOAT*) noexcept override;
        HRESULT __stdcall GetClipPlane(DWORD, FLOAT*) noexcept override;
        HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE, DWORD) noexcept override;
        HRESULT __stdcall GetRenderState(D3DRENDERSTATETYPE, DWORD*) noexcept override;
        HRESULT __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE, IDirect3DStateBlock9**) noexcept override;
        HRESULT __stdcall BeginStateBlock() noexcept override;
        HRESULT __stdcall EndStateBlock(IDirect3DStateBlock9**) noexcept override;
        HRESULT __stdcall SetClipStatus(CONST D3DCLIPSTATUS9*) noexcept override;
        HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS9*) noexcept override;
        HRESULT __stdcall GetTexture(DWORD, IDirect3DBaseTexture9**) noexcept override;
        HRESULT __stdcall SetTexture(DWORD, IDirect3DBaseTexture9*) noexcept override;
        HRESULT __stdcall GetTextureStageState(DWORD, D3DTEXTURESTAGESTATETYPE, DWORD*) noexcept override;
        HRESULT __stdcall SetTextureStageState(DWORD, D3DTEXTURESTAGESTATETYPE, DWORD) noexcept override;
        HRESULT __stdcall GetSamplerState(DWORD, D3DSAMPLERSTATETYPE, DWORD*) noexcept override;
        HRESULT __stdcall SetSamplerState(DWORD, D3DSAMPLERSTATETYPE, DWORD) noexcept override;
        HRESULT __stdcall ValidateDevice(DWORD*) noexcept override;
        HRESULT __stdcall SetPaletteEntries(UINT, CONST PALETTEENTRY*) noexcept override;
        HRESULT __stdcall GetPaletteEntries(UINT, PALETTEENTRY*) noexcept override;
        HRESULT __stdcall SetCurrentTexturePalette(UINT) noexcept override;
        HRESULT __stdcall GetCurrentTexturePalette(UINT*) noexcept override;
        HRESULT __stdcall SetScissorRect(CONST RECT*) noexcept override;
        HRESULT __stdcall GetScissorRect(RECT*) noexcept override;
        HRESULT __stdcall SetSoftwareVertexProcessing(BOOL) noexcept override;
        BOOL __stdcall GetSoftwareVertexProcessing() noexcept override;
        HRESULT __stdcall SetNPatchMode(FLOAT) noexcept override;
        FLOAT __stdcall GetNPatchMode() noexcept override;
        HRESULT __stdcall DrawPrimitive(D3DPRIMITIVETYPE, UINT, UINT) noexcept override;
        HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT, UINT, UINT, UINT, UINT) noexcept override;
        HRESULT __stdcall DrawPrimitiveUP(D3DPRIMITIVETYPE, UINT, CONST VOID*, UINT) noexcept override;
        HRESULT __stdcall DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE, UINT, UINT, UINT, CONST VOID*, D3DFORMAT, CONST VOID*, UINT) noexcept override;
        HRESULT __stdcall ProcessVertices(UINT, UINT, UINT, IDirect3DVertexBuffer9*, IDirect3DVertexDeclaration9*, DWORD) noexcept override;
        HRESULT __stdcall CreateVertexDeclaration(CONST D3DVERTEXELEMENT9*, IDirect3DVertexDeclaration9**) noexcept override;
        HRESULT __stdcall SetVertexDeclaration(IDirect3DVertexDeclaration9*) noexcept override;
        HRESULT __stdcall GetVertexDeclaration(IDirect3DVertexDeclaration9**) noexcept override;
        HRESULT __stdcall SetFVF(DWORD) noexcept override;
        HRESULT __stdcall GetFVF(DWORD*) noexcept override;
        HRESULT __stdcall CreateVertexShader(CONST DWORD*, IDirect3DVertexShader9**) noexcept override;
        HRESULT __stdcall SetVertexShader(IDirect3DVertexShader9*) noexcept override;
        HRESULT __stdcall GetVertexShader(IDirect3DVertexShader9**) noexcept override;
        HRESULT __stdcall SetVertexShaderConstantF(UINT, CONST FLOAT*, UINT) noexcept override;
        HRESULT __stdcall GetVertexShaderConstantF(UINT, FLOAT*, UINT) noexcept override;
        HRESULT __stdcall SetVertexShaderConstantI(UINT, CONST INT*, UINT) noexcept override;
        HRESULT __stdcall GetVertexShaderConstantI(UINT, INT*, UINT) noexcept override;
        HRESULT __stdcall SetVertexShaderConstantB(UINT, CONST BOOL*, UINT) noexcept override;
        HRESULT __stdcall GetVertexShaderConstantB(UINT, BOOL*, UINT) noexcept override;
        HRESULT __stdcall SetStreamSource(UINT, IDirect3DVertexBuffer9*, UINT, UINT) noexcept override;
        HRESULT __stdcall GetStreamSource(UINT, IDirect3DVertexBuffer9**, UINT*, UINT*) noexcept override;
        HRESULT __stdcall SetStreamSourceFreq(UINT, UINT) noexcept override;
        HRESULT __stdcall GetStreamSourceFreq(UINT, UINT*) noexcept override;
        HRESULT __stdcall SetIndices(IDirect3DIndexBuffer9*) noexcept override;
        HRESULT __stdcall GetIndices(IDirect3DIndexBuffer9**) noexcept override;
        HRESULT __stdcall CreatePixelShader(CONST DWORD*, IDirect3DPixelShader9**) noexcept override;
        HRESULT __stdcall SetPixelShader(IDirect3DPixelShader9*) noexcept override;
        HRESULT __stdcall GetPixelShader(IDirect3DPixelShader9**) noexcept override;
        HRESULT __stdcall SetPixelShaderConstantF(UINT, CONST FLOAT*, UINT) noexcept override;
        HRESULT __stdcall GetPixelShaderConstantF(UINT, FLOAT*, UINT) noexcept override;
        HRESULT __stdcall SetPixelShaderConstantI(UINT, CONST INT*, UINT) noexcept override;
        HRESULT __stdcall GetPixelShaderConstantI(UINT, INT*, UINT) noexcept override;
        HRESULT __stdcall SetPixelShaderConstantB(UINT, CONST BOOL*, UINT) noexcept override;
        HRESULT __stdcall GetPixelShaderConstantB(UINT, BOOL*, UINT) noexcept override;
        HRESULT __stdcall DrawRectPatch(UINT, CONST FLOAT*, CONST D3DRECTPATCH_INFO*) noexcept override;
        HRESULT __stdcall DrawTriPatch(UINT, CONST FLOAT*, CONST D3DTRIPATCH_INFO*) noexcept override;
        HRESULT __stdcall DeletePatch(UINT) noexcept override;
        HRESULT __stdcall CreateQuery(D3DQUERYTYPE, IDirect3DQuery9**) noexcept override;

    private:

        IDirect3DDevice9* _orig_interface = nullptr;

        bool _reset_status = false;
        bool _scene_status = false;

    };

    interface IDirect3D9Hook : public IDirect3D9 {

        IDirect3D9Hook() = delete;
        ~IDirect3D9Hook() noexcept;
        IDirect3D9Hook(const IDirect3D9Hook&) = delete;
        IDirect3D9Hook(IDirect3D9Hook&&) = delete;
        IDirect3D9Hook& operator=(const IDirect3D9Hook&) = delete;
        IDirect3D9Hook& operator=(IDirect3D9Hook&&) = delete;

    public:

        IDirect3D9Hook(IDirect3D9* orig_interface) noexcept;

    public:
        
        HRESULT __stdcall QueryInterface(REFIID, VOID**) noexcept;
        ULONG __stdcall AddRef() noexcept;
        ULONG __stdcall Release() noexcept;
        HRESULT __stdcall RegisterSoftwareDevice(VOID*) noexcept;
        UINT __stdcall GetAdapterCount() noexcept;
        HRESULT __stdcall GetAdapterIdentifier(UINT, DWORD, D3DADAPTER_IDENTIFIER9*) noexcept;
        UINT __stdcall GetAdapterModeCount(UINT, D3DFORMAT) noexcept;
        HRESULT __stdcall EnumAdapterModes(UINT, D3DFORMAT, UINT Mode, D3DDISPLAYMODE*) noexcept;
        HRESULT __stdcall GetAdapterDisplayMode(UINT, D3DDISPLAYMODE*) noexcept;
        HRESULT __stdcall CheckDeviceType(UINT, D3DDEVTYPE DevType, D3DFORMAT, D3DFORMAT, BOOL) noexcept;
        HRESULT __stdcall CheckDeviceFormat(UINT, D3DDEVTYPE, D3DFORMAT, DWORD, D3DRESOURCETYPE, D3DFORMAT) noexcept;
        HRESULT __stdcall CheckDeviceMultiSampleType(UINT, D3DDEVTYPE, D3DFORMAT, BOOL, D3DMULTISAMPLE_TYPE, DWORD*) noexcept;
        HRESULT __stdcall CheckDepthStencilMatch(UINT, D3DDEVTYPE, D3DFORMAT, D3DFORMAT, D3DFORMAT) noexcept;
        HRESULT __stdcall CheckDeviceFormatConversion(UINT, D3DDEVTYPE, D3DFORMAT, D3DFORMAT) noexcept;
        HRESULT __stdcall GetDeviceCaps(UINT, D3DDEVTYPE, D3DCAPS9*) noexcept;
        HMONITOR __stdcall GetAdapterMonitor(UINT) noexcept;
        HRESULT __stdcall CreateDevice(UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS*, IDirect3DDevice9**) noexcept;

    private:

        IDirect3D9* _orig_interface = nullptr;

    };

private:

    static IDirect3D9* CALLBACK HookDirect3DCreate9(UINT) noexcept;

private:

    static bool _init_status;

    static std::mutex            _device_mutex;
    static IDirect3D9*           _direct_interface;
    static IDirect3DDevice9*     _device_interface;
    static D3DPRESENT_PARAMETERS _device_parameters;

    static std::vector<DeviceInitCallback>  _device_init_callbacks;
    static std::vector<BeforeResetCallback> _before_reset_callbacks;
    static std::vector<BeginSceneCallback>  _begin_scene_callbacks;
    static std::vector<RenderCallback>      _render_callbacks;
    static std::vector<EndSceneCallback>    _end_scene_callbacks;
    static std::vector<AfterResetCallback>  _after_reset_callbacks;
    static std::vector<DeviceFreeCallback>  _device_free_callbacks;

    static Memory::JumpHook _hook_direct3d_create9;

};
