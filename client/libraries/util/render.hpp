/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <functional>

#include <d3d9.h>
#include <d3dx9.h>

#include <execution/spinlock.hpp>
#include <memory/hacks/jump_hook.hpp>

#include "logger.hpp"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

namespace /* variables */
{
    volatile auto& gGameDirect     = *reinterpret_cast<IDirect3D9**>(0xC97C20);
    volatile auto& gGameDevice     = *reinterpret_cast<IDirect3DDevice9**>(0xC97C28);
    volatile auto& gGameParameters = *reinterpret_cast<D3DPRESENT_PARAMETERS*>(0xC9C040);
}

namespace /* functions */
{
    const auto GameDirect3DCreate9 = reinterpret_cast<IDirect3D9*(CALLBACK*)(UINT)>(0x807C2B);
}

struct Render {

    Render(const Render&) = delete;
    Render(Render&&) = delete;
    Render& operator=(const Render&) = delete;
    Render& operator=(Render&&) = delete;

private:

    Render() noexcept = default;
    ~Render() noexcept = default;

public:

    static Render& Instance() noexcept
    {
        static Render instance;
        return instance;
    }

public:

    static constexpr float kBaseWidth  = 640;
    static constexpr float kBaseHeight = 480;

public:

    bool Initialize() noexcept
    {
        Logger::Instance().LogToFile("[dbg:render:initialize] module initializing...");

        if (!_direct3d_create_hook.Initialize(GameDirect3DCreate9, Direct3DCreate9Hook))
        {
            Logger::Instance().LogToFile("[err:render:initialize] failed to initialize hook");
            _direct3d_create_hook.Deinitialize();
            return false;
        }

        Logger::Instance().LogToFile("[dbg:render:initialize] module initialized");

        return true;
    }

    void Deinitialize() noexcept
    {
        Logger::Instance().LogToFile("[dbg:render:deinitialize] module releasing...");

        _direct3d_create_hook.Deinitialize();

        _device_lock.Lock();
        _direct_interface = nullptr;
        _device_interface = nullptr;
        _device_lock.Unlock();

        Logger::Instance().LogToFile("[dbg:render:deinitialize] module released");
    }

public:

    bool GetWindowHandle(HWND& window_handle) const noexcept
    {
        _device_lock.Lock();

        const auto device_interface = _device_interface;
        if (device_interface != nullptr)
        {
            window_handle = _device_parameters.hDeviceWindow;
        }

        _device_lock.Unlock();

        return device_interface != nullptr;
    }

    bool GetScreenSize(float& width, float& height) const noexcept
    {
        _device_lock.Lock();

        const auto device_interface = _device_interface;
        if (device_interface != nullptr)
        {
            width  = _device_parameters.BackBufferWidth;
            height = _device_parameters.BackBufferHeight;
        }

        _device_lock.Unlock();

        return device_interface != nullptr;
    }

public:

    bool ConvertBaseXValueToScreenXValue(const float base_value, float& screen_value) const noexcept
    {
        _device_lock.Lock();

        const auto device_interface = _device_interface;
        if (device_interface != nullptr)
        {
            const float screen_width = _device_parameters.BackBufferWidth;
            screen_value = (screen_width / kBaseWidth) * base_value;
        }

        _device_lock.Unlock();

        return device_interface != nullptr;
    }

    bool ConvertBaseYValueToScreenYValue(const float base_value, float& screen_value) const noexcept
    {
        _device_lock.Lock();

        const auto device_interface = _device_interface;
        if (device_interface != nullptr)
        {
            const float screen_height = _device_parameters.BackBufferHeight;
            screen_value = (screen_height / kBaseHeight) * base_value;
        }

        _device_lock.Unlock();

        return device_interface != nullptr;
    }

    bool ConvertScreenXValueToBaseXValue(const float screen_value, float& base_value) const noexcept
    {
        _device_lock.Lock();

        const auto device_interface = _device_interface;
        if (device_interface != nullptr)
        {
            const float screen_width = _device_parameters.BackBufferWidth;
            base_value = (kBaseWidth / screen_width) * screen_value;
        }

        _device_lock.Unlock();

        return device_interface != nullptr;
    }

    bool ConvertScreenYValueToBaseYValue(const float screen_value, float& base_value) const noexcept
    {
        _device_lock.Lock();

        const auto device_interface = _device_interface;
        if (device_interface != nullptr)
        {
            const float screen_height = _device_parameters.BackBufferHeight;
            base_value = (kBaseHeight / screen_height) * screen_value;
        }

        _device_lock.Unlock();

        return device_interface != nullptr;
    }

private:

    struct IDirect3DDevice9Hook : IDirect3DDevice9 {

        IDirect3DDevice9Hook() = delete;
        ~IDirect3DDevice9Hook() noexcept
        {
            _orig_interface->Release();
        }

        IDirect3DDevice9Hook(const IDirect3DDevice9Hook&) = delete;
        IDirect3DDevice9Hook(IDirect3DDevice9Hook&&) = delete;
        IDirect3DDevice9Hook& operator=(const IDirect3DDevice9Hook&) = delete;
        IDirect3DDevice9Hook& operator=(IDirect3DDevice9Hook&&) = delete;

    public:

        IDirect3DDevice9Hook(IDirect3DDevice9* const orig_interface) noexcept
            : _orig_interface { orig_interface }
        {
            orig_interface->AddRef();
        }

    public:

