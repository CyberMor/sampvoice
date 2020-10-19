/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "PluginMenu.h"

#include <samp/CChat.h>
#include <samp/CInput.h>
#include <samp/CScoreboard.h>
#include <util/ImGuiUtil.h>
#include <util/GameUtil.h>
#include <util/Logger.h>

#include "BlackList.h"
#include "SpeakerList.h"
#include "MicroIcon.h"
#include "Playback.h"
#include "Record.h"

bool PluginMenu::Init(IDirect3DDevice9* const pDevice, const AddressesBase& const addrBase,
    const Resource& const rShader, const Resource& const rLogo, const Resource& const rFont) noexcept
{
    if (pDevice == nullptr)
        return false;

    if (PluginMenu::initStatus || !ImGuiUtil::IsInited())
        return false;

    try
    {
        const BYTE returnOpcode { 0xC3 };
        PluginMenu::openChatFuncPatch = MakePatch(addrBase.GetOpenChatFunc(),
            &returnOpcode, sizeof(returnOpcode), false);
        PluginMenu::openScoreboardFuncPatch = MakePatch(addrBase.GetOpenScoreboardFunc(),
            &returnOpcode, sizeof(returnOpcode), false);
        PluginMenu::switchModeFuncPatch = MakePatch(addrBase.GetSwitchModeFunc(),
            &returnOpcode, sizeof(returnOpcode), false);
    }
    catch (const std::exception& exception)
    {
        Logger::LogToFile("[sv:err:pluginmenu:init] : failed to create function patches");
        PluginMenu::openChatFuncPatch.reset();
        PluginMenu::openScoreboardFuncPatch.reset();
        PluginMenu::switchModeFuncPatch.reset();
        return false;
    }

    Memory::ScopeExit patchesResetScope { [] { PluginMenu::openChatFuncPatch.reset();
                                               PluginMenu::openScoreboardFuncPatch.reset();
                                               PluginMenu::switchModeFuncPatch.reset(); } };

    try
    {
        PluginMenu::blurEffect = MakeBlurEffect(pDevice, rShader);
        PluginMenu::tLogo = MakeTexture(pDevice, rLogo);
    }
    catch (const std::exception& exception)
    {
        Logger::LogToFile("[sv:err:pluginmenu:init] : failed to create resources");
        PluginMenu::blurEffect.reset();
        PluginMenu::tLogo.reset();
        return false;
    }

    Memory::ScopeExit resourcesResetScope { [] { PluginMenu::blurEffect.reset();
                                                 PluginMenu::tLogo.reset(); } };

    ImGui::StyleColorsClassic();

    if (float varWindowPaddingX { 0.f }, varWindowPaddingY { 0.f };
        Render::ConvertBaseXValueToScreenXValue(kBaseMenuPaddingX, varWindowPaddingX) &&
        Render::ConvertBaseYValueToScreenYValue(kBaseMenuPaddingY, varWindowPaddingY))
    {
        ImGui::GetStyle().WindowPadding = { varWindowPaddingX, varWindowPaddingY };
    }

    if (float varFramePaddingX { 0.f }, varFramePaddingY { 0.f };
        Render::ConvertBaseXValueToScreenXValue(kBaseMenuFramePaddingX, varFramePaddingX) &&
        Render::ConvertBaseYValueToScreenYValue(kBaseMenuFramePaddingY, varFramePaddingY))
    {
        ImGui::GetStyle().FramePadding = { varFramePaddingX, varFramePaddingY };
    }

    if (float varItemSpacingX { 0.f }, varItemSpacingY { 0.f };
        Render::ConvertBaseXValueToScreenXValue(kBaseMenuItemSpacingX, varItemSpacingX) &&
        Render::ConvertBaseYValueToScreenYValue(kBaseMenuItemSpacingY, varItemSpacingY))
    {
        ImGui::GetStyle().ItemSpacing = { varItemSpacingX, varItemSpacingY };
    }

    if (float varItemInnerSpacingX { 0.f }, varItemInnerSpacingY { 0.f };
        Render::ConvertBaseXValueToScreenXValue(kBaseMenuItemInnerSpacingX, varItemInnerSpacingX) &&
        Render::ConvertBaseYValueToScreenYValue(kBaseMenuItemInnerSpacingY, varItemInnerSpacingY))
    {
        ImGui::GetStyle().ItemInnerSpacing = { varItemInnerSpacingX, varItemInnerSpacingY };
    }

    if (float varRounding { 0.f }; Render::ConvertBaseXValueToScreenXValue(kBaseMenuRounding, varRounding))
    {
        ImGui::GetStyle().FrameRounding = varRounding;
        ImGui::GetStyle().ScrollbarRounding = varRounding;
        ImGui::GetStyle().ChildRounding = varRounding;
        ImGui::GetStyle().PopupRounding = varRounding;
        ImGui::GetStyle().GrabRounding = varRounding;
        ImGui::GetStyle().TabRounding = varRounding;
    }

    ImGui::GetStyle().WindowRounding = 0.0f;
    ImGui::GetStyle().WindowBorderSize = 0.0f;
    ImGui::GetStyle().FrameBorderSize = 0.0f;
    ImGui::GetStyle().ChildBorderSize = 0.0f;
    ImGui::GetStyle().PopupBorderSize = 0.0f;
    ImGui::GetStyle().TabBorderSize = 0.0f;
    ImGui::GetStyle().AntiAliasedFill = true;
    ImGui::GetStyle().AntiAliasedLines = true;
    ImGui::GetStyle().GrabMinSize = 20.0f;
    ImGui::GetStyle().ScrollbarSize = 10.0f;
    ImGui::GetStyle().Colors[ImGuiCol_Text] = { 0.94f, 0.94f, 0.94f, 1.f };
    ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = { 0.3f, 0.3f, 0.3f, 0.2f };
    ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] = { 0.4f, 0.4f, 0.4f, 0.4f };
    ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive] = { 0.5f, 0.5f, 0.5f, 0.6f };
    ImGui::GetStyle().Colors[ImGuiCol_Button] = { 0.3f, 0.3f, 0.3f, 0.2f };
    ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = { 0.4f, 0.4f, 0.4f, 0.4f };
    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = { 0.5f, 0.5f, 0.5f, 0.6f };
    ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = { 0.3f, 0.3f, 0.3f, 0.2f };
    ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive] = { 0.5f, 0.5f, 0.5f, 0.6f };

    ImGui::GetIO().Fonts->AddFontDefault();

    {
        float varTitleFontSize { 0.f };

        if (!Render::ConvertBaseYValueToScreenYValue(kBaseFontTitleSize, varTitleFontSize))
        {
            Logger::LogToFile("[sv:err:pluginmenu:init] : failed to convert title font size");
            return false;
        }

        PluginMenu::pTitleFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(rFont.GetDataPtr(),
            rFont.GetDataSize(), varTitleFontSize, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

        if (PluginMenu::pTitleFont == nullptr)
        {
            Logger::LogToFile("[sv:err:pluginmenu:init] : failed to create title font");
            return false;
        }
    }

    Memory::ScopeExit titleFontResetScope { [] { delete PluginMenu::pTitleFont; } };

    {
        float varTabFontSize { 0.f };

        if (!Render::ConvertBaseYValueToScreenYValue(kBaseFontTabSize, varTabFontSize))
        {
            Logger::LogToFile("[sv:err:pluginmenu:init] : failed to convert tab font size");
            return false;
        }

        PluginMenu::pTabFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(rFont.GetDataPtr(),
            rFont.GetDataSize(), varTabFontSize, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

        if (PluginMenu::pTabFont == nullptr)
        {
            Logger::LogToFile("[sv:err:pluginmenu:init] : failed to create tab font");
            return false;
        }
    }

    Memory::ScopeExit tabFontResetScope { [] { delete PluginMenu::pTabFont; } };

    {
        float varDescFontSize { 0.f };

        if (!Render::ConvertBaseYValueToScreenYValue(kBaseFontDescSize, varDescFontSize))
        {
            Logger::LogToFile("[sv:err:pluginmenu:init] : failed to convert description font size");
            return false;
        }

        PluginMenu::pDescFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(rFont.GetDataPtr(),
            rFont.GetDataSize(), varDescFontSize, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

        if (PluginMenu::pDescFont == nullptr)
        {
            Logger::LogToFile("[sv:err:pluginmenu:init] : failed to create description font");
            return false;
        }
    }

    Memory::ScopeExit descFontResetScope { [] { delete PluginMenu::pDescFont; } };

    {
        float varFontSize { 0.f };

        if (!Render::ConvertBaseYValueToScreenYValue(kBaseFontSize, varFontSize))
        {
            Logger::LogToFile("[sv:err:pluginmenu:init] : failed to convert default font size");
            return false;
        }

        PluginMenu::pDefFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(rFont.GetDataPtr(),
            rFont.GetDataSize(), varFontSize, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

        if (PluginMenu::pDefFont == nullptr)
        {
            Logger::LogToFile("[sv:err:pluginmenu:init] : failed to create default font");
            return false;
        }
    }

    PluginMenu::blurLevel = 0;
    PluginMenu::blurLevelDeviation = 0;
    PluginMenu::showStatus = false;

    patchesResetScope.Release();
    resourcesResetScope.Release();
    titleFontResetScope.Release();
    tabFontResetScope.Release();
    descFontResetScope.Release();

    PluginMenu::initStatus = true;
    PluginMenu::SyncOptions();

    return true;
}

void PluginMenu::Free() noexcept
{
    PluginMenu::Hide();

    if (!PluginMenu::initStatus)
        return;

    PluginMenu::tLogo.reset();
    PluginMenu::blurEffect.reset();

    delete PluginMenu::pTitleFont;
    delete PluginMenu::pTabFont;
    delete PluginMenu::pDescFont;
    delete PluginMenu::pDefFont;

    PluginMenu::openChatFuncPatch.reset();
    PluginMenu::openScoreboardFuncPatch.reset();
    PluginMenu::switchModeFuncPatch.reset();

    PluginMenu::prevChatMode = SAMP::CChat::Normal;
    PluginMenu::blurLevelDeviation = 0.f;
    PluginMenu::blurLevel = 0.f;

    PluginMenu::bMicroMovement = false;

    PluginMenu::initStatus = false;
}

bool PluginMenu::Show() noexcept
{
    if (!PluginMenu::initStatus || GameUtil::IsMenuActive() || PluginMenu::showStatus)
        return false;

    PluginMenu::openChatFuncPatch->Enable();
    PluginMenu::openScoreboardFuncPatch->Enable();
    PluginMenu::switchModeFuncPatch->Enable();

    PluginMenu::blurLevelDeviation = kBlurLevelIncrement;

    if (const auto pChat = SAMP::pChat(); pChat != nullptr)
    {
        PluginMenu::prevChatMode = pChat->m_nMode;
        pChat->m_nMode = SAMP::CChat::Off;
    }

    if (const auto pScoreboard = SAMP::pScoreboard();
        pScoreboard != nullptr && pScoreboard->m_bIsEnabled)
    {
        pScoreboard->m_bIsEnabled = FALSE;
        pScoreboard->Close(true);
    }

    if (const auto pInputBox = SAMP::pInputBox();
        pInputBox != nullptr && pInputBox->m_bEnabled)
    {
        pInputBox->m_bEnabled = FALSE;
        pInputBox->m_szInput[0] = '\0';
        pInputBox->Close();
    }

    PluginMenu::SyncOptions();
    PluginMenu::showStatus = true;

    return true;
}

bool PluginMenu::IsShowed() noexcept
{
    return PluginMenu::showStatus;
}

void PluginMenu::Hide() noexcept
{
    if (!PluginMenu::initStatus || !PluginMenu::showStatus)
        return;

    Samp::ToggleSampCursor(0);

    PluginMenu::blurLevelDeviation = kBlurLevelDecrement;

    if (const auto pChat = SAMP::pChat(); pChat != nullptr)
    {
        pChat->m_nMode = PluginMenu::prevChatMode;
    }

    PluginMenu::switchModeFuncPatch->Disable();
    PluginMenu::openScoreboardFuncPatch->Disable();
    PluginMenu::openChatFuncPatch->Disable();

    ImGui::SetWindowFocus(nullptr);

    PluginMenu::nBuffer[0] = '\0';
    PluginMenu::bCheckDevice = false;
    Record::StopChecking();

    PluginMenu::showStatus = false;
}

void PluginMenu::Render() noexcept
{
    if (!PluginMenu::initStatus)
        return;

    if (PluginMenu::blurLevel > 0.f)
    {
        PluginMenu::blurEffect->Render(PluginMenu::blurLevel);
    }

    if (!PluginMenu::showStatus)
        return;

    float vWindowWidth { 0.f }, vWindowHeight { 0.f };

    if (!Render::ConvertBaseXValueToScreenXValue(kBaseMenuWidth, vWindowWidth) ||
        !Render::ConvertBaseYValueToScreenYValue(kBaseMenuHeight, vWindowHeight))
        return;

    float vTabWidth { 0.f }, vTabHeight { 0.f };

    if (!Render::ConvertBaseXValueToScreenXValue(kBaseTabWidth, vTabWidth) ||
        !Render::ConvertBaseYValueToScreenYValue(kBaseTabHeight, vTabHeight))
        return;

    Samp::ToggleSampCursor(2);

    if (!ImGuiUtil::BeginRender())
        return;

    ImGui::SetNextWindowSize({ vWindowWidth, vWindowHeight });
    ImGui::SetNextWindowPosCenter();

    if (ImGui::Begin("configWindow", nullptr,
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

        ImGui::PushFont(PluginMenu::pTitleFont);

        ImGui::Text(kTitleText);

        ImGui::SameLine(ImGui::GetWindowWidth() - (4 * ImGui::CalcTextSize(kTitleText).y + (vTabWidth -
            4 * ImGui::CalcTextSize(kTitleText).y) / 2.f + ImGui::GetStyle().WindowPadding.x));

        ImGui::Image(PluginMenu::tLogo->GetTexture(), { 4 * ImGui::CalcTextSize(kTitleText).y, ImGui::CalcTextSize(kTitleText).y });

        ImGui::PopFont();
        ImGui::NewLine();

        // Tabs rendering...
        // -------------------------------

        ImGui::PushFont(PluginMenu::pTabFont);

        if (ImGui::Button(kTab1TitleText, { vTabWidth, vTabHeight })) PluginMenu::iSelectedMenu = 0;
        ImGui::SameLine(ImGui::GetStyle().WindowPadding.x + vTabWidth + kBaseTabPadding);
        if (ImGui::Button(kTab2TitleText, { vTabWidth, vTabHeight })) PluginMenu::iSelectedMenu = 1;
        ImGui::SameLine(ImGui::GetStyle().WindowPadding.x + 2 * (vTabWidth + kBaseTabPadding));
        if (ImGui::Button(kTab3TitleText, { vTabWidth, vTabHeight })) PluginMenu::iSelectedMenu = 2;

        ImGui::PopFont();

        // Description rendering...
        // -------------------------------

        ImGui::PushFont(PluginMenu::pDefFont);

        switch (PluginMenu::iSelectedMenu)
        {
            case 0:
            {
                // Common settings rendering...
                // -------------------------------

                ImGui::NewLine();
                ImGui::PushFont(PluginMenu::pDescFont);
                ImGui::Text(kTab1Desc1TitleText);
                ImGui::Separator();
                ImGui::PopFont();
                ImGui::NewLine();

                if (ImGui::Checkbox(kTab1Desc1EnableSoundText, &PluginMenu::soundEnable))
                {
                    // Enabling/Disabling sound
                    Playback::SetSoundEnable(PluginMenu::soundEnable);
                }

                if (PluginMenu::soundEnable)
                {
                    if (ImGui::SliderInt(kTab1Desc1VolumeSoundText, &PluginMenu::soundVolume, 0, 100))
                    {
                        // Setting volume
                        Playback::SetSoundVolume(PluginMenu::soundVolume);
                    }
                }

                ImGui::NewLine();
                ImGui::PushFont(PluginMenu::pDescFont);
                ImGui::Text(kTab1Desc2TitleText);
                ImGui::Separator();
                ImGui::PopFont();
                ImGui::NewLine();

                if (ImGui::Checkbox(kTab1Desc2BalancerText, &PluginMenu::soundBalancer))
                {
                    // Enabling/Disabling sound balancer
                    Playback::SetSoundBalancer(PluginMenu::soundBalancer);
                }

                if (ImGui::Checkbox(kTab1Desc2FilterText, &PluginMenu::soundFilter))
                {
                    // Enabling/Disabling sound filter
                    Playback::SetSoundFilter(PluginMenu::soundFilter);
                }

                ImGui::NewLine();
                ImGui::PushFont(PluginMenu::pDescFont);
                ImGui::Text(kTab1Desc3TitleText);
                ImGui::Separator();
                ImGui::PopFont();
                ImGui::NewLine();

                if (ImGui::SliderFloat(kTab1Desc3SpeakerIconScaleText, &PluginMenu::speakerIconScale, 0.2f, 2.0f))
                {
                    SpeakerList::SetSpeakerIconScale(PluginMenu::speakerIconScale);
                }

                if (ImGui::SliderInt(kTab1Desc3SpeakerIconOffsetXText, &PluginMenu::speakerIconOffsetX, -500, 500))
                {
                    SpeakerList::SetSpeakerIconOffsetX(PluginMenu::speakerIconOffsetX);
                }

                if (ImGui::SliderInt(kTab1Desc3SpeakerIconOffsetYText, &PluginMenu::speakerIconOffsetY, -500, 500))
                {
                    SpeakerList::SetSpeakerIconOffsetY(PluginMenu::speakerIconOffsetY);
                }

                const auto rstBtnSize = ImGui::GetItemRectSize();

                ImGui::NewLine();
                ImGui::PushFont(PluginMenu::pDescFont);
                ImGui::Text(kTab1Desc4TitleText);
                ImGui::Separator();
                ImGui::PopFont();
                ImGui::NewLine();

                if (ImGui::Button(kTab1Desc4ConfigResetText, rstBtnSize))
                {
                    SpeakerList::ResetConfigs();
                    SpeakerList::SyncConfigs();
                    Playback::ResetConfigs();
                    Playback::SyncConfigs();
                    PluginMenu::SyncOptions();
                }
            } break;
            case 1:
            {
                // Micro settings rendering...
                // -------------------------------

                ImGui::NewLine();
                ImGui::PushFont(PluginMenu::pDescFont);
                ImGui::Text(kTab2Desc1TitleText);
                ImGui::Separator();
                ImGui::PopFont();
                ImGui::NewLine();

                const auto& devList = Record::GetDeviceNamesList();

                if (!devList.empty())
                {
                    if (ImGui::Checkbox(kTab2Desc1EnableMicroText, &PluginMenu::microEnable))
                    {
                        // Enabling/Disabling microphone
                        Record::SetMicroEnable(PluginMenu::microEnable);

                        if (!PluginMenu::microEnable && PluginMenu::bCheckDevice)
                            PluginMenu::bCheckDevice = false;
                    }

                    if (PluginMenu::microEnable)
                    {
                        if (ImGui::SliderInt(kTab2Desc1MicroVolumeText, &PluginMenu::microVolume, 0, 100))
                        {
                            // Setting volume micro
                            Record::SetMicroVolume(PluginMenu::microVolume);
                        }

                        if (ImGui::BeginCombo(kTab2Desc1DeviceNameText, devList[PluginMenu::deviceIndex].c_str()))
                        {
                            for (int i { 0 }; i < devList.size(); ++i)
                            {
                                if (ImGui::Selectable(devList[i].c_str(), i == PluginMenu::deviceIndex))
                                    Record::SetMicroDevice(PluginMenu::deviceIndex = i);

                                if (i == PluginMenu::deviceIndex)
                                    ImGui::SetItemDefaultFocus();
                            }

                            ImGui::EndCombo();
                        }

                        if (ImGui::Checkbox(kTab2Desc1CheckDeviceText, &PluginMenu::bCheckDevice))
                        {
                            if (PluginMenu::bCheckDevice) Record::StartChecking();
                            else Record::StopChecking();
                        }
                    }
                }
                else
                {
                    ImGui::TextDisabled(kTab2Desc3MicroNotFoundText);
                }

                ImGui::NewLine();
                ImGui::PushFont(PluginMenu::pDescFont);
                ImGui::Text(kTab2Desc2TitleText);
                ImGui::Separator();
                ImGui::PopFont();
                ImGui::NewLine();

                if (ImGui::SliderFloat(kTab2Desc2MicroIconScaleText, &PluginMenu::microIconScale, 0.2f, 2.0f))
                {
                    // Setting scale micro icon
                    MicroIcon::SetMicroIconScale(PluginMenu::microIconScale);
                }

                if (float screenWidth { 0.f }, screenHeight { 0.f };
                    Render::GetScreenSize(screenWidth, screenHeight))
                {
                    if (ImGui::SliderInt(kTab2Desc2MicroIconPositionXText, &PluginMenu::microIconPositionX, 0, screenWidth))
                        MicroIcon::SetMicroIconPositionX(PluginMenu::microIconPositionX);

                    if (ImGui::SliderInt(kTab2Desc2MicroIconPositionYText, &PluginMenu::microIconPositionY, 0, screenHeight))
                        MicroIcon::SetMicroIconPositionY(PluginMenu::microIconPositionY);
                }

                if (ImGui::Button(kTab2Desc2MicroIconMoveText, ImGui::GetItemRectSize()))
                    PluginMenu::bMicroMovement = true;

                const auto rstBtnSize = ImGui::GetItemRectSize();

                ImGui::NewLine();
                ImGui::PushFont(PluginMenu::pDescFont);
                ImGui::Text(kTab1Desc4TitleText);
                ImGui::Separator();
                ImGui::PopFont();
                ImGui::NewLine();

                if (ImGui::Button(kTab1Desc4ConfigResetText, rstBtnSize))
                {
                    MicroIcon::ResetConfigs();
                    MicroIcon::SyncConfigs();
                    Record::ResetConfigs();
                    Record::SyncConfigs();
                    PluginMenu::SyncOptions();
                }
            } break;
            case 2:
            {
                // Blacklist rendering...
                // -------------------------------

                ImGui::NewLine();
                ImGui::PushFont(PluginMenu::pDescFont);
                ImGui::Text(kTab3Desc1TitleText);
                ImGui::Separator();
                ImGui::PopFont();
                ImGui::NewLine();

                ImGui::PushItemWidth(-1);
                const ImVec2 oldCurPos = ImGui::GetCursorPos();
                ImGui::InputText("##label", PluginMenu::nBuffer.data(), PluginMenu::nBuffer.size());
                if (!ImGui::IsItemActive() && PluginMenu::nBuffer[0] == '\0')
                {
                    const ImVec2 newCurPos = ImGui::GetCursorPos();
                    const float inputTextHeight = newCurPos.y - oldCurPos.y;
                    ImGui::SetCursorPosX(oldCurPos.x + 10.f);
                    ImGui::SetCursorPosY(oldCurPos.y + (inputTextHeight - ImGui::CalcTextSize(kTab3Desc1InputPlaceholderText).y) / 2.f);
                    ImGui::TextColored({ 0.6f, 0.6f, 0.6f, 0.8f }, kTab3Desc1InputPlaceholderText);
                    ImGui::SetCursorPos(newCurPos);
                } ImGui::PopItemWidth();
                ImGui::NewLine();
                ImGui::Separator();
                ImGui::NewLine();

                const float listWidth = (ImGui::GetWindowWidth() - 4.f * ImGui::GetStyle().WindowPadding.x) / 2.f;

                WORD iPlayerId;

                if (_snscanf_s(PluginMenu::nBuffer.data(), PluginMenu::nBuffer.size(), "%hu", &iPlayerId) == 0)
                    iPlayerId = SV::kNonePlayer;

                ImGui::PushItemWidth(listWidth);
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);

                // Rendering all players list...
                // ---------------------------------------

                ImGui::BeginGroup();

                ImGui::Text(kTab3Desc2PlayerListText);
                ImGui::GetWindowDrawList()->AddLine(ImGui::GetCursorScreenPos(),
                    { ImGui::GetCursorScreenPos().x + listWidth, ImGui::GetCursorScreenPos().y }, 0xff808080);

                ImGui::NewLine();

                float listHeight = (ImGui::GetWindowHeight() - ImGui::GetCursorPosY()) - ImGui::GetStyle().WindowPadding.y;

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.f, 0.f });
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.f, 0.f });

                if (ImGui::BeginChildFrame(1, { listWidth, listHeight },
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
                            for (WORD playerId { 0 }; playerId < MAX_PLAYERS; ++playerId)
                            {
                                if (pPlayerPool->IsConnected(playerId) == FALSE)
                                    continue;

                                if (const auto playerName = pPlayerPool->GetName(playerId); playerName != nullptr)
                                {
                                    if ((PluginMenu::nBuffer[0] == '\0' || (iPlayerId != SV::kNonePlayer ? playerId == iPlayerId :
                                         static_cast<bool>(std::strstr(playerName, PluginMenu::nBuffer.data())))) &&
                                        !BlackList::IsPlayerBlocked(playerId))
                                    {
                                        ImGui::PushID(playerId);
                                        const ImVec2 oldCurPos = ImGui::GetCursorPos();

                                        if (ImGui::Button("##label", { listWidth, ImGui::GetFontSize() + 2.f }))
                                        {
                                            // Add player black list
                                            BlackList::LockPlayer(playerId);
                                        }

                                        ImGui::SetCursorPos({ oldCurPos.x + 5.f, oldCurPos.y + 1.f });
                                        if (auto stPlayer = SAMP::pNetGame()->GetPlayerPool()->GetPlayer(playerId))
                                            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(htonl(stPlayer->GetColorAsRGBA() |
                                                0xff000000)), "(%hu) %s", playerId, playerName);
                                        else ImGui::Text("(%hu) %s", playerId, playerName);
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

                ImGui::SameLine(listWidth + 3.f * ImGui::GetStyle().WindowPadding.x);

                // Rendering blocked players list...
                // ---------------------------------------

                ImGui::BeginGroup();

                ImGui::Text(kTab3Desc3BlackListText);
                ImGui::GetWindowDrawList()->AddLine(ImGui::GetCursorScreenPos(),
                    { ImGui::GetCursorScreenPos().x + listWidth, ImGui::GetCursorScreenPos().y }, 0xff808080);

                ImGui::NewLine();

                listHeight = (ImGui::GetWindowHeight() - ImGui::GetCursorPosY()) - ImGui::GetStyle().WindowPadding.y;

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.f, 0.f });
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.f, 0.f });

                if (ImGui::BeginChildFrame(2, { listWidth, listHeight },
                    ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoFocusOnAppearing |
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoSavedSettings |
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoResize))
                {
                    const auto& blackList = BlackList::RequestBlackList();

                    for (const auto& playerInfo : blackList)
                    {
                        if (!(PluginMenu::nBuffer[0] == '\0' || (iPlayerId != SV::kNonePlayer ? playerInfo.playerId == iPlayerId :
                            static_cast<bool>(std::strstr(playerInfo.playerName.c_str(), PluginMenu::nBuffer.data()))))) continue;

                        const ImVec2 oldCurPos = ImGui::GetCursorPos();
                        const ImVec2 oldCurScreenPos = ImGui::GetCursorScreenPos();

                        ImGui::PushID(&playerInfo);

                        if (ImGui::Button("##label", { listWidth, ImGui::GetFontSize() + 2.f }))
                        {
                            // Remove player from black list
                            BlackList::UnlockPlayer(playerInfo.playerName);
                        }

                        ImGui::PopID();

                        ImGui::SetCursorPos({ oldCurPos.x + 5.f, oldCurPos.y + 1.f });

                        if (playerInfo.playerId != SV::kNonePlayer)
                        {
                            if (auto stPlayer = SAMP::pNetGame()->GetPlayerPool()->GetPlayer(playerInfo.playerId))
                                ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(htonl(stPlayer->GetColorAsRGBA())),
                                    "%s (%hu)", playerInfo.playerName.c_str(), playerInfo.playerId);
                            else ImGui::Text("%s (%hu)", playerInfo.playerName.c_str(), playerInfo.playerId);
                        }
                        else ImGui::TextDisabled("%s", playerInfo.playerName.c_str());

                        const ImVec2 cPos { (oldCurScreenPos.x + listWidth) - (ImGui::GetFontSize() / 2.f + 2.f +
                            ImGui::GetStyle().ScrollbarSize), oldCurScreenPos.y + (ImGui::GetFontSize() / 2.f + 1.f) };

                        if (playerInfo.playerId != SV::kNonePlayer)
                            ImGui::GetWindowDrawList()->AddCircleFilled(cPos, ImGui::GetFontSize() / 4.f, 0xff7dfe3f);
                        else ImGui::GetWindowDrawList()->AddCircle(cPos, ImGui::GetFontSize() / 4.f, 0xff808080);
                    }

                    ImGui::EndChildFrame();
                }

                ImGui::PopStyleVar();
                ImGui::PopStyleVar();

                ImGui::EndGroup();

                // ---------------------------------------

                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
            } break;
        }

        ImGui::PopFont();
        ImGui::End();

        ImGuiUtil::EndRender();
    }
}

