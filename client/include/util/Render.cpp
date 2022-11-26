/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Render.h"

#include "Logger.h"

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

bool Render::Init() noexcept
{
    if (_init_status) return false;

    Logger::LogToFile("[dbg:render:init] : module initializing...");

    _hook_direct3d_create9 = Memory::JumpHook((LPVOID)(GameDirect3DCreate9), &HookDirect3DCreate9);

    _device_init_callback = nullptr;
    _before_reset_callback = nullptr;
    _begin_scene_callback = nullptr;
    _render_callback = nullptr;
    _end_scene_callback = nullptr;
    _after_reset_callback = nullptr;
    _device_free_callback = nullptr;

    _device_mutex.lock();
    _direct_interface = nullptr;
    _device_interface = nullptr;
    _device_parameters = {};
    _device_mutex.unlock();

    Logger::LogToFile("[dbg:render:init] : module initialized");

    _init_status = true;

    return true;
}

void Render::Free() noexcept
{
    if (_init_status)
    {
        Logger::LogToFile("[dbg:render:free] : module releasing...");

        _hook_direct3d_create9 = {};

        _device_init_callback = nullptr;
        _before_reset_callback = nullptr;
        _begin_scene_callback = nullptr;
        _render_callback = nullptr;
        _end_scene_callback = nullptr;
        _after_reset_callback = nullptr;
        _device_free_callback = nullptr;

        _device_mutex.lock();
        _direct_interface = nullptr;
        _device_interface = nullptr;
        _device_mutex.unlock();

        Logger::LogToFile("[dbg:render:free] : module released");

        _init_status = false;
    }
}

bool Render::GetWindowHandle(HWND& window_handle) noexcept
{
    const std::lock_guard lock { _device_mutex };

    if (_device_interface == nullptr) return false;

    window_handle = _device_parameters.hDeviceWindow;

    return true;
}

bool Render::GetScreenSize(float& screen_width, float& screen_height) noexcept
{
    const std::lock_guard lock { _device_mutex };

    if (_device_interface == nullptr) return false;

    screen_width  = _device_parameters.BackBufferWidth;
    screen_height = _device_parameters.BackBufferHeight;

    return true;
}

bool Render::ConvertBaseXValueToScreenXValue(const float base_value, float& screen_value) noexcept
{
    const std::lock_guard lock { _device_mutex };

    if (_device_interface == nullptr) return false;

    const float screen_width = _device_parameters.BackBufferWidth;
    screen_value = (screen_width / kBaseWidth) * base_value;

    return true;
}

bool Render::ConvertBaseYValueToScreenYValue(const float base_value, float& screen_value) noexcept
{
    const std::lock_guard lock { _device_mutex };

    if (_device_interface == nullptr) return false;

    const float screen_height = _device_parameters.BackBufferHeight;
    screen_value = (screen_height / kBaseHeight) * base_value;

    return true;
}

bool Render::ConvertScreenXValueToBaseXValue(const float screen_value, float& base_value) noexcept
{
    const std::lock_guard lock { _device_mutex };

    if (_device_interface == nullptr) return false;

    const float screen_width = _device_parameters.BackBufferWidth;
    base_value = (kBaseWidth / screen_width) * screen_value;

    return true;
}

bool Render::ConvertScreenYValueToBaseYValue(const float screen_value, float& base_value) noexcept
{
    const std::lock_guard lock { _device_mutex };

    if (_device_interface == nullptr) return false;

    const float screen_height = _device_parameters.BackBufferHeight;
    base_value = (kBaseHeight / screen_height) * screen_value;

    return true;
}

void Render::SetDeviceInitCallback(DeviceInitCallback&& callback) noexcept
{
    if (_init_status) _device_init_callback = std::move(callback);
}

void Render::SetBeforeResetCallback(BeforeResetCallback&& callback) noexcept
{
    if (_init_status) _before_reset_callback = std::move(callback);
}

void Render::SetBeginSceneCallback(BeginSceneCallback&& callback) noexcept
{
    if (_init_status) _begin_scene_callback = std::move(callback);
}

void Render::SetRenderCallback(RenderCallback&& callback) noexcept
{
    if (_init_status) _render_callback = std::move(callback);
}

void Render::SetEndSceneCallback(EndSceneCallback&& callback) noexcept
{
    if (_init_status) _end_scene_callback = std::move(callback);
}

void Render::SetAfterResetCallback(AfterResetCallback&& callback) noexcept
{
    if (_init_status) _after_reset_callback = std::move(callback);
}

void Render::SetDeviceFreeCallback(DeviceFreeCallback&& callback) noexcept
{
    if (_init_status) _device_free_callback = std::move(callback);
}

// --------------------------------------------------------------------------------------------------

Render::IDirect3DDevice9Hook::IDirect3DDevice9Hook(IDirect3DDevice9* const orig_interface) noexcept
    : _orig_interface { orig_interface }
{
    _orig_interface->AddRef();
}

