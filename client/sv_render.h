#pragma once

#include <forward_list>

#include <d3d9.h>
#include <d3dx9.h>
#include <ddraw.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "logger.h"
#include "memory.hpp"

namespace render {

	namespace core {

		// Статус модуля
		static bool status = false;

		// Режим отладки
		constexpr bool debug_mode = false;

		// Адреса модуля
		static memory::address_t class_hwnd							= 0xC97C1Cu;
		static memory::address_t class_direct						= 0xC97C20u;
		static memory::address_t class_direct_device				= 0xC97C28u;
		static memory::address_t class_present_parameters			= 0xC9C040u;
		static memory::address_t addr_func_device_control_proc		= 0x402C1Fu;
		static memory::address_t addr_func_direct_3d_create_9		= 0x807C2Bu;
		static memory::address_t addr_func_direct_draw_create_ex	= 0x81805Cu;

		// Дескриптор окна
		static HWND hwnd;

		// Поток инициализации
		static volatile HANDLE thread_init;
		static volatile bool return_result;

		// Перехватчики
		static memory::hooks::jump_hook *hook_direct_3d_create;
		static memory::hooks::jump_hook *hook_direct_draw_create_ex;

		// Списки интерфейсов-перехватчиков
		static std::forward_list<interface IDirect3D9Hook*> list_d3d9;
		static std::forward_list<interface IDirect3DDevice9Hook*> list_d3ddevice;

		// Обработчики
		static void(*handler_render)(IDirect3DDevice9*, CONST RECT*, HWND)		= nullptr; // Обработчик отрисовки
		static void(*handler_init)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)	= nullptr; // Обработчик инициализации
		static void(*handler_reset)(void)										= nullptr; // Обработчик сброса

		// Оригинальные указатели
		static IDirectDraw7 *pDDraw							= nullptr;
		static D3DPRESENT_PARAMETERS *pD3DPresentParameters	= nullptr;

		// Интерфейсы-перехватчики
		interface IDirect3DDevice9Hook : public IDirect3DDevice9 {
		private:

			/*** Variables ***/

			IDirect3DDevice9 * const orig_pointer;
			bool status_init;

		public:

			/*** Additional methods ***/

			// Создать интерфейс-перехватчик
			IDirect3DDevice9Hook(
				IDirect3DDevice9 *orig_d3ddevice9
			) : orig_pointer(orig_d3ddevice9) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				status_init = false;
			}

			/// [WARNING]
			// Удалить интерфейс-перехватчик
			void remove() {
				const auto temp = orig_pointer;
				realloc(this, sizeof(IDirect3DDevice9));
				*((IDirect3DDevice9*)(this)) = *temp;
				delete temp;
			}

			/*** IDirect3DDevice9 methods ***/