void PluginMenu::Update() noexcept
{
    if (PluginMenu::blurLevelDeviation != 0.f)
    {
        PluginMenu::blurLevel += PluginMenu::blurLevelDeviation;

        if (PluginMenu::blurLevel > 100.f)
        {
            PluginMenu::blurLevelDeviation = 0.f;
            PluginMenu::blurLevel = 100.f;
        }
        else if (PluginMenu::blurLevel < 0.f)
        {
            PluginMenu::blurLevelDeviation = 0.f;
            PluginMenu::blurLevel = 0.f;
        }
    }

    if (PluginMenu::bMicroMovement)
    {
        static bool movementInitStatus { false };
        static int oldMousePosX { 0 }, oldMousePosY { 0 };

        if (!movementInitStatus)
        {
            PluginMenu::Hide();

            oldMousePosX = MicroIcon::GetMicroIconPositionX();
            oldMousePosY = MicroIcon::GetMicroIconPositionY();

            movementInitStatus = true;
        }

        Samp::ToggleSampCursor(2);

        const ImVec2 mousePosition = ImGui::GetMousePos();

        MicroIcon::SetMicroIconPosition(mousePosition.x, mousePosition.y);

        if (GameUtil::IsKeyPressed(0x01) || GameUtil::IsKeyPressed(0x02) || GameUtil::IsKeyPressed(0x1B))
        {
            if (GameUtil::IsKeyPressed(0x02) || GameUtil::IsKeyPressed(0x1B))
            {
                MicroIcon::SetMicroIconPosition(oldMousePosX, oldMousePosY);
            }

            movementInitStatus = false;
            PluginMenu::bMicroMovement = false;

            PluginMenu::Show();
        }
    }
}

