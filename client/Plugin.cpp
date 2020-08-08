/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Plugin.h"

#include <assert.h>

#include <game/CRadar.h>
#include <game/CWorld.h>
#include <game/CSprite.h>

#include <samp/CScoreboard.h>

#include <util/Path.h>

#include <util/GameUtil.h>
#include <util/ImGuiUtil.h>
#include <util/KeyFilter.h>
#include <util/RakNet.h>

#include "Record.h"
#include "Playback.h"
#include "Network.h"

#include "BlackList.h"
#include "PluginConfig.h"

#include "MicroIcon.h"
#include "SpeakerList.h"
#include "PluginMenu.h"

#include "GlobalStream.h"
#include "StreamAtPoint.h"
#include "StreamAtVehicle.h"
#include "StreamAtPlayer.h"
#include "StreamAtObject.h"

#define FontResource				IDR_FONT1,	RT_FONT
#define LogoIconResource			IDB_PNG1,	"PNG"
#define BlurShaderResource			IDR_HLSL1,	"HLSL"
#define PassiveMicroIconResource	IDB_PNG4,	"PNG"
#define ActiveMicroIconResource		IDB_PNG2,	"PNG"
#define MutedMicroIconResource		IDB_PNG3,	"PNG"
#define SpeakerIconResource			IDB_PNG5,	"PNG"

HMODULE Plugin::pModuleHandle(NULL);
AddressesBasePtr Plugin::pAddresses(nullptr);

bool Plugin::muteStatus(false);
bool Plugin::recordStatus(false);
bool Plugin::recordBusy(false);

std::map<DWORD, StreamPtr> Plugin::streamTable;

std::string Plugin::blacklistFilePath;

bool Plugin::gameStatus(false);

LONG Plugin::origWndProc(NULL);
HWND Plugin::origWndHandle(NULL);

IDirect3D9* Plugin::pDirect(nullptr);
IDirect3DDevice9* Plugin::pDevice(nullptr);
D3DPRESENT_PARAMETERS Plugin::parameters;

void Plugin::ConnectHandler(const std::string& serverIp, const WORD serverPort) {

	Plugin::blacklistFilePath = (const std::string&)(Path() / "svblacklist_" + serverIp + "_" + std::to_string(serverPort) + ".txt");

	if (!BlackList::Load(Plugin::blacklistFilePath)) Logger::LogToFile("[sv:err:plugin] : failed to open blacklist file");
	if (!BlackList::Init(*Plugin::pAddresses)) Logger::LogToFile("[sv:err:plugin] : failed to init blacklist");

}

void Plugin::PluginConnectHandler(SV::ConnectPacket& connectStruct) {

	connectStruct.version = SV::Version;
	connectStruct.signature = SV::Signature;
	connectStruct.micro = Record::HasMicro();

}

bool Plugin::PluginInitHandler(const SV::PluginInitPacket& initPacket) {

	Plugin::muteStatus = initPacket.mute;

	if (!Record::Init(initPacket.bitrate))
		Logger::LogToFile("[sv:inf:plugin:packet:init] : failed init record");

	return true;

}

