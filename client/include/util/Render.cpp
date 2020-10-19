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

namespace
{
    volatile auto& pGameDirect = *reinterpret_cast<IDirect3D9**>(0xC97C20);
    volatile auto& pGameDevice = *reinterpret_cast<IDirect3DDevice9**>(0xC97C28);
    volatile auto& pGameParameters = *reinterpret_cast<D3DPRESENT_PARAMETERS*>(0xC9C040);
    const auto GameDirect3DCreate9 = reinterpret_cast<IDirect3D9*(CALLBACK*)(UINT)>(0x807C2B);
}

bool Render::Init() noexcept
{
    if (Render::initStatus) return false;

    Logger::LogToFile("[dbg:render:init] : module initializing...");

    try
    {
        Render::hookDirect3DCreate9 = MakeJumpHook(GameDirect3DCreate9, Render::HookDirect3DCreate9);
    }
    catch (const std::exception& exception)
    {
        Logger::LogToFile("[err:render:init] : failed to create function hooks");
        Render::hookDirect3DCreate9.reset();
        return false;
    }

    Render::deviceInitCallbacks.clear();
    Render::beforeResetCallbacks.clear();
    Render::beginSceneCallbacks.clear();
    Render::renderCallbacks.clear();
    Render::endSceneCallbacks.clear();
    Render::afterResetCallbacks.clear();
    Render::deviceFreeCallbacks.clear();

    Render::deviceMutex.lock();
    Render::pDirectInterface = nullptr;
    Render::pDeviceInterface = nullptr;
    Render::deviceParameters = {};
    Render::deviceMutex.unlock();

    Logger::LogToFile("[dbg:render:init] : module initialized");

    Render::initStatus = true;

    return true;
}

void Render::Free() noexcept
{
    if (!Render::initStatus) return;

    Logger::LogToFile("[dbg:render:free] : module releasing...");

    Render::hookDirect3DCreate9.reset();

    Render::deviceInitCallbacks.clear();
    Render::beforeResetCallbacks.clear();
    Render::beginSceneCallbacks.clear();
    Render::renderCallbacks.clear();
    Render::endSceneCallbacks.clear();
    Render::afterResetCallbacks.clear();
    Render::deviceFreeCallbacks.clear();

    Render::deviceMutex.lock();
    Render::pDirectInterface = nullptr;
    Render::pDeviceInterface = nullptr;
    Render::deviceMutex.unlock();

    Logger::LogToFile("[dbg:render:free] : module released");

    Render::initStatus = false;
}

bool Render::GetWindowHandle(HWND& windowHandle) noexcept
{
    const std::scoped_lock lock { Render::deviceMutex };

    if (Render::pDeviceInterface == nullptr)
        return false;

    windowHandle = Render::deviceParameters.hDeviceWindow;

    return true;
}

bool Render::GetScreenSize(float& screenWidth, float& screenHeight) noexcept
{
    const std::scoped_lock lock { Render::deviceMutex };

    if (Render::pDeviceInterface == nullptr)
        return false;

    screenWidth = Render::deviceParameters.BackBufferWidth;
    screenHeight = Render::deviceParameters.BackBufferHeight;

    return true;
}

bool Render::ConvertBaseXValueToScreenXValue(const float baseValue, float& screenValue) noexcept
{
    const std::scoped_lock lock { Render::deviceMutex };

    if (Render::pDeviceInterface == nullptr)
        return false;

    const float screenWidth = Render::deviceParameters.BackBufferWidth;
    screenValue = (screenWidth / Render::kBaseWidth) * baseValue;

    return true;
}

bool Render::ConvertBaseYValueToScreenYValue(const float baseValue, float& screenValue) noexcept
{
    const std::scoped_lock lock { Render::deviceMutex };

    if (Render::pDeviceInterface == nullptr)
        return false;

    const float screenHeight = Render::deviceParameters.BackBufferHeight;
    screenValue = (screenHeight / Render::kBaseHeight) * baseValue;

    return true;
}

bool Render::ConvertScreenXValueToBaseXValue(const float screenValue, float& baseValue) noexcept
{
    const std::scoped_lock lock { Render::deviceMutex };

    if (Render::pDeviceInterface == nullptr)
        return false;

    const float screenWidth = Render::deviceParameters.BackBufferWidth;
    baseValue = (Render::kBaseWidth / screenWidth) * screenValue;

    return true;
}

bool Render::ConvertScreenYValueToBaseYValue(const float screenValue, float& baseValue) noexcept
{
    const std::scoped_lock lock { Render::deviceMutex };

    if (Render::pDeviceInterface == nullptr)
        return false;

    const float screenHeight = Render::deviceParameters.BackBufferHeight;
    baseValue = (Render::kBaseHeight / screenHeight) * screenValue;

    return true;
}

std::size_t Render::AddDeviceInitCallback(DeviceInitCallback callback) noexcept
{
    if (!Render::initStatus) return -1;

    for (std::size_t i { 0 }; i < Render::deviceInitCallbacks.size(); ++i)
    {
        if (Render::deviceInitCallbacks[i] == nullptr)
        {
            Render::deviceInitCallbacks[i] = std::move(callback);
            return i;
        }
    }

    Render::deviceInitCallbacks.emplace_back(std::move(callback));
    return Render::deviceInitCallbacks.size() - 1;
}

std::size_t Render::AddBeforeResetCallback(BeforeResetCallback callback) noexcept
{
    if (!Render::initStatus) return -1;

    for (std::size_t i { 0 }; i < Render::beforeResetCallbacks.size(); ++i)
    {
        if (Render::beforeResetCallbacks[i] == nullptr)
        {
            Render::beforeResetCallbacks[i] = std::move(callback);
            return i;
        }
    }

    Render::beforeResetCallbacks.emplace_back(std::move(callback));
    return Render::beforeResetCallbacks.size() - 1;
}

