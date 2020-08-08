/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "SpeakerList.h"

#include <assert.h>

#include <game/CPed.h>
#include <game/CSprite.h>
#include <game/CCamera.h>

#include <util/ImGuiUtil.h>
#include <util/GameUtil.h>
#include <util/Render.h>

#include "PluginConfig.h"

bool SpeakerList::initStatus(false);
bool SpeakerList::showStatus(false);

ImFont* SpeakerList::pSpeakerFont(nullptr);
TexturePtr SpeakerList::tSpeakerIcon(nullptr);

std::array<std::list<StreamInfoPtr>, MAX_PLAYERS> SpeakerList::playerStreams;

void SpeakerList::OnSpeakerPlay(const Stream& stream, const WORD speaker) {

	if (speaker >= MAX_PLAYERS) return;

	SpeakerList::playerStreams[speaker].push_front(stream.GetInfo());

}

void SpeakerList::OnSpeakerStop(const Stream& stream, const WORD speaker) {

	if (speaker >= MAX_PLAYERS) return;

	SpeakerList::playerStreams[speaker].remove(stream.GetInfo());

}

bool SpeakerList::Init(
	IDirect3DDevice9* pDevice,
	const AddressesBase& addrBase,
	const Resource& rSpeakerIcon,
	const Resource& rSpeakerFont
) {

	assert(pDevice);

	if (SpeakerList::initStatus) return false;

	if (!ImGuiUtil::IsInited()) return false;

	if (!(SpeakerList::tSpeakerIcon = MakeTexture(pDevice, rSpeakerIcon))) {

		Logger::LogToFile("[sv:err:speakerlist:init] : failed to create speaker icon");

		return false;

	}

	float varFontSize; const ImGuiIO& io = ImGui::GetIO();

	if (!Render::ConvertBaseYValueToScreenYValue(BaseFontSize, varFontSize) ||
		!(SpeakerList::pSpeakerFont = io.Fonts->AddFontFromMemoryTTF(rSpeakerFont.GetDataPtr(),
		rSpeakerFont.GetDataSize(), varFontSize, nullptr, io.Fonts->GetGlyphRangesCyrillic()))
	) {

		Logger::LogToFile("[sv:err:speakerlist:init] : failed to create speaker font");

		SpeakerList::tSpeakerIcon.reset();

		return false;

	}

	if (!PluginConfig::IsSpeakerLoaded()) {

		PluginConfig::SetSpeakerLoaded(true);
		SpeakerList::ResetConfigs();

	}

	SpeakerList::initStatus = true;

	SpeakerList::SyncConfigs();

	return true;

}

int SpeakerList::GetSpeakerIconOffsetX() {

	return PluginConfig::GetSpeakerIconOffsetX();

}

int SpeakerList::GetSpeakerIconOffsetY() {

	return PluginConfig::GetSpeakerIconOffsetY();

}

float SpeakerList::GetSpeakerIconScale() {

	return PluginConfig::GetSpeakerIconScale();

}

void SpeakerList::SetSpeakerIconOffsetX(int speakerIconOffsetX) {

	if (speakerIconOffsetX > 500) speakerIconOffsetX = 500;
	if (speakerIconOffsetX < -500) speakerIconOffsetX = -500;

	PluginConfig::SetSpeakerIconOffsetX(speakerIconOffsetX);

}

void SpeakerList::SetSpeakerIconOffsetY(int speakerIconOffsetY) {

	if (speakerIconOffsetY > 500) speakerIconOffsetY = 500;
	if (speakerIconOffsetY < -500) speakerIconOffsetY = -500;

	PluginConfig::SetSpeakerIconOffsetY(speakerIconOffsetY);

}

void SpeakerList::SetSpeakerIconScale(float speakerIconScale) {

	if (speakerIconScale > 2.f) speakerIconScale = 2.f;
	if (speakerIconScale < 0.2f) speakerIconScale = 0.2f;

	PluginConfig::SetSpeakerIconScale(speakerIconScale);

}

void SpeakerList::SyncConfigs() {

	SpeakerList::SetSpeakerIconOffsetX(PluginConfig::GetSpeakerIconOffsetX());
	SpeakerList::SetSpeakerIconOffsetY(PluginConfig::GetSpeakerIconOffsetY());
	SpeakerList::SetSpeakerIconScale(PluginConfig::GetSpeakerIconScale());

}

void SpeakerList::ResetConfigs() {

	PluginConfig::SetSpeakerIconOffsetX(PluginConfig::DefVal_SpeakerIconOffsetX);
	PluginConfig::SetSpeakerIconOffsetY(PluginConfig::DefVal_SpeakerIconOffsetY);
	PluginConfig::SetSpeakerIconScale(PluginConfig::DefVal_SpeakerIconScale);

}