LRESULT PluginMenu::WindowProc(const HWND hWnd, const UINT uMsg,
    const WPARAM wParam, const LPARAM lParam) noexcept
{
    if (!PluginMenu::initStatus)
        return FALSE;

    if (uMsg == WM_KEYDOWN && static_cast<BYTE>(wParam) == 0x7A)
    {
        if (!PluginMenu::Show()) PluginMenu::Hide();
        return TRUE;
    }

    if (!PluginMenu::showStatus && !PluginMenu::bMicroMovement)
        return FALSE;

    if (uMsg == WM_KEYDOWN && static_cast<BYTE>(wParam) == 0x1B)
    {
        PluginMenu::Hide();
        return TRUE;
    }

    return ImGuiUtil::WindowProc(hWnd, uMsg, wParam, lParam);
}

void PluginMenu::SyncOptions() noexcept
{
    PluginMenu::soundEnable = Playback::GetSoundEnable();
    PluginMenu::soundVolume = Playback::GetSoundVolume();
    PluginMenu::soundBalancer = Playback::GetSoundBalancer();
    PluginMenu::soundFilter = Playback::GetSoundFilter();

    PluginMenu::speakerIconScale = SpeakerList::GetSpeakerIconScale();
    PluginMenu::speakerIconOffsetX = SpeakerList::GetSpeakerIconOffsetX();
    PluginMenu::speakerIconOffsetY = SpeakerList::GetSpeakerIconOffsetY();

    PluginMenu::microEnable = Record::GetMicroEnable();
    PluginMenu::microVolume = Record::GetMicroVolume();
    PluginMenu::deviceIndex = Record::GetMicroDevice();

    PluginMenu::microIconScale = MicroIcon::GetMicroIconScale();
    PluginMenu::microIconPositionX = MicroIcon::GetMicroIconPositionX();
    PluginMenu::microIconPositionY = MicroIcon::GetMicroIconPositionY();
    PluginMenu::microIconColor = MicroIcon::GetMicroIconColor();
    PluginMenu::microIconAngle = MicroIcon::GetMicroIconAngle();
}