Render::IDirect3DDevice9Hook::~IDirect3DDevice9Hook() noexcept
{
    _orig_interface->Release();
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::Present(CONST RECT* const source_rect,
    CONST RECT* const dest_rect, const HWND dest_window_override,
    CONST RGNDATA* const dirty_region) noexcept
{
    if (_orig_interface == _device_interface && !_reset_status)
    {
        if (_render_callback != nullptr) _render_callback();
    }

    return _orig_interface->Present(source_rect, dest_rect, dest_window_override, dirty_region);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::QueryInterface(const REFIID riid, VOID** const ppv_obj) noexcept
{
    *ppv_obj = nullptr;
    const auto result = _orig_interface->QueryInterface(riid, ppv_obj);
    if (SUCCEEDED(result)) *ppv_obj = this;
    return result;
}

ULONG __stdcall Render::IDirect3DDevice9Hook::AddRef() noexcept
{
    return _orig_interface->AddRef();
}

ULONG __stdcall Render::IDirect3DDevice9Hook::Release() noexcept
{
    const auto count = _orig_interface->Release();
    if (count <= 1)
    {
        if (_orig_interface == _device_interface)
        {
            if (_device_free_callback != nullptr) _device_free_callback();

            _device_mutex.lock();
            _direct_interface = nullptr;
            _device_interface = nullptr;
            _device_parameters = {};
            _device_mutex.unlock();
        }

        delete this;
        return 0;
    }

    return count;
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::TestCooperativeLevel() noexcept
{
    return _orig_interface->TestCooperativeLevel();
}

UINT __stdcall Render::IDirect3DDevice9Hook::GetAvailableTextureMem() noexcept
{
    return _orig_interface->GetAvailableTextureMem();
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::EvictManagedResources() noexcept
{
    return _orig_interface->EvictManagedResources();
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetDirect3D(IDirect3D9** const d3d9) noexcept
{
    return _orig_interface->GetDirect3D(d3d9);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetDeviceCaps(D3DCAPS9* const caps) noexcept
{
    return _orig_interface->GetDeviceCaps(caps);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetDisplayMode(const UINT swapchain_index, D3DDISPLAYMODE* const display_mode) noexcept
{
    return _orig_interface->GetDisplayMode(swapchain_index, display_mode);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* const parameters) noexcept
{
    return _orig_interface->GetCreationParameters(parameters);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetCursorProperties(const UINT x_hot_spot,
    const UINT y_hot_spot, IDirect3DSurface9* const cursor_bitmap) noexcept
{
    return _orig_interface->SetCursorProperties(x_hot_spot, y_hot_spot, cursor_bitmap);
}

VOID __stdcall Render::IDirect3DDevice9Hook::SetCursorPosition(const INT x, const INT y, const DWORD flags) noexcept
{
    _orig_interface->SetCursorPosition(x, y, flags);
}

BOOL __stdcall Render::IDirect3DDevice9Hook::ShowCursor(const BOOL show) noexcept
{
    return _orig_interface->ShowCursor(show);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* const presentation_parameters,
    IDirect3DSwapChain9** const swapchain) noexcept
{
    return _orig_interface->CreateAdditionalSwapChain(presentation_parameters, swapchain);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetSwapChain(const UINT swapchain_index,
    IDirect3DSwapChain9** const swapchain) noexcept
{
    return _orig_interface->GetSwapChain(swapchain_index, swapchain);
}

UINT __stdcall Render::IDirect3DDevice9Hook::GetNumberOfSwapChains() noexcept
{
    return _orig_interface->GetNumberOfSwapChains();
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::Reset(D3DPRESENT_PARAMETERS* const presentation_parameters) noexcept
{
    if (_orig_interface == _device_interface && !_reset_status)
    {
        if (_before_reset_callback != nullptr) _before_reset_callback();
    }

    _reset_status = true;

    const auto result = _orig_interface->Reset(presentation_parameters);

    if (_orig_interface == _device_interface && SUCCEEDED(result))
    {
        _device_mutex.lock();
        _device_parameters = *presentation_parameters;
        _device_mutex.unlock();

        if (_reset_status)
        {
            if (_after_reset_callback != nullptr)
                _after_reset_callback(_device_interface, _device_parameters);
        }

        _reset_status = false;
    }

    return result;
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetBackBuffer(const UINT swapchain_index, const UINT backbuffer_index,
    const D3DBACKBUFFER_TYPE type, IDirect3DSurface9** const out_backbuffer) noexcept
{
    return _orig_interface->GetBackBuffer(swapchain_index, backbuffer_index, type, out_backbuffer);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetRasterStatus(const UINT swapchain_index, D3DRASTER_STATUS* const raster_status) noexcept
{
    return _orig_interface->GetRasterStatus(swapchain_index, raster_status);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetDialogBoxMode(const BOOL enable_dialogs) noexcept
{
    return _orig_interface->SetDialogBoxMode(enable_dialogs);
}

VOID __stdcall Render::IDirect3DDevice9Hook::SetGammaRamp(const UINT swapchain_index, const DWORD flags, CONST D3DGAMMARAMP* const ramp) noexcept
{
    _orig_interface->SetGammaRamp(swapchain_index, flags, ramp);
}

VOID __stdcall Render::IDirect3DDevice9Hook::GetGammaRamp(const UINT swapchain_index, D3DGAMMARAMP* const ramp) noexcept
{
    _orig_interface->GetGammaRamp(swapchain_index, ramp);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateTexture(const UINT width, const UINT height,
    const UINT levels, const DWORD usage, const D3DFORMAT format, const D3DPOOL pool,
    IDirect3DTexture9** const out_texture, HANDLE* const shared_handle) noexcept
{
    return _orig_interface->CreateTexture(width, height, levels, usage, format, pool, out_texture, shared_handle);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateVolumeTexture(const UINT width, const UINT height,
    const UINT Depth, const UINT levels, const DWORD usage, const D3DFORMAT format, const D3DPOOL pool,
    IDirect3DVolumeTexture9** const out_volume_texture, HANDLE* const shared_handle) noexcept
{
    return _orig_interface->CreateVolumeTexture(width, height, Depth, levels, usage, format, pool, out_volume_texture, shared_handle);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateCubeTexture(const UINT edge_length, const UINT levels, const DWORD usage,
    const D3DFORMAT format, const D3DPOOL pool, IDirect3DCubeTexture9** const out_cube_texture, HANDLE* const shared_handle) noexcept
{
    return _orig_interface->CreateCubeTexture(edge_length, levels, usage, format, pool, out_cube_texture, shared_handle);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateVertexBuffer(const UINT length, const DWORD usage, const DWORD fvf,
    const D3DPOOL pool, IDirect3DVertexBuffer9** const out_vertex_buffer, HANDLE* const shared_handle) noexcept
{
    return _orig_interface->CreateVertexBuffer(length, usage, fvf, pool, out_vertex_buffer, shared_handle);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateIndexBuffer(const UINT length, const DWORD usage, const D3DFORMAT format,
    const D3DPOOL pool, IDirect3DIndexBuffer9** const out_index_buffer, HANDLE* const shared_handle) noexcept
{
    return _orig_interface->CreateIndexBuffer(length, usage, format, pool, out_index_buffer, shared_handle);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateRenderTarget(const UINT width, const UINT height,
    const D3DFORMAT format, const D3DMULTISAMPLE_TYPE multisample, const DWORD multisample_quality, const BOOL lockable,
    IDirect3DSurface9** const out_surface, HANDLE* const shared_handle) noexcept
{
    return _orig_interface->CreateRenderTarget(width, height, format, multisample,
        multisample_quality, lockable, out_surface, shared_handle);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateDepthStencilSurface(const UINT width, const UINT height,
    const D3DFORMAT format, const D3DMULTISAMPLE_TYPE multisample, const DWORD multisample_quality,
    const BOOL discard, IDirect3DSurface9** const out_surface, HANDLE* const shared_handle) noexcept
{
    return _orig_interface->CreateDepthStencilSurface(width, height, format, multisample,
        multisample_quality, discard, out_surface, shared_handle);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::UpdateSurface(IDirect3DSurface9* const source_surface,
    CONST RECT* const source_rect, IDirect3DSurface9* const destination_surface, CONST POINT* const dest_point) noexcept
{
    return _orig_interface->UpdateSurface(source_surface, source_rect, destination_surface, dest_point);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::UpdateTexture(IDirect3DBaseTexture9* const source_texture,
    IDirect3DBaseTexture9* const destination_texture) noexcept
{
    return _orig_interface->UpdateTexture(source_texture, destination_texture);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetRenderTargetData(IDirect3DSurface9* const render_target,
    IDirect3DSurface9* const dest_surface) noexcept
{
    return _orig_interface->GetRenderTargetData(render_target, dest_surface);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetFrontBufferData(const UINT swapchain_index,
    IDirect3DSurface9* const dest_surface) noexcept
{
    return _orig_interface->GetFrontBufferData(swapchain_index, dest_surface);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::StretchRect(IDirect3DSurface9* const source_surface,
    CONST RECT* const source_rect, IDirect3DSurface9* const dest_surface,
    CONST RECT* const dest_rect, const D3DTEXTUREFILTERTYPE filter) noexcept
{
    return _orig_interface->StretchRect(source_surface, source_rect, dest_surface, dest_rect, filter);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::ColorFill(IDirect3DSurface9* const surface,
    CONST RECT* const rect, const D3DCOLOR color) noexcept
{
    return _orig_interface->ColorFill(surface, rect, color);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateOffscreenPlainSurface(const UINT width,
    const UINT height, const D3DFORMAT format, const D3DPOOL pool,
    IDirect3DSurface9** const out_surface, HANDLE* const shared_handle) noexcept
{
    return _orig_interface->CreateOffscreenPlainSurface(width, height, format, pool, out_surface, shared_handle);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetRenderTarget(const DWORD render_target_index,
    IDirect3DSurface9* const render_target) noexcept
{
    return _orig_interface->SetRenderTarget(render_target_index, render_target);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetRenderTarget(const DWORD render_target_index,
    IDirect3DSurface9** const out_render_target) noexcept
{
    return _orig_interface->GetRenderTarget(render_target_index, out_render_target);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetDepthStencilSurface(IDirect3DSurface9* const new_z_stencil) noexcept
{
    return _orig_interface->SetDepthStencilSurface(new_z_stencil);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetDepthStencilSurface(IDirect3DSurface9** const out_z_stencil_surface) noexcept
{
    return _orig_interface->GetDepthStencilSurface(out_z_stencil_surface);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::BeginScene() noexcept
{
    const auto result = _orig_interface->BeginScene();

    if (_orig_interface == _device_interface && SUCCEEDED(result) && !_reset_status)
    {
        if (!_scene_status)
        {
            if (_begin_scene_callback != nullptr) _begin_scene_callback();
        }

        _scene_status = true;
    }

    return result;
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::EndScene() noexcept
{
    if (_orig_interface == _device_interface && !_reset_status)
    {
        if (_scene_status)
        {
            if (_end_scene_callback != nullptr) _end_scene_callback();
        }

        _scene_status = false;
    }

    return _orig_interface->EndScene();
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::Clear(const DWORD count, CONST D3DRECT* const rects,
    const DWORD flags, const D3DCOLOR color, const FLOAT z, const DWORD stencil) noexcept
{
    return _orig_interface->Clear(count, rects, flags, color, z, stencil);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetTransform(const D3DTRANSFORMSTATETYPE state, CONST D3DMATRIX* const mat) noexcept
{
    return _orig_interface->SetTransform(state, mat);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetTransform(const D3DTRANSFORMSTATETYPE state, D3DMATRIX* const mat) noexcept
{
    return _orig_interface->GetTransform(state, mat);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::MultiplyTransform(const D3DTRANSFORMSTATETYPE state, CONST D3DMATRIX* const mat) noexcept
{
    return _orig_interface->MultiplyTransform(state, mat);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetViewport(CONST D3DVIEWPORT9* const viewport) noexcept
{
    return _orig_interface->SetViewport(viewport);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetViewport(D3DVIEWPORT9* const viewport) noexcept
{
    return _orig_interface->GetViewport(viewport);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetMaterial(CONST D3DMATERIAL9* const material) noexcept
{
    return _orig_interface->SetMaterial(material);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetMaterial(D3DMATERIAL9* const material) noexcept
{
    return _orig_interface->GetMaterial(material);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetLight(const DWORD index, CONST D3DLIGHT9* const light) noexcept
{
    return _orig_interface->SetLight(index, light);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetLight(const DWORD index, D3DLIGHT9* const light) noexcept
{
    return _orig_interface->GetLight(index, light);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::LightEnable(const DWORD index, const BOOL enable) noexcept
{
    return _orig_interface->LightEnable(index, enable);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetLightEnable(const DWORD index, BOOL* const out_enable) noexcept
{
    return _orig_interface->GetLightEnable(index, out_enable);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetClipPlane(const DWORD index, CONST FLOAT* const plane) noexcept
{
    return _orig_interface->SetClipPlane(index, plane);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetClipPlane(const DWORD index, FLOAT* const plane) noexcept
{
    return _orig_interface->GetClipPlane(index, plane);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetRenderState(const D3DRENDERSTATETYPE state, const DWORD value) noexcept
{
    return _orig_interface->SetRenderState(state, value);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetRenderState(const D3DRENDERSTATETYPE state, DWORD* const out_value) noexcept
{
    return _orig_interface->GetRenderState(state, out_value);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateStateBlock(const D3DSTATEBLOCKTYPE type, IDirect3DStateBlock9** const out_sb) noexcept
{
    return _orig_interface->CreateStateBlock(type, out_sb);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::BeginStateBlock() noexcept
{
    return _orig_interface->BeginStateBlock();
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::EndStateBlock(IDirect3DStateBlock9** const sb_ptr) noexcept
{
    return _orig_interface->EndStateBlock(sb_ptr);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetClipStatus(CONST D3DCLIPSTATUS9* const clip_status) noexcept
{
    return _orig_interface->SetClipStatus(clip_status);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetClipStatus(D3DCLIPSTATUS9* const clip_status) noexcept
{
    return _orig_interface->GetClipStatus(clip_status);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetTexture(const DWORD stage, IDirect3DBaseTexture9** const out_texture) noexcept
{
    return _orig_interface->GetTexture(stage, out_texture);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetTexture(const DWORD stage, IDirect3DBaseTexture9* const texture) noexcept
{
    return _orig_interface->SetTexture(stage, texture);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetTextureStageState(const DWORD stage,
    const D3DTEXTURESTAGESTATETYPE type, DWORD* const out_value) noexcept
{
    return _orig_interface->GetTextureStageState(stage, type, out_value);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetTextureStageState(const DWORD stage,
    const D3DTEXTURESTAGESTATETYPE type, const DWORD value) noexcept
{
    return _orig_interface->SetTextureStageState(stage, type, value);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetSamplerState(const DWORD sampler,
    const D3DSAMPLERSTATETYPE type, DWORD* const out_value) noexcept
{
    return _orig_interface->GetSamplerState(sampler, type, out_value);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetSamplerState(const DWORD sampler,
    const D3DSAMPLERSTATETYPE type, const DWORD value) noexcept
{
    return _orig_interface->SetSamplerState(sampler, type, value);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::ValidateDevice(DWORD* const num_passes) noexcept
{
    return _orig_interface->ValidateDevice(num_passes);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetPaletteEntries(const UINT palette_number,
    CONST PALETTEENTRY* const entries) noexcept
{
    return _orig_interface->SetPaletteEntries(palette_number, entries);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetPaletteEntries(const UINT palette_number,
    PALETTEENTRY* const entries) noexcept
{
    return _orig_interface->GetPaletteEntries(palette_number, entries);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetCurrentTexturePalette(const UINT palette_number) noexcept
{
    return _orig_interface->SetCurrentTexturePalette(palette_number);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetCurrentTexturePalette(UINT* const palette_number) noexcept
{
    return _orig_interface->GetCurrentTexturePalette(palette_number);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetScissorRect(CONST RECT* const rect) noexcept
{
    return _orig_interface->SetScissorRect(rect);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetScissorRect(RECT* const rect) noexcept
{
    return _orig_interface->GetScissorRect(rect);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetSoftwareVertexProcessing(const BOOL software) noexcept
{
    return _orig_interface->SetSoftwareVertexProcessing(software);
}

BOOL __stdcall Render::IDirect3DDevice9Hook::GetSoftwareVertexProcessing() noexcept
{
    return _orig_interface->GetSoftwareVertexProcessing();
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetNPatchMode(const FLOAT segments) noexcept
{
    return _orig_interface->SetNPatchMode(segments);
}

FLOAT __stdcall Render::IDirect3DDevice9Hook::GetNPatchMode() noexcept
{
    return _orig_interface->GetNPatchMode();
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::DrawPrimitive(const D3DPRIMITIVETYPE primitive_type,
    const UINT start_vertex, const UINT primitive_count) noexcept
{
    return _orig_interface->DrawPrimitive(primitive_type, start_vertex, primitive_count);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::DrawIndexedPrimitive(const D3DPRIMITIVETYPE type,
    const INT base_vertex_index, const UINT min_vertex_index, const UINT num_vertices,
    const UINT start_index, const UINT prim_count) noexcept
{
    return _orig_interface->DrawIndexedPrimitive(type, base_vertex_index, min_vertex_index, num_vertices, start_index, prim_count);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::DrawPrimitiveUP(const D3DPRIMITIVETYPE primitive_type,
    const UINT primitive_count, CONST VOID* const vertex_stream_zero_data, const UINT vertex_stream_zero_stride) noexcept
{
    return _orig_interface->DrawPrimitiveUP(primitive_type, primitive_count, vertex_stream_zero_data, vertex_stream_zero_stride);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::DrawIndexedPrimitiveUP(const D3DPRIMITIVETYPE primitive_type,
    const UINT min_vertex_index, const UINT num_vertices, const UINT primitive_count, CONST VOID* const index_data,
    const D3DFORMAT index_data_format, CONST VOID* const vertex_stream_zero_data, const UINT vertex_stream_zero_stride) noexcept
{
    return _orig_interface->DrawIndexedPrimitiveUP(primitive_type, min_vertex_index, num_vertices,
        primitive_count, index_data, index_data_format, vertex_stream_zero_data, vertex_stream_zero_stride);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::ProcessVertices(const UINT src_start_index,
    const UINT dest_index, const UINT vertex_count, IDirect3DVertexBuffer9* const dest_buffer,
    IDirect3DVertexDeclaration9* const vertex_decl, const DWORD flags) noexcept
{
    return _orig_interface->ProcessVertices(src_start_index, dest_index, vertex_count, dest_buffer, vertex_decl, flags);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* const vertex_elements,
    IDirect3DVertexDeclaration9** const out_decl) noexcept
{
    return _orig_interface->CreateVertexDeclaration(vertex_elements, out_decl);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetVertexDeclaration(IDirect3DVertexDeclaration9* const decl) noexcept
{
    return _orig_interface->SetVertexDeclaration(decl);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetVertexDeclaration(IDirect3DVertexDeclaration9** const out_decl) noexcept
{
    return _orig_interface->GetVertexDeclaration(out_decl);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetFVF(const DWORD fvf) noexcept
{
    return _orig_interface->SetFVF(fvf);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetFVF(DWORD* const out_fvf) noexcept
{
    return _orig_interface->GetFVF(out_fvf);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateVertexShader(CONST DWORD* const function,
    IDirect3DVertexShader9** const out_shader) noexcept
{
    return _orig_interface->CreateVertexShader(function, out_shader);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetVertexShader(IDirect3DVertexShader9* const shader) noexcept
{
    return _orig_interface->SetVertexShader(shader);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetVertexShader(IDirect3DVertexShader9** const out_shader) noexcept
{
    return _orig_interface->GetVertexShader(out_shader);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetVertexShaderConstantF(const UINT start_register,
    CONST FLOAT* const constant_data, const UINT vector4f_count) noexcept
{
    return _orig_interface->SetVertexShaderConstantF(start_register, constant_data, vector4f_count);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetVertexShaderConstantF(const UINT start_register,
    FLOAT* const constant_data, const UINT vector4f_count) noexcept
{
    return _orig_interface->GetVertexShaderConstantF(start_register, constant_data, vector4f_count);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetVertexShaderConstantI(const UINT start_register,
    CONST INT* const constant_data, const UINT vector4i_count) noexcept
{
    return _orig_interface->SetVertexShaderConstantI(start_register, constant_data, vector4i_count);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetVertexShaderConstantI(const UINT start_register,
    INT* const constant_data, const UINT vector4i_count) noexcept
{
    return _orig_interface->GetVertexShaderConstantI(start_register, constant_data, vector4i_count);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetVertexShaderConstantB(const UINT start_register,
    CONST BOOL* const constant_data, const UINT bool_count) noexcept
{
    return _orig_interface->SetVertexShaderConstantB(start_register, constant_data, bool_count);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetVertexShaderConstantB(const UINT start_register,
    BOOL* const constant_data, const UINT bool_count) noexcept
{
    return _orig_interface->GetVertexShaderConstantB(start_register, constant_data, bool_count);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetStreamSource(const UINT stream_number,
    IDirect3DVertexBuffer9* const stream_data, const UINT offset_in_bytes, const UINT stride) noexcept
{
    return _orig_interface->SetStreamSource(stream_number, stream_data, offset_in_bytes, stride);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetStreamSource(const UINT stream_number,
    IDirect3DVertexBuffer9** const out_stream_data, UINT* const offset_in_bytes, UINT* const out_stride) noexcept
{
    return _orig_interface->GetStreamSource(stream_number, out_stream_data, offset_in_bytes, out_stride);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetStreamSourceFreq(const UINT stream_number, const UINT divider) noexcept
{
    return _orig_interface->SetStreamSourceFreq(stream_number, divider);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetStreamSourceFreq(const UINT stream_number, UINT* const divider) noexcept
{
    return _orig_interface->GetStreamSourceFreq(stream_number, divider);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetIndices(IDirect3DIndexBuffer9* const index_data) noexcept
{
    return _orig_interface->SetIndices(index_data);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetIndices(IDirect3DIndexBuffer9** const out_index_data) noexcept
{
    return _orig_interface->GetIndices(out_index_data);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreatePixelShader(CONST DWORD* const function,
    IDirect3DPixelShader9** const out_shader) noexcept
{
    return _orig_interface->CreatePixelShader(function, out_shader);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetPixelShader(IDirect3DPixelShader9* const shader) noexcept
{
    return _orig_interface->SetPixelShader(shader);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetPixelShader(IDirect3DPixelShader9** const out_shader) noexcept
{
    return _orig_interface->GetPixelShader(out_shader);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetPixelShaderConstantF(const UINT start_register,
    CONST FLOAT* const constant_data, const UINT vector4f_count) noexcept
{
    return _orig_interface->SetPixelShaderConstantF(start_register, constant_data, vector4f_count);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetPixelShaderConstantF(const UINT start_register,
    FLOAT* const constant_data, const UINT vector4f_count) noexcept
{
    return _orig_interface->GetPixelShaderConstantF(start_register, constant_data, vector4f_count);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetPixelShaderConstantI(const UINT start_register,
    CONST INT* const constant_data, const UINT vector4i_count) noexcept
{
    return _orig_interface->SetPixelShaderConstantI(start_register, constant_data, vector4i_count);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetPixelShaderConstantI(const UINT start_register,
    INT* const constant_data, const UINT vector4i_count) noexcept
{
    return _orig_interface->GetPixelShaderConstantI(start_register, constant_data, vector4i_count);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetPixelShaderConstantB(const UINT start_register,
    CONST BOOL* const constant_data, const UINT bool_count) noexcept
{
    return _orig_interface->SetPixelShaderConstantB(start_register, constant_data, bool_count);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetPixelShaderConstantB(const UINT start_register,
    BOOL* const constant_data, const UINT bool_count) noexcept
{
    return _orig_interface->GetPixelShaderConstantB(start_register, constant_data, bool_count);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::DrawRectPatch(const UINT handle, CONST FLOAT* const num_segs,
    CONST D3DRECTPATCH_INFO* const rect_patch_info) noexcept
{
    return _orig_interface->DrawRectPatch(handle, num_segs, rect_patch_info);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::DrawTriPatch(const UINT handle, CONST FLOAT* const num_segs,
    CONST D3DTRIPATCH_INFO* const tri_patch_info) noexcept
{
    return _orig_interface->DrawTriPatch(handle, num_segs, tri_patch_info);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::DeletePatch(const UINT handle) noexcept
{
    return _orig_interface->DeletePatch(handle);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateQuery(const D3DQUERYTYPE type, IDirect3DQuery9** const out_query) noexcept
{
    return _orig_interface->CreateQuery(type, out_query);
}

// --------------------------------------------------------------------------------------------------

Render::IDirect3D9Hook::IDirect3D9Hook(IDirect3D9* const orig_interface) noexcept
    : _orig_interface { orig_interface }
{
    _orig_interface->AddRef();
}

Render::IDirect3D9Hook::~IDirect3D9Hook() noexcept
{
    _orig_interface->Release();
}

HRESULT __stdcall Render::IDirect3D9Hook::QueryInterface(const REFIID riid, VOID** const ppv_obj) noexcept
{
    *ppv_obj = nullptr;
    const auto result = _orig_interface->QueryInterface(riid, ppv_obj);
    if (SUCCEEDED(result)) *ppv_obj = this;
    return result;
}

ULONG __stdcall Render::IDirect3D9Hook::AddRef() noexcept
{
    return _orig_interface->AddRef();
}

ULONG __stdcall Render::IDirect3D9Hook::Release() noexcept
{
    const auto count = _orig_interface->Release();
    if (count <= 1)
    {
        delete this;
        return 0;
    }

    return count;
}

HRESULT __stdcall Render::IDirect3D9Hook::RegisterSoftwareDevice(VOID* const initialize_function) noexcept
{
    return _orig_interface->RegisterSoftwareDevice(initialize_function);
}

UINT __stdcall Render::IDirect3D9Hook::GetAdapterCount() noexcept
{
    return _orig_interface->GetAdapterCount();
}

HRESULT __stdcall Render::IDirect3D9Hook::GetAdapterIdentifier(const UINT adapter, const DWORD flags,
    D3DADAPTER_IDENTIFIER9* const identifier) noexcept
{
    return _orig_interface->GetAdapterIdentifier(adapter, flags, identifier);
}

UINT __stdcall Render::IDirect3D9Hook::GetAdapterModeCount(const UINT adapter, const D3DFORMAT format) noexcept
{
    return _orig_interface->GetAdapterModeCount(adapter, format);
}

HRESULT __stdcall Render::IDirect3D9Hook::EnumAdapterModes(const UINT adapter, const D3DFORMAT format,
    const UINT mode, D3DDISPLAYMODE* const display_mode) noexcept
{
    return _orig_interface->EnumAdapterModes(adapter, format, mode, display_mode);
}

HRESULT __stdcall Render::IDirect3D9Hook::GetAdapterDisplayMode(const UINT adapter, D3DDISPLAYMODE* const display_mode) noexcept
{
    return _orig_interface->GetAdapterDisplayMode(adapter, display_mode);
}

HRESULT __stdcall Render::IDirect3D9Hook::CheckDeviceType(const UINT adapter, const D3DDEVTYPE dev_type,
    const D3DFORMAT adapter_format, const D3DFORMAT backbuffer_format, const BOOL windowed) noexcept
{
    return _orig_interface->CheckDeviceType(adapter, dev_type, adapter_format, backbuffer_format, windowed);
}

HRESULT __stdcall Render::IDirect3D9Hook::CheckDeviceFormat(const UINT adapter, const D3DDEVTYPE device_type,
    const D3DFORMAT adapter_format, const DWORD usage, const D3DRESOURCETYPE resource_type,
    D3DFORMAT check_format) noexcept
{
    return _orig_interface->CheckDeviceFormat(adapter, device_type, adapter_format, usage, resource_type, check_format);
}

HRESULT __stdcall Render::IDirect3D9Hook::CheckDeviceMultiSampleType(const UINT adapter, const D3DDEVTYPE device_type,
    const D3DFORMAT surface_format, const BOOL windowed, const D3DMULTISAMPLE_TYPE multisample_type,
    DWORD* const quality_levels) noexcept
{
    return _orig_interface->CheckDeviceMultiSampleType(adapter, device_type,
        surface_format, windowed, multisample_type, quality_levels);
}

HRESULT __stdcall Render::IDirect3D9Hook::CheckDepthStencilMatch(const UINT adapter, const D3DDEVTYPE device_type,
    const D3DFORMAT adapter_format, const D3DFORMAT render_target_format, const D3DFORMAT depth_stencil_format) noexcept
{
    return _orig_interface->CheckDepthStencilMatch(adapter, device_type,
        adapter_format, render_target_format, depth_stencil_format);
}

HRESULT __stdcall Render::IDirect3D9Hook::CheckDeviceFormatConversion(const UINT adapter, const D3DDEVTYPE device_type,
    const D3DFORMAT source_format, const D3DFORMAT target_format) noexcept
{
    return _orig_interface->CheckDeviceFormatConversion(adapter, device_type, source_format, target_format);
}

HRESULT __stdcall Render::IDirect3D9Hook::GetDeviceCaps(const UINT adapter,
    const D3DDEVTYPE device_type, D3DCAPS9* const caps) noexcept
{
    return _orig_interface->GetDeviceCaps(adapter, device_type, caps);
}

HMONITOR __stdcall Render::IDirect3D9Hook::GetAdapterMonitor(const UINT adapter) noexcept
{
    return _orig_interface->GetAdapterMonitor(adapter);
}

HRESULT __stdcall Render::IDirect3D9Hook::CreateDevice(const UINT adapter, const D3DDEVTYPE device_type, const HWND focus_window,
    const DWORD behavior_flags, D3DPRESENT_PARAMETERS* const presentation_parameters,
    IDirect3DDevice9** const out_returned_device_interface) noexcept
{
    const auto result = _orig_interface->CreateDevice(adapter, device_type,
        focus_window, behavior_flags, presentation_parameters, out_returned_device_interface);

    if (FAILED(result) || this != gGameDirect || out_returned_device_interface != &gGameDevice)
        return result;

    Logger::LogToFile("[dbg:render:direct] : device interface (ptr:%p) success created with "
        "hwnd (value:%u) windowed (value:%d) screenwidth (value:%u) screenheight (value:%u)",
        *out_returned_device_interface, focus_window, presentation_parameters->windowed,
        presentation_parameters->BackBufferWidth, presentation_parameters->BackBufferHeight);

    const auto hook_device = new (std::nothrow) IDirect3DDevice9Hook(*out_returned_device_interface);
    if (hook_device == nullptr)
    {
        Logger::LogToFile("[err:render:direct] : failed to allocate memory for hook interface");
        return result;
    }

    Logger::LogToFile("[dbg:render:direct] : pointer successfully replaced from orig (ptr:%p) to hook (ptr:%p)",
        *out_returned_device_interface, hook_device);

    if (_direct_interface != nullptr)
    {
        if (_device_free_callback != nullptr) _device_free_callback();
    }

    _device_mutex.lock();
    _direct_interface = _orig_interface;
    _device_interface = *out_returned_device_interface;
    _device_parameters = *presentation_parameters;
    _device_mutex.unlock();

    if (_device_init_callback != nullptr)
        _device_init_callback(_direct_interface, _device_interface, _device_parameters);

    *out_returned_device_interface = hook_device;

    return result;
}

// --------------------------------------------------------------------------------------------------

IDirect3D9* CALLBACK Render::HookDirect3DCreate9(const UINT sdk_version) noexcept
{
    _hook_direct3d_create9->Disable();
    auto orig_direct = GameDirect3DCreate9(sdk_version);
    _hook_direct3d_create9->Enable();

    if (orig_direct != nullptr)
    {
        Logger::LogToFile("[dbg:render:hookdirect3dcreate9] : intercepted instance "
            "(ptr:%p) of IDirect3D9", orig_direct);

        if (const auto hook_direct = new (std::nothrow) IDirect3D9Hook(orig_direct); hook_direct != nullptr)
        {
            Logger::LogToFile("[dbg:render:hookdirect3dcreate9] : pointer successfully "
                "replaced from orig (ptr:%p) to hook (ptr:%p)", orig_direct, hook_direct);

            orig_direct = hook_direct;
        }
    }

    return orig_direct;
}

bool Render::_init_status = false;

std::mutex            Render::_device_mutex;
IDirect3D9*           Render::_direct_interface = nullptr;
IDirect3DDevice9*     Render::_device_interface = nullptr;
D3DPRESENT_PARAMETERS Render::_device_parameters {};

Render::DeviceInitCallback  Render::_device_init_callback;
Render::BeforeResetCallback Render::_before_reset_callback;
Render::BeginSceneCallback  Render::_begin_scene_callback;
Render::RenderCallback      Render::_render_callback;
Render::EndSceneCallback    Render::_end_scene_callback;
Render::AfterResetCallback  Render::_after_reset_callback;
Render::DeviceFreeCallback  Render::_device_free_callback;

Memory::JumpHook Render::_hook_direct3d_create9;