void SpeakerList::Render() {

	if (!SpeakerList::initStatus) return;
	if (!SpeakerList::showStatus) return;

	const auto pNetGame = SAMP::pNetGame();
	if (!pNetGame) return;

	const auto pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) return;

	float vLeftIndent = 0.f, vScrWidth = 0.f, vScrHeight = 0.f;

	if (!Render::ConvertBaseXValueToScreenXValue(BaseLeftIndent, vLeftIndent)) return;
	if (!Render::GetScreenSize(vScrWidth, vScrHeight)) return;

	if (!ImGuiUtil::RenderBegin()) return;

	const ImVec2 vWindowPadding = ImVec2(4, 8);
	const ImVec2 vFramePadding = ImVec2(4, 8);
	const ImVec2 vItemPadding = ImVec2(4, 8);

	ImGui::PushFont(SpeakerList::pSpeakerFont);

	const float vWidth = vScrWidth / 2.f - vLeftIndent;
	const float vHeight = 2.f * vWindowPadding.y + (BaseLinesCount *
		(ImGui::GetTextLineHeight() + vFramePadding.y));

	const float vIconWidth = ImGui::GetTextLineHeight() + 5.f;
	const float vNickWidth = 0.2f * (vWidth - vIconWidth);
	const float vStreamsWidth = 0.8f * (vWidth - vIconWidth);

	const float vPosX = vLeftIndent;
	const float vPosY = (vScrHeight - vHeight) / 2.f;

	ImGui::SetNextWindowPos(ImVec2(vPosX, vPosY));
	ImGui::SetNextWindowSize(ImVec2(vWidth, vHeight));

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, vFramePadding);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, vWindowPadding);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, vItemPadding);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, vItemPadding);

	if (ImGui::Begin(
		"speakerListWindow", nullptr,
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoInputs
	)) {

		ImGui::Columns(3, nullptr, false);

		ImGui::SetColumnWidth(0, vIconWidth);
		ImGui::SetColumnWidth(1, vNickWidth);
		ImGui::SetColumnWidth(2, vStreamsWidth);

		ImGui::SetColumnOffset(0, vWindowPadding.x);
		ImGui::SetColumnOffset(1, vWindowPadding.x + vIconWidth + vFramePadding.x);
		ImGui::SetColumnOffset(2, vWindowPadding.x + vIconWidth + vFramePadding.x + vNickWidth + vFramePadding.x);

		int curTextLine = 0;

		for (int playerId = 0; playerId < MAX_PLAYERS; ++playerId) {

			if (curTextLine >= BaseLinesCount) break;

			if (const auto playerName = pPlayerPool->GetName(playerId)) {

				if (!SpeakerList::playerStreams[playerId].empty()) {

					for (const auto& playerStream : SpeakerList::playerStreams[playerId])
						if (playerStream->type == StreamType::LocalStreamAtPlayer) {

							if (GameUtil::IsPlayerVisible(playerId)) {

								if (const auto pPlayer = pPlayerPool->GetPlayer(playerId)) {

									if (const auto pPlayerPed = pPlayer->m_pPed) {

										if (const auto pGamePed = pPlayerPed->m_pGamePed) {

											const float distanceToCamera = (TheCamera.GetPosition() - pGamePed->GetPosition()).Magnitude();

											float vSpeakerIconSize;

											if (Render::ConvertBaseYValueToScreenYValue(BaseIconSize, vSpeakerIconSize)) {

												vSpeakerIconSize *= PluginConfig::GetSpeakerIconScale();
												vSpeakerIconSize *= (5.f / distanceToCamera);

												float width, height;
												RwV3d playerPos, screenPos;

												pGamePed->GetBonePosition(playerPos, 1, false);
												playerPos.z += 1.f;

												if (CSprite::CalcScreenCoors(playerPos, &screenPos, &width, &height, true, true)) {

													screenPos.x -= (vSpeakerIconSize / 2.f);
													screenPos.y -= (vSpeakerIconSize / 2.f);

													const float addX = PluginConfig::GetSpeakerIconOffsetX() * (5.f / distanceToCamera);
													const float addY = PluginConfig::GetSpeakerIconOffsetY() * (5.f / distanceToCamera);

													SpeakerList::tSpeakerIcon->Draw(
														screenPos.x + addX, screenPos.y + addY,
														vSpeakerIconSize, vSpeakerIconSize, -1, 0.f
													);

												}

											}

										}

									}

								}

							}

							break;

						}

					ImGui::PushID(playerId);

					ImGui::Image(
						SpeakerList::tSpeakerIcon->GetTexture(),
						ImVec2(ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight()),
						ImVec2(0, 0), ImVec2(1, 1), ImGui::ColorConvertU32ToFloat4(0x00ffffff |
						((DWORD)((1.f - (float)(curTextLine) / (float)(BaseLinesCount)) * 255.f) << 24))
					);

					ImGui::NextColumn();

					ImGui::TextColored(
						ImGui::ColorConvertU32ToFloat4(0x00ffffff |
						((DWORD)((1.f - (float)(curTextLine) / (float)(BaseLinesCount)) * 255.f) << 24)),
						"%s (%d)", playerName, playerId
					);

					ImGui::NextColumn();

					for (const auto& streamInfo : SpeakerList::playerStreams[playerId]) {

						if (!streamInfo->color) continue;

						ImGui::PushID(&streamInfo);

						ImGui::TextColored(
							ImGui::ColorConvertU32ToFloat4((streamInfo->color & 0x00ffffff) |
							((DWORD)((1.f - (float)(curTextLine) / (float)(BaseLinesCount)) * 255.f) << 24)),
							"[%s]", streamInfo->name.c_str()
						);

						ImGui::SameLine();
						ImGui::PopID();

					}

					ImGui::NextColumn();
					ImGui::PopID();

					curTextLine++;

				}

			}

		}

		ImGui::End();

	}

	ImGui::PopStyleVar(4);
	ImGui::PopFont();

	ImGuiUtil::RenderEnd();

}

void SpeakerList::Show() {

	SpeakerList::showStatus = true;

}

bool SpeakerList::IsShowed() {

	return SpeakerList::showStatus;

}

void SpeakerList::Hide() {

	SpeakerList::showStatus = false;

}

void SpeakerList::Free() {

	if (!SpeakerList::initStatus) return;

	SpeakerList::tSpeakerIcon.reset();
	delete SpeakerList::pSpeakerFont;

	SpeakerList::initStatus = false;

}
