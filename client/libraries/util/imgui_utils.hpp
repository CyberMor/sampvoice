/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>

#include <Windows.h>
#include <d3d9.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_impl_win32.h>

#include "logger.hpp"

struct ImGuiUtils {

    ImGuiUtils(const ImGuiUtils&) = delete;
    ImGuiUtils(ImGuiUtils&&) = delete;
    ImGuiUtils& operator=(const ImGuiUtils&) = delete;
    ImGuiUtils& operator=(ImGuiUtils&&) = delete;

private:

    ImGuiUtils() noexcept = default;
    ~ImGuiUtils() noexcept = default;

public:

    static ImGuiUtils& Instance() noexcept
    {
        static ImGuiUtils instance;
        return instance;
    }

public:

    bool Initialize(IDirect3DDevice9* const device) noexcept
    {
        assert(device != nullptr);

        if (_is_initialized == false)
        {
            HWND device_window;
            {
                IDirect3DSwapChain9* swapchain;

                if (const HRESULT result = device->GetSwapChain(0, &swapchain); FAILED(result))
                {
                    Logger::Instance().LogToFile("[err:imgui:initialize] failed to "
                        "get swapchain (code:%ld)", result);
                    return false;
                }

                D3DPRESENT_PARAMETERS parameters;

                const HRESULT result = swapchain->GetPresentParameters(&parameters);
                swapchain->Release();
                if (FAILED(result))
                {
                    Logger::Instance().LogToFile("[err:imgui:initialize] failed to "
                        "get present parameters (code:%ld)", result);
                    return false;
                }

                device_window = parameters.hDeviceWindow;
            }

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();

            ImGui::GetIO().IniFilename = nullptr;
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

            _is_win32_loaded = ImGui_ImplWin32_Init(device_window);
            _is_dx9_loaded = ImGui_ImplDX9_Init(device);

            _is_initialized = true;
        }

        return true;
    }

    void Deinitialize() noexcept
    {
        StopRendering();

        if (_is_dx9_loaded == true)
        {
            ImGui_ImplDX9_Shutdown();
            _is_dx9_loaded = false;
        }

        if (_is_win32_loaded == true)
        {
            ImGui_ImplWin32_Shutdown();
            _is_win32_loaded = false;
        }

        if (_is_initialized == true)
        {
            ImGui::DestroyContext();
            _is_initialized = false;
        }
    }

public:

    void StartRendering() noexcept
    {
        if (_is_initialized == true)
        {
            if (_is_rendering == false)
            {
                if (_is_dx9_loaded) ImGui_ImplDX9_NewFrame();
                if (_is_win32_loaded) ImGui_ImplWin32_NewFrame();

                ImGui::NewFrame();

                _is_rendering = true;
            }
        }
    }

    bool IsRendering() const noexcept
    {
        return _is_rendering;
    }

    void StopRendering() noexcept
    {
        if (_is_rendering == true)
        {
            ImGui::EndFrame();
            ImGui::Render();

            if (_is_dx9_loaded)
            {
                ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            }

            _is_rendering = false;
        }
    }

public:

    LRESULT WindowProcedure(const HWND window, const UINT message,
        WPARAM wparam, const LPARAM lparam) noexcept
    {
        if (_is_win32_loaded == false)
            return 0;

        if (message == WM_CHAR)
        {
            if (wchar_t wchar; MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,
                reinterpret_cast<const char*>(&wparam), 1, &wchar, 1) == 1)
                wparam = wchar;
        }

        extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);
        return ImGui_ImplWin32_WndProcHandler(window, message, wparam, lparam);
    }

private:

    bool _is_initialized = false;
    bool _is_rendering   = false;

    bool _is_win32_loaded = false;
    bool _is_dx9_loaded   = false;

};
