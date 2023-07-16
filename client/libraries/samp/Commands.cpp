/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "Commands.h"

#if defined(SAMP_R1)

SAMP::CMDPROC SAMP::Commands::Default() { return SAMP::CMDPROC(SAMP_ADDROF(0x64910)); }
SAMP::CMDPROC SAMP::Commands::TestDeathWindow() { return SAMP::CMDPROC(SAMP_ADDROF(0x64930)); }
SAMP::CMDPROC SAMP::Commands::ToggleCameraTargetLabels() { return SAMP::CMDPROC(SAMP_ADDROF(0x64A10)); }
SAMP::CMDPROC SAMP::Commands::SetChatPageSize() { return SAMP::CMDPROC(SAMP_ADDROF(0x64A20)); }
SAMP::CMDPROC SAMP::Commands::SetChatFontSize() { return SAMP::CMDPROC(SAMP_ADDROF(0x64AA0)); }
SAMP::CMDPROC SAMP::Commands::DrawNameTagStatus() { return SAMP::CMDPROC(SAMP_ADDROF(0x65B50)); }
SAMP::CMDPROC SAMP::Commands::DrawChatTimestamps() { return SAMP::CMDPROC(SAMP_ADDROF(0x64B60)); }
SAMP::CMDPROC SAMP::Commands::ToggleAudioStreamMessages() { return SAMP::CMDPROC(SAMP_ADDROF(0x64BC0)); }
SAMP::CMDPROC SAMP::Commands::PrintMemory() { return SAMP::CMDPROC(SAMP_ADDROF(0x64C40)); }
SAMP::CMDPROC SAMP::Commands::SetFrameLimiter() { return SAMP::CMDPROC(SAMP_ADDROF(0x64C60)); }
SAMP::CMDPROC SAMP::Commands::ToggleHeadMoves() { return SAMP::CMDPROC(SAMP_ADDROF(0x64CF0)); }
SAMP::CMDPROC SAMP::Commands::Quit() { return SAMP::CMDPROC(SAMP_ADDROF(0x64D70)); }
SAMP::CMDPROC SAMP::Commands::CmpStat() { return SAMP::CMDPROC(SAMP_ADDROF(0x64D80)); }
SAMP::CMDPROC SAMP::Commands::SavePosition() { return SAMP::CMDPROC(SAMP_ADDROF(0x64D90)); }
SAMP::CMDPROC SAMP::Commands::SavePositionOnly() { return SAMP::CMDPROC(SAMP_ADDROF(0x64F10)); }
SAMP::CMDPROC SAMP::Commands::PrintCurrentInterior() { return SAMP::CMDPROC(SAMP_ADDROF(0x65360)); }
SAMP::CMDPROC SAMP::Commands::ToggleObjectsLight() { return SAMP::CMDPROC(SAMP_ADDROF(0x65390)); }
SAMP::CMDPROC SAMP::Commands::ToggleDebugLabels() { return SAMP::CMDPROC(SAMP_ADDROF(0x653B0)); }
SAMP::CMDPROC SAMP::Commands::SendRconCommand() { return SAMP::CMDPROC(SAMP_ADDROF(0x653C0)); }
SAMP::CMDPROC SAMP::Commands::Debug::SetPlayerSkin() { return SAMP::CMDPROC(SAMP_ADDROF(0x65090)); }
SAMP::CMDPROC SAMP::Commands::Debug::CreateVehicle() { return SAMP::CMDPROC(SAMP_ADDROF(0x65100)); }
SAMP::CMDPROC SAMP::Commands::Debug::EnableVehicleSelection() { return SAMP::CMDPROC(SAMP_ADDROF(0x65240)); }
SAMP::CMDPROC SAMP::Commands::Debug::SetWorldWeather() { return SAMP::CMDPROC(SAMP_ADDROF(0x65260)); }
SAMP::CMDPROC SAMP::Commands::Debug::SetWorldTime() { return SAMP::CMDPROC(SAMP_ADDROF(0x652B0)); }

void SAMP::Commands::Setup() {
	((void(__cdecl*)())SAMP_ADDROF(0x654A0))();
}

#elif defined(SAMP_R3)