        HRESULT __stdcall Present(CONST RECT* const source_rect,
            CONST RECT* const dest_rect, const HWND dest_window_override,
            CONST RGNDATA* const dirty_region) noexcept
        {
            if (_orig_interface == Instance()._device_interface)
            {
                if (_reset_status == false)
                {
                    if (Instance().OnPresent != nullptr)
                        Instance().OnPresent();
                }
            }

            return _orig_interface->Present(source_rect, dest_rect,
                dest_window_override, dirty_region);
        }

        HRESULT __stdcall QueryInterface(REFIID riid, VOID** const ppv_obj) noexcept
        {
            *ppv_obj = nullptr;
            const HRESULT result = _orig_interface->QueryInterface(riid, ppv_obj);
            if (SUCCEEDED(result)) *ppv_obj = this;
            return result;
        }

        ULONG __stdcall AddRef() noexcept
        {
            return _orig_interface->AddRef();
        }

        ULONG __stdcall Release() noexcept
        {
            const ULONG count = _orig_interface->Release();
            if (count <= 1)
            {
                if (_orig_interface == Instance()._device_interface)
                {
                    if (Instance().OnDeviceDestroy != nullptr)
                        Instance().OnDeviceDestroy();

                    Instance()._device_lock.Lock();
                    Instance()._direct_interface = nullptr;
                    Instance()._device_interface = nullptr;
                    Instance()._device_lock.Unlock();
                }

                delete this;
                return 0;
            }

            return count;
        }

        HRESULT __stdcall TestCooperativeLevel() noexcept
        {
            return _orig_interface->TestCooperativeLevel();
        }

        UINT __stdcall GetAvailableTextureMem() noexcept
        {
            return _orig_interface->GetAvailableTextureMem();
        }

        HRESULT __stdcall EvictManagedResources() noexcept
        {
            return _orig_interface->EvictManagedResources();
        }

        HRESULT __stdcall GetDirect3D(IDirect3D9** const d3d9) noexcept
        {
            return _orig_interface->GetDirect3D(d3d9);
        }

        HRESULT __stdcall GetDeviceCaps(D3DCAPS9* const caps) noexcept
        {
            return _orig_interface->GetDeviceCaps(caps);
        }

        HRESULT __stdcall GetDisplayMode(const UINT swapchain_index,
            D3DDISPLAYMODE* const display_mode) noexcept
        {
            return _orig_interface->GetDisplayMode(swapchain_index, display_mode);
        }

        HRESULT __stdcall GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* const parameters) noexcept
        {
            return _orig_interface->GetCreationParameters(parameters);
        }

        HRESULT __stdcall SetCursorProperties(const UINT x_hot_spot,
            const UINT y_hot_spot, IDirect3DSurface9* const cursor_bitmap) noexcept
        {
            return _orig_interface->SetCursorProperties(x_hot_spot,
                y_hot_spot, cursor_bitmap);
        }

        VOID __stdcall SetCursorPosition(const INT x, const INT y, const DWORD flags) noexcept
        {
            _orig_interface->SetCursorPosition(x, y, flags);
        }

        BOOL __stdcall ShowCursor(const BOOL show) noexcept
        {
            return _orig_interface->ShowCursor(show);
        }

