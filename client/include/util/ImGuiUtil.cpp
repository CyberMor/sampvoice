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

bool ImGuiUtil::Init(IDirect3DDevice9* pDevice,
                     D3DPRESENT_PARAMETERS* pParameters) noexcept
{
    assert(pDevice);
    assert(pParameters);

    if (ImGuiUtil::initStatus)
        return false;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::GetIO().IniFilename = NULL;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGuiUtil::win32loadStatus = ImGui_ImplWin32_Init(pParameters->hDeviceWindow);
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
    ImGuiUtil::RenderEnd();

    if (!ImGuiUtil::initStatus) return;

    if (ImGuiUtil::dx9loadStatus) ImGui_ImplDX9_Shutdown();
    if (ImGuiUtil::win32loadStatus) ImGui_ImplWin32_Shutdown();

    ImGuiUtil::dx9loadStatus = false;
    ImGuiUtil::win32loadStatus = false;

    ImGui::DestroyContext();

    ImGuiUtil::initStatus = false;
}

bool ImGuiUtil::RenderBegin() noexcept
{
    if (!ImGuiUtil::initStatus) return false;
    if (ImGuiUtil::renderStatus) return false;

    if (ImGuiUtil::dx9loadStatus) ImGui_ImplDX9_NewFrame();
    if (ImGuiUtil::win32loadStatus) ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();

    ImGuiUtil::renderStatus = true;

    return true;
}

bool ImGuiUtil::IsRendering() noexcept
{
    return ImGuiUtil::renderStatus;
}

void ImGuiUtil::RenderEnd() noexcept
{
    if (!ImGuiUtil::initStatus) return;
    if (!ImGuiUtil::renderStatus) return;

    ImGui::EndFrame();
    ImGui::Render();

    if (ImGuiUtil::dx9loadStatus)
    {
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }

    ImGuiUtil::renderStatus = false;
}

LRESULT ImGuiUtil::OnWndMessage(HWND hWnd, UINT uMsg,
                                WPARAM wParam, LPARAM lParam) noexcept
{
    if (!ImGuiUtil::initStatus) return FALSE;
    if (!ImGuiUtil::win32loadStatus) return FALSE;

    if (uMsg == WM_CHAR)
    {
        wchar_t wChar { NULL };
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char*)(&wParam), 1, &wChar, 1);
        ImGui::GetIO().AddInputCharacter(wChar);
        return TRUE;
    }

    extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);
    return ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
}

bool ImGuiUtil::initStatus { false };
bool ImGuiUtil::win32loadStatus { false };
bool ImGuiUtil::dx9loadStatus { false };
bool ImGuiUtil::renderStatus { false };