SAMP::CMDPROC SAMP::Commands::Default() { return SAMP::CMDPROC(SAMP_ADDROF(0x67D60)); }
SAMP::CMDPROC SAMP::Commands::TestDeathWindow() { return SAMP::CMDPROC(SAMP_ADDROF(0x67D90)); }
SAMP::CMDPROC SAMP::Commands::ToggleCameraTargetLabels() { return SAMP::CMDPROC(SAMP_ADDROF(0x67E70)); }
SAMP::CMDPROC SAMP::Commands::SetChatPageSize() { return SAMP::CMDPROC(SAMP_ADDROF(0x67E80)); }
SAMP::CMDPROC SAMP::Commands::SetChatFontSize() { return SAMP::CMDPROC(SAMP_ADDROF(0x67F00)); }
SAMP::CMDPROC SAMP::Commands::DrawNameTagStatus() { return SAMP::CMDPROC(SAMP_ADDROF(0x67FB0)); }
SAMP::CMDPROC SAMP::Commands::DrawChatTimestamps() { return SAMP::CMDPROC(SAMP_ADDROF(0x67FC0)); }
SAMP::CMDPROC SAMP::Commands::ToggleAudioStreamMessages() { return SAMP::CMDPROC(SAMP_ADDROF(0x68020)); }
SAMP::CMDPROC SAMP::Commands::ToggleURLMessages() { return SAMP::CMDPROC(SAMP_ADDROF(0x68090)); }
SAMP::CMDPROC SAMP::Commands::ToggleHUDScaleFix() { return SAMP::CMDPROC(SAMP_ADDROF(0x68100)); }
SAMP::CMDPROC SAMP::Commands::PrintMemory() { return SAMP::CMDPROC(SAMP_ADDROF(0x68140)); }
SAMP::CMDPROC SAMP::Commands::SetFrameLimiter() { return SAMP::CMDPROC(SAMP_ADDROF(0x68160)); }
SAMP::CMDPROC SAMP::Commands::ToggleHeadMoves() { return SAMP::CMDPROC(SAMP_ADDROF(0x681F0)); }
SAMP::CMDPROC SAMP::Commands::Quit() { return SAMP::CMDPROC(SAMP_ADDROF(0x68270)); }
SAMP::CMDPROC SAMP::Commands::CmpStat() { return SAMP::CMDPROC(SAMP_ADDROF(0x68280)); }
SAMP::CMDPROC SAMP::Commands::SavePosition() { return SAMP::CMDPROC(SAMP_ADDROF(0x68290)); }
SAMP::CMDPROC SAMP::Commands::SavePositionOnly() { return SAMP::CMDPROC(SAMP_ADDROF(0x68410)); }
SAMP::CMDPROC SAMP::Commands::PrintCurrentInterior() { return SAMP::CMDPROC(SAMP_ADDROF(0x68860)); }
SAMP::CMDPROC SAMP::Commands::ToggleObjectsLight() { return SAMP::CMDPROC(SAMP_ADDROF(0x68890)); }
SAMP::CMDPROC SAMP::Commands::ToggleDebugLabels() { return SAMP::CMDPROC(SAMP_ADDROF(0x688B0)); }
SAMP::CMDPROC SAMP::Commands::SendRconCommand() { return SAMP::CMDPROC(SAMP_ADDROF(0x688C0)); }
SAMP::CMDPROC SAMP::Commands::Debug::SetPlayerSkin() { return SAMP::CMDPROC(SAMP_ADDROF(0x68590)); }
SAMP::CMDPROC SAMP::Commands::Debug::CreateVehicle() { return SAMP::CMDPROC(SAMP_ADDROF(0x68600)); }
SAMP::CMDPROC SAMP::Commands::Debug::EnableVehicleSelection() { return SAMP::CMDPROC(SAMP_ADDROF(0x68740)); }
SAMP::CMDPROC SAMP::Commands::Debug::SetWorldWeather() { return SAMP::CMDPROC(SAMP_ADDROF(0x68760)); }
SAMP::CMDPROC SAMP::Commands::Debug::SetWorldTime() { return SAMP::CMDPROC(SAMP_ADDROF(0x687B0)); }

void SAMP::Commands::Setup() {
	((void(__cdecl*)())SAMP_ADDROF(0x689A0))();
}

#endif
