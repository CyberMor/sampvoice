/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <Windows.h>
#include <d3d9.h>

#include <imgui/imgui.h>
#include <samp/CChat.h>
#include <samp/CInput.h>
#include <samp/CScoreboard.h>
#include <util/addresses.hpp>
#include <util/blur_effect.hpp>
#include <util/texture.hpp>
#include <util/render.hpp>
#include <util/imgui_utils.hpp>
#include <util/game_utils.hpp>
#include <util/logger.hpp>

#include "black_list.hpp"
#include "speaker_list.hpp"
#include "localization.hpp"
#include "micro_icon.hpp"
#include "listener.hpp"
#include "speaker.hpp"

struct PluginMenu {

    PluginMenu(const PluginMenu&) = delete;
    PluginMenu(PluginMenu&&) = delete;
    PluginMenu& operator=(const PluginMenu&) = delete;
    PluginMenu& operator=(PluginMenu&&) = delete;

private:

    PluginMenu() noexcept { *_buffer = '\0'; }
    ~PluginMenu() noexcept = default;

public:

    static PluginMenu& Instance() noexcept
    {
        static PluginMenu instance;
        return instance;
    }

private:

    static constexpr float kBaseMenuWidth             = 0.6 * Render::kBaseWidth;
    static constexpr float kBaseMenuHeight            = 0.7 * Render::kBaseHeight;
    static constexpr float kBaseMenuPaddingX          = 20;
    static constexpr float kBaseMenuPaddingY          = 10;
    static constexpr float kBaseMenuFramePaddingX     = 10;
    static constexpr float kBaseMenuFramePaddingY     = 0.5;
    static constexpr float kBaseMenuItemSpacingX      = 20;
    static constexpr float kBaseMenuItemSpacingY      = 2;
    static constexpr float kBaseMenuItemInnerSpacingX = 10;
    static constexpr float kBaseMenuItemInnerSpacingY = 10;
    static constexpr float kBaseMenuRounding          = 10;
    static constexpr float kBaseFontTitleSize         = 20;
    static constexpr float kBaseFontTabSize           = 14;
    static constexpr float kBaseFontDescSize          = 12;
    static constexpr float kBaseFontSize              = 10;
    static constexpr int   kTabsCount                 = 3;
    static constexpr float kBaseTabPadding            = 4;
    static constexpr float kBaseTabWidth              = (kBaseMenuWidth - (2 * kBaseMenuPaddingX +
                                                        (kTabsCount - 1) * kBaseTabPadding)) / kTabsCount;
    static constexpr float kBaseTabHeight             = kBaseTabWidth / 6;
    static constexpr float kBlurLevelIncrement        = 5;
    static constexpr float kBlurLevelDecrement        = -5;

public:

