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

bool SpeakerList::Init(IDirect3DDevice9* const pDevice, const AddressesBase& addrBase,
    const Resource& rSpeakerIcon, const Resource& rSpeakerFont) noexcept
{
    if (pDevice == nullptr)
        return false;

    if (SpeakerList::initStatus || !ImGuiUtil::IsInited())
        return false;

    try
    {
        SpeakerList::tSpeakerIcon = MakeTexture(pDevice, rSpeakerIcon);
    }
    catch (const std::exception& exception)
    {
        Logger::LogToFile("[sv:err:speakerlist:init] : failed to create speaker icon");
        SpeakerList::tSpeakerIcon.reset();
        return false;
    }

    Memory::ScopeExit iconResetScope { [] { SpeakerList::tSpeakerIcon.reset(); } };

    {
        float varFontSize { 0.f };

        if (!Render::ConvertBaseYValueToScreenYValue(kBaseFontSize, varFontSize))
        {
            Logger::LogToFile("[sv:err:speakerlist:init] : failed to convert font size");
            return false;
        }

        SpeakerList::pSpeakerFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(rSpeakerFont.GetDataPtr(),
            rSpeakerFont.GetDataSize(), varFontSize, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

        if (SpeakerList::pSpeakerFont == nullptr)
        {
            Logger::LogToFile("[sv:err:speakerlist:init] : failed to create speaker font");
            return false;
        }
    }

    if (!PluginConfig::IsSpeakerLoaded())
    {
        PluginConfig::SetSpeakerLoaded(true);
        SpeakerList::ResetConfigs();
    }

    iconResetScope.Release();

    SpeakerList::initStatus = true;
    SpeakerList::SyncConfigs();

    return true;
}

void SpeakerList::Free() noexcept
{
    if (!SpeakerList::initStatus)
        return;

    SpeakerList::tSpeakerIcon.reset();
    delete SpeakerList::pSpeakerFont;

    SpeakerList::initStatus = false;
}

void SpeakerList::Show() noexcept
{
    SpeakerList::showStatus = true;
}

bool SpeakerList::IsShowed() noexcept
{
    return SpeakerList::showStatus;
}

void SpeakerList::Hide() noexcept
{
    SpeakerList::showStatus = false;
}

void SpeakerList::Render()
{
    if (!SpeakerList::initStatus || !SpeakerList::showStatus)
        return;

    const auto pNetGame = SAMP::pNetGame();
    if (pNetGame == nullptr) return;

    const auto pPlayerPool = pNetGame->GetPlayerPool();
    if (pPlayerPool == nullptr) return;

    float vLeftIndent { 0.f }, vScrWidth { 0.f }, vScrHeight { 0.f };

    if (!Render::ConvertBaseXValueToScreenXValue(kBaseLeftIndent, vLeftIndent)) return;
    if (!Render::GetScreenSize(vScrWidth, vScrHeight)) return;

    if (!ImGuiUtil::BeginRender()) return;

    const ImVec2 vWindowPadding { 4, 8 };
    const ImVec2 vFramePadding { 4, 8 };
    const ImVec2 vItemPadding { 4, 8 };

    ImGui::PushFont(SpeakerList::pSpeakerFont);

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

        int curTextLine { 0 };

        for (WORD playerId { 0 }; playerId < MAX_PLAYERS; ++playerId)
        {
            if (curTextLine >= kBaseLinesCount) break;

            if (const auto playerName = pPlayerPool->GetName(playerId); playerName != nullptr)
            {
                if (!SpeakerList::playerStreams[playerId].empty())
                {
                    for (const auto& playerStream : SpeakerList::playerStreams[playerId])
                    {
                        if (playerStream.second.GetType() == StreamType::LocalStreamAtPlayer)
                        {
                            if (GameUtil::IsPlayerVisible(playerId))
                            {
                                if (const auto pPlayer = pPlayerPool->GetPlayer(playerId); pPlayer != nullptr)
                                {
                                    if (const auto pPlayerPed = pPlayer->m_pPed; pPlayerPed != nullptr)
                                    {
                                        if (const auto pGamePed = pPlayerPed->m_pGamePed; pGamePed != nullptr)
                                        {
                                            const float distanceToCamera = (TheCamera.GetPosition() - pGamePed->GetPosition()).Magnitude();

                                            float vSpeakerIconSize { 0.f };

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

                                                    SpeakerList::tSpeakerIcon->Draw(screenPos.x + addX, screenPos.y + addY,
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

                    ImGui::Image(SpeakerList::tSpeakerIcon->GetTexture(), { ImGui::GetTextLineHeight(),
                        ImGui::GetTextLineHeight() }, { 0, 0 }, { 1, 1 }, color);

                    ImGui::NextColumn();

                    ImGui::TextColored(color, "%s (%hu)", playerName, playerId);

                    ImGui::NextColumn();

                    for (const auto& streamInfo : SpeakerList::playerStreams[playerId])
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

void SpeakerList::SetSpeakerIconOffsetX(const int speakerIconOffsetX) noexcept
{
    PluginConfig::SetSpeakerIconOffsetX(std::clamp(speakerIconOffsetX, -500, 500));
}

void SpeakerList::SetSpeakerIconOffsetY(const int speakerIconOffsetY) noexcept
{
    PluginConfig::SetSpeakerIconOffsetY(std::clamp(speakerIconOffsetY, -500, 500));
}

void SpeakerList::SetSpeakerIconScale(const float speakerIconScale) noexcept
{
    PluginConfig::SetSpeakerIconScale(std::clamp(speakerIconScale, 0.2f, 2.f));
}

void SpeakerList::SyncConfigs() noexcept
{
    SpeakerList::SetSpeakerIconOffsetX(PluginConfig::GetSpeakerIconOffsetX());
    SpeakerList::SetSpeakerIconOffsetY(PluginConfig::GetSpeakerIconOffsetY());
    SpeakerList::SetSpeakerIconScale(PluginConfig::GetSpeakerIconScale());
}

void SpeakerList::ResetConfigs() noexcept
{
    PluginConfig::SetSpeakerIconOffsetX(PluginConfig::kDefValSpeakerIconOffsetX);
    PluginConfig::SetSpeakerIconOffsetY(PluginConfig::kDefValSpeakerIconOffsetY);
    PluginConfig::SetSpeakerIconScale(PluginConfig::kDefValSpeakerIconScale);
}

void SpeakerList::OnSpeakerPlay(const Stream& stream, const WORD speaker) noexcept
{
    if (speaker != std::clamp<WORD>(speaker, 0, MAX_PLAYERS - 1))
        return;

    SpeakerList::playerStreams[speaker][(Stream*)(&stream)] = stream.GetInfo();
}

void SpeakerList::OnSpeakerStop(const Stream& stream, const WORD speaker) noexcept
{
    if (speaker != std::clamp<WORD>(speaker, 0, MAX_PLAYERS - 1))
        return;

    SpeakerList::playerStreams[speaker].erase((Stream*)(&stream));
}

bool SpeakerList::initStatus { false };
bool SpeakerList::showStatus { false };

ImFont* SpeakerList::pSpeakerFont { nullptr };
TexturePtr SpeakerList::tSpeakerIcon { nullptr };

std::array<std::unordered_map<Stream*, StreamInfo>, MAX_PLAYERS> SpeakerList::playerStreams;
