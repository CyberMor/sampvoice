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

bool ImGuiUtil::Init(IDirect3DDevice9* const pDevice) noexcept
{
    assert(pDevice != nullptr);

    if (ImGuiUtil::initStatus)
        return false;

    HWND hDeviceWindow { nullptr };
    {
        IDirect3DSwapChain9* pSwapChain { nullptr };

        if (const auto hResult = pDevice->GetSwapChain(0, &pSwapChain); FAILED(hResult))
        {
            Logger::LogToFile("[err:imgui:init] : failed to get swap chain (code:%ld)", hResult);
            return false;
        }

        const Memory::ScopeExit scope { [&pSwapChain] { pSwapChain->Release(); } };

        D3DPRESENT_PARAMETERS dParameters {};

        if (const auto hResult = pSwapChain->GetPresentParameters(&dParameters); FAILED(hResult))
        {
            Logger::LogToFile("[err:imgui:init] : failed to get present parameters (code:%ld)", hResult);
            return false;
        }

        hDeviceWindow = dParameters.hDeviceWindow;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::GetIO().IniFilename = NULL;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGuiUtil::win32loadStatus = ImGui_ImplWin32_Init(hDeviceWindow);
    ImGuiUtil::dx9loadStatus = ImGui_ImplDX9_Init(pDevice);

    ImGuiUtil::renderStatus = false;
    ImGuiUtil::initStatus = true;

    return true;
}

bool ImGuiUtil::IsInited() noexcept
{
    return ImGuiUtil::initStatus;
}

void ImGuiUtil::Free() noexcept
{
    ImGuiUtil::EndRender();

    if (ImGuiUtil::dx9loadStatus)
        ImGui_ImplDX9_Shutdown();

    ImGuiUtil::dx9loadStatus = false;

    if (ImGuiUtil::win32loadStatus)
        ImGui_ImplWin32_Shutdown();

    ImGuiUtil::win32loadStatus = false;

    if (ImGuiUtil::initStatus)
        ImGui::DestroyContext();

    ImGuiUtil::initStatus = false;
}

bool ImGuiUtil::BeginRender() noexcept
{
    if (!ImGuiUtil::initStatus)
        return false;

    if (ImGuiUtil::renderStatus)
        return false;

    if (ImGuiUtil::dx9loadStatus)
        ImGui_ImplDX9_NewFrame();

    if (ImGuiUtil::win32loadStatus)
        ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();

    ImGuiUtil::renderStatus = true;

    return true;
}

bool ImGuiUtil::IsRendering() noexcept
{
    return ImGuiUtil::renderStatus;
}

void ImGuiUtil::EndRender() noexcept
{
    if (!ImGuiUtil::initStatus)
        return;

    if (!ImGuiUtil::renderStatus)
        return;

    ImGui::EndFrame();
    ImGui::Render();

    if (ImGuiUtil::dx9loadStatus)
    {
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }

    ImGuiUtil::renderStatus = false;
}

LRESULT ImGuiUtil::WindowProc(const HWND hWnd, const UINT uMsg,
                              const WPARAM wParam, const LPARAM lParam) noexcept
{
    if (!ImGuiUtil::initStatus)
        return FALSE;

    if (!ImGuiUtil::win32loadStatus)
        return FALSE;

    if (uMsg == WM_CHAR)
    {
        if (wchar_t wChar; MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,
            reinterpret_cast<LPCSTR>(&wParam), 1, &wChar, 1) == 1)
        {
            ImGui::GetIO().AddInputCharacter(wChar);
        }

        return TRUE;
    }

    extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);
    return ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
}

bool ImGuiUtil::initStatus { false };
bool ImGuiUtil::win32loadStatus { false };
bool ImGuiUtil::dx9loadStatus { false };
bool ImGuiUtil::renderStatus { false };