			HRESULT __stdcall Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				if (!status_init && handler_init) {
					handler_init(this->orig_pointer, pD3DPresentParameters); status_init = true;
				} if (handler_render) handler_render(this->orig_pointer, pSourceRect, hDestWindowOverride);
				return orig_pointer->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
			}

			HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				HRESULT result = orig_pointer->QueryInterface(riid, ppvObj);
				if (!result) *ppvObj = this;
				return result;
			}

			ULONG __stdcall AddRef() {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->AddRef();
			}

			ULONG __stdcall Release() {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				ULONG result = orig_pointer->Release();
				if (!result) {
					list_d3ddevice.remove(this);
					delete(this);
				} return result;
			}

			HRESULT __stdcall TestCooperativeLevel() {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->TestCooperativeLevel();
			}

			UINT __stdcall GetAvailableTextureMem() {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetAvailableTextureMem();
			}

			HRESULT __stdcall EvictManagedResources() {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->EvictManagedResources();
			}

			HRESULT __stdcall GetDirect3D(IDirect3D9** ppD3D9) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetDirect3D(ppD3D9);
			}

			HRESULT __stdcall GetDeviceCaps(D3DCAPS9* pCaps) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetDeviceCaps(pCaps);
			}

			HRESULT __stdcall GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetDisplayMode(iSwapChain, pMode);
			}

			HRESULT __stdcall GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetCreationParameters(pParameters);
			}

			HRESULT __stdcall SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
			}

			void __stdcall SetCursorPosition(int X, int Y, DWORD Flags) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				orig_pointer->SetCursorPosition(X, Y, Flags);
			}

			BOOL __stdcall ShowCursor(BOOL bShow) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->ShowCursor(bShow);
			}

			HRESULT __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
			}

			HRESULT __stdcall GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetSwapChain(iSwapChain, pSwapChain);
			}

			UINT __stdcall GetNumberOfSwapChains() {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetNumberOfSwapChains();
			}

			HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				HRESULT result;

				// Вызываем обработчик сброса
				if (handler_reset && status_init) {
					handler_reset();
					status_init = false;
				}

				// Сбрасываем устройство
				if ((result = orig_pointer->Reset(pPresentationParameters)) == D3D_OK) {

					// Вызываем обработчик инициализации
					if (handler_init && !status_init) {
						handler_init(this->orig_pointer, pPresentationParameters);
						status_init = true;
					}
					
				}

				// Сохраняем новые параметры
				pD3DPresentParameters = pPresentationParameters;
				return result;

			}

			HRESULT __stdcall GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
			}

			HRESULT __stdcall GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetRasterStatus(iSwapChain, pRasterStatus);
			}

			HRESULT __stdcall SetDialogBoxMode(BOOL bEnableDialogs) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetDialogBoxMode(bEnableDialogs);
			}

			void __stdcall SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				orig_pointer->SetGammaRamp(iSwapChain, Flags, pRamp);
			}

			void __stdcall GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				orig_pointer->GetGammaRamp(iSwapChain, pRamp);
			}

			HRESULT __stdcall CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
			}

			HRESULT __stdcall CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
			}

			HRESULT __stdcall CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
			}

			HRESULT __stdcall CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
			}

			HRESULT __stdcall CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
			}

			HRESULT __stdcall CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
			}

			HRESULT __stdcall CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
			}

			HRESULT __stdcall UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
			}

			HRESULT __stdcall UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->UpdateTexture(pSourceTexture, pDestinationTexture);
			}

			HRESULT __stdcall GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetRenderTargetData(pRenderTarget, pDestSurface);
			}

			HRESULT __stdcall GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetFrontBufferData(iSwapChain, pDestSurface);
			}

			HRESULT __stdcall StretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
			}

			HRESULT __stdcall ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->ColorFill(pSurface, pRect, color);
			}

			HRESULT __stdcall CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
			}

			HRESULT __stdcall SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetRenderTarget(RenderTargetIndex, pRenderTarget);
			}

			HRESULT __stdcall GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
			}

			HRESULT __stdcall SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetDepthStencilSurface(pNewZStencil);
			}

			HRESULT __stdcall GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetDepthStencilSurface(ppZStencilSurface);
			}

			HRESULT __stdcall BeginScene() {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->BeginScene();
			}

			HRESULT __stdcall EndScene() {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->EndScene();
			}

			HRESULT __stdcall Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->Clear(Count, pRects, Flags, Color, Z, Stencil);
			}

			HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* mat) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetTransform(State, mat);
			}

			HRESULT __stdcall GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* mat) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetTransform(State, mat);
			}

			HRESULT __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* mat) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->MultiplyTransform(State, mat);
			}

			HRESULT __stdcall SetViewport(CONST D3DVIEWPORT9* pViewport) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetViewport(pViewport);
			}

			HRESULT __stdcall GetViewport(D3DVIEWPORT9* pViewport) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetViewport(pViewport);
			}

			HRESULT __stdcall SetMaterial(CONST D3DMATERIAL9* pMaterial) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetMaterial(pMaterial);
			}

			HRESULT __stdcall GetMaterial(D3DMATERIAL9* pMaterial) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetMaterial(pMaterial);
			}

			HRESULT __stdcall SetLight(DWORD Index, CONST D3DLIGHT9* pLight) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetLight(Index, pLight);
			}

			HRESULT __stdcall GetLight(DWORD Index, D3DLIGHT9* pLight) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetLight(Index, pLight);
			}

			HRESULT __stdcall LightEnable(DWORD Index, BOOL Enable) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->LightEnable(Index, Enable);
			}

			HRESULT __stdcall GetLightEnable(DWORD Index, BOOL* pEnable) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetLightEnable(Index, pEnable);
			}

			HRESULT __stdcall SetClipPlane(DWORD Index, CONST float* pPlane) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetClipPlane(Index, pPlane);
			}

			HRESULT __stdcall GetClipPlane(DWORD Index, float* pPlane) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetClipPlane(Index, pPlane);
			}

			HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetRenderState(State, Value);
			}

			HRESULT __stdcall GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetRenderState(State, pValue);
			}

			HRESULT __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CreateStateBlock(Type, ppSB);
			}

			HRESULT __stdcall BeginStateBlock() {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->BeginStateBlock();
			}

			HRESULT __stdcall EndStateBlock(IDirect3DStateBlock9** ppSB) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->EndStateBlock(ppSB);
			}

			HRESULT __stdcall SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetClipStatus(pClipStatus);
			}

			HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS9* pClipStatus) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetClipStatus(pClipStatus);
			}

			HRESULT __stdcall GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetTexture(Stage, ppTexture);
			}

			HRESULT __stdcall SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetTexture(Stage, pTexture);
			}

			HRESULT __stdcall GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetTextureStageState(Stage, Type, pValue);
			}

			HRESULT __stdcall SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetTextureStageState(Stage, Type, Value);
			}

			HRESULT __stdcall GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetSamplerState(Sampler, Type, pValue);
			}

			HRESULT __stdcall SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetSamplerState(Sampler, Type, Value);
			}

			HRESULT __stdcall ValidateDevice(DWORD* pNumPasses) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->ValidateDevice(pNumPasses);
			}

			HRESULT __stdcall SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY* pEntries) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetPaletteEntries(PaletteNumber, pEntries);
			}

			HRESULT __stdcall GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetPaletteEntries(PaletteNumber, pEntries);
			}

			HRESULT __stdcall SetCurrentTexturePalette(UINT PaletteNumber) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetCurrentTexturePalette(PaletteNumber);
			}

			HRESULT __stdcall GetCurrentTexturePalette(UINT *PaletteNumber) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetCurrentTexturePalette(PaletteNumber);
			}

			HRESULT __stdcall SetScissorRect(CONST RECT* pRect) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetScissorRect(pRect);
			}

			HRESULT __stdcall GetScissorRect(RECT* pRect) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetScissorRect(pRect);
			}

			HRESULT __stdcall SetSoftwareVertexProcessing(BOOL bSoftware) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetSoftwareVertexProcessing(bSoftware);
			}

			BOOL __stdcall GetSoftwareVertexProcessing() {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetSoftwareVertexProcessing();
			}

			HRESULT __stdcall SetNPatchMode(float nSegments) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetNPatchMode(nSegments);
			}

			float __stdcall GetNPatchMode() {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetNPatchMode();
			}

			HRESULT __stdcall DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
			}

			HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->DrawIndexedPrimitive(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
			}

			HRESULT __stdcall DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
			}

			HRESULT __stdcall DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
			}

			HRESULT __stdcall ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
			}

			HRESULT __stdcall CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CreateVertexDeclaration(pVertexElements, ppDecl);
			}

			HRESULT __stdcall SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetVertexDeclaration(pDecl);
			}

			HRESULT __stdcall GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetVertexDeclaration(ppDecl);
			}

			HRESULT __stdcall SetFVF(DWORD FVF) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetFVF(FVF);
			}

			HRESULT __stdcall GetFVF(DWORD* pFVF) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetFVF(pFVF);
			}

			HRESULT __stdcall CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CreateVertexShader(pFunction, ppShader);
			}

			HRESULT __stdcall SetVertexShader(IDirect3DVertexShader9* pShader) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetVertexShader(pShader);
			}

			HRESULT __stdcall GetVertexShader(IDirect3DVertexShader9** ppShader) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetVertexShader(ppShader);
			}

			HRESULT __stdcall SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
			}

			HRESULT __stdcall GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
			}

			HRESULT __stdcall SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
			}

			HRESULT __stdcall GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
			}

			HRESULT __stdcall SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
			}

			HRESULT __stdcall GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
			}

			HRESULT __stdcall SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
			}

			HRESULT __stdcall GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* OffsetInBytes, UINT* pStride) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetStreamSource(StreamNumber, ppStreamData, OffsetInBytes, pStride);
			}

			HRESULT __stdcall SetStreamSourceFreq(UINT StreamNumber, UINT Divider) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetStreamSourceFreq(StreamNumber, Divider);
			}

			HRESULT __stdcall GetStreamSourceFreq(UINT StreamNumber, UINT* Divider) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetStreamSourceFreq(StreamNumber, Divider);
			}

			HRESULT __stdcall SetIndices(IDirect3DIndexBuffer9* pIndexData) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetIndices(pIndexData);
			}

			HRESULT __stdcall GetIndices(IDirect3DIndexBuffer9** ppIndexData) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetIndices(ppIndexData);
			}

			HRESULT __stdcall CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CreatePixelShader(pFunction, ppShader);
			}

			HRESULT __stdcall SetPixelShader(IDirect3DPixelShader9* pShader) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetPixelShader(pShader);
			}

			HRESULT __stdcall GetPixelShader(IDirect3DPixelShader9** ppShader) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetPixelShader(ppShader);
			}

			HRESULT __stdcall SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
			}

			HRESULT __stdcall GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
			}

			HRESULT __stdcall SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
			}

			HRESULT __stdcall GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
			}

			HRESULT __stdcall SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
			}

			HRESULT __stdcall GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
			}

			HRESULT __stdcall DrawRectPatch(UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
			}

			HRESULT __stdcall DrawTriPatch(UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
			}

			HRESULT __stdcall DeletePatch(UINT Handle) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->DeletePatch(Handle);
			}

			HRESULT __stdcall CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CreateQuery(Type, ppQuery);
			}

		};
		interface IDirect3D9Hook : public IDirect3D9 {
		private:

			/*** Variables ***/

			IDirect3D9 * const orig_pointer;

		public:

			/*** Additional methods ***/

			// Создать интерфейс-перехватчик
			IDirect3D9Hook(
				IDirect3D9 *orig_d3d9
			) : orig_pointer(orig_d3d9) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
			}

			/// [WARNING]
			// Удалить интерфейс-перехватчик
			void remove() {
				const auto temp = orig_pointer;
				realloc(this, sizeof(IDirect3D9));
				*((IDirect3D9*)(this)) = *temp;
				delete temp;
			}

			/*** IUnknown methods ***/

			__declspec(nothrow) HRESULT __stdcall QueryInterface(THIS_ REFIID riid, void** ppvObj) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				HRESULT result;
				if (!(result = orig_pointer->QueryInterface(riid, ppvObj)))
					*ppvObj = this;
				return result;
			}

			__declspec(nothrow) ULONG __stdcall AddRef() {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->AddRef();
			}

			__declspec(nothrow) ULONG __stdcall Release() {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				ULONG result;
				if (!(result = orig_pointer->Release())) {
					list_d3d9.remove(this); delete(this);
					if (list_d3d9.empty()) core::status = false;
				} return result;
			}

			/*** IDirect3D9 methods ***/

			__declspec(nothrow) HRESULT __stdcall RegisterSoftwareDevice(THIS_ void* pInitializeFunction) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->RegisterSoftwareDevice(pInitializeFunction);
			}

			__declspec(nothrow) UINT __stdcall GetAdapterCount() {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetAdapterCount();
			}

			__declspec(nothrow) HRESULT __stdcall GetAdapterIdentifier(THIS_ UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
			}

			__declspec(nothrow) UINT __stdcall GetAdapterModeCount(THIS_ UINT Adapter, D3DFORMAT Format) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetAdapterModeCount(Adapter, Format);
			}

			__declspec(nothrow) HRESULT __stdcall EnumAdapterModes(THIS_ UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->EnumAdapterModes(Adapter, Format, Mode, pMode);
			}

			__declspec(nothrow) HRESULT __stdcall GetAdapterDisplayMode(THIS_ UINT Adapter, D3DDISPLAYMODE* pMode) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetAdapterDisplayMode(Adapter, pMode);
			}

			__declspec(nothrow) HRESULT __stdcall CheckDeviceType(THIS_ UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
			}

			__declspec(nothrow) HRESULT __stdcall CheckDeviceFormat(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
			}

			__declspec(nothrow) HRESULT __stdcall CheckDeviceMultiSampleType(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
			}

			__declspec(nothrow) HRESULT __stdcall CheckDepthStencilMatch(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
			}

			__declspec(nothrow) HRESULT __stdcall CheckDeviceFormatConversion(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
			}

			__declspec(nothrow) HRESULT __stdcall GetDeviceCaps(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetDeviceCaps(Adapter, DeviceType, pCaps);
			}

			__declspec(nothrow) HMONITOR __stdcall GetAdapterMonitor(THIS_ UINT Adapter) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);
				return orig_pointer->GetAdapterMonitor(Adapter);
			}

			__declspec(nothrow) HRESULT __stdcall CreateDevice(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface) {
				if (debug_mode) LogDebug("[call] : [%p] : %s\n", this, __FUNCTION__);

				/*if (*(byte*)0x746225 != 0x90) {
					int x = GetSystemMetrics(SM_CXSCREEN);
					int y = GetSystemMetrics(SM_CYSCREEN);
					SetWindowLong(pPresentationParameters->hDeviceWindow, GWL_STYLE, WS_POPUP);
					MoveWindow(pPresentationParameters->hDeviceWindow, (x / 2) - (pPresentationParameters->BackBufferWidth / 2),
						(y / 2) - (pPresentationParameters->BackBufferHeight / 2), pPresentationParameters->BackBufferWidth,
						pPresentationParameters->BackBufferHeight, TRUE);
					pPresentationParameters->Windowed = true;
					pPresentationParameters->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
				}*/

				// Получаем оригинальное устройство
				HRESULT result = orig_pointer->CreateDevice(
					Adapter,
					DeviceType,
					hFocusWindow,
					BehaviorFlags,
					pPresentationParameters,
					ppReturnedDeviceInterface
				);

				// Обработка устройства
				if (result == D3D_OK) {
					hwnd = hFocusWindow;
					pD3DPresentParameters = pPresentationParameters;
					if (IDirect3DDevice9Hook *temp = new IDirect3DDevice9Hook(*ppReturnedDeviceInterface)) {
						list_d3ddevice.push_front(temp);
						*ppReturnedDeviceInterface = temp;
						return_result = status = true;
					}
				}

				// Возобновить поток
				if (thread_init) ResumeThread(thread_init);
				
				return result;

			}

		};

		// Перехватчик инициализации Direct3D
		static IDirect3D9* WINAPI hook_func_direct_3d_create_9(
			UINT SDKVersion
		) {

			IDirect3D9 *result;
			hook_direct_3d_create->disable();

			// Получаем оригинальный интерфейс и сохраняем его
			if (result = reinterpret_cast<IDirect3D9*(*)(UINT)>(hook_direct_3d_create->get_original_addr())(SDKVersion)) {
				if (IDirect3D9Hook *temp = new IDirect3D9Hook(result)) {
					list_d3d9.push_front(temp);
					result = temp;
				}
			}
			
			hook_direct_3d_create->enable();
			return result;

		}

		// Перехватчик инициализации DirectDraw
		static HRESULT WINAPI hook_func_direct_draw_create_ex(
			_In_  GUID FAR     *lpGUID,
			_Out_ LPVOID       *lplpDD,
			_In_  REFIID       iid,
			_In_  IUnknown FAR *pUnkOuter
		) {

			hook_direct_draw_create_ex->disable();
			
			HRESULT result = reinterpret_cast<HRESULT(*)(GUID FAR *, LPVOID*, REFIID, IUnknown FAR *)>(
				hook_direct_draw_create_ex->get_original_addr()
			)(lpGUID, lplpDD, iid, pUnkOuter);
			if (!result) pDDraw = reinterpret_cast<IDirectDraw7*>(*lplpDD);
			
			hook_direct_draw_create_ex->enable();
			return result;

		}

		// Инициализация графического модуля
		static bool init(
			void(*handler_render)(IDirect3DDevice9*, CONST RECT*, HWND) = nullptr,
			void(*handler_init)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*) = nullptr,
			void(*handler_reset)(void) = nullptr
		) {

			if (core::status) return false;
			return_result = false;

			core::handler_render = handler_render;
			core::handler_reset = handler_reset;
			core::handler_init = handler_init;

			if (!(hook_direct_3d_create = new memory::hooks::jump_hook(
				addr_func_direct_3d_create_9,
				hook_func_direct_3d_create_9
			))) {
				LogError("render", "could not allocate memory for hook_direct_3d_create");
				return false;
			} if (!(hook_direct_draw_create_ex = new memory::hooks::jump_hook(
				addr_func_direct_draw_create_ex,
				hook_func_direct_draw_create_ex
			))) {
				LogError("render", "could not allocate memory for hook_direct_draw_create_ex");
				return false;
			}

			// Входим в режим ожидания
			if (DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), (LPHANDLE)&thread_init, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
				if (SuspendThread(thread_init) == -1) {
					LogError("render", "could not suspend thread");
					CloseHandle(thread_init);
					return false;
				} CloseHandle(thread_init);
			} else {
				LogError("render", "could not duplicate handle current thread");
				return false;
			} thread_init = NULL;

			return return_result;

		}

		// Освободить модуль
		static void free() {

			if (!core::status) return;

			if (handler_reset) handler_reset();

			for (auto i = list_d3ddevice.begin(); i != list_d3ddevice.end(); i++) (*i)->remove();
			for (auto i = list_d3d9.begin(); i != list_d3d9.end(); i++) (*i)->remove();

			core::status = false;

		}

	}

	// Проверка статуса модуля
	static inline bool get_module_status() {
		return core::status;
	}

	// Установить обработчик цикла отрисовки
	static inline void set_handler(
		void(*handler_render)(IDirect3DDevice9*, CONST RECT*, HWND)
	) {
		reinterpret_cast<std::atomic<void(*)(IDirect3DDevice9*, CONST RECT*, HWND)>*>(&core::handler_render)->store(handler_render);
	}

	// Удалить обработчик цикла отрисовки
	static inline void remove_handler() {
		reinterpret_cast<std::atomic<void(*)(IDirect3DDevice9*, CONST RECT*, HWND)>*>(&core::handler_render)->store(nullptr);
	}

	// Получить последний интерфейс IDirectDraw7
	static inline IDirectDraw7* get_ddraw7() {
		return core::pDDraw;
	}

	// Получить последние настройки Direct3D
	static inline D3DPRESENT_PARAMETERS* get_parameters() {
		return core::pD3DPresentParameters;
	}

	// Получить последний дескриптор окна
	static inline HWND get_hwnd() {
		return core::hwnd;
	}

	// Класс текстуры
	class texture {
	public:

		texture() = delete;
		texture(IDirect3DDevice9 *device, VOID *addr, DWORD size) {
			D3DXCreateSprite((this->pDevice = device), &this->pSprite);
			D3DXCreateTextureFromFileInMemory(this->pDevice, addr, size, &this->pTexture);
			this->pTexture->GetLevelDesc(0, &this->textureInfo);
			this->pTexture->GetSurfaceLevel(0, &this->ppSurface);
		}

		// Получить ширину текстуры
		inline int width() {
			return this->textureInfo.Width;
		}

		// Получить высоту текстуры
		inline int height() {
			return this->textureInfo.Height;
		}

		// Масштабировать текстуру
		void scale(
			float w, float h
		) {
			D3DXMATRIX matrix;
			this->pSprite->GetTransform(&matrix);
			D3DXMatrixScaling(&matrix, w / this->width(), h / this->height(), 1.f);
			this->pSprite->SetTransform(&matrix);
		}

		// Отрисовать текстуру
		void draw(CONST RECT* pSourceRect, int x, int y) {
			if (SUCCEEDED(this->pDevice->TestCooperativeLevel()) &&
				SUCCEEDED(this->pSprite->Begin(D3DXSPRITE_ALPHABLEND))) {
				this->pSprite->Draw(this->pTexture, pSourceRect, NULL, &D3DXVECTOR3(x, y, 0), -1);
				this->pSprite->End();
			}
		}

		~texture() {
			if (this->pTexture) this->pTexture->Release();
			if (this->pSprite) this->pSprite->Release();
		}

	private:

		D3DSURFACE_DESC textureInfo;
		IDirect3DSurface9 * ppSurface = nullptr;
		IDirect3DDevice9 * pDevice = nullptr;
		ID3DXSprite * pSprite = nullptr;
		IDirect3DTexture9 *pTexture = nullptr;

	};

}