void Plugin::ControlPacketHandler(const ControlPacket& controlPacket) {

	switch (controlPacket.packet) {
		case SV::ControlPacketType::muteEnable:
		{

			if (controlPacket.length) break;

			Logger::LogToFile("[sv:dbg:plugin:muteenable]");

			Plugin::muteStatus = true;
			Plugin::recordStatus = false;
			Plugin::recordBusy = false;

		} break;
		case SV::ControlPacketType::muteDisable:
		{

			if (controlPacket.length) break;

			Logger::LogToFile("[sv:dbg:plugin:mutedisable]");

			Plugin::muteStatus = false;

		} break;
		case SV::ControlPacketType::startRecord:
		{

			if (controlPacket.length) break;

			Logger::LogToFile("[sv:dbg:plugin:startrecord]");

			if (Plugin::muteStatus) break;

			Plugin::recordBusy = true;
			Plugin::recordStatus = true;

			Record::StartRecording();

		} break;
		case SV::ControlPacketType::stopRecord:
		{

			if (controlPacket.length) break;

			Logger::LogToFile("[sv:dbg:plugin:stoprecord]");

			if (Plugin::muteStatus) break;

			Plugin::recordStatus = false;
			Plugin::recordBusy = false;

		} break;
		case SV::ControlPacketType::addKey:
		{

			const auto& stData = *(SV::AddKeyPacket*)(controlPacket.data);

			if (controlPacket.length != sizeof(stData)) break;

			Logger::LogToFile("[sv:dbg:plugin:addkey] : keyid(0x%hhx)", stData.keyId);

			KeyFilter::AddKey(stData.keyId);

		} break;
		case SV::ControlPacketType::removeKey:
		{

			const auto& stData = *(SV::RemoveKeyPacket*)(controlPacket.data);

			if (controlPacket.length != sizeof(stData)) break;

			Logger::LogToFile("[sv:dbg:plugin:removekey] : keyid(0x%hhx)", stData.keyId);

			KeyFilter::RemoveKey(stData.keyId);

		} break;
		case SV::ControlPacketType::removeAllKeys:
		{

			if (controlPacket.length) break;

			Logger::LogToFile("[sv:dbg:plugin:removeallkeys]");

			KeyFilter::RemoveAllKeys();

		} break;
		case SV::ControlPacketType::createGStream:
		{

			const auto& stData = *(SV::CreateGStreamPacket*)(controlPacket.data);

			if (controlPacket.length < sizeof(stData)) break;

			Logger::LogToFile(
				"[sv:dbg:plugin:creategstream] : stream(%p), color(0x%x), name(%s)",
				stData.stream, stData.color, stData.color ? stData.name : ""
			);

			Plugin::streamTable[stData.stream] = MakeGlobalStream(
				&SpeakerList::OnSpeakerPlay, &SpeakerList::OnSpeakerStop,
				stData.name, stData.color
			);

		} break;
		case SV::ControlPacketType::createLPStream:
		{

			const auto& stData = *(SV::CreateLPStreamPacket*)(controlPacket.data);

			if (controlPacket.length < sizeof(stData)) break;

			Logger::LogToFile(
				"[sv:dbg:plugin:createlpstream] : stream(%p), dist(%.2f), pos(%.2f;%.2f;%.2f), color(0x%x), name(%s)",
				stData.stream, stData.distance, stData.position.x, stData.position.y, stData.position.z,
				stData.color, stData.color ? stData.name : ""
			);

			Plugin::streamTable[stData.stream] = MakeStreamAtPoint(
				&SpeakerList::OnSpeakerPlay, &SpeakerList::OnSpeakerStop,
				stData.name, stData.color, stData.position, stData.distance
			);

		} break;
		case SV::ControlPacketType::createLStreamAtVehicle:
		{

			const auto& stData = *(SV::CreateLStreamAtPacket*)(controlPacket.data);

			if (controlPacket.length < sizeof(stData)) break;

			Logger::LogToFile(
				"[sv:dbg:plugin:createlstreamatvehicle] : stream(%p), dist(%.2f), vehicle(%hu), color(0x%x), name(%s)",
				stData.stream, stData.distance, stData.target, stData.color, stData.color ? stData.name : ""
			);

			Plugin::streamTable[stData.stream] = MakeStreamAtVehicle(
				&SpeakerList::OnSpeakerPlay, &SpeakerList::OnSpeakerStop,
				stData.name, stData.color, stData.target, stData.distance
			);

		} break;
		case SV::ControlPacketType::createLStreamAtPlayer:
		{

			const auto& stData = *(SV::CreateLStreamAtPacket*)(controlPacket.data);

			if (controlPacket.length < sizeof(stData)) break;

			Logger::LogToFile(
				"[sv:dbg:plugin:createlstreamatplayer] : stream(%p), dist(%.2f), player(%hu), color(0x%x), name(%s)",
				stData.stream, stData.distance, stData.target, stData.color, stData.color ? stData.name : ""
			);

			Plugin::streamTable[stData.stream] = MakeStreamAtPlayer(
				&SpeakerList::OnSpeakerPlay, &SpeakerList::OnSpeakerStop,
				stData.name, stData.color, stData.target, stData.distance
			);

		} break;
		case SV::ControlPacketType::createLStreamAtObject:
		{

			const auto& stData = *(SV::CreateLStreamAtPacket*)(controlPacket.data);

			if (controlPacket.length < sizeof(stData)) break;

			Logger::LogToFile(
				"[sv:dbg:plugin:createlstreamatobject] : stream(%p), dist(%.2f), object(%hu), color(0x%x), name(%s)",
				stData.stream, stData.distance, stData.target, stData.color, stData.color ? stData.name : ""
			);

			Plugin::streamTable[stData.stream] = MakeStreamAtObject(
				&SpeakerList::OnSpeakerPlay, &SpeakerList::OnSpeakerStop,
				stData.name, stData.color, stData.target, stData.distance
			);

		} break;
		case SV::ControlPacketType::updateLPStreamDistance:
		{

			const auto& stData = *(SV::UpdateLPStreamDistancePacket*)(controlPacket.data);

			if (controlPacket.length != sizeof(stData)) break;

			Logger::LogToFile(
				"[sv:dbg:plugin:updatelpstreamdistance] : stream(%p), dist(%.2f)",
				stData.stream, stData.distance
			);

			const auto iter = Plugin::streamTable.find(stData.stream);

			if (iter == Plugin::streamTable.end()) break;

			static_cast<LocalStream*>(iter->second.get())->UpdateDistance(stData.distance);

		} break;
		case SV::ControlPacketType::updateLPStreamPosition:
		{

			const auto& stData = *(SV::UpdateLPStreamPositionPacket*)(controlPacket.data);

			if (controlPacket.length != sizeof(stData)) break;

			Logger::LogToFile(
				"[sv:dbg:plugin:updatelpstreamcoords] : stream(%p), pos(%.2f;%.2f;%.2f)",
				stData.stream, stData.position.x, stData.position.y, stData.position.z
			);

			const auto iter = Plugin::streamTable.find(stData.stream);

			if (iter == Plugin::streamTable.end()) break;

			static_cast<StreamAtPoint*>(iter->second.get())->UpdatePosition(stData.position);

		} break;
		case SV::ControlPacketType::deleteStream:
		{

			const auto& stData = *(SV::DeleteStreamPacket*)(controlPacket.data);

			if (controlPacket.length != sizeof(stData)) break;

			Logger::LogToFile("[sv:dbg:plugin:deletestream] : stream(%p)", stData.stream);

			Plugin::streamTable.erase(stData.stream);

		}
	}

}

