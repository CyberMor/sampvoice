/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <string>
#include <map>

#include <cassert>
#include <algorithm>

#include <Windows.h>
#include <d3d9.h>

#include <imgui/imgui.h>
#include <game/CPed.h>
#include <game/CSprite.h>
#include <game/CCamera.h>
#include <samp/CNetGame.h>
#include <util/addresses.hpp>
#include <util/texture.hpp>
#include <util/imgui_utils.hpp>
#include <util/game_utils.hpp>
#include <util/logger.hpp>
#include <util/render.hpp>

#include <other/utils.hpp>

#include "main.hpp"
#include "storage.hpp"
#include "plugin_config.hpp"
#include "stream.hpp"

struct SpeakerList {

    SpeakerList(const SpeakerList&) = delete;
    SpeakerList(SpeakerList&&) = delete;
    SpeakerList& operator=(const SpeakerList&) = delete;
    SpeakerList& operator=(SpeakerList&&) = delete;

private:

    SpeakerList() = default;
    ~SpeakerList() noexcept = default;

public:

    static SpeakerList& Instance() noexcept
    {
        static SpeakerList instance;
        return instance;
    }

private:

    static constexpr int   kBaseLinesCount = 12;
    static constexpr float kBaseLeftIndent = 37;
    static constexpr float kBaseIconSize   = 36;
    static constexpr float kBaseFontSize   = 7.5;

public:

    struct ShowInfo
    {
        uword_t stream = None<uword_t>;
        uword_t target = 0;
        std::shared_ptr<Texture> icon;
    };

public:

    bool Initialize(IDirect3DDevice9* const device,
        const DataView<ubyte_t>& resource_speaker_icon, const cstr_t speaker_font_file) noexcept
    {
        assert(device != nullptr);

        if (!_speaker_icon.Initialize(device, resource_speaker_icon))
        {
            Logger::Instance().LogToFile("[sv:err:speakerlist:initialize] : failed to create speaker icon");
            return false;
        }

        {
            float font_size;

            if (!Render::Instance().ConvertBaseYValueToScreenYValue(kBaseFontSize, font_size))
            {
                Logger::Instance().LogToFile("[sv:err:speakerlist:initialize] : failed to convert font size");
                _speaker_icon.Deinitialize();
                return false;
            }
            
            _speaker_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(speaker_font_file,
                font_size, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
            if (_speaker_font == nullptr)
            {
                Logger::Instance().LogToFile("[sv:err:speakerlist:initialize] : failed to create speaker font");
                _speaker_icon.Deinitialize();
                return false;
            }
        }

        const auto callback = [this, device](const cstr_t path) noexcept -> void
        {
            constexpr const char kIconExtension[] = ".png";
            if (cstr_t iterator = path + std::strlen(path) - (sizeof(kIconExtension) - 1);
                std::memcmp(iterator, kIconExtension, sizeof(kIconExtension) - 1) == 0)
            {
                const cstr_t end = iterator;
                while (*iterator != '\\') --iterator;
                const cstr_t begin = iterator + 1;

                _icons.emplace(std::string(begin, end - begin),
                    MakeShared(Texture, device, File::Content(path).Area()));
            }
        };

        if (!Storage::Instance().ForEachResourceFile(callback))
        {
            Logger::Instance().LogToFile("[sv:err:speakerlist:initialize] : failed to initialize icons table");
            _speaker_icon.Deinitialize();
            // delete _speaker_font;
            _speaker_font = nullptr;
            return false;
        }

        SyncConfigs();
        Show();

        return true;
    }

    void Deinitialize() noexcept
    {
        _speaker_icon.Deinitialize();

        // delete _speaker_font;
        _speaker_font = nullptr;

        for (size_t player = 0; player != MAX_PLAYERS; ++player)
        {
            for (size_t i = 0; i != std::size(_table[player]) - 1; ++i)
            {
                _table[player][i].stream = None<uword_t>;
                _table[player][i].target = 0;
                _table[player][i].icon = nullptr;
            }
        }

        _icons.clear();
    }

public:

    void Show() noexcept
    {
        _is_showed = true;
    }

    bool IsShowed() noexcept
    {
        return _is_showed;
    }

    void Hide() noexcept
    {
        _is_showed = false;
    }

public:

    void Render() noexcept
    {
        if (_is_showed == false)
            return;

        const auto pNetGame = SAMP::pNetGame();
        if (pNetGame == nullptr) return;

        const auto pPlayerPool = pNetGame->GetPlayerPool();
        if (pPlayerPool == nullptr) return;

        float left_indent;
        float screen_width, screen_height;

        if (!Render::Instance().ConvertBaseXValueToScreenXValue(kBaseLeftIndent, left_indent)) return;
        if (!Render::Instance().GetScreenSize(screen_width, screen_height)) return;

        ImGuiUtils::Instance().StartRendering();

        const ImVec2 window_padding { 4, 8 };
        const ImVec2 frame_padding { 4, 8 };
        const ImVec2 item_padding { 4, 8 };

        ImGui::PushFont(_speaker_font);

        const float width = screen_width / 2 - left_indent;
        const float height = 2 * window_padding.y + (kBaseLinesCount *
            (ImGui::GetTextLineHeight() + frame_padding.y));

        const float nick_width = 0.3F * width;
        const float streams_width = 0.15F * width;

        const float position_x = left_indent;
        const float position_y = (screen_height - height) / 2;

        ImGui::SetNextWindowPos({ position_x, position_y });
        ImGui::SetNextWindowSize({ width, height });

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, frame_padding);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, window_padding);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, item_padding);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, item_padding);

        if (ImGui::Begin("speaker_list_window", nullptr,
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoInputs))
        {
            ImGui::Columns(2, nullptr, false);

            ImGui::SetColumnWidth(0, nick_width);
            ImGui::SetColumnWidth(1, streams_width);

            ImGui::SetColumnOffset(0, window_padding.x);
            ImGui::SetColumnOffset(1, window_padding.x + nick_width + frame_padding.x);

            int current_text_line = 0;

            for (uword_t player_id = 0; player_id != MAX_PLAYERS; ++player_id)
            {
                if (current_text_line >= kBaseLinesCount)
                    break;

                if (_table[player_id][0].stream != None<uword_t>)
                {
                    if (const auto player_name = pPlayerPool->GetName(player_id); player_name != nullptr)
                    {
                        for (size_t i = 0; i != std::size(_table[player_id]) - 1; ++i)
                        {
                            if (_table[player_id][i].stream != None<uword_t> &&
                                _table[player_id][i].target == MakeTarget(kPlayer, player_id))
                            {
                                if (game_utils::is_player_visible(player_id))
                                {
                                    if (const auto pPlayer = pPlayerPool->GetPlayer(player_id); pPlayer != nullptr)
                                    {
                                        if (const auto pPlayerPed = pPlayer->m_pPed; pPlayerPed != nullptr)
                                        {
                                            if (const auto pGamePed = pPlayerPed->m_pGamePed; pGamePed != nullptr)
                                            {
                                                if (float speaker_icon_size; Render::Instance().ConvertBaseYValueToScreenYValue(kBaseIconSize, speaker_icon_size))
                                                {
                                                    const float distance_to_camera = (TheCamera.GetPosition() - pGamePed->GetPosition()).Magnitude();

                                                    speaker_icon_size *= PluginConfig::Instance().GetSpeakerIconScale();
                                                    speaker_icon_size *= 5 / distance_to_camera;

                                                    float screen_width, screen_height;
                                                    RwV3d player_position, screen_position;

                                                    pGamePed->GetBonePosition(player_position, 1, false);
                                                    player_position.z += 1;

                                                    if (CSprite::CalcScreenCoors(player_position, &screen_position, &screen_width, &screen_height, true, true))
                                                    {
                                                        screen_position.x -= speaker_icon_size / 2;
                                                        screen_position.y -= speaker_icon_size / 2;

                                                        const float screen_offset_x = PluginConfig::Instance().GetSpeakerIconOffsetX() * 5 / distance_to_camera;
                                                        const float screen_offset_y = PluginConfig::Instance().GetSpeakerIconOffsetY() * 5 / distance_to_camera;

                                                        _speaker_icon.Draw(screen_position.x + screen_offset_x, screen_position.y + screen_offset_y,
                                                            speaker_icon_size, speaker_icon_size);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }

                                break;
                            }
                        }

                        ImGui::PushID(player_id);

                        const auto alpha_level = static_cast<DWORD>((1 - static_cast<float>(current_text_line) /
                            static_cast<float>(kBaseLinesCount)) * 255) << 24;

                        const auto color = ImGui::ColorConvertU32ToFloat4(0x00FFFFFF | alpha_level);

                        ImGui::TextColored(color, "%4hu %16.16s", player_id, player_name);

                        ImGui::NextColumn();

                        for (size_t i = 0; i != std::size(_table[player_id]) - 1; ++i)
                        {
                            if (_table[player_id][i].stream != None<uword_t>)
                            {
                                if (_table[player_id][i].icon != nullptr)
                                {
                                    ImGui::PushID(&_table[player_id][i]);
                                    ImGui::Image(_table[player_id][i].icon->Handle(),
                                        { ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight() });
                                    ImGui::SameLine();
                                    ImGui::PopID();
                                }
                            }
                        }

                        ImGui::NextColumn();
                        ImGui::PopID();

                        ++current_text_line;
                    }
                }
            }

            ImGui::End();
        }

        ImGui::PopStyleVar(4);
        ImGui::PopFont();

        ImGuiUtils::Instance().StopRendering();
    }

public:

    sdword_t GetSpeakerIconOffsetX() const noexcept
    {
        return PluginConfig::Instance().GetSpeakerIconOffsetX();
    }

    sdword_t GetSpeakerIconOffsetY() const noexcept
    {
        return PluginConfig::Instance().GetSpeakerIconOffsetY();
    }

    fdword_t GetSpeakerIconScale() const noexcept
    {
        return PluginConfig::Instance().GetSpeakerIconScale();
    }

public:

    void SetSpeakerIconOffsetX(const sdword_t speaker_icon_offset_x) noexcept
    {
        PluginConfig::Instance().SetSpeakerIconOffsetX(std::clamp(speaker_icon_offset_x, -500, 500));
    }

    void SetSpeakerIconOffsetY(const sdword_t speaker_icon_offset_y) noexcept
    {
        PluginConfig::Instance().SetSpeakerIconOffsetY(std::clamp(speaker_icon_offset_y, -500, 500));
    }

    void SetSpeakerIconOffset(const sdword_t speaker_icon_offset_x, const sdword_t speaker_icon_offset_y) noexcept
    {
        PluginConfig::Instance().SetSpeakerIconOffsetX(std::clamp(speaker_icon_offset_x, -500, 500));
        PluginConfig::Instance().SetSpeakerIconOffsetY(std::clamp(speaker_icon_offset_y, -500, 500));
    }

    void SetSpeakerIconScale(const fdword_t speaker_icon_scale) noexcept
    {
        PluginConfig::Instance().SetSpeakerIconScale(std::clamp(speaker_icon_scale, 0.2F, 2.0F));
    }

public:

    void SyncConfigs() noexcept
    {
        SetSpeakerIconOffsetX(PluginConfig::Instance().GetSpeakerIconOffsetX());
        SetSpeakerIconOffsetY(PluginConfig::Instance().GetSpeakerIconOffsetY());
        SetSpeakerIconScale(PluginConfig::Instance().GetSpeakerIconScale());
    }

    void ResetConfigs() noexcept
    {
        PluginConfig::Instance().SetSpeakerIconOffsetX(PluginConfig::kDefaultSpeakerIconOffsetX);
        PluginConfig::Instance().SetSpeakerIconOffsetY(PluginConfig::kDefaultSpeakerIconOffsetY);
        PluginConfig::Instance().SetSpeakerIconScale(PluginConfig::kDefaultSpeakerIconScale);
    }

public:

    bool Play(const uword_t player, const uword_t stream, const uword_t target, const std::string& icon) noexcept
    {
        assert(player < MAX_PLAYERS);

        for (size_t i = 0; i != std::size(_table[player]) - 1; ++i)
        {
            if (_table[player][i].stream == stream)
            {
                _table[player][i].target = target;
                _table[player][i].icon = nullptr;

                if (icon.empty() == false)
                {
                    if (const auto iterator = _icons.find(icon); iterator != _icons.end())
                    {
                        _table[player][i].icon = iterator->second;
                    }
                }

                return true;
            }
        }
        for (size_t i = 0; i != std::size(_table[player]) - 1; ++i)
        {
            if (_table[player][i].stream == None<uword_t>)
            {
                _table[player][i].stream = stream;
                _table[player][i].target = target;
                _table[player][i].icon = nullptr;

                if (icon.empty() == false)
                {
                    if (const auto iterator = _icons.find(icon); iterator != _icons.end())
                    {
                        _table[player][i].icon = iterator->second;
                    }
                }

                return true;
            }
        }

        return false;
    }

    bool Stop(const uword_t player, const uword_t stream) noexcept
    {
        assert(player < MAX_PLAYERS);

        for (size_t i = 0; i != std::size(_table[player]) - 1; ++i)
        {
            if (_table[player][i].stream == stream)
            {
                for (size_t j = i; j != std::size(_table[player]) - 1; ++j)
                {
                    _table[player][j].stream = std::move(_table[player][j + 1].stream);
                    _table[player][j].target = std::move(_table[player][j + 1].target);
                    _table[player][j].icon   = std::move(_table[player][j + 1].icon);
                }

                return true;
            }
        }

        return false;
    }

public:

    void UpdateTargetForStream(const uword_t stream, const uword_t target) noexcept
    {
        for (size_t player = 0; player != MAX_PLAYERS; ++player)
        {
            for (size_t info = 0; info != std::size(_table[player]) - 1; ++info)
            {
                if (_table[player][info].stream == stream)
                {
                    _table[player][info].target = target;
                }
            }
        }
    }

    void UpdateIconForStream(const uword_t stream, const std::string& icon) noexcept
    {
        std::shared_ptr<Texture> texture = nullptr;

        if (icon.empty() == false)
        {
            if (const auto iterator = _icons.find(icon); iterator != _icons.end())
            {
                texture = iterator->second;
            }
        }

        for (size_t player = 0; player != MAX_PLAYERS; ++player)
        {
            for (size_t info = 0; info != std::size(_table[player]) - 1; ++info)
            {
                if (_table[player][info].stream == stream)
                {
                    _table[player][info].icon = texture;
                }
            }
        }
    }

private:

    volatile bool _is_showed = false;

private:

    ImFont* _speaker_font = nullptr;
    Texture _speaker_icon;

private:

    std::map<std::string, std::shared_ptr<Texture>> _icons;

private:

    ShowInfo _table[MAX_PLAYERS][5] = {};

};