        HRESULT __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* const presentation_parameters,
            IDirect3DSwapChain9** const swapchain) noexcept
        {
            return _orig_interface->CreateAdditionalSwapChain(presentation_parameters, swapchain);
        }

        HRESULT __stdcall GetSwapChain(const UINT swapchain_index,
            IDirect3DSwapChain9** const swapchain) noexcept
        {
            return _orig_interface->GetSwapChain(swapchain_index, swapchain);
        }

        UINT __stdcall GetNumberOfSwapChains() noexcept
        {
            return _orig_interface->GetNumberOfSwapChains();
        }

        HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS* const presentation_parameters) noexcept
        {
            if (_orig_interface == Instance()._device_interface)
            {
                if (_reset_status == false)
                {
                    if (Instance().OnBeforeReset != nullptr)
                        Instance().OnBeforeReset();

                    _reset_status = true;
                }
            }

            const HRESULT result = _orig_interface->Reset(presentation_parameters);

            if (_orig_interface == Instance()._device_interface)
            {
                if (SUCCEEDED(result))
                {
                    Instance()._device_lock.Lock();
                    Instance()._device_parameters = *presentation_parameters;
                    Instance()._device_lock.Unlock();

                    if (_reset_status == true)
                    {
                        if (Instance().OnAfterReset != nullptr)
                            Instance().OnAfterReset(Instance()._device_interface, Instance()._device_parameters);

                        _reset_status = false;
                    }
                }
            }

            return result;
        }

        HRESULT __stdcall GetBackBuffer(const UINT swapchain_index, const UINT backbuffer_index,
            const D3DBACKBUFFER_TYPE type, IDirect3DSurface9** const out_backbuffer) noexcept
        {
            return _orig_interface->GetBackBuffer(swapchain_index,
                backbuffer_index, type, out_backbuffer);
        }

        HRESULT __stdcall GetRasterStatus(const UINT swapchain_index,
            D3DRASTER_STATUS* const raster_status) noexcept
        {
            return _orig_interface->GetRasterStatus(swapchain_index, raster_status);
        }

        HRESULT __stdcall SetDialogBoxMode(const BOOL enable_dialogs) noexcept
        {
            return _orig_interface->SetDialogBoxMode(enable_dialogs);
        }

        VOID __stdcall SetGammaRamp(const UINT swapchain_index, const DWORD flags,
            CONST D3DGAMMARAMP* const ramp) noexcept
        {
            _orig_interface->SetGammaRamp(swapchain_index, flags, ramp);
        }

        VOID __stdcall GetGammaRamp(const UINT swapchain_index, D3DGAMMARAMP* const ramp) noexcept
        {
            _orig_interface->GetGammaRamp(swapchain_index, ramp);
        }

        HRESULT __stdcall CreateTexture(const UINT width, const UINT height,
            const UINT levels, const DWORD usage, const D3DFORMAT format, const D3DPOOL pool,
            IDirect3DTexture9** const out_texture, HANDLE* const shared_handle) noexcept
        {
            return _orig_interface->CreateTexture(width, height, levels, usage,
                format, pool, out_texture, shared_handle);
        }

        HRESULT __stdcall CreateVolumeTexture(const UINT width, const UINT height,
            const UINT Depth, const UINT levels, const DWORD usage, const D3DFORMAT format, const D3DPOOL pool,
            IDirect3DVolumeTexture9** const out_volume_texture, HANDLE* const shared_handle) noexcept
        {
            return _orig_interface->CreateVolumeTexture(width, height, Depth, levels,
                usage, format, pool, out_volume_texture, shared_handle);
        }

        HRESULT __stdcall CreateCubeTexture(const UINT edge_length, const UINT levels,
            const DWORD usage, const D3DFORMAT format, const D3DPOOL pool,
            IDirect3DCubeTexture9** const out_cube_texture, HANDLE* const shared_handle) noexcept
        {
            return _orig_interface->CreateCubeTexture(edge_length, levels, usage,
                format, pool, out_cube_texture, shared_handle);
        }

        HRESULT __stdcall CreateVertexBuffer(const UINT length, const DWORD usage, const DWORD fvf,
            const D3DPOOL pool, IDirect3DVertexBuffer9** const out_vertex_buffer,
            HANDLE* const shared_handle) noexcept
        {
            return _orig_interface->CreateVertexBuffer(length, usage, fvf, pool,
                out_vertex_buffer, shared_handle);
        }

        HRESULT __stdcall CreateIndexBuffer(const UINT length, const DWORD usage, const D3DFORMAT format,
            const D3DPOOL pool, IDirect3DIndexBuffer9** const out_index_buffer, HANDLE* const shared_handle) noexcept
        {
            return _orig_interface->CreateIndexBuffer(length, usage, format, pool,
                out_index_buffer, shared_handle);
        }

        HRESULT __stdcall CreateRenderTarget(const UINT width, const UINT height,
            const D3DFORMAT format, const D3DMULTISAMPLE_TYPE multisample, const DWORD multisample_quality, const BOOL lockable,
            IDirect3DSurface9** const out_surface, HANDLE* const shared_handle) noexcept
        {
            return _orig_interface->CreateRenderTarget(width, height, format, multisample,
                multisample_quality, lockable, out_surface, shared_handle);
        }

        HRESULT __stdcall CreateDepthStencilSurface(const UINT width, const UINT height,
            const D3DFORMAT format, const D3DMULTISAMPLE_TYPE multisample, const DWORD multisample_quality,
            const BOOL discard, IDirect3DSurface9** const out_surface, HANDLE* const shared_handle) noexcept
        {
            return _orig_interface->CreateDepthStencilSurface(width, height, format, multisample,
                multisample_quality, discard, out_surface, shared_handle);
        }

        HRESULT __stdcall UpdateSurface(IDirect3DSurface9* const source_surface,
            CONST RECT* const source_rect, IDirect3DSurface9* const destination_surface, CONST POINT* const dest_point) noexcept
        {
            return _orig_interface->UpdateSurface(source_surface, source_rect,
                destination_surface, dest_point);
        }

        HRESULT __stdcall UpdateTexture(IDirect3DBaseTexture9* const source_texture,
            IDirect3DBaseTexture9* const destination_texture) noexcept
        {
            return _orig_interface->UpdateTexture(source_texture, destination_texture);
        }

        HRESULT __stdcall GetRenderTargetData(IDirect3DSurface9* const render_target,
            IDirect3DSurface9* const dest_surface) noexcept
        {
            return _orig_interface->GetRenderTargetData(render_target, dest_surface);
        }

        HRESULT __stdcall GetFrontBufferData(const UINT swapchain_index,
            IDirect3DSurface9* const dest_surface) noexcept
        {
            return _orig_interface->GetFrontBufferData(swapchain_index, dest_surface);
        }

        HRESULT __stdcall StretchRect(IDirect3DSurface9* const source_surface,
            CONST RECT* const source_rect, IDirect3DSurface9* const dest_surface,
            CONST RECT* const dest_rect, const D3DTEXTUREFILTERTYPE filter) noexcept
        {
            return _orig_interface->StretchRect(source_surface, source_rect,
                dest_surface, dest_rect, filter);
        }

        HRESULT __stdcall ColorFill(IDirect3DSurface9* const surface,
            CONST RECT* const rect, const D3DCOLOR color) noexcept
        {
            return _orig_interface->ColorFill(surface, rect, color);
        }

        HRESULT __stdcall CreateOffscreenPlainSurface(const UINT width,
            const UINT height, const D3DFORMAT format, const D3DPOOL pool,
            IDirect3DSurface9** const out_surface, HANDLE* const shared_handle) noexcept
        {
            return _orig_interface->CreateOffscreenPlainSurface(width, height,
                format, pool, out_surface, shared_handle);
        }

        HRESULT __stdcall SetRenderTarget(const DWORD render_target_index,
            IDirect3DSurface9* const render_target) noexcept
        {
            return _orig_interface->SetRenderTarget(render_target_index, render_target);
        }

        HRESULT __stdcall GetRenderTarget(const DWORD render_target_index,
            IDirect3DSurface9** const out_render_target) noexcept
        {
            return _orig_interface->GetRenderTarget(render_target_index, out_render_target);
        }

        HRESULT __stdcall SetDepthStencilSurface(IDirect3DSurface9* const new_z_stencil) noexcept
        {
            return _orig_interface->SetDepthStencilSurface(new_z_stencil);
        }

        HRESULT __stdcall GetDepthStencilSurface(IDirect3DSurface9** const out_z_stencil_surface) noexcept
        {
            return _orig_interface->GetDepthStencilSurface(out_z_stencil_surface);
        }

        HRESULT __stdcall BeginScene() noexcept
        {
            if (_orig_interface == Instance()._device_interface)
            {
                if (_reset_status == false)
                {
                    if (_scene_status == false)
                    {
                        if (Instance().OnBeforeBeginScene != nullptr)
                            Instance().OnBeforeBeginScene();
                    }
                }
            }

            const HRESULT result = _orig_interface->BeginScene();

            if (_orig_interface == Instance()._device_interface)
            {
                if (SUCCEEDED(result))
                {
                    if (_reset_status == false)
                    {
                        if (_scene_status == false)
                        {
                            if (Instance().OnAfterBeginScene != nullptr)
                                Instance().OnAfterBeginScene();

                            _scene_status = true;
                        }
                    }
                }
            }

            return result;
        }

        HRESULT __stdcall EndScene() noexcept
        {
            if (_orig_interface == Instance()._device_interface)
            {
                if (_reset_status == false)
                {
                    if (_scene_status == true)
                    {
                        if (Instance().OnBeforeEndScene != nullptr)
                            Instance().OnBeforeEndScene();
                    }
                }
            }

            const HRESULT result = _orig_interface->EndScene();

            if (_orig_interface == Instance()._device_interface)
            {
                if (SUCCEEDED(result))
                {
                    if (_reset_status == false)
                    {
                        if (_scene_status == true)
                        {
                            if (Instance().OnAfterEndScene != nullptr)
                                Instance().OnAfterEndScene();

                            _scene_status = false;
                        }
                    }
                }
            }

            return result;
        }

        HRESULT __stdcall Clear(const DWORD count, CONST D3DRECT* const rects,
            const DWORD flags, const D3DCOLOR color, const FLOAT z, const DWORD stencil) noexcept
        {
            return _orig_interface->Clear(count, rects, flags, color, z, stencil);
        }

        HRESULT __stdcall SetTransform(const D3DTRANSFORMSTATETYPE state,
            CONST D3DMATRIX* const mat) noexcept
        {
            return _orig_interface->SetTransform(state, mat);
        }

        HRESULT __stdcall GetTransform(const D3DTRANSFORMSTATETYPE state,
            D3DMATRIX* const mat) noexcept
        {
            return _orig_interface->GetTransform(state, mat);
        }

        HRESULT __stdcall MultiplyTransform(const D3DTRANSFORMSTATETYPE state,
            CONST D3DMATRIX* const mat) noexcept
        {
            return _orig_interface->MultiplyTransform(state, mat);
        }

        HRESULT __stdcall SetViewport(CONST D3DVIEWPORT9* const viewport) noexcept
        {
            return _orig_interface->SetViewport(viewport);
        }

        HRESULT __stdcall GetViewport(D3DVIEWPORT9* const viewport) noexcept
        {
            return _orig_interface->GetViewport(viewport);
        }

        HRESULT __stdcall SetMaterial(CONST D3DMATERIAL9* const material) noexcept
        {
            return _orig_interface->SetMaterial(material);
        }

        HRESULT __stdcall GetMaterial(D3DMATERIAL9* const material) noexcept
        {
            return _orig_interface->GetMaterial(material);
        }

        HRESULT __stdcall SetLight(const DWORD index, CONST D3DLIGHT9* const light) noexcept
        {
            return _orig_interface->SetLight(index, light);
        }

        HRESULT __stdcall GetLight(const DWORD index, D3DLIGHT9* const light) noexcept
        {
            return _orig_interface->GetLight(index, light);
        }

        HRESULT __stdcall LightEnable(const DWORD index, const BOOL enable) noexcept
        {
            return _orig_interface->LightEnable(index, enable);
        }

        HRESULT __stdcall GetLightEnable(const DWORD index, BOOL* const out_enable) noexcept
        {
            return _orig_interface->GetLightEnable(index, out_enable);
        }

        HRESULT __stdcall SetClipPlane(const DWORD index, CONST FLOAT* const plane) noexcept
        {
            return _orig_interface->SetClipPlane(index, plane);
        }

        HRESULT __stdcall GetClipPlane(const DWORD index, FLOAT* const plane) noexcept
        {
            return _orig_interface->GetClipPlane(index, plane);
        }

        HRESULT __stdcall SetRenderState(const D3DRENDERSTATETYPE state,
            const DWORD value) noexcept
        {
            return _orig_interface->SetRenderState(state, value);
        }

        HRESULT __stdcall GetRenderState(const D3DRENDERSTATETYPE state,
            DWORD* const out_value) noexcept
        {
            return _orig_interface->GetRenderState(state, out_value);
        }

        HRESULT __stdcall CreateStateBlock(const D3DSTATEBLOCKTYPE type,
            IDirect3DStateBlock9** const out_sb) noexcept
        {
            return _orig_interface->CreateStateBlock(type, out_sb);
        }

        HRESULT __stdcall BeginStateBlock() noexcept
        {
            return _orig_interface->BeginStateBlock();
        }

        HRESULT __stdcall EndStateBlock(IDirect3DStateBlock9** const sb_ptr) noexcept
        {
            return _orig_interface->EndStateBlock(sb_ptr);
        }

        HRESULT __stdcall SetClipStatus(CONST D3DCLIPSTATUS9* const clip_status) noexcept
        {
            return _orig_interface->SetClipStatus(clip_status);
        }

        HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS9* const clip_status) noexcept
        {
            return _orig_interface->GetClipStatus(clip_status);
        }

        HRESULT __stdcall GetTexture(const DWORD stage, IDirect3DBaseTexture9** const out_texture) noexcept
        {
            return _orig_interface->GetTexture(stage, out_texture);
        }

        HRESULT __stdcall SetTexture(const DWORD stage, IDirect3DBaseTexture9* const texture) noexcept
        {
            return _orig_interface->SetTexture(stage, texture);
        }

        HRESULT __stdcall GetTextureStageState(const DWORD stage,
            const D3DTEXTURESTAGESTATETYPE type, DWORD* const out_value) noexcept
        {
            return _orig_interface->GetTextureStageState(stage, type, out_value);
        }

        HRESULT __stdcall SetTextureStageState(const DWORD stage,
            const D3DTEXTURESTAGESTATETYPE type, const DWORD value) noexcept
        {
            return _orig_interface->SetTextureStageState(stage, type, value);
        }

        HRESULT __stdcall GetSamplerState(const DWORD sampler,
            const D3DSAMPLERSTATETYPE type, DWORD* const out_value) noexcept
        {
            return _orig_interface->GetSamplerState(sampler, type, out_value);
        }

        HRESULT __stdcall SetSamplerState(const DWORD sampler,
            const D3DSAMPLERSTATETYPE type, const DWORD value) noexcept
        {
            return _orig_interface->SetSamplerState(sampler, type, value);
        }

        HRESULT __stdcall ValidateDevice(DWORD* const num_passes) noexcept
        {
            return _orig_interface->ValidateDevice(num_passes);
        }

        HRESULT __stdcall SetPaletteEntries(const UINT palette_number,
            CONST PALETTEENTRY* const entries) noexcept
        {
            return _orig_interface->SetPaletteEntries(palette_number, entries);
        }

        HRESULT __stdcall GetPaletteEntries(const UINT palette_number,
            PALETTEENTRY* const entries) noexcept
        {
            return _orig_interface->GetPaletteEntries(palette_number, entries);
        }

        HRESULT __stdcall SetCurrentTexturePalette(const UINT palette_number) noexcept
        {
            return _orig_interface->SetCurrentTexturePalette(palette_number);
        }

        HRESULT __stdcall GetCurrentTexturePalette(UINT* const palette_number) noexcept
        {
            return _orig_interface->GetCurrentTexturePalette(palette_number);
        }

        HRESULT __stdcall SetScissorRect(CONST RECT* const rect) noexcept
        {
            return _orig_interface->SetScissorRect(rect);
        }

        HRESULT __stdcall GetScissorRect(RECT* const rect) noexcept
        {
            return _orig_interface->GetScissorRect(rect);
        }

        HRESULT __stdcall SetSoftwareVertexProcessing(const BOOL software) noexcept
        {
            return _orig_interface->SetSoftwareVertexProcessing(software);
        }

        BOOL __stdcall GetSoftwareVertexProcessing() noexcept
        {
            return _orig_interface->GetSoftwareVertexProcessing();
        }

        HRESULT __stdcall SetNPatchMode(const FLOAT segments) noexcept
        {
            return _orig_interface->SetNPatchMode(segments);
        }

        FLOAT __stdcall GetNPatchMode() noexcept
        {
            return _orig_interface->GetNPatchMode();
        }

        HRESULT __stdcall DrawPrimitive(const D3DPRIMITIVETYPE primitive_type,
            const UINT start_vertex, const UINT primitive_count) noexcept
        {
            return _orig_interface->DrawPrimitive(primitive_type, start_vertex, primitive_count);
        }

        HRESULT __stdcall DrawIndexedPrimitive(const D3DPRIMITIVETYPE type,
            const INT base_vertex_index, const UINT min_vertex_index, const UINT num_vertices,
            const UINT start_index, const UINT prim_count) noexcept
        {
            return _orig_interface->DrawIndexedPrimitive(type, base_vertex_index,
                min_vertex_index, num_vertices, start_index, prim_count);
        }

        HRESULT __stdcall DrawPrimitiveUP(const D3DPRIMITIVETYPE primitive_type,
            const UINT primitive_count, CONST VOID* const vertex_stream_zero_data, const UINT vertex_stream_zero_stride) noexcept
        {
            return _orig_interface->DrawPrimitiveUP(primitive_type, primitive_count,
                vertex_stream_zero_data, vertex_stream_zero_stride);
        }

        HRESULT __stdcall DrawIndexedPrimitiveUP(const D3DPRIMITIVETYPE primitive_type,
            const UINT min_vertex_index, const UINT num_vertices, const UINT primitive_count, CONST VOID* const index_data,
            const D3DFORMAT index_data_format, CONST VOID* const vertex_stream_zero_data, const UINT vertex_stream_zero_stride) noexcept
        {
            return _orig_interface->DrawIndexedPrimitiveUP(primitive_type, min_vertex_index,
                num_vertices, primitive_count, index_data, index_data_format,
                vertex_stream_zero_data, vertex_stream_zero_stride);
        }

        HRESULT __stdcall ProcessVertices(const UINT src_start_index,
            const UINT dest_index, const UINT vertex_count, IDirect3DVertexBuffer9* const dest_buffer,
            IDirect3DVertexDeclaration9* const vertex_decl, const DWORD flags) noexcept
        {
            return _orig_interface->ProcessVertices(src_start_index, dest_index,
                vertex_count, dest_buffer, vertex_decl, flags);
        }

        HRESULT __stdcall CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* const vertex_elements,
            IDirect3DVertexDeclaration9** const out_decl) noexcept
        {
            return _orig_interface->CreateVertexDeclaration(vertex_elements, out_decl);
        }

        HRESULT __stdcall SetVertexDeclaration(IDirect3DVertexDeclaration9* const decl) noexcept
        {
            return _orig_interface->SetVertexDeclaration(decl);
        }

        HRESULT __stdcall GetVertexDeclaration(IDirect3DVertexDeclaration9** const out_decl) noexcept
        {
            return _orig_interface->GetVertexDeclaration(out_decl);
        }

        HRESULT __stdcall SetFVF(const DWORD fvf) noexcept
        {
            return _orig_interface->SetFVF(fvf);
        }

        HRESULT __stdcall GetFVF(DWORD* const out_fvf) noexcept
        {
            return _orig_interface->GetFVF(out_fvf);
        }

        HRESULT __stdcall CreateVertexShader(CONST DWORD* const function,
            IDirect3DVertexShader9** const out_shader) noexcept
        {
            return _orig_interface->CreateVertexShader(function, out_shader);
        }

        HRESULT __stdcall SetVertexShader(IDirect3DVertexShader9* const shader) noexcept
        {
            return _orig_interface->SetVertexShader(shader);
        }

        HRESULT __stdcall GetVertexShader(IDirect3DVertexShader9** const out_shader) noexcept
        {
            return _orig_interface->GetVertexShader(out_shader);
        }

        HRESULT __stdcall SetVertexShaderConstantF(const UINT start_register,
            CONST FLOAT* const constant_data, const UINT vector4f_count) noexcept
        {
            return _orig_interface->SetVertexShaderConstantF(start_register,
                constant_data, vector4f_count);
        }

        HRESULT __stdcall GetVertexShaderConstantF(const UINT start_register,
            FLOAT* const constant_data, const UINT vector4f_count) noexcept
        {
            return _orig_interface->GetVertexShaderConstantF(start_register,
                constant_data, vector4f_count);
        }

        HRESULT __stdcall SetVertexShaderConstantI(const UINT start_register,
            CONST INT* const constant_data, const UINT vector4i_count) noexcept
        {
            return _orig_interface->SetVertexShaderConstantI(start_register,
                constant_data, vector4i_count);
        }

        HRESULT __stdcall GetVertexShaderConstantI(const UINT start_register,
            INT* const constant_data, const UINT vector4i_count) noexcept
        {
            return _orig_interface->GetVertexShaderConstantI(start_register,
                constant_data, vector4i_count);
        }

        HRESULT __stdcall SetVertexShaderConstantB(const UINT start_register,
            CONST BOOL* const constant_data, const UINT bool_count) noexcept
        {
            return _orig_interface->SetVertexShaderConstantB(start_register,
                constant_data, bool_count);
        }

        HRESULT __stdcall GetVertexShaderConstantB(const UINT start_register,
            BOOL* const constant_data, const UINT bool_count) noexcept
        {
            return _orig_interface->GetVertexShaderConstantB(start_register,
                constant_data, bool_count);
        }

        HRESULT __stdcall SetStreamSource(const UINT stream_number,
            IDirect3DVertexBuffer9* const stream_data, const UINT offset_in_bytes, const UINT stride) noexcept
        {
            return _orig_interface->SetStreamSource(stream_number, stream_data,
                offset_in_bytes, stride);
        }

        HRESULT __stdcall GetStreamSource(const UINT stream_number,
            IDirect3DVertexBuffer9** const out_stream_data, UINT* const offset_in_bytes, UINT* const out_stride) noexcept
        {
            return _orig_interface->GetStreamSource(stream_number, out_stream_data,
                offset_in_bytes, out_stride);
        }

        HRESULT __stdcall SetStreamSourceFreq(const UINT stream_number, const UINT divider) noexcept
        {
            return _orig_interface->SetStreamSourceFreq(stream_number, divider);
        }

        HRESULT __stdcall GetStreamSourceFreq(const UINT stream_number, UINT* const divider) noexcept
        {
            return _orig_interface->GetStreamSourceFreq(stream_number, divider);
        }

        HRESULT __stdcall SetIndices(IDirect3DIndexBuffer9* const index_data) noexcept
        {
            return _orig_interface->SetIndices(index_data);
        }

        HRESULT __stdcall GetIndices(IDirect3DIndexBuffer9** const out_index_data) noexcept
        {
            return _orig_interface->GetIndices(out_index_data);
        }

        HRESULT __stdcall CreatePixelShader(CONST DWORD* const function,
            IDirect3DPixelShader9** const out_shader) noexcept
        {
            return _orig_interface->CreatePixelShader(function, out_shader);
        }

        HRESULT __stdcall SetPixelShader(IDirect3DPixelShader9* const shader) noexcept
        {
            return _orig_interface->SetPixelShader(shader);
        }

        HRESULT __stdcall GetPixelShader(IDirect3DPixelShader9** const out_shader) noexcept
        {
            return _orig_interface->GetPixelShader(out_shader);
        }

        HRESULT __stdcall SetPixelShaderConstantF(const UINT start_register,
            CONST FLOAT* const constant_data, const UINT vector4f_count) noexcept
        {
            return _orig_interface->SetPixelShaderConstantF(start_register,
                constant_data, vector4f_count);
        }

        HRESULT __stdcall GetPixelShaderConstantF(const UINT start_register,
            FLOAT* const constant_data, const UINT vector4f_count) noexcept
        {
            return _orig_interface->GetPixelShaderConstantF(start_register,
                constant_data, vector4f_count);
        }

        HRESULT __stdcall SetPixelShaderConstantI(const UINT start_register,
            CONST INT* const constant_data, const UINT vector4i_count) noexcept
        {
            return _orig_interface->SetPixelShaderConstantI(start_register,
                constant_data, vector4i_count);
        }

        HRESULT __stdcall GetPixelShaderConstantI(const UINT start_register,
            INT* const constant_data, const UINT vector4i_count) noexcept
        {
            return _orig_interface->GetPixelShaderConstantI(start_register,
                constant_data, vector4i_count);
        }

        HRESULT __stdcall SetPixelShaderConstantB(const UINT start_register,
            CONST BOOL* const constant_data, const UINT bool_count) noexcept
        {
            return _orig_interface->SetPixelShaderConstantB(start_register,
                constant_data, bool_count);
        }

        HRESULT __stdcall GetPixelShaderConstantB(const UINT start_register,
            BOOL* const constant_data, const UINT bool_count) noexcept
        {
            return _orig_interface->GetPixelShaderConstantB(start_register,
                constant_data, bool_count);
        }

        HRESULT __stdcall DrawRectPatch(const UINT handle, CONST FLOAT* const num_segs,
            CONST D3DRECTPATCH_INFO* const rect_patch_info) noexcept
        {
            return _orig_interface->DrawRectPatch(handle, num_segs, rect_patch_info);
        }

        HRESULT __stdcall DrawTriPatch(const UINT handle, CONST FLOAT* const num_segs,
            CONST D3DTRIPATCH_INFO* const tri_patch_info) noexcept
        {
            return _orig_interface->DrawTriPatch(handle, num_segs, tri_patch_info);
        }

        HRESULT __stdcall DeletePatch(const UINT handle) noexcept
        {
            return _orig_interface->DeletePatch(handle);
        }

        HRESULT __stdcall CreateQuery(const D3DQUERYTYPE type, IDirect3DQuery9** const out_query) noexcept
        {
            return _orig_interface->CreateQuery(type, out_query);
        }

    private:

        IDirect3DDevice9* const _orig_interface;

    private:

        bool _reset_status = false;
        bool _scene_status = false;

    };

    struct IDirect3D9Hook : IDirect3D9 {

        IDirect3D9Hook() = delete;
        ~IDirect3D9Hook() noexcept
        {
            _orig_interface->Release();
        }

        IDirect3D9Hook(const IDirect3D9Hook&) = delete;
        IDirect3D9Hook(IDirect3D9Hook&&) = delete;
        IDirect3D9Hook& operator=(const IDirect3D9Hook&) = delete;
        IDirect3D9Hook& operator=(IDirect3D9Hook&&) = delete;

    public:

        IDirect3D9Hook(IDirect3D9* const orig_interface) noexcept
            : _orig_interface { orig_interface }
        {
            orig_interface->AddRef();
        }

    public:
        
        HRESULT __stdcall QueryInterface(REFIID riid, VOID** const ppv_obj) noexcept
        {
            *ppv_obj = nullptr;
            const HRESULT result = _orig_interface->QueryInterface(riid, ppv_obj);
            if (SUCCEEDED(result)) *ppv_obj = this;
            return result;
        }

        ULONG __stdcall AddRef() noexcept
        {
            return _orig_interface->AddRef();
        }

        ULONG __stdcall Release() noexcept
        {
            const ULONG count = _orig_interface->Release();
            if (count <= 1)
            {
                delete this;
                return 0;
            }

            return count;
        }

        HRESULT __stdcall RegisterSoftwareDevice(VOID* const initialize_function) noexcept
        {
            return _orig_interface->RegisterSoftwareDevice(initialize_function);
        }

        UINT __stdcall GetAdapterCount() noexcept
        {
            return _orig_interface->GetAdapterCount();
        }

        HRESULT __stdcall GetAdapterIdentifier(const UINT adapter, const DWORD flags,
            D3DADAPTER_IDENTIFIER9* const identifier) noexcept
        {
            return _orig_interface->GetAdapterIdentifier(adapter, flags, identifier);
        }

        UINT __stdcall GetAdapterModeCount(const UINT adapter, const D3DFORMAT format) noexcept
        {
            return _orig_interface->GetAdapterModeCount(adapter, format);
        }

        HRESULT __stdcall EnumAdapterModes(const UINT adapter, const D3DFORMAT format,
            const UINT mode, D3DDISPLAYMODE* const display_mode) noexcept
        {
            return _orig_interface->EnumAdapterModes(adapter, format, mode, display_mode);
        }

        HRESULT __stdcall GetAdapterDisplayMode(const UINT adapter, D3DDISPLAYMODE* const display_mode) noexcept
        {
            return _orig_interface->GetAdapterDisplayMode(adapter, display_mode);
        }

        HRESULT __stdcall CheckDeviceType(const UINT adapter, const D3DDEVTYPE dev_type,
            const D3DFORMAT adapter_format, const D3DFORMAT backbuffer_format, const BOOL windowed) noexcept
        {
            return _orig_interface->CheckDeviceType(adapter, dev_type,
                adapter_format, backbuffer_format, windowed);
        }

        HRESULT __stdcall CheckDeviceFormat(const UINT adapter, const D3DDEVTYPE device_type,
            const D3DFORMAT adapter_format, const DWORD usage, const D3DRESOURCETYPE resource_type,
            D3DFORMAT check_format) noexcept
        {
            return _orig_interface->CheckDeviceFormat(adapter, device_type,
                adapter_format, usage, resource_type, check_format);
        }

        HRESULT __stdcall CheckDeviceMultiSampleType(const UINT adapter, const D3DDEVTYPE device_type,
            const D3DFORMAT surface_format, const BOOL windowed, const D3DMULTISAMPLE_TYPE multisample_type,
            DWORD* const quality_levels) noexcept
        {
            return _orig_interface->CheckDeviceMultiSampleType(adapter, device_type,
                surface_format, windowed, multisample_type, quality_levels);
        }

        HRESULT __stdcall CheckDepthStencilMatch(const UINT adapter, const D3DDEVTYPE device_type,
            const D3DFORMAT adapter_format, const D3DFORMAT render_target_format, const D3DFORMAT depth_stencil_format) noexcept
        {
            return _orig_interface->CheckDepthStencilMatch(adapter, device_type,
                adapter_format, render_target_format, depth_stencil_format);
        }

        HRESULT __stdcall CheckDeviceFormatConversion(const UINT adapter, const D3DDEVTYPE device_type,
            const D3DFORMAT source_format, const D3DFORMAT target_format) noexcept
        {
            return _orig_interface->CheckDeviceFormatConversion(adapter, device_type,
                source_format, target_format);
        }

        HRESULT __stdcall GetDeviceCaps(const UINT adapter,
            const D3DDEVTYPE device_type, D3DCAPS9* const caps) noexcept
        {
            return _orig_interface->GetDeviceCaps(adapter, device_type, caps);
        }

        HMONITOR __stdcall GetAdapterMonitor(const UINT adapter) noexcept
        {
            return _orig_interface->GetAdapterMonitor(adapter);
        }

        HRESULT __stdcall CreateDevice(const UINT adapter, const D3DDEVTYPE device_type, const HWND focus_window,
            const DWORD behavior_flags, D3DPRESENT_PARAMETERS* const presentation_parameters,
            IDirect3DDevice9** const out_returned_device_interface) noexcept
        {
            const HRESULT result = _orig_interface->CreateDevice(adapter, device_type,
                focus_window, behavior_flags, presentation_parameters, out_returned_device_interface);

            if (FAILED(result) || this != gGameDirect || out_returned_device_interface != &gGameDevice)
                return result;

            Logger::Instance().LogToFile("[dbg:render:direct] device interface (ptr:%p) success created with "
                "hwnd (value:%u) windowed (value:%d) screenwidth (value:%u) screenheight (value:%u)",
                *out_returned_device_interface, focus_window, presentation_parameters->Windowed,
                presentation_parameters->BackBufferWidth, presentation_parameters->BackBufferHeight);

            const auto hook_device = new (std::nothrow) IDirect3DDevice9Hook { *out_returned_device_interface };
            if (hook_device == nullptr)
            {
                Logger::Instance().LogToFile("[err:render:direct] failed to "
                    "allocate memory for hook interface");
                return result;
            }

            Logger::Instance().LogToFile("[dbg:render:direct] pointer successfully replaced "
                "from orig (ptr:%p) to hook (ptr:%p)", *out_returned_device_interface, hook_device);

            if (Instance()._direct_interface != nullptr)
            {
                if (Instance().OnDeviceDestroy != nullptr)
                    Instance().OnDeviceDestroy();
            }

            Instance()._device_lock.Lock();
            Instance()._direct_interface = _orig_interface;
            Instance()._device_interface = *out_returned_device_interface;
            Instance()._device_parameters = *presentation_parameters;
            Instance()._device_lock.Unlock();

            if (Instance().OnDeviceInitialize != nullptr)
                Instance().OnDeviceInitialize(Instance()._direct_interface,
                    Instance()._device_interface, Instance()._device_parameters);

            *out_returned_device_interface = hook_device;

            return result;
        }

    private:

        IDirect3D9* const _orig_interface;

    };