std::size_t Render::AddBeginSceneCallback(BeginSceneCallback callback) noexcept
{
    if (!Render::initStatus) return -1;

    for (std::size_t i { 0 }; i < Render::beginSceneCallbacks.size(); ++i)
    {
        if (Render::beginSceneCallbacks[i] == nullptr)
        {
            Render::beginSceneCallbacks[i] = std::move(callback);
            return i;
        }
    }

    Render::beginSceneCallbacks.emplace_back(std::move(callback));
    return Render::beginSceneCallbacks.size() - 1;
}

std::size_t Render::AddRenderCallback(RenderCallback callback) noexcept
{
    if (!Render::initStatus) return -1;

    for (std::size_t i { 0 }; i < Render::renderCallbacks.size(); ++i)
    {
        if (Render::renderCallbacks[i] == nullptr)
        {
            Render::renderCallbacks[i] = std::move(callback);
            return i;
        }
    }

    Render::renderCallbacks.emplace_back(std::move(callback));
    return Render::renderCallbacks.size() - 1;
}

std::size_t Render::AddEndSceneCallback(EndSceneCallback callback) noexcept
{
    if (!Render::initStatus) return -1;

    for (std::size_t i { 0 }; i < Render::endSceneCallbacks.size(); ++i)
    {
        if (Render::endSceneCallbacks[i] == nullptr)
        {
            Render::endSceneCallbacks[i] = std::move(callback);
            return i;
        }
    }

    Render::endSceneCallbacks.emplace_back(std::move(callback));
    return Render::endSceneCallbacks.size() - 1;
}

std::size_t Render::AddAfterResetCallback(AfterResetCallback callback) noexcept
{
    if (!Render::initStatus) return -1;

    for (std::size_t i { 0 }; i < Render::afterResetCallbacks.size(); ++i)
    {
        if (Render::afterResetCallbacks[i] == nullptr)
        {
            Render::afterResetCallbacks[i] = std::move(callback);
            return i;
        }
    }

    Render::afterResetCallbacks.emplace_back(std::move(callback));
    return Render::afterResetCallbacks.size() - 1;
}

std::size_t Render::AddDeviceFreeCallback(DeviceFreeCallback callback) noexcept
{
    if (!Render::initStatus) return -1;

    for (std::size_t i { 0 }; i < Render::deviceFreeCallbacks.size(); ++i)
    {
        if (Render::deviceFreeCallbacks[i] == nullptr)
        {
            Render::deviceFreeCallbacks[i] = std::move(callback);
            return i;
        }
    }

    Render::deviceFreeCallbacks.emplace_back(std::move(callback));
    return Render::deviceFreeCallbacks.size() - 1;
}

void Render::RemoveDeviceInitCallback(const std::size_t callback) noexcept
{
    if (!Render::initStatus) return;

    if (callback >= Render::deviceInitCallbacks.size())
        return;

    Render::deviceInitCallbacks[callback] = nullptr;
}

void Render::RemoveBeforeResetCallback(const std::size_t callback) noexcept
{
    if (!Render::initStatus) return;

    if (callback >= Render::beforeResetCallbacks.size())
        return;

    Render::beforeResetCallbacks[callback] = nullptr;
}

void Render::RemoveBeginSceneCallback(const std::size_t callback) noexcept
{
    if (!Render::initStatus) return;

    if (callback >= Render::beginSceneCallbacks.size())
        return;

    Render::beginSceneCallbacks[callback] = nullptr;
}

void Render::RemoveRenderCallback(const std::size_t callback) noexcept
{
    if (!Render::initStatus) return;

    if (callback >= Render::renderCallbacks.size())
        return;

    Render::renderCallbacks[callback] = nullptr;
}

void Render::RemoveEndSceneCallback(const std::size_t callback) noexcept
{
    if (!Render::initStatus) return;

    if (callback >= Render::endSceneCallbacks.size())
        return;

    Render::endSceneCallbacks[callback] = nullptr;
}

void Render::RemoveAfterResetCallback(const std::size_t callback) noexcept
{
    if (!Render::initStatus) return;

    if (callback >= Render::afterResetCallbacks.size())
        return;

    Render::afterResetCallbacks[callback] = nullptr;
}

void Render::RemoveDeviceFreeCallback(const std::size_t callback) noexcept
{
    if (!Render::initStatus) return;

    if (callback >= Render::deviceFreeCallbacks.size())
        return;

    Render::deviceFreeCallbacks[callback] = nullptr;
}

// --------------------------------------------------------------------------------------------------

Render::IDirect3DDevice9Hook::IDirect3DDevice9Hook(IDirect3DDevice9* const pOrigInterface) noexcept
    : pOrigInterface(pOrigInterface)
{
    this->pOrigInterface->AddRef();
}

