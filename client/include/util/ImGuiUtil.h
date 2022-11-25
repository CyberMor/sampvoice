/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <Windows.h>
#include <d3d9.h>

struct ImGuiUtil {

    ImGuiUtil() = delete;
    ~ImGuiUtil() = delete;
    ImGuiUtil(const ImGuiUtil&) = delete;
    ImGuiUtil(ImGuiUtil&&) = delete;
    ImGuiUtil& operator=(const ImGuiUtil&) = delete;
    ImGuiUtil& operator=(ImGuiUtil&&) = delete;

public:

    static bool Init(IDirect3DDevice9* device) noexcept;
    static bool IsInited() noexcept;
    static void Free() noexcept;

    static bool BeginRender() noexcept;
    static bool IsRendering() noexcept;
    static void EndRender() noexcept;

public:

    static LRESULT WindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param) noexcept;

private:

    static bool _init_status;
    static bool _win32_load_status;
    static bool _dx9_load_status;
    static bool _render_status;

};