private:

    static IDirect3D9* CALLBACK Direct3DCreate9Hook(const UINT sdk_version) noexcept
    {
        Instance()._direct3d_create_hook.Disable();
        IDirect3D9* orig_direct = GameDirect3DCreate9(sdk_version);
        Instance()._direct3d_create_hook.Enable();

        if (orig_direct != nullptr)
        {
            Logger::Instance().LogToFile("[dbg:render:hookdirect3dcreate9] intercepted instance "
                "(ptr:%p) of IDirect3D9", orig_direct);

            if (const auto hook_direct = new (std::nothrow) IDirect3D9Hook { orig_direct };
                           hook_direct != nullptr)
            {
                Logger::Instance().LogToFile("[dbg:render:hookdirect3dcreate9] pointer successfully "
                    "replaced from orig (ptr:%p) to hook (ptr:%p)", orig_direct, hook_direct);

                orig_direct = hook_direct;
            }
        }

        return orig_direct;
    }

private:

    mutable Spinlock      _device_lock;
    IDirect3D9*           _direct_interface = nullptr;
    IDirect3DDevice9*     _device_interface = nullptr;
    D3DPRESENT_PARAMETERS _device_parameters;

public:

    std::function<void(IDirect3D9*, IDirect3DDevice9*, const D3DPRESENT_PARAMETERS&)>
                          OnDeviceInitialize;
    std::function<void()> OnBeforeBeginScene;
    std::function<void()> OnAfterBeginScene;
    std::function<void()> OnBeforeEndScene;
    std::function<void()> OnAfterEndScene;
    std::function<void()> OnPresent;
    std::function<void()> OnBeforeReset;
    std::function<void(IDirect3DDevice9*, const D3DPRESENT_PARAMETERS&)>
                          OnAfterReset;
    std::function<void()> OnDeviceDestroy;

private:

    JumpHook _direct3d_create_hook;

};