Render::IDirect3DDevice9Hook::~IDirect3DDevice9Hook() noexcept
{
    this->pOrigInterface->Release();
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::Present(CONST RECT* const pSourceRect, CONST RECT* const pDestRect,
    const HWND hDestWindowOverride, CONST RGNDATA* const pDirtyRegion) noexcept
{
    if (this->pOrigInterface == Render::pDeviceInterface && !this->resetStatus)
    {
        for (const auto& renderCallback : Render::renderCallbacks)
        {
            if (renderCallback != nullptr) renderCallback();
        }
    }

    return this->pOrigInterface->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::QueryInterface(REFIID riid, VOID** const ppvObj) noexcept
{
    *ppvObj = nullptr;

    const auto hResult = this->pOrigInterface->QueryInterface(riid, ppvObj);

    if (SUCCEEDED(hResult)) *ppvObj = this;

    return hResult;
}

ULONG __stdcall Render::IDirect3DDevice9Hook::AddRef() noexcept
{
    return this->pOrigInterface->AddRef();
}

ULONG __stdcall Render::IDirect3DDevice9Hook::Release() noexcept
{
    const auto count = this->pOrigInterface->Release();

    if (count <= 1)
    {
        if (this->pOrigInterface == Render::pDeviceInterface)
        {
            for (const auto& deviceFreeCallback : Render::deviceFreeCallbacks)
            {
                if (deviceFreeCallback != nullptr) deviceFreeCallback();
            }

            Render::deviceMutex.lock();
            Render::pDirectInterface = nullptr;
            Render::pDeviceInterface = nullptr;
            Render::deviceParameters = {};
            Render::deviceMutex.unlock();
        }

        delete this;
        return 0;
    }

    return count;
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::TestCooperativeLevel() noexcept
{
    return this->pOrigInterface->TestCooperativeLevel();
}

UINT __stdcall Render::IDirect3DDevice9Hook::GetAvailableTextureMem() noexcept
{
    return this->pOrigInterface->GetAvailableTextureMem();
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::EvictManagedResources() noexcept
{
    return this->pOrigInterface->EvictManagedResources();
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetDirect3D(IDirect3D9** const ppD3D9) noexcept
{
    return this->pOrigInterface->GetDirect3D(ppD3D9);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetDeviceCaps(D3DCAPS9* const pCaps) noexcept
{
    return this->pOrigInterface->GetDeviceCaps(pCaps);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetDisplayMode(const UINT iSwapChain, D3DDISPLAYMODE* const pMode) noexcept
{
    return this->pOrigInterface->GetDisplayMode(iSwapChain, pMode);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* const pParameters) noexcept
{
    return this->pOrigInterface->GetCreationParameters(pParameters);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetCursorProperties(const UINT XHotSpot,
    const UINT YHotSpot, IDirect3DSurface9* const pCursorBitmap) noexcept
{
    return this->pOrigInterface->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

VOID __stdcall Render::IDirect3DDevice9Hook::SetCursorPosition(const INT X, const INT Y, const DWORD Flags) noexcept
{
    this->pOrigInterface->SetCursorPosition(X, Y, Flags);
}

BOOL __stdcall Render::IDirect3DDevice9Hook::ShowCursor(const BOOL bShow) noexcept
{
    return this->pOrigInterface->ShowCursor(bShow);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* const pPresentationParameters,
    IDirect3DSwapChain9** const pSwapChain) noexcept
{
    return this->pOrigInterface->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetSwapChain(const UINT iSwapChain, IDirect3DSwapChain9** const pSwapChain) noexcept
{
    return this->pOrigInterface->GetSwapChain(iSwapChain, pSwapChain);
}

UINT __stdcall Render::IDirect3DDevice9Hook::GetNumberOfSwapChains() noexcept
{
    return this->pOrigInterface->GetNumberOfSwapChains();
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::Reset(D3DPRESENT_PARAMETERS* const pPresentationParameters) noexcept
{
    if (this->pOrigInterface == Render::pDeviceInterface && !this->resetStatus)
    {
        for (const auto& beforeResetCallback : Render::beforeResetCallbacks)
        {
            if (beforeResetCallback != nullptr) beforeResetCallback();
        }
    }

    this->resetStatus = true;

    const auto hResult = this->pOrigInterface->Reset(pPresentationParameters);

    if (this->pOrigInterface == Render::pDeviceInterface && SUCCEEDED(hResult))
    {
        Render::deviceMutex.lock();
        Render::deviceParameters = *pPresentationParameters;
        Render::deviceMutex.unlock();

        if (this->resetStatus)
        {
            for (const auto& afterResetCallback : Render::afterResetCallbacks)
            {
                if (afterResetCallback != nullptr) afterResetCallback(Render::pDeviceInterface, Render::deviceParameters);
            }
        }

        this->resetStatus = false;
    }

    return hResult;
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetBackBuffer(const UINT iSwapChain, const UINT iBackBuffer,
    const D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** const ppBackBuffer) noexcept
{
    return this->pOrigInterface->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetRasterStatus(const UINT iSwapChain, D3DRASTER_STATUS* const pRasterStatus) noexcept
{
    return this->pOrigInterface->GetRasterStatus(iSwapChain, pRasterStatus);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetDialogBoxMode(const BOOL bEnableDialogs) noexcept
{
    return this->pOrigInterface->SetDialogBoxMode(bEnableDialogs);
}

VOID __stdcall Render::IDirect3DDevice9Hook::SetGammaRamp(const UINT iSwapChain, const DWORD Flags, CONST D3DGAMMARAMP* const pRamp) noexcept
{
    this->pOrigInterface->SetGammaRamp(iSwapChain, Flags, pRamp);
}

VOID __stdcall Render::IDirect3DDevice9Hook::GetGammaRamp(const UINT iSwapChain, D3DGAMMARAMP* const pRamp) noexcept
{
    this->pOrigInterface->GetGammaRamp(iSwapChain, pRamp);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateTexture(const UINT Width, const UINT Height,
    const UINT Levels, const DWORD Usage, const D3DFORMAT Format, const D3DPOOL Pool,
    IDirect3DTexture9** const ppTexture, HANDLE* const pSharedHandle) noexcept
{
    return this->pOrigInterface->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateVolumeTexture(const UINT Width, const UINT Height,
    const UINT Depth, const UINT Levels, const DWORD Usage, const D3DFORMAT Format, const D3DPOOL Pool,
    IDirect3DVolumeTexture9** const ppVolumeTexture, HANDLE* const pSharedHandle) noexcept
{
    return this->pOrigInterface->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateCubeTexture(const UINT EdgeLength, const UINT Levels, const DWORD Usage,
    const D3DFORMAT Format, const D3DPOOL Pool, IDirect3DCubeTexture9** const ppCubeTexture, HANDLE* const pSharedHandle) noexcept
{
    return this->pOrigInterface->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateVertexBuffer(const UINT Length, const DWORD Usage, const DWORD FVF,
    const D3DPOOL Pool, IDirect3DVertexBuffer9** const ppVertexBuffer, HANDLE* const pSharedHandle) noexcept
{
    return this->pOrigInterface->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateIndexBuffer(const UINT Length, const DWORD Usage, const D3DFORMAT Format,
    const D3DPOOL Pool, IDirect3DIndexBuffer9** const ppIndexBuffer, HANDLE* const pSharedHandle) noexcept
{
    return this->pOrigInterface->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateRenderTarget(const UINT Width, const UINT Height,
    const D3DFORMAT Format, const D3DMULTISAMPLE_TYPE MultiSample, const DWORD MultisampleQuality, const BOOL Lockable,
    IDirect3DSurface9** const ppSurface, HANDLE* const pSharedHandle) noexcept
{
    return this->pOrigInterface->CreateRenderTarget(Width, Height, Format, MultiSample,
        MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateDepthStencilSurface(const UINT Width, const UINT Height,
    const D3DFORMAT Format, const D3DMULTISAMPLE_TYPE MultiSample, const DWORD MultisampleQuality,
    const BOOL Discard, IDirect3DSurface9** const ppSurface, HANDLE* const pSharedHandle) noexcept
{
    return this->pOrigInterface->CreateDepthStencilSurface(Width, Height, Format, MultiSample,
        MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::UpdateSurface(IDirect3DSurface9* const pSourceSurface,
    CONST RECT* const pSourceRect, IDirect3DSurface9* const pDestinationSurface, CONST POINT* const pDestPoint) noexcept
{
    return this->pOrigInterface->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::UpdateTexture(IDirect3DBaseTexture9* const pSourceTexture,
    IDirect3DBaseTexture9* const pDestinationTexture) noexcept
{
    return this->pOrigInterface->UpdateTexture(pSourceTexture, pDestinationTexture);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetRenderTargetData(IDirect3DSurface9* const pRenderTarget,
    IDirect3DSurface9* const pDestSurface) noexcept
{
    return this->pOrigInterface->GetRenderTargetData(pRenderTarget, pDestSurface);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetFrontBufferData(const UINT iSwapChain,
    IDirect3DSurface9* const pDestSurface) noexcept
{
    return this->pOrigInterface->GetFrontBufferData(iSwapChain, pDestSurface);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::StretchRect(IDirect3DSurface9* const pSourceSurface,
    CONST RECT* const pSourceRect, IDirect3DSurface9* const pDestSurface,
    CONST RECT* const pDestRect, const D3DTEXTUREFILTERTYPE Filter) noexcept
{
    return this->pOrigInterface->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::ColorFill(IDirect3DSurface9* const pSurface,
    CONST RECT* const pRect, const D3DCOLOR color) noexcept
{
    return this->pOrigInterface->ColorFill(pSurface, pRect, color);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateOffscreenPlainSurface(const UINT Width,
    const UINT Height, const D3DFORMAT Format, const D3DPOOL Pool,
    IDirect3DSurface9** const ppSurface, HANDLE* const pSharedHandle) noexcept
{
    return this->pOrigInterface->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetRenderTarget(const DWORD RenderTargetIndex,
    IDirect3DSurface9* const pRenderTarget) noexcept
{
    return this->pOrigInterface->SetRenderTarget(RenderTargetIndex, pRenderTarget);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetRenderTarget(const DWORD RenderTargetIndex,
    IDirect3DSurface9** const ppRenderTarget) noexcept
{
    return this->pOrigInterface->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetDepthStencilSurface(IDirect3DSurface9* const pNewZStencil) noexcept
{
    return this->pOrigInterface->SetDepthStencilSurface(pNewZStencil);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetDepthStencilSurface(IDirect3DSurface9** const ppZStencilSurface) noexcept
{
    return this->pOrigInterface->GetDepthStencilSurface(ppZStencilSurface);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::BeginScene() noexcept
{
    const auto hResult = this->pOrigInterface->BeginScene();

    if (this->pOrigInterface == Render::pDeviceInterface && SUCCEEDED(hResult) && !this->resetStatus)
    {
        if (!this->sceneStatus)
        {
            for (const auto& beginSceneCallback : Render::beginSceneCallbacks)
            {
                if (beginSceneCallback != nullptr) beginSceneCallback();
            }
        }

        this->sceneStatus = true;
    }

    return hResult;
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::EndScene() noexcept
{
    if (this->pOrigInterface == Render::pDeviceInterface && !this->resetStatus)
    {
        if (this->sceneStatus)
        {
            for (const auto& endSceneCallback : Render::endSceneCallbacks)
            {
                if (endSceneCallback != nullptr) endSceneCallback();
            }
        }

        this->sceneStatus = false;
    }

    return this->pOrigInterface->EndScene();
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::Clear(const DWORD Count, CONST D3DRECT* const pRects,
    const DWORD Flags, const D3DCOLOR Color, const FLOAT Z, const DWORD Stencil) noexcept
{
    return this->pOrigInterface->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetTransform(const D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* const mat) noexcept
{
    return this->pOrigInterface->SetTransform(State, mat);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetTransform(const D3DTRANSFORMSTATETYPE State, D3DMATRIX* const mat) noexcept
{
    return this->pOrigInterface->GetTransform(State, mat);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::MultiplyTransform(const D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* const mat) noexcept
{
    return this->pOrigInterface->MultiplyTransform(State, mat);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetViewport(CONST D3DVIEWPORT9* const pViewport) noexcept
{
    return this->pOrigInterface->SetViewport(pViewport);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetViewport(D3DVIEWPORT9* const pViewport) noexcept
{
    return this->pOrigInterface->GetViewport(pViewport);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetMaterial(CONST D3DMATERIAL9* const pMaterial) noexcept
{
    return this->pOrigInterface->SetMaterial(pMaterial);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetMaterial(D3DMATERIAL9* const pMaterial) noexcept
{
    return this->pOrigInterface->GetMaterial(pMaterial);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetLight(const DWORD Index, CONST D3DLIGHT9* const pLight) noexcept
{
    return this->pOrigInterface->SetLight(Index, pLight);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetLight(const DWORD Index, D3DLIGHT9* const pLight) noexcept
{
    return this->pOrigInterface->GetLight(Index, pLight);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::LightEnable(const DWORD Index, const BOOL Enable) noexcept
{
    return this->pOrigInterface->LightEnable(Index, Enable);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetLightEnable(const DWORD Index, BOOL* const pEnable) noexcept
{
    return this->pOrigInterface->GetLightEnable(Index, pEnable);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetClipPlane(const DWORD Index, CONST FLOAT* const pPlane) noexcept
{
    return this->pOrigInterface->SetClipPlane(Index, pPlane);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetClipPlane(const DWORD Index, FLOAT* const pPlane) noexcept
{
    return this->pOrigInterface->GetClipPlane(Index, pPlane);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetRenderState(const D3DRENDERSTATETYPE State, const DWORD Value) noexcept
{
    return this->pOrigInterface->SetRenderState(State, Value);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetRenderState(const D3DRENDERSTATETYPE State, DWORD* const pValue) noexcept
{
    return this->pOrigInterface->GetRenderState(State, pValue);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateStateBlock(const D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** const ppSB) noexcept
{
    return this->pOrigInterface->CreateStateBlock(Type, ppSB);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::BeginStateBlock() noexcept
{
    return this->pOrigInterface->BeginStateBlock();
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::EndStateBlock(IDirect3DStateBlock9** const ppSB) noexcept
{
    return this->pOrigInterface->EndStateBlock(ppSB);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetClipStatus(CONST D3DCLIPSTATUS9* const pClipStatus) noexcept
{
    return this->pOrigInterface->SetClipStatus(pClipStatus);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetClipStatus(D3DCLIPSTATUS9* const pClipStatus) noexcept
{
    return this->pOrigInterface->GetClipStatus(pClipStatus);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetTexture(const DWORD Stage, IDirect3DBaseTexture9** const ppTexture) noexcept
{
    return this->pOrigInterface->GetTexture(Stage, ppTexture);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetTexture(const DWORD Stage, IDirect3DBaseTexture9* const pTexture) noexcept
{
    return this->pOrigInterface->SetTexture(Stage, pTexture);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetTextureStageState(const DWORD Stage,
    const D3DTEXTURESTAGESTATETYPE Type, DWORD* const pValue) noexcept
{
    return this->pOrigInterface->GetTextureStageState(Stage, Type, pValue);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetTextureStageState(const DWORD Stage,
    const D3DTEXTURESTAGESTATETYPE Type, const DWORD Value) noexcept
{
    return this->pOrigInterface->SetTextureStageState(Stage, Type, Value);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetSamplerState(const DWORD Sampler,
    const D3DSAMPLERSTATETYPE Type, DWORD* const pValue) noexcept
{
    return this->pOrigInterface->GetSamplerState(Sampler, Type, pValue);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetSamplerState(const DWORD Sampler,
    const D3DSAMPLERSTATETYPE Type, const DWORD Value) noexcept
{
    return this->pOrigInterface->SetSamplerState(Sampler, Type, Value);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::ValidateDevice(DWORD* const pNumPasses) noexcept
{
    return this->pOrigInterface->ValidateDevice(pNumPasses);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetPaletteEntries(const UINT PaletteNumber,
    CONST PALETTEENTRY* const pEntries) noexcept
{
    return this->pOrigInterface->SetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetPaletteEntries(const UINT PaletteNumber,
    PALETTEENTRY* const pEntries) noexcept
{
    return this->pOrigInterface->GetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetCurrentTexturePalette(const UINT PaletteNumber) noexcept
{
    return this->pOrigInterface->SetCurrentTexturePalette(PaletteNumber);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetCurrentTexturePalette(UINT* const PaletteNumber) noexcept
{
    return this->pOrigInterface->GetCurrentTexturePalette(PaletteNumber);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetScissorRect(CONST RECT* const pRect) noexcept
{
    return this->pOrigInterface->SetScissorRect(pRect);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetScissorRect(RECT* const pRect) noexcept
{
    return this->pOrigInterface->GetScissorRect(pRect);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetSoftwareVertexProcessing(const BOOL bSoftware) noexcept
{
    return this->pOrigInterface->SetSoftwareVertexProcessing(bSoftware);
}

BOOL __stdcall Render::IDirect3DDevice9Hook::GetSoftwareVertexProcessing() noexcept
{
    return this->pOrigInterface->GetSoftwareVertexProcessing();
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetNPatchMode(const FLOAT nSegments) noexcept
{
    return this->pOrigInterface->SetNPatchMode(nSegments);
}

FLOAT __stdcall Render::IDirect3DDevice9Hook::GetNPatchMode() noexcept
{
    return this->pOrigInterface->GetNPatchMode();
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::DrawPrimitive(const D3DPRIMITIVETYPE PrimitiveType,
    const UINT StartVertex, const UINT PrimitiveCount) noexcept
{
    return this->pOrigInterface->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::DrawIndexedPrimitive(const D3DPRIMITIVETYPE Type,
    const INT BaseVertexIndex, const UINT MinVertexIndex, const UINT NumVertices,
    const UINT startIndex, const UINT primCount) noexcept
{
    return this->pOrigInterface->DrawIndexedPrimitive(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::DrawPrimitiveUP(const D3DPRIMITIVETYPE PrimitiveType,
    const UINT PrimitiveCount, CONST VOID* const pVertexStreamZeroData, const UINT VertexStreamZeroStride) noexcept
{
    return this->pOrigInterface->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::DrawIndexedPrimitiveUP(const D3DPRIMITIVETYPE PrimitiveType,
    const UINT MinVertexIndex, const UINT NumVertices, const UINT PrimitiveCount, CONST VOID* const pIndexData,
    const D3DFORMAT IndexDataFormat, CONST VOID* const pVertexStreamZeroData, const UINT VertexStreamZeroStride) noexcept
{
    return this->pOrigInterface->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices,
        PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::ProcessVertices(const UINT SrcStartIndex,
    const UINT DestIndex, const UINT VertexCount, IDirect3DVertexBuffer9* const pDestBuffer,
    IDirect3DVertexDeclaration9* const pVertexDecl, const DWORD Flags) noexcept
{
    return this->pOrigInterface->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* const pVertexElements,
    IDirect3DVertexDeclaration9** const ppDecl) noexcept
{
    return this->pOrigInterface->CreateVertexDeclaration(pVertexElements, ppDecl);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetVertexDeclaration(IDirect3DVertexDeclaration9* const pDecl) noexcept
{
    return this->pOrigInterface->SetVertexDeclaration(pDecl);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetVertexDeclaration(IDirect3DVertexDeclaration9** const ppDecl) noexcept
{
    return this->pOrigInterface->GetVertexDeclaration(ppDecl);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetFVF(const DWORD FVF) noexcept
{
    return this->pOrigInterface->SetFVF(FVF);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetFVF(DWORD* const pFVF) noexcept
{
    return this->pOrigInterface->GetFVF(pFVF);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateVertexShader(CONST DWORD* const pFunction,
    IDirect3DVertexShader9** const ppShader) noexcept
{
    return this->pOrigInterface->CreateVertexShader(pFunction, ppShader);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetVertexShader(IDirect3DVertexShader9* const pShader) noexcept
{
    return this->pOrigInterface->SetVertexShader(pShader);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetVertexShader(IDirect3DVertexShader9** const ppShader) noexcept
{
    return this->pOrigInterface->GetVertexShader(ppShader);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetVertexShaderConstantF(const UINT StartRegister,
    CONST FLOAT* const pConstantData, const UINT Vector4fCount) noexcept
{
    return this->pOrigInterface->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetVertexShaderConstantF(const UINT StartRegister,
    FLOAT* const pConstantData, const UINT Vector4fCount) noexcept
{
    return this->pOrigInterface->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetVertexShaderConstantI(const UINT StartRegister,
    CONST INT* const pConstantData, const UINT Vector4iCount) noexcept
{
    return this->pOrigInterface->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetVertexShaderConstantI(const UINT StartRegister,
    INT* const pConstantData, const UINT Vector4iCount) noexcept
{
    return this->pOrigInterface->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetVertexShaderConstantB(const UINT StartRegister,
    CONST BOOL* const pConstantData, const UINT BoolCount) noexcept
{
    return this->pOrigInterface->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetVertexShaderConstantB(const UINT StartRegister,
    BOOL* const pConstantData, const UINT BoolCount) noexcept
{
    return this->pOrigInterface->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetStreamSource(const UINT StreamNumber,
    IDirect3DVertexBuffer9* const pStreamData, const UINT OffsetInBytes, const UINT Stride) noexcept
{
    return this->pOrigInterface->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetStreamSource(const UINT StreamNumber,
    IDirect3DVertexBuffer9** const ppStreamData, UINT* const OffsetInBytes, UINT* const pStride) noexcept
{
    return this->pOrigInterface->GetStreamSource(StreamNumber, ppStreamData, OffsetInBytes, pStride);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetStreamSourceFreq(const UINT StreamNumber, const UINT Divider) noexcept
{
    return this->pOrigInterface->SetStreamSourceFreq(StreamNumber, Divider);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetStreamSourceFreq(const UINT StreamNumber, UINT* const Divider) noexcept
{
    return this->pOrigInterface->GetStreamSourceFreq(StreamNumber, Divider);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetIndices(IDirect3DIndexBuffer9* const pIndexData) noexcept
{
    return this->pOrigInterface->SetIndices(pIndexData);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetIndices(IDirect3DIndexBuffer9** const ppIndexData) noexcept
{
    return this->pOrigInterface->GetIndices(ppIndexData);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreatePixelShader(CONST DWORD* const pFunction,
    IDirect3DPixelShader9** const ppShader) noexcept
{
    return this->pOrigInterface->CreatePixelShader(pFunction, ppShader);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetPixelShader(IDirect3DPixelShader9* const pShader) noexcept
{
    return this->pOrigInterface->SetPixelShader(pShader);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetPixelShader(IDirect3DPixelShader9** const ppShader) noexcept
{
    return this->pOrigInterface->GetPixelShader(ppShader);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetPixelShaderConstantF(const UINT StartRegister,
    CONST FLOAT* const pConstantData, const UINT Vector4fCount) noexcept
{
    return this->pOrigInterface->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetPixelShaderConstantF(const UINT StartRegister,
    FLOAT* const pConstantData, const UINT Vector4fCount) noexcept
{
    return this->pOrigInterface->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetPixelShaderConstantI(const UINT StartRegister,
    CONST INT* const pConstantData, const UINT Vector4iCount) noexcept
{
    return this->pOrigInterface->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetPixelShaderConstantI(const UINT StartRegister,
    INT* const pConstantData, const UINT Vector4iCount) noexcept
{
    return this->pOrigInterface->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::SetPixelShaderConstantB(const UINT StartRegister,
    CONST BOOL* const pConstantData, const UINT BoolCount) noexcept
{
    return this->pOrigInterface->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::GetPixelShaderConstantB(const UINT StartRegister,
    BOOL* const pConstantData, const UINT BoolCount) noexcept
{
    return this->pOrigInterface->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::DrawRectPatch(const UINT Handle, CONST FLOAT* const pNumSegs,
    CONST D3DRECTPATCH_INFO* const pRectPatchInfo) noexcept
{
    return this->pOrigInterface->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::DrawTriPatch(const UINT Handle, CONST FLOAT* const pNumSegs,
    CONST D3DTRIPATCH_INFO* const pTriPatchInfo) noexcept
{
    return this->pOrigInterface->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::DeletePatch(const UINT Handle) noexcept
{
    return this->pOrigInterface->DeletePatch(Handle);
}

HRESULT __stdcall Render::IDirect3DDevice9Hook::CreateQuery(const D3DQUERYTYPE Type, IDirect3DQuery9** const ppQuery) noexcept
{
    return this->pOrigInterface->CreateQuery(Type, ppQuery);
}

// --------------------------------------------------------------------------------------------------

Render::IDirect3D9Hook::IDirect3D9Hook(IDirect3D9* const pOrigInterface) noexcept
    : pOrigInterface(pOrigInterface)
{
    this->pOrigInterface->AddRef();
}

Render::IDirect3D9Hook::~IDirect3D9Hook() noexcept
{
    this->pOrigInterface->Release();
}

HRESULT __stdcall Render::IDirect3D9Hook::QueryInterface(REFIID riid, VOID** const ppvObj) noexcept
{
    *ppvObj = nullptr;

    const auto hResult = this->pOrigInterface->QueryInterface(riid, ppvObj);

    if (SUCCEEDED(hResult)) *ppvObj = this;

    return hResult;
}

ULONG __stdcall Render::IDirect3D9Hook::AddRef() noexcept
{
    return this->pOrigInterface->AddRef();
}

ULONG __stdcall Render::IDirect3D9Hook::Release() noexcept
{
    const auto count = this->pOrigInterface->Release();

    if (count <= 1)
    {
        delete this;
        return 0;
    }

    return count;
}

HRESULT __stdcall Render::IDirect3D9Hook::RegisterSoftwareDevice(VOID* const pInitializeFunction) noexcept
{
    return this->pOrigInterface->RegisterSoftwareDevice(pInitializeFunction);
}

UINT __stdcall Render::IDirect3D9Hook::GetAdapterCount() noexcept
{
    return this->pOrigInterface->GetAdapterCount();
}

HRESULT __stdcall Render::IDirect3D9Hook::GetAdapterIdentifier(const UINT Adapter, const DWORD Flags,
    D3DADAPTER_IDENTIFIER9* const pIdentifier) noexcept
{
    return this->pOrigInterface->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

UINT __stdcall Render::IDirect3D9Hook::GetAdapterModeCount(const UINT Adapter, const D3DFORMAT Format) noexcept
{
    return this->pOrigInterface->GetAdapterModeCount(Adapter, Format);
}

HRESULT __stdcall Render::IDirect3D9Hook::EnumAdapterModes(const UINT Adapter, const D3DFORMAT Format,
    const UINT Mode, D3DDISPLAYMODE* const pMode) noexcept
{
    return this->pOrigInterface->EnumAdapterModes(Adapter, Format, Mode, pMode);
}

HRESULT __stdcall Render::IDirect3D9Hook::GetAdapterDisplayMode(const UINT Adapter, D3DDISPLAYMODE* const pMode) noexcept
{
    return this->pOrigInterface->GetAdapterDisplayMode(Adapter, pMode);
}

HRESULT __stdcall Render::IDirect3D9Hook::CheckDeviceType(const UINT Adapter, const D3DDEVTYPE DevType,
    const D3DFORMAT AdapterFormat, const D3DFORMAT BackBufferFormat, const BOOL bWindowed) noexcept
{
    return this->pOrigInterface->CheckDeviceType(Adapter, DevType,
        AdapterFormat, BackBufferFormat, bWindowed);
}

HRESULT __stdcall Render::IDirect3D9Hook::CheckDeviceFormat(const UINT Adapter, const D3DDEVTYPE DeviceType,
    const D3DFORMAT AdapterFormat, const DWORD Usage, const D3DRESOURCETYPE RType,
    D3DFORMAT CheckFormat) noexcept
{
    return this->pOrigInterface->CheckDeviceFormat(Adapter, DeviceType,
        AdapterFormat, Usage, RType, CheckFormat);
}

HRESULT __stdcall Render::IDirect3D9Hook::CheckDeviceMultiSampleType(const UINT Adapter, const D3DDEVTYPE DeviceType,
    const D3DFORMAT SurfaceFormat, const BOOL Windowed, const D3DMULTISAMPLE_TYPE MultiSampleType,
    DWORD* const pQualityLevels) noexcept
{
    return this->pOrigInterface->CheckDeviceMultiSampleType(Adapter, DeviceType,
        SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
}

HRESULT __stdcall Render::IDirect3D9Hook::CheckDepthStencilMatch(const UINT Adapter, const D3DDEVTYPE DeviceType,
    const D3DFORMAT AdapterFormat, const D3DFORMAT RenderTargetFormat, const D3DFORMAT DepthStencilFormat) noexcept
{
    return this->pOrigInterface->CheckDepthStencilMatch(Adapter, DeviceType,
        AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}

HRESULT __stdcall Render::IDirect3D9Hook::CheckDeviceFormatConversion(const UINT Adapter, const D3DDEVTYPE DeviceType,
    const D3DFORMAT SourceFormat, const D3DFORMAT TargetFormat) noexcept
{
    return this->pOrigInterface->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
}

HRESULT __stdcall Render::IDirect3D9Hook::GetDeviceCaps(const UINT Adapter,
    const D3DDEVTYPE DeviceType, D3DCAPS9* const pCaps) noexcept
{
    return this->pOrigInterface->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

HMONITOR __stdcall Render::IDirect3D9Hook::GetAdapterMonitor(const UINT Adapter) noexcept
{
    return this->pOrigInterface->GetAdapterMonitor(Adapter);
}

HRESULT __stdcall Render::IDirect3D9Hook::CreateDevice(const UINT Adapter, const D3DDEVTYPE DeviceType, const HWND hFocusWindow,
    const DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* const pPresentationParameters,
    IDirect3DDevice9** const ppReturnedDeviceInterface) noexcept
{
    const auto hResult = this->pOrigInterface->CreateDevice(Adapter, DeviceType,
        hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

    if (FAILED(hResult) || this != pGameDirect || ppReturnedDeviceInterface != &pGameDevice)
        return hResult;

    Logger::LogToFile("[dbg:render:direct] : device interface (ptr:%p) success created with "
        "hwnd (value:%u) windowed (value:%d) screenwidth (value:%u) screenheight (value:%u)",
        *ppReturnedDeviceInterface, hFocusWindow, pPresentationParameters->Windowed,
        pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);

    const auto pHookDevice = new (std::nothrow) IDirect3DDevice9Hook(*ppReturnedDeviceInterface);

    if (pHookDevice == nullptr)
    {
        Logger::LogToFile("[err:render:direct] : failed to allocate memory for hook interface");
        return hResult;
    }

    Logger::LogToFile("[dbg:render:direct] : pointer successfully replaced from orig (ptr:%p) to hook (ptr:%p)",
        *ppReturnedDeviceInterface, pHookDevice);

    if (Render::pDirectInterface != nullptr)
    {
        for (const auto& deviceFreeCallback : Render::deviceFreeCallbacks)
        {
            if (deviceFreeCallback != nullptr) deviceFreeCallback();
        }
    }

    Render::deviceMutex.lock();
    Render::pDirectInterface = this->pOrigInterface;
    Render::pDeviceInterface = *ppReturnedDeviceInterface;
    Render::deviceParameters = *pPresentationParameters;
    Render::deviceMutex.unlock();

    for (const auto& deviceInitCallback : Render::deviceInitCallbacks)
    {
        if (deviceInitCallback != nullptr) deviceInitCallback(Render::pDirectInterface,
            Render::pDeviceInterface, Render::deviceParameters);
    }

    *ppReturnedDeviceInterface = pHookDevice;

    return hResult;
}

// --------------------------------------------------------------------------------------------------

IDirect3D9* CALLBACK Render::HookDirect3DCreate9(const UINT SDKVersion) noexcept
{
    Render::hookDirect3DCreate9->Disable();
    auto pOrigDirect = GameDirect3DCreate9(SDKVersion);
    Render::hookDirect3DCreate9->Enable();

    if (pOrigDirect != nullptr)
    {
        Logger::LogToFile("[dbg:render:hookdirect3dcreate9] : intercepted instance "
            "(ptr:%p) of IDirect3D9", pOrigDirect);

        if (const auto pHookDirect = new (std::nothrow) IDirect3D9Hook(pOrigDirect); pHookDirect != nullptr)
        {
            Logger::LogToFile("[dbg:render:hookdirect3dcreate9] : pointer successfully "
                "replaced from orig (ptr:%p) to hook (ptr:%p)", pOrigDirect, pHookDirect);

            pOrigDirect = pHookDirect;
        }
    }

    return pOrigDirect;
}

bool Render::initStatus { false };

std::mutex Render::deviceMutex;
IDirect3D9* Render::pDirectInterface { nullptr };
IDirect3DDevice9* Render::pDeviceInterface { nullptr };
D3DPRESENT_PARAMETERS Render::deviceParameters {};

std::vector<Render::DeviceInitCallback> Render::deviceInitCallbacks;
std::vector<Render::BeforeResetCallback> Render::beforeResetCallbacks;
std::vector<Render::BeginSceneCallback> Render::beginSceneCallbacks;
std::vector<Render::RenderCallback> Render::renderCallbacks;
std::vector<Render::EndSceneCallback> Render::endSceneCallbacks;
std::vector<Render::AfterResetCallback> Render::afterResetCallbacks;
std::vector<Render::DeviceFreeCallback> Render::deviceFreeCallbacks;

Memory::JumpHookPtr Render::hookDirect3DCreate9 { nullptr };