    bool Initialize(IDirect3DDevice9* const device,
        const DataView<ubyte_t>& resource_shader,
        const DataView<ubyte_t>& resource_logo,
        const cstr_t font_file) noexcept
    {
        assert(device != nullptr);

        if (const BYTE return_opcode = 0xC3;
            !_open_chat_patch.Initialize(Addresses::Instance().OpenChatFunction(), &return_opcode, false) ||
            !_open_scoreboard_patch.Initialize(Addresses::Instance().OpenScoreboardFunction(), &return_opcode, false) ||
            !_switch_mode_patch.Initialize(Addresses::Instance().SwitchModeFunction(), &return_opcode, false))
        {
            Logger::Instance().LogToFile("[sv:err:pluginmenu:initialize] failed to initialize patches");
            _open_chat_patch.Deinitialize();
            _open_scoreboard_patch.Deinitialize();
            _switch_mode_patch.Deinitialize();
            return false;
        }

        ScopeExit patches_scope
        {
            [this]() noexcept -> void
            {
                _open_chat_patch.Deinitialize();
                _open_scoreboard_patch.Deinitialize();
                _switch_mode_patch.Deinitialize();
            }
        };

        if (!_blur_effect.Initialize(device, resource_shader) ||
            !_logo_texture.Initialize(device, resource_logo))
        {
            Logger::Instance().LogToFile("[sv:err:pluginmenu:initialize] failed to initialize resources");
            _blur_effect.Deinitialize();
            _logo_texture.Deinitialize();
            return false;
        }

        ScopeExit resources_scope
        {
            [this]() noexcept -> void
            {
                _blur_effect.Deinitialize();
                _logo_texture.Deinitialize();
            }
        };

        ImGui::StyleColorsClassic();

        if (float window_padding_x, window_padding_y;
            Render::Instance().ConvertBaseXValueToScreenXValue(kBaseMenuPaddingX, window_padding_x) &&
            Render::Instance().ConvertBaseYValueToScreenYValue(kBaseMenuPaddingY, window_padding_y))
        {
            ImGui::GetStyle().WindowPadding = { window_padding_x, window_padding_y };
        }

        if (float frame_padding_x, frame_padding_y;
            Render::Instance().ConvertBaseXValueToScreenXValue(kBaseMenuFramePaddingX, frame_padding_x) &&
            Render::Instance().ConvertBaseYValueToScreenYValue(kBaseMenuFramePaddingY, frame_padding_y))
        {
            ImGui::GetStyle().FramePadding = { frame_padding_x, frame_padding_y };
        }

        if (float item_spacing_x, item_spacing_y;
            Render::Instance().ConvertBaseXValueToScreenXValue(kBaseMenuItemSpacingX, item_spacing_x) &&
            Render::Instance().ConvertBaseYValueToScreenYValue(kBaseMenuItemSpacingY, item_spacing_y))
        {
            ImGui::GetStyle().ItemSpacing = { item_spacing_x, item_spacing_y };
        }

        if (float item_inner_spacing_x, item_inner_spacing_y;
            Render::Instance().ConvertBaseXValueToScreenXValue(kBaseMenuItemInnerSpacingX, item_inner_spacing_x) &&
            Render::Instance().ConvertBaseYValueToScreenYValue(kBaseMenuItemInnerSpacingY, item_inner_spacing_y))
        {
            ImGui::GetStyle().ItemInnerSpacing = { item_inner_spacing_x, item_inner_spacing_y };
        }

        if (float rounding; Render::Instance().ConvertBaseXValueToScreenXValue(kBaseMenuRounding, rounding))
        {
            ImGui::GetStyle().FrameRounding = rounding;
            ImGui::GetStyle().ScrollbarRounding = rounding;
            ImGui::GetStyle().ChildRounding = rounding;
            ImGui::GetStyle().PopupRounding = rounding;
            ImGui::GetStyle().GrabRounding = rounding;
            ImGui::GetStyle().TabRounding = rounding;
        }

        ImGui::GetStyle().WindowRounding = 0;
        ImGui::GetStyle().WindowBorderSize = 0;
        ImGui::GetStyle().FrameBorderSize = 0;
        ImGui::GetStyle().ChildBorderSize = 0;
        ImGui::GetStyle().PopupBorderSize = 0;
        ImGui::GetStyle().TabBorderSize = 0;
        ImGui::GetStyle().AntiAliasedFill = true;
        ImGui::GetStyle().AntiAliasedLines = true;
        ImGui::GetStyle().GrabMinSize = 20;
        ImGui::GetStyle().ScrollbarSize = 10;
        ImGui::GetStyle().Colors[ImGuiCol_Text] = { 0.94, 0.94, 0.94, 1 };
        ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = { 0.3, 0.3, 0.3, 0.2 };
        ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] = { 0.4, 0.4, 0.4, 0.4 };
        ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive] = { 0.5, 0.5, 0.5, 0.6 };
        ImGui::GetStyle().Colors[ImGuiCol_Button] = { 0.3, 0.3, 0.3, 0.2 };
        ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = { 0.4, 0.4, 0.4, 0.4 };
        ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = { 0.5, 0.5, 0.5, 0.6 };
        ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = { 0.3, 0.3, 0.3, 0.2 };
        ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive] = { 0.5, 0.5, 0.5, 0.6 };

        ImGui::GetIO().Fonts->AddFontDefault();

        {
            float title_font_size;

            if (!Render::Instance().ConvertBaseYValueToScreenYValue(kBaseFontTitleSize, title_font_size))
            {
                Logger::Instance().LogToFile("[sv:err:pluginmenu:initialize] failed to convert title font size");
                return false;
            }

            _title_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(font_file,
                title_font_size, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
            if (_title_font == nullptr)
            {
                Logger::Instance().LogToFile("[sv:err:pluginmenu:initialize] failed to create title font");
                return false;
            }
        }

        /*
            ScopeExit title_font_scope
            {
                [this]() noexcept -> void
                {
                    delete _title_font;
                    _title_font = nullptr;
                }
            };
        */

        {
            float tab_font_size;

            if (!Render::Instance().ConvertBaseYValueToScreenYValue(kBaseFontTabSize, tab_font_size))
            {
                Logger::Instance().LogToFile("[sv:err:pluginmenu:initialize] failed to convert tab font size");
                return false;
            }

            _tab_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(font_file,
                tab_font_size, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
            if (_tab_font == nullptr)
            {
                Logger::Instance().LogToFile("[sv:err:pluginmenu:initialize] failed to create tab font");
                return false;
            }
        }

        /*
            ScopeExit tab_font_scope
            {
                [this]() noexcept -> void
                {
                    delete _tab_font;
                    _tab_font = nullptr;
                }
            };
        */

        {
            float desc_font_size;

            if (!Render::Instance().ConvertBaseYValueToScreenYValue(kBaseFontDescSize, desc_font_size))
            {
                Logger::Instance().LogToFile("[sv:err:pluginmenu:initialize] failed to convert description font size");
                return false;
            }

            _desc_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(font_file,
                desc_font_size, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
            if (_desc_font == nullptr)
            {
                Logger::Instance().LogToFile("[sv:err:pluginmenu:initialize] failed to create description font");
                return false;
            }
        }

        /*
            ScopeExit desc_font_scope
            {
                [this]() noexcept -> void
                {
                    delete _desc_font;
                    _desc_font = nullptr;
                }
            };
        */

        {
            float font_size;

            if (!Render::Instance().ConvertBaseYValueToScreenYValue(kBaseFontSize, font_size))
            {
                Logger::Instance().LogToFile("[sv:err:pluginmenu:initialize] failed to convert default font size");
                return false;
            }

            _def_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(font_file,
                font_size, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
            if (_def_font == nullptr)
            {
                Logger::Instance().LogToFile("[sv:err:pluginmenu:initialize] failed to create default font");
                return false;
            }
        }

        _blur_level = 0;
        _blur_level_deviation = 0;
        _is_showed = false;

        SyncOptions();

        patches_scope.Release();
        resources_scope.Release();

        /*
            title_font_scope.Release();
            tab_font_scope.Release();
            desc_font_scope.Release();
        */

        return true;
    }

    void Deinitialize() noexcept
    {
        Hide();

        _logo_texture.Deinitialize();
        _blur_effect.Deinitialize();

        // delete _title_font;
        _title_font = nullptr;
        // delete _tab_font;
        _tab_font = nullptr;
        // delete _desc_font;
        _desc_font = nullptr;
        // delete _def_font;
        _def_font = nullptr;

        _open_chat_patch.Deinitialize();
        _open_scoreboard_patch.Deinitialize();
        _switch_mode_patch.Deinitialize();

        _prev_chat_mode = SAMP::CChat::Normal;
        _blur_level_deviation = 0;
        _blur_level = 0;

        _is_micro_moving = false;
    }

public:

    bool Show() noexcept
    {
        if (_is_showed) return false;
        if (game_utils::is_menu_active())
            return false;

        _open_chat_patch.Enable();
        _open_scoreboard_patch.Enable();
        _switch_mode_patch.Enable();

        _blur_level_deviation = kBlurLevelIncrement;

        if (const auto pChat = SAMP::pChat(); pChat != nullptr)
        {
            _prev_chat_mode = pChat->m_nMode;
            pChat->m_nMode = SAMP::CChat::Off;
        }

        if (const auto pScoreboard = SAMP::pScoreboard();
            pScoreboard != nullptr && pScoreboard->m_bIsEnabled != FALSE)
        {
            pScoreboard->m_bIsEnabled = FALSE;
            pScoreboard->Close(true);
        }

        if (const auto pInputBox = SAMP::pInputBox();
            pInputBox != nullptr && pInputBox->m_bEnabled != FALSE)
        {
            pInputBox->m_bEnabled = FALSE;
            *pInputBox->m_szInput = '\0';
            pInputBox->Close();
        }

        SyncOptions();
        _is_showed = true;

        return true;
    }

    bool IsShowed() const noexcept
    {
        return _is_showed;
    }

    void Hide() noexcept
    {
        if (!_is_showed) return;

        Samp::Instance().ToggleSampCursor(0);

        _blur_level_deviation = kBlurLevelDecrement;

        if (const auto pChat = SAMP::pChat(); pChat != nullptr)
            pChat->m_nMode = _prev_chat_mode;

        _switch_mode_patch.Disable();
        _open_scoreboard_patch.Disable();
        _open_chat_patch.Disable();

        ImGui::SetWindowFocus(nullptr);

        *_buffer = '\0';
        _is_device_checking = false;
        Speaker::Instance().StopChecking();

        _is_showed = false;
    }

public:

    void Render() noexcept
    {
        if (_blur_level > 0)
        {
            _blur_effect.Render(_blur_level);
        }

        if (!_is_showed) return;

        float window_width, window_height;

        if (!Render::Instance().ConvertBaseXValueToScreenXValue(kBaseMenuWidth, window_width) ||
            !Render::Instance().ConvertBaseYValueToScreenYValue(kBaseMenuHeight, window_height))
            return;

        float tab_width, tab_height;

        if (!Render::Instance().ConvertBaseXValueToScreenXValue(kBaseTabWidth, tab_width) ||
            !Render::Instance().ConvertBaseYValueToScreenYValue(kBaseTabHeight, tab_height))
            return;

        Samp::Instance().ToggleSampCursor(2);

        ImGuiUtils::Instance().StartRendering();

        ImGui::SetNextWindowSize({ window_width, window_height });
        ImGui::SetNextWindowPosCenter();

        if (ImGui::Begin("config_window", nullptr,
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize))
        {
            // Title rendering...
            // -------------------------------

            ImGui::PushFont(_title_font);

            {
                const cstr_t menu_title = Localization::Instance().GetCurrentLanguage()[Localization::MenuTitle].c_str();

                ImGui::Text(menu_title);

                ImGui::SameLine(ImGui::GetWindowWidth() - (4 * ImGui::CalcTextSize(menu_title).y +
                    (tab_width - 4 * ImGui::CalcTextSize(menu_title).y) / 2 + ImGui::GetStyle().WindowPadding.x));

                ImGui::Image(_logo_texture.Handle(), { 4 * ImGui::CalcTextSize(menu_title).y,
                    ImGui::CalcTextSize(menu_title).y });
            }

            ImGui::PopFont();
            ImGui::NewLine();

            // Tabs rendering...
            // -------------------------------

            ImGui::PushFont(_tab_font);

            if (ImGui::Button(Localization::Instance().GetCurrentLanguage()[Localization::MenuCommon].c_str(),
                { tab_width, tab_height })) _selected_tab = 0;
            ImGui::SameLine(ImGui::GetStyle().WindowPadding.x + 1 * (tab_width + kBaseTabPadding));
            if (ImGui::Button(Localization::Instance().GetCurrentLanguage()[Localization::MenuMicrophone].c_str(),
                { tab_width, tab_height })) _selected_tab = 1;
            ImGui::SameLine(ImGui::GetStyle().WindowPadding.x + 2 * (tab_width + kBaseTabPadding));
            if (ImGui::Button(Localization::Instance().GetCurrentLanguage()[Localization::MenuBlacklist].c_str(),
                { tab_width, tab_height })) _selected_tab = 2;

            ImGui::PopFont();

            // Description rendering...
            // -------------------------------

            ImGui::PushFont(_def_font);

            switch (_selected_tab)
            {
                case 0:
                {
                    // Common settings rendering...
                    // -------------------------------

                    ImGui::NewLine();

                    if (ImGui::BeginCombo(Localization::Instance().GetCurrentLanguage()
                        [Localization::MenuCommonLanguage].c_str(),
                        Localization::Instance().GetCurrentLanguage()[Localization::Language].c_str()))
                    {
                        for (size_t i = 0; i != Localization::Instance().GetLanguages().size(); ++i)
                        {
                            if (ImGui::Selectable(Localization::Instance().GetLanguages()[i][Localization::Language].c_str(),
                                i == Localization::Instance().GetLanguage())) Localization::Instance().SetLanguage(i);
                            if (i == Localization::Instance().GetLanguage()) ImGui::SetItemDefaultFocus();
                        }

                        ImGui::EndCombo();
                    }

                    const float item_width = ImGui::GetCursorPosX() - (ImGui::CalcTextSize
                    (Localization::Instance().GetCurrentLanguage()[Localization::MenuCommonLanguage].c_str(),
                        NULL, true).x - ImGui::GetStyle().ItemInnerSpacing.x);

                    ImGui::NewLine();
                    ImGui::PushFont(_desc_font);
                    ImGui::Text(Localization::Instance().GetCurrentLanguage()
                        [Localization::MenuCommonSound].c_str());
                    ImGui::Separator();
                    ImGui::PopFont();
                    ImGui::NewLine();

                    if (ImGui::Checkbox(Localization::Instance().GetCurrentLanguage()
                        [Localization::MenuCommonSoundEnable].c_str(), &_is_sound_enable))
                    {
                        // Enabling/Disabling sound
                        Listener::Instance().SetSoundEnable(_is_sound_enable);
                    }

                    if (_is_sound_enable)
                    {
                        ImGui::SameLine();
                        ImGui::PushItemWidth(item_width - ImGui::GetCursorPosX());

                        if (ImGui::SliderInt(Localization::Instance().GetCurrentLanguage()
                            [Localization::MenuCommonSoundVolume].c_str(), &_sound_volume, 0, 100))
                        {
                            // Setting volume
                            Listener::Instance().SetSoundVolume(_sound_volume);
                        }

                        ImGui::PopItemWidth();
                    }

                    ImGui::NewLine();
                    ImGui::PushFont(_desc_font);
                    ImGui::Text(Localization::Instance().GetCurrentLanguage()
                        [Localization::MenuCommonEffects].c_str());
                    ImGui::Separator();
                    ImGui::PopFont();
                    ImGui::NewLine();

                    if (ImGui::Checkbox(Localization::Instance().GetCurrentLanguage()
                        [Localization::MenuCommonEffectsBalancer].c_str(), &_is_sound_balancer))
                    {
                        // Enabling/Disabling sound balancer
                        Listener::Instance().SetSoundBalancer(_is_sound_balancer);
                    }

                    ImGui::SameLine();

                    if (ImGui::Checkbox(Localization::Instance().GetCurrentLanguage()
                        [Localization::MenuCommonEffectsFilter].c_str(), &_is_sound_filter))
                    {
                        // Enabling/Disabling sound filter
                        Listener::Instance().SetSoundFilter(_is_sound_filter);
                    }

                    ImGui::NewLine();
                    ImGui::PushFont(_desc_font);
                    ImGui::Text(Localization::Instance().GetCurrentLanguage()
                        [Localization::MenuCommonIcon].c_str());
                    ImGui::Separator();
                    ImGui::PopFont();
                    ImGui::NewLine();

                    if (ImGui::SliderFloat(Localization::Instance().GetCurrentLanguage()
                        [Localization::MenuCommonIconScale].c_str(), &_speaker_icon_scale, 0.2, 2.0))
                        SpeakerList::Instance().SetSpeakerIconScale(_speaker_icon_scale);

                    if (ImGui::SliderInt2(Localization::Instance().GetCurrentLanguage()
                        [Localization::MenuCommonIconOffset].c_str(), _speaker_icon_offset, -500, 500))
                        SpeakerList::Instance().SetSpeakerIconOffset(_speaker_icon_offset[0], _speaker_icon_offset[1]);

                    const ImVec2 reset_button_size = ImGui::GetItemRectSize();

                    ImGui::NewLine();
                    ImGui::PushFont(_desc_font);
                    ImGui::Text(Localization::Instance().GetCurrentLanguage()
                        [Localization::MenuReset].c_str());
                    ImGui::Separator();
                    ImGui::PopFont();
                    ImGui::NewLine();

                    if (ImGui::Button(Localization::Instance().GetCurrentLanguage()
                        [Localization::MenuResetButton].c_str(), reset_button_size))
                    {
                        SpeakerList::Instance().ResetConfigs();
                        SpeakerList::Instance().SyncConfigs();
                        Listener::Instance().ResetConfigs();
                        Listener::Instance().SyncConfigs();
                        SyncOptions();
                    }

                    break;
                }
                case 1:
                {
                    // Micro settings rendering...
                    // -------------------------------

                    ImGui::NewLine();
                    ImGui::PushFont(_desc_font);
                    ImGui::Text(Localization::Instance().GetCurrentLanguage()
                        [Localization::MenuMicrophoneDevice].c_str());
                    ImGui::Separator();
                    ImGui::PopFont();
                    ImGui::NewLine();

                    if (const auto& devices = Speaker::Instance().Devices(); !devices.empty())
                    {
                        if (ImGui::Checkbox(Localization::Instance().GetCurrentLanguage()
                            [Localization::MenuMicrophoneDeviceEnable].c_str(), &_is_micro_enable))
                        {
                            // Enabling/Disabling microphone
                            Speaker::Instance().SetMicroEnable(_is_micro_enable);

                            if (!_is_micro_enable && _is_device_checking)
                                _is_device_checking = false;
                        }

                        if (_is_micro_enable)
                        {
                            if (ImGui::SliderInt(Localization::Instance().GetCurrentLanguage()
                                [Localization::MenuMicrophoneDeviceVolume].c_str(), &_micro_volume, 0, 100))
                            {
                                // Setting volume micro
                                Speaker::Instance().SetMicroVolume(_micro_volume);
                            }

                            if (ImGui::BeginCombo(Localization::Instance().GetCurrentLanguage()
                                [Localization::MenuMicrophoneDeviceSelect].c_str(), devices[_selected_device].first.c_str()))
                            {
                                for (size_t i = 0; i != devices.size(); ++i)
                                {
                                    if (ImGui::Selectable(devices[i].first.c_str(), i == _selected_device))
                                        Speaker::Instance().SetMicroDevice(_selected_device = i);

                                    if (i == _selected_device)
                                        ImGui::SetItemDefaultFocus();
                                }

                                ImGui::EndCombo();
                            }

                            if (ImGui::Checkbox(Localization::Instance().GetCurrentLanguage()
                                [Localization::MenuMicrophoneDeviceCheck].c_str(), &_is_device_checking))
                            {
                                if (_is_device_checking) Speaker::Instance().StartChecking();
                                else Speaker::Instance().StopChecking();
                            }
                        }
                    }
                    else
                    {
                        ImGui::TextDisabled(Localization::Instance().GetCurrentLanguage()
                            [Localization::MenuMicrophoneNotFound].c_str());
                    }

                    ImGui::NewLine();
                    ImGui::PushFont(_desc_font);
                    ImGui::Text(Localization::Instance().GetCurrentLanguage()
                        [Localization::MenuMicrophoneIcon].c_str());
                    ImGui::Separator();
                    ImGui::PopFont();
                    ImGui::NewLine();

                    if (ImGui::SliderFloat(Localization::Instance().GetCurrentLanguage()
                        [Localization::MenuMicrophoneIconScale].c_str(), &_micro_icon_scale, 0.2, 2.0))
                    {
                        // Setting scale micro icon
                        MicroIcon::Instance().SetMicroIconScale(_micro_icon_scale);
                    }

                    if (float screen_width = 0, screen_height = 0;
                        Render::Instance().GetScreenSize(screen_width, screen_height))
                    {
                        if (ImGui::SliderInt(Localization::Instance().GetCurrentLanguage()
                            [Localization::MenuMicrophoneIconPositionX].c_str(), &_micro_icon_position_x, 0, screen_width))
                            MicroIcon::Instance().SetMicroIconPositionX(_micro_icon_position_x);
                        if (ImGui::SliderInt(Localization::Instance().GetCurrentLanguage()
                            [Localization::MenuMicrophoneIconPositionY].c_str(), &_micro_icon_position_y, 0, screen_height))
                            MicroIcon::Instance().SetMicroIconPositionY(_micro_icon_position_y);
                    }

                    if (ImGui::Button(Localization::Instance().GetCurrentLanguage()
                        [Localization::MenuMicrophoneIconMove].c_str(), ImGui::GetItemRectSize()))
                        _is_micro_moving = true;

                    const auto reset_button_size = ImGui::GetItemRectSize();

                    ImGui::NewLine();
                    ImGui::PushFont(_desc_font);
                    ImGui::Text(Localization::Instance().GetCurrentLanguage()[Localization::MenuReset].c_str());
                    ImGui::Separator();
                    ImGui::PopFont();
                    ImGui::NewLine();

                    if (ImGui::Button(Localization::Instance().GetCurrentLanguage()
                        [Localization::MenuResetButton].c_str(), reset_button_size))
                    {
                        MicroIcon::Instance().ResetConfigs();
                        MicroIcon::Instance().SyncConfigs();
                        Speaker::Instance().ResetConfigs();
                        Speaker::Instance().SyncConfigs();
                        SyncOptions();
                    }

                    break;
                }
                case 2:
                {
                    // Blacklist rendering...
                    // -------------------------------

                    ImGui::NewLine();
                    ImGui::PushFont(_desc_font);
                    ImGui::Text(Localization::Instance().GetCurrentLanguage()
                        [Localization::MenuBlacklistFilter].c_str());
                    ImGui::Separator();
                    ImGui::PopFont();
                    ImGui::NewLine();

                    const cstr_t placeholder = Localization::Instance().GetCurrentLanguage()
                        [Localization::MenuBlacklistFilterPlaceholder].c_str();

                    ImGui::PushItemWidth(-1);
                    const ImVec2 old_cursor_position = ImGui::GetCursorPos();
                    ImGui::InputText("##label", _buffer, sizeof(_buffer));
                    if (!ImGui::IsItemActive() && *_buffer == '\0')
                    {
                        const ImVec2 new_cursor_position = ImGui::GetCursorPos();
                        const float input_text_height = new_cursor_position.y - old_cursor_position.y;
                        ImGui::SetCursorPosX(old_cursor_position.x + 10);
                        ImGui::SetCursorPosY(old_cursor_position.y + (input_text_height -
                            ImGui::CalcTextSize(placeholder).y) / 2);
                        ImGui::TextColored({ 0.6, 0.6, 0.6, 0.8 }, placeholder);
                        ImGui::SetCursorPos(new_cursor_position);
                    } ImGui::PopItemWidth();
                    ImGui::NewLine();
                    ImGui::Separator();
                    ImGui::NewLine();

                    const float list_width = (ImGui::GetWindowWidth() - 4 * ImGui::GetStyle().WindowPadding.x) / 2;

                    uword_t target_id;

                    if (_snscanf_s(_buffer, sizeof(_buffer), "%hu", &target_id) == 0)
                        target_id = None<uword_t>;

                    ImGui::PushItemWidth(list_width);
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

                    // Rendering all players list...
                    // ---------------------------------------

                    ImGui::BeginGroup();

                    ImGui::Text(Localization::Instance().GetCurrentLanguage()[Localization::MenuBlacklistOnline].c_str());
                    ImGui::GetWindowDrawList()->AddLine(ImGui::GetCursorScreenPos(),
                        { ImGui::GetCursorScreenPos().x + list_width, ImGui::GetCursorScreenPos().y }, 0xFF808080);

                    ImGui::NewLine();

                    float list_height = (ImGui::GetWindowHeight() - ImGui::GetCursorPosY()) -
                        ImGui::GetStyle().WindowPadding.y;

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

                    if (ImGui::BeginChildFrame(1, { list_width, list_height },
                        ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoFocusOnAppearing |
                        ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoSavedSettings |
                        ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoResize))
                    {
                        if (const auto pNetGame = SAMP::pNetGame(); pNetGame != nullptr)
                        {
                            if (const auto pPlayerPool = pNetGame->GetPlayerPool(); pPlayerPool != nullptr)
                            {
                                for (uword_t player_id = 0; player_id != MAX_PLAYERS; ++player_id)
                                {
                                    if (pPlayerPool->IsConnected(player_id) == FALSE)
                                        continue;

                                    if (const auto player_name = pPlayerPool->GetName(player_id); player_name != nullptr)
                                    {
                                        if ((*_buffer == '\0' || (target_id != None<uword_t> ?
                                            player_id == target_id : std::strstr(player_name, _buffer) != nullptr)) &&
                                            !BlackList::Instance().IsPlayerBlocked(player_id))
                                        {
                                            ImGui::PushID(player_id);

                                            const ImVec2 old_cursor_position = ImGui::GetCursorPos();

                                            if (ImGui::Button("##label", { list_width, ImGui::GetFontSize() + 2 }))
                                            {
                                                // Add player black list
                                                BlackList::Instance().LockPlayer(player_id);
                                            }

                                            ImGui::SetCursorPos({ old_cursor_position.x + 5, old_cursor_position.y + 1 });
                                            if (auto player = SAMP::pNetGame()->GetPlayerPool()->GetPlayer(player_id))
                                                ImGui::TextColored(ImGui::ColorConvertU32ToFloat4
                                                (player->GetColorAsRGBA() | 0xFF000000), "(%hu) %s", player_id, player_name);
                                            else ImGui::Text("(%hu) %s", player_id, player_name);

                                            ImGui::PopID();
                                        }
                                    }
                                }
                            }
                        }

                        ImGui::EndChildFrame();
                    }

                    ImGui::PopStyleVar();
                    ImGui::PopStyleVar();

                    ImGui::EndGroup();

                    // ---------------------------------------

                    ImGui::SameLine(list_width + 3 * ImGui::GetStyle().WindowPadding.x);

                    // Rendering blocked players list...
                    // ---------------------------------------

                    ImGui::BeginGroup();

                    ImGui::Text(Localization::Instance().GetCurrentLanguage()[Localization::MenuBlacklistLocked].c_str());
                    ImGui::GetWindowDrawList()->AddLine(ImGui::GetCursorScreenPos(),
                        { ImGui::GetCursorScreenPos().x + list_width, ImGui::GetCursorScreenPos().y }, 0xFF808080);

                    ImGui::NewLine();

                    list_height = (ImGui::GetWindowHeight() - ImGui::GetCursorPosY()) - ImGui::GetStyle().WindowPadding.y;

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

                    if (ImGui::BeginChildFrame(2, { list_width, list_height },
                        ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoFocusOnAppearing |
                        ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoSavedSettings |
                        ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoResize))
                    {
                        ForwardForEach(iterator, BlackList::Instance().LockedPlayers())
                        {
                            if (!(*_buffer == '\0' || (target_id != None<uword_t> ? iterator->id == target_id :
                                static_cast<bool>(std::strstr(iterator->name, _buffer))))) continue;

                            const ImVec2 old_cursor_position = ImGui::GetCursorPos();
                            const ImVec2 old_cursor_screen_position = ImGui::GetCursorScreenPos();

                            ImGui::PushID(iterator);

                            if (ImGui::Button("##label", { list_width, ImGui::GetFontSize() + 2 }))
                            {
                                // Remove player from black list
                                BlackList::Instance().UnlockPlayer(iterator->name);
                            }

                            ImGui::PopID();

                            ImGui::SetCursorPos({ old_cursor_position.x + 5, old_cursor_position.y + 1 });

                            if (iterator->id != None<uword_t>)
                            {
                                if (auto player = SAMP::pNetGame()->GetPlayerPool()->GetPlayer(iterator->id))
                                    ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(player->GetColorAsRGBA() | 0xFF000000),
                                        "(%hu) %s", iterator->id, iterator->name);
                                else ImGui::Text("(%hu) %s", iterator->id, iterator->name);
                            }
                            else ImGui::TextDisabled("%s", iterator->name);

                            const ImVec2 online_circle_position { (old_cursor_screen_position.x + list_width) - (ImGui::GetFontSize() / 2 + 2 +
                                ImGui::GetStyle().ScrollbarSize), old_cursor_screen_position.y + (ImGui::GetFontSize() / 2 + 1) };

                            if (iterator->id != None<uword_t>)
                                ImGui::GetWindowDrawList()->AddCircleFilled(online_circle_position, ImGui::GetFontSize() / 4, 0xFF7DFE3F);
                            else ImGui::GetWindowDrawList()->AddCircle(online_circle_position, ImGui::GetFontSize() / 4, 0xFF808080);
                        }

                        ImGui::EndChildFrame();
                    }

                    ImGui::PopStyleVar();
                    ImGui::PopStyleVar();

                    ImGui::EndGroup();

                    // ---------------------------------------

                    ImGui::PopStyleVar();
                    ImGui::PopItemWidth();

                    break;
                }
            }

            ImGui::PopFont();
            ImGui::End();
        }

        ImGuiUtils::Instance().StopRendering();
    }

    void Update() noexcept
    {
        if (_blur_level_deviation != 0)
        {
            _blur_level += _blur_level_deviation;
            if (_blur_level < 0)
            {
                _blur_level_deviation = 0;
                _blur_level = 0;
            }
            if (_blur_level > 100)
            {
                _blur_level_deviation = 0;
                _blur_level = 100;
            }
        }

        if (_is_micro_moving)
        {
            static bool is_moving_started = false;
            static int old_mouse_position_x = 0, old_mouse_position_y = 0;

            if (!is_moving_started)
            {
                Hide();

                old_mouse_position_x = MicroIcon::Instance().GetMicroIconPositionX();
                old_mouse_position_y = MicroIcon::Instance().GetMicroIconPositionY();

                is_moving_started = true;
            }

            Samp::Instance().ToggleSampCursor(2);

            const ImVec2 mouse_position = ImGui::GetMousePos();

            MicroIcon::Instance().SetMicroIconPosition(mouse_position.x, mouse_position.y);

            if (game_utils::is_key_pressed(0x01) || game_utils::is_key_pressed(0x02) || game_utils::is_key_pressed(0x1B))
            {
                if (game_utils::is_key_pressed(0x02) || game_utils::is_key_pressed(0x1B))
                {
                    MicroIcon::Instance().SetMicroIconPosition(old_mouse_position_x, old_mouse_position_y);
                }

                is_moving_started = false;
                _is_micro_moving = false;

                Show();
            }
        }
    }

public:

    LRESULT WindowProcedure(const HWND window, const UINT message,
        const WPARAM wparam, const LPARAM lparam) noexcept
    {
        if (message == WM_KEYDOWN && static_cast<BYTE>(wparam) == 0x7A) // F11 key
            return (IsShowed() ? Hide() : Show()), 1;

        if (_is_showed == false && _is_micro_moving == false)
            return 0;

        if (message == WM_KEYDOWN && static_cast<BYTE>(wparam) == 0x1B) // ESC key
            return (Hide()), 1;

        return ImGuiUtils::Instance().WindowProcedure(window, message, wparam, lparam);
    }

private:

    void SyncOptions() noexcept
    {
        _is_sound_enable   = Listener::Instance().IsSoundEnable();
        _sound_volume      = Listener::Instance().GetSoundVolume();
        _is_sound_balancer = Listener::Instance().IsSoundBalancer();
        _is_sound_filter   = Listener::Instance().IsSoundFilter();

        _speaker_icon_scale     = SpeakerList::Instance().GetSpeakerIconScale();
        _speaker_icon_offset[0] = SpeakerList::Instance().GetSpeakerIconOffsetX();
        _speaker_icon_offset[1] = SpeakerList::Instance().GetSpeakerIconOffsetY();

        _is_micro_enable = Speaker::Instance().IsMicroEnable();
        _micro_volume    = Speaker::Instance().GetMicroVolume();
        _selected_device = Speaker::Instance().GetMicroDevice();

        _micro_icon_scale      = MicroIcon::Instance().GetMicroIconScale();
        _micro_icon_position_x = MicroIcon::Instance().GetMicroIconPositionX();
        _micro_icon_position_y = MicroIcon::Instance().GetMicroIconPositionY();
        _micro_icon_color      = MicroIcon::Instance().GetMicroIconColor();
        _micro_icon_angle      = MicroIcon::Instance().GetMicroIconAngle();
    }

private:

    bool _is_showed = false;

    float      _blur_level           = 0;
    float      _blur_level_deviation = 0;
    BlurEffect _blur_effect;

    Texture _logo_texture;

    ImFont* _title_font = nullptr;
    ImFont* _tab_font   = nullptr;
    ImFont* _desc_font  = nullptr;
    ImFont* _def_font   = nullptr;

    Patch<sizeof(BYTE)> _open_chat_patch;
    Patch<sizeof(BYTE)> _open_scoreboard_patch;
    Patch<sizeof(BYTE)> _switch_mode_patch;

    int _prev_chat_mode = 0;

    // Configs
    // ------------------------------------------------------------------------------------------

    bool     _is_sound_enable   = false;
    int      _sound_volume      = 0;
    bool     _is_sound_balancer = false;
    bool     _is_sound_filter   = false;

    float    _speaker_icon_scale     = 0;
    int      _speaker_icon_offset[2] = {};

    bool     _is_micro_enable = false;
    int      _micro_volume    = 0;
    size_t   _selected_device = 0;

    float    _micro_icon_scale       = 0;
    int      _micro_icon_position_x  = 0;
    int      _micro_icon_position_y  = 0;
    D3DCOLOR _micro_icon_color       = 0;
    float    _micro_icon_angle       = 0;

    // Internal options
    // ------------------------------------------------------------------------------------------

    int  _selected_tab       = 0;
    bool _is_device_checking = false;
    bool _is_micro_moving    = false;
    char _buffer[64];

};
