/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "ImGuiUtil.h"

#include <cassert>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_impl_win32.h>

#include "Memory.hpp"
#include "Logger.h"

bool ImGuiUtil::Init(IDirect3DDevice9* const device) noexcept
{
    assert(device != nullptr);

    if (_init_status) return false;

    HWND device_window;
    {
        IDirect3DSwapChain9* swapchain;

        if (const auto result = device->GetSwapChain(0, &swapchain); FAILED(result))
        {
            Logger::LogToFile("[err:imgui:init] : failed to get swapchain (code:%ld)", result);
            return false;
        }

        D3DPRESENT_PARAMETERS parameters;

        const auto result = swapchain->GetPresentParameters(&parameters);
        swapchain->Release();
        if (FAILED(result))
        {
            Logger::LogToFile("[err:imgui:init] : failed to get present parameters (code:%ld)", result);
            return false;
        }

        device_window = parameters.hDeviceWindow;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::GetIO().IniFilename = nullptr;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    _win32_load_status = ImGui_ImplWin32_Init(device_window);
    _dx9_load_status = ImGui_ImplDX9_Init(device);

    _render_status = false;
    _init_status = true;

    return true;
}

bool ImGuiUtil::IsInited() noexcept
{
    return _init_status;
}

void ImGuiUtil::Free() noexcept
{
    EndRender();

    if (_dx9_load_status) ImGui_ImplDX9_Shutdown();
    _dx9_load_status = false;

    if (_win32_load_status) ImGui_ImplWin32_Shutdown();
    _win32_load_status = false;

    if (_init_status) ImGui::DestroyContext();
    _init_status = false;
}

bool ImGuiUtil::BeginRender() noexcept
{
    if (!_init_status || _render_status) return false;

    if (_dx9_load_status) ImGui_ImplDX9_NewFrame();
    if (_win32_load_status) ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();

    _render_status = true;

    return true;
}

bool ImGuiUtil::IsRendering() noexcept
{
    return _render_status;
}

void ImGuiUtil::EndRender() noexcept
{
    if (_init_status && _render_status)
    {
        ImGui::EndFrame();
        ImGui::Render();

        if (_dx9_load_status)
        {
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        }

        _render_status = false;
    }
}

LRESULT ImGuiUtil::WindowProc(const HWND window, const UINT message,
    const WPARAM w_param, const LPARAM l_param) noexcept
{
    if (!_init_status || !_win32_load_status) return FALSE;

    if (message == WM_CHAR)
    {
        if (wchar_t wChar; MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPCSTR)(&w_param), 1, &wChar, 1) == 1)
        {
            ImGui::GetIO().AddInputCharacter(wChar);
        }

        return TRUE;
    }

    extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);
    return ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param);
}

bool ImGuiUtil::_init_status       = false;
bool ImGuiUtil::_win32_load_status = false;
bool ImGuiUtil::_dx9_load_status   = false;
bool ImGuiUtil::_render_status     = false;