void Plugin::DisconnectHandler() {

	Plugin::streamTable.clear();

	Plugin::muteStatus = false;
	Plugin::recordStatus = false;
	Plugin::recordBusy = false;

	if (!BlackList::Save(Plugin::blacklistFilePath))
		Logger::LogToFile("[sv:err:plugin] : failed to save blacklist file");

	BlackList::Free();
	Record::Free();

}

void Plugin::MainLoop() {

	if (!Samp::IsLoaded()) return;

	if (Plugin::gameStatus && !GameUtil::IsGameActive()) {

		Logger::LogToFile("[sv:dbg:plugin] : game paused");

		for (auto& it : Plugin::streamTable) it.second->Reset();

		KeyFilter::ReleaseAllKeys();

		Plugin::gameStatus = false;

	} else if (!Plugin::gameStatus && GameUtil::IsGameActive()) {

		Logger::LogToFile("[sv:dbg:plugin] : game resumed");

		for (auto& it : Plugin::streamTable) it.second->Reset();

		Plugin::gameStatus = true;

	}

	while (const auto controlPacket = Network::ReceiveControlPacket())
		Plugin::ControlPacketHandler(*&*controlPacket);

	while (const auto voicePacket = Network::ReceiveVoicePacket()) {

		const auto voicePacketRef = *voicePacket;

		if (BlackList::IsPlayerBlocked(voicePacketRef->sender)) continue;

		const auto iter = Plugin::streamTable.find(voicePacketRef->stream);
		if (iter != Plugin::streamTable.end()) iter->second->Push(*&voicePacketRef);

	}

	for (auto& it : Plugin::streamTable) it.second->Tick();

	Playback::Update();
	Record::Tick();

	KeyFilter::KeyEvent keyEvent;

	while (KeyFilter::PopKey(keyEvent)) {

		if (keyEvent.isPressed) {

			Logger::LogToFile("[sv:dbg:plugin] : activation key(%hhx) pressed", keyEvent.keyId);

			if (!Record::GetMicroEnable()) continue;

			if (!Plugin::recordBusy && !Plugin::recordStatus && keyEvent.actKeys == 1) {

				if (!Plugin::muteStatus) {

					Plugin::recordStatus = true;
					MicroIcon::SwitchToActiveIcon();
					Record::StartRecording();

				} else {

					MicroIcon::SwitchToMutedIcon();

				}

			}

			if (Plugin::muteStatus) continue;

			SV::PressKeyPacket pressKeyPacket = {};

			pressKeyPacket.keyId = keyEvent.keyId;

			if (!Network::SendControlPacket(SV::ControlPacketType::pressKey, &pressKeyPacket, sizeof(pressKeyPacket)))
				Logger::LogToFile("[sv:err:main:HookWndProc] : failed to send PressKey packet");

		} else {

			Logger::LogToFile("[sv:dbg:plugin] : activation key(%hhx) released", keyEvent.keyId);

			if (!Record::GetMicroEnable()) continue;

			if (!Plugin::recordBusy && Plugin::recordStatus && !keyEvent.actKeys) {

				MicroIcon::SwitchToPassiveIcon();
				Plugin::recordStatus = false;

			}

			if (Plugin::muteStatus) continue;

			SV::ReleaseKeyPacket releaseKeyPacket = {};

			releaseKeyPacket.keyId = keyEvent.keyId;

			if (!Network::SendControlPacket(SV::ControlPacketType::releaseKey, &releaseKeyPacket, sizeof(releaseKeyPacket)))
				Logger::LogToFile("[sv:err:main:HookWndProc] : failed to send ReleaseKey packet");

		}

	}

	BYTE frameBuffer[Network::MaxVoiceDataSize];

	if (const DWORD frameSize = Record::GetFrame(frameBuffer, sizeof(frameBuffer))) {

		if (!Network::SendVoicePacket(frameBuffer, frameSize))
			Logger::LogToFile("[sv:err:plugin] : failed to send voice packet");

		if (!Plugin::recordStatus) {

			Record::StopRecording();
			Network::EndSequence();

		}

	}

}

