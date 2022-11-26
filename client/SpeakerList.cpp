/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "SpeakerList.h"

#include <cassert>
#include <algorithm>

#include <game/CPed.h>
#include <game/CSprite.h>
#include <game/CCamera.h>
#include <util/ImGuiUtil.h>
#include <util/GameUtil.h>
#include <util/Logger.h>
#include <util/Render.h>

#include "PluginConfig.h"

bool SpeakerList::Init(IDirect3DDevice9* const device, const Resource& resource_speaker_icon,
    const Resource& resource_speaker_font) noexcept
{
    if (device == nullptr) return false;

    if (_init_status || !ImGuiUtil::IsInited()) return false;

    _speaker_icon = Texture(device, resource_speaker_icon);
    if (!_speaker_icon.Valid())
    {
        Logger::LogToFile("[sv:err:speakerlist:init] : failed to create speaker icon");
        return false;
    }

    {
        float font_size = 0.f;

        if (!Render::ConvertBaseYValueToScreenYValue(kBaseFontSize, font_size))
        {
            Logger::LogToFile("[sv:err:speakerlist:init] : failed to convert font size");
            _speaker_icon = {};
            return false;
        }

        _speaker_font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(resource_speaker_font.GetData(),
            resource_speaker_font.GetSize(), font_size, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        if (_speaker_font == nullptr)
        {
            Logger::LogToFile("[sv:err:speakerlist:init] : failed to create speaker font");
            _speaker_icon = {};
            return false;
        }
    }

    if (!PluginConfig::IsSpeakerLoaded())
    {
        PluginConfig::SetSpeakerLoaded(true);
        ResetConfigs();
    }

    _init_status = true;
    SyncConfigs();

    return true;
}

void SpeakerList::Free() noexcept
{
    if (_init_status)
    {
        _speaker_icon = {};
        delete _speaker_font;
    }

    _init_status = false;
}

void SpeakerList::Show() noexcept
{
    _show_status = true;
}

bool SpeakerList::IsShowed() noexcept
{
    return _show_status;
}

void SpeakerList::Hide() noexcept
{
    _show_status = false;
}

void SpeakerList::Render()
{
    if (_init_status && _show_status)
    {
        const auto net_game = SAMP::pNetGame();
        if (net_game == nullptr) return;

        const auto player_pool = net_game->GetPlayerPool();
        if (player_pool == nullptr) return;

        float vLeftIndent = 0.f;
        float vScrWidth   = 0.f;
        float vScrHeight  = 0.f;

        if (!Render::ConvertBaseXValueToScreenXValue(kBaseLeftIndent, vLeftIndent)) return;
        if (!Render::GetScreenSize(vScrWidth, vScrHeight)) return;

        if (!ImGuiUtil::BeginRender()) return;

        const ImVec2 vWindowPadding { 4, 8 };
        const ImVec2 vFramePadding  { 4, 8 };
        const ImVec2 vItemPadding   { 4, 8 };

        ImGui::PushFont(_speaker_font);

        const float vWidth = vScrWidth / 2.f - vLeftIndent;
        const float vHeight = 2.f * vWindowPadding.y + (kBaseLinesCount *
            (ImGui::GetTextLineHeight() + vFramePadding.y));

        const float vIconWidth = ImGui::GetTextLineHeight() + 5.f;
        const float vNickWidth = 0.2f * (vWidth - vIconWidth);
        const float vStreamsWidth = 0.8f * (vWidth - vIconWidth);

        const float vPosX = vLeftIndent;
        const float vPosY = (vScrHeight - vHeight) / 2.f;

        ImGui::SetNextWindowPos({ vPosX, vPosY });
        ImGui::SetNextWindowSize({ vWidth, vHeight });

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, vFramePadding);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, vWindowPadding);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, vItemPadding);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, vItemPadding);

        if (ImGui::Begin("speakerListWindow", nullptr,
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
            ImGui::Columns(3, nullptr, false);

            ImGui::SetColumnWidth(0, vIconWidth);
            ImGui::SetColumnWidth(1, vNickWidth);
            ImGui::SetColumnWidth(2, vStreamsWidth);

            ImGui::SetColumnOffset(0, vWindowPadding.x);
            ImGui::SetColumnOffset(1, vWindowPadding.x + vIconWidth + vFramePadding.x);
            ImGui::SetColumnOffset(2, vWindowPadding.x + vIconWidth + vFramePadding.x + vNickWidth + vFramePadding.x);

            int curTextLine = 0;

            for (WORD playerId = 0; playerId < MAX_PLAYERS; ++playerId)
            {
                if (curTextLine >= kBaseLinesCount) break;

                if (const auto playerName = player_pool->GetName(playerId); playerName != nullptr)
                {
                    if (!_player_streams[playerId].empty())
                    {
                        for (const auto& playerStream : _player_streams[playerId])
                        {
                            if (playerStream.second.GetType() == StreamType::LocalStreamAtPlayer)
                            {
                                if (GameUtil::IsPlayerVisible(playerId))
                                {
                                    if (const auto pPlayer = player_pool->GetPlayer(playerId); pPlayer != nullptr)
                                    {
                                        if (const auto pPlayerPed = pPlayer->m_pPed; pPlayerPed != nullptr)
                                        {
                                            if (const auto pGamePed = pPlayerPed->m_pGamePed; pGamePed != nullptr)
                                            {
                                                const float distanceToCamera = (TheCamera.GetPosition() - pGamePed->GetPosition()).Magnitude();

                                                float vSpeakerIconSize = 0.f;

                                                if (Render::ConvertBaseYValueToScreenYValue(kBaseIconSize, vSpeakerIconSize))
                                                {
                                                    vSpeakerIconSize *= PluginConfig::GetSpeakerIconScale();
                                                    vSpeakerIconSize *= 5.f / distanceToCamera;

                                                    float width, height;
                                                    RwV3d playerPos, screenPos;

                                                    pGamePed->GetBonePosition(playerPos, 1, false);
                                                    playerPos.z += 1.f;

                                                    if (CSprite::CalcScreenCoors(playerPos, &screenPos, &width, &height, true, true))
                                                    {
                                                        screenPos.x -= vSpeakerIconSize / 2.f;
                                                        screenPos.y -= vSpeakerIconSize / 2.f;

                                                        const float addX = PluginConfig::GetSpeakerIconOffsetX() * 5.f / distanceToCamera;
                                                        const float addY = PluginConfig::GetSpeakerIconOffsetY() * 5.f / distanceToCamera;

                                                        _speaker_icon->Draw(screenPos.x + addX, screenPos.y + addY,
                                                            vSpeakerIconSize, vSpeakerIconSize, -1, 0.f);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }

                                break;
                            }
                        }

                        ImGui::PushID(playerId);

                        const auto alphaLevel = static_cast<DWORD>((1.f - static_cast<float>(curTextLine) /
                            static_cast<float>(kBaseLinesCount)) * 255.f) << 24;

                        const auto color = ImGui::ColorConvertU32ToFloat4(0x00ffffff | alphaLevel);

                        ImGui::Image(_speaker_icon->GetTexture(), { ImGui::GetTextLineHeight(),
                            ImGui::GetTextLineHeight() }, { 0, 0 }, { 1, 1 }, color);

                        ImGui::NextColumn();

                        ImGui::TextColored(color, "%s (%hu)", playerName, playerId);

                        ImGui::NextColumn();

                        for (const auto& streamInfo : _player_streams[playerId])
                        {
                            if (streamInfo.second.GetColor() == NULL)
                                continue;

                            ImGui::PushID(&streamInfo);

                            const auto streamColor = ImGui::ColorConvertU32ToFloat4((streamInfo.second.GetColor() & 0x00ffffff) | alphaLevel);

                            ImGui::TextColored(streamColor, "[%s]", streamInfo.second.GetName().c_str());

                            ImGui::SameLine();
                            ImGui::PopID();
                        }

                        ImGui::NextColumn();
                        ImGui::PopID();

                        ++curTextLine;
                    }
                }
            }

            ImGui::End();
        }

        ImGui::PopStyleVar(4);
        ImGui::PopFont();

        ImGuiUtil::EndRender();
    }
}

int SpeakerList::GetSpeakerIconOffsetX() noexcept
{
    return PluginConfig::GetSpeakerIconOffsetX();
}

int SpeakerList::GetSpeakerIconOffsetY() noexcept
{
    return PluginConfig::GetSpeakerIconOffsetY();
}

float SpeakerList::GetSpeakerIconScale() noexcept
{
    return PluginConfig::GetSpeakerIconScale();
}

void SpeakerList::SetSpeakerIconOffsetX(const int speaker_icon_offset_x) noexcept
{
    PluginConfig::SetSpeakerIconOffsetX(std::clamp(speaker_icon_offset_x, -500, 500));
}

void SpeakerList::SetSpeakerIconOffsetY(const int speaker_icon_offset_y) noexcept
{
    PluginConfig::SetSpeakerIconOffsetY(std::clamp(speaker_icon_offset_y, -500, 500));
}

void SpeakerList::SetSpeakerIconScale(const float speaker_icon_scale) noexcept
{
    PluginConfig::SetSpeakerIconScale(std::clamp(speaker_icon_scale, 0.2f, 2.f));
}

void SpeakerList::SyncConfigs() noexcept
{
    SetSpeakerIconOffsetX(PluginConfig::GetSpeakerIconOffsetX());
    SetSpeakerIconOffsetY(PluginConfig::GetSpeakerIconOffsetY());
    SetSpeakerIconScale(PluginConfig::GetSpeakerIconScale());
}

void SpeakerList::ResetConfigs() noexcept
{
    PluginConfig::SetSpeakerIconOffsetX(PluginConfig::kDefValSpeakerIconOffsetX);
    PluginConfig::SetSpeakerIconOffsetY(PluginConfig::kDefValSpeakerIconOffsetY);
    PluginConfig::SetSpeakerIconScale(PluginConfig::kDefValSpeakerIconScale);
}

void SpeakerList::OnSpeakerPlay(const Stream& stream, const WORD speaker) noexcept
{
    if (speaker < MAX_PLAYERS)
    {
        _player_streams[speaker][&stream] = stream.GetInfo();
    }
}

void SpeakerList::OnSpeakerStop(const Stream& stream, const WORD speaker) noexcept
{
    if (speaker < MAX_PLAYERS)
    {
        _player_streams[speaker].erase(&stream);
    }
}

bool    SpeakerList::_init_status = false;
bool    SpeakerList::_show_status = false;

ImFont* SpeakerList::_speaker_font = nullptr;
Texture SpeakerList::_speaker_icon;

std::array<std::unordered_map<Stream*, StreamInfo>, MAX_PLAYERS>
        SpeakerList::_player_streams;