bool PluginMenu::initStatus { false };
bool PluginMenu::showStatus { false };

float PluginMenu::blurLevel { 0.f };
float PluginMenu::blurLevelDeviation { 0.f };
BlurEffectPtr PluginMenu::blurEffect { nullptr };

TexturePtr PluginMenu::tLogo { nullptr };

ImFont* PluginMenu::pTitleFont { nullptr };
ImFont* PluginMenu::pTabFont { nullptr };
ImFont* PluginMenu::pDescFont { nullptr };
ImFont* PluginMenu::pDefFont { nullptr };

Memory::PatchPtr PluginMenu::openChatFuncPatch { nullptr };
Memory::PatchPtr PluginMenu::openScoreboardFuncPatch { nullptr };
Memory::PatchPtr PluginMenu::switchModeFuncPatch { nullptr };

int PluginMenu::prevChatMode { 0 };

bool PluginMenu::soundEnable { false };
int PluginMenu::soundVolume { 0 };
bool PluginMenu::soundBalancer { false };
bool PluginMenu::soundFilter { false };
float PluginMenu::speakerIconScale { 0.f };
int PluginMenu::speakerIconOffsetX { 0 };
int PluginMenu::speakerIconOffsetY { 0 };

bool PluginMenu::microEnable { false };
int PluginMenu::microVolume { 0 };
int PluginMenu::deviceIndex { 0 };
float PluginMenu::microIconScale { 0.f };
int PluginMenu::microIconPositionX { 0 };
int PluginMenu::microIconPositionY { 0 };
D3DCOLOR PluginMenu::microIconColor { 0 };
float PluginMenu::microIconAngle { 0.f };

int PluginMenu::iSelectedMenu { 0 };
bool PluginMenu::bCheckDevice { false };
bool PluginMenu::bMicroMovement { false };
std::array<char, 64> PluginMenu::nBuffer {};