Memory::CallHookPtr Plugin::drawRadarHook(nullptr);

void Plugin::DrawRadarHook() {

	((void(*)())(Plugin::drawRadarHook->GetCallFuncAddr()))();

	SpeakerList::Render();
	MicroIcon::Render();

}

void Plugin::OnInitGame() {

	if (!(Plugin::drawRadarHook = MakeCallHook(0x58FC53, &Plugin::DrawRadarHook)))
		Logger::LogToFile("[sv:err:plugin:sampinit] : failed to create 'DrawRadar' function hook");

	GameUtil::DisableAntiCheat(*Plugin::pAddresses);

	SpeakerList::Show();
	MicroIcon::Show();

}

void Plugin::OnExitGame() {

	Network::Free();

	Plugin::streamTable.clear();

	Record::Free();
	Playback::Free();

	PluginConfig::Save(Path() / SV::ConfigFileName);
	BlackList::Free();

	Render::Free();
	Logger::Free();

}

LRESULT CALLBACK Plugin::OnWndMessage(
	HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam
) {

	if (PluginMenu::OnWndMessage(hWnd, uMsg, wParam, lParam)) return TRUE;

	KeyFilter::OnWndMessage(hWnd, uMsg, wParam, lParam);

	return CallWindowProc((WNDPROC)(Plugin::origWndProc), hWnd, uMsg, wParam, lParam);

}

void Plugin::OnDeviceInit(
	IDirect3D9* pDirect,
	IDirect3DDevice9* pDevice,
	D3DPRESENT_PARAMETERS* pParameters
) {

	assert(pDirect);
	assert(pDevice);
	assert(pParameters);

	Plugin::pDirect = pDirect;
	Plugin::pDevice = pDevice;
	Plugin::parameters = *pParameters;

	MicroIcon::Init(pDevice,
		Resource(Plugin::pModuleHandle, PassiveMicroIconResource),
		Resource(Plugin::pModuleHandle, ActiveMicroIconResource),
		Resource(Plugin::pModuleHandle, MutedMicroIconResource)
	);

	ImGuiUtil::Init(pDevice, pParameters);

	SpeakerList::Init(
		pDevice, *Plugin::pAddresses,
		Resource(Plugin::pModuleHandle, SpeakerIconResource),
		Resource(Plugin::pModuleHandle, FontResource)
	);

	PluginMenu::Init(
		pDevice, pParameters, *Plugin::pAddresses,
		Resource(Plugin::pModuleHandle, BlurShaderResource),
		Resource(Plugin::pModuleHandle, LogoIconResource),
		Resource(Plugin::pModuleHandle, FontResource)
	);

	Plugin::origWndHandle = pParameters->hDeviceWindow;
	Plugin::origWndProc = GetWindowLong(Plugin::origWndHandle, GWL_WNDPROC);
	SetWindowLong(Plugin::origWndHandle, GWL_WNDPROC, (LONG)(&Plugin::OnWndMessage));

	Plugin::gameStatus = GameUtil::IsGameActive();

}

void Plugin::OnBeforeReset() {

	PluginMenu::Free();
	SpeakerList::Free();

	ImGuiUtil::Free();

	MicroIcon::Free();

}

void Plugin::OnRender() {

	Plugin::MainLoop();

	MicroIcon::Update();
	PluginMenu::Update();

	PluginMenu::Render();

}

void Plugin::OnAfterReset(
	IDirect3DDevice9* pDevice,
	D3DPRESENT_PARAMETERS* pParameters
) {

	assert(pDevice);
	assert(pParameters);

	Plugin::parameters = *pParameters;

	MicroIcon::Init(pDevice,
		Resource(Plugin::pModuleHandle, PassiveMicroIconResource),
		Resource(Plugin::pModuleHandle, ActiveMicroIconResource),
		Resource(Plugin::pModuleHandle, MutedMicroIconResource)
	);

	ImGuiUtil::Init(pDevice, pParameters);

	SpeakerList::Init(
		pDevice, *Plugin::pAddresses,
		Resource(Plugin::pModuleHandle, SpeakerIconResource),
		Resource(Plugin::pModuleHandle, FontResource)
	);

	PluginMenu::Init(
		pDevice, pParameters, *Plugin::pAddresses,
		Resource(Plugin::pModuleHandle, BlurShaderResource),
		Resource(Plugin::pModuleHandle, LogoIconResource),
		Resource(Plugin::pModuleHandle, FontResource)
	);

	Plugin::gameStatus = GameUtil::IsGameActive();

}

void Plugin::OnDeviceFree() {

	SetWindowLong(Plugin::origWndHandle, GWL_WNDPROC, Plugin::origWndProc);

	PluginMenu::Free();
	SpeakerList::Free();

	ImGuiUtil::Free();

	MicroIcon::Free();

}

bool Plugin::OnPluginLoad(HMODULE hModule) {

	if (!(Plugin::pModuleHandle = hModule)) return false;
	if (!Logger::Init(Path() / SV::LogFileName)) return false;

	if (!Render::Init(&Plugin::OnDeviceInit, &Plugin::OnBeforeReset, nullptr, &Plugin::OnRender, nullptr, &Plugin::OnAfterReset, &Plugin::OnDeviceFree)) {

		Logger::LogToFile("[sv:err:plugin] : failed to init render module");

		Logger::Free();

		return false;

	}

	return true;

}

bool Plugin::OnSampLoad(HMODULE hModule) {

	if (!(Plugin::pAddresses = MakeAddressesBase((DWORD)(hModule)))) {

		Logger::LogToFile("[sv:err:plugin] : failed to init addresses");

		Render::Free();
		Logger::Free();

		return false;

	}

	if (!PluginConfig::Load(Path() / SV::ConfigFileName)) Logger::LogToFile("[sv:err:plugin] : failed to load configs");

	if (!Samp::Init(*Plugin::pAddresses, &Plugin::OnInitGame, &Plugin::OnExitGame)) {

		Logger::LogToFile("[sv:err:plugin] : failed to init samp");

		Render::Free();
		Logger::Free();

		return false;

	}

	if (!Network::Init(*Plugin::pAddresses, &Plugin::ConnectHandler, &Plugin::PluginConnectHandler, &Plugin::PluginInitHandler, &Plugin::DisconnectHandler)) {

		Logger::LogToFile("[sv:err:plugin] : failed to init network");

		Samp::Free();
		Render::Free();
		Logger::Free();

		return false;

	}

	if (!Playback::Init(*Plugin::pAddresses)) {

		Logger::LogToFile("[sv:err:plugin] : failed to init playback");

		Network::Free();
		Samp::Free();
		Render::Free();
		Logger::Free();

		return false;

	}

	return true;

}
