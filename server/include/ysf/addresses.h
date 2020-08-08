#pragma once

#include "utils/memory.h"

enum class SAMPVersion {
	VERSION_UNKNOWN,
	VERSION_037,
	VERSION_037_R2,
};

class CAddress {
public:

#ifdef _WIN32
	static constexpr unsigned long FUNC_Logprintf_03Z		= 0x00486CB0;
	static constexpr unsigned long FUNC_Logprintf_03ZR2_2	= 0x00487310;
	static constexpr unsigned long FUNC_Logprintf_03ZR3		= 0x00487460;
	static constexpr unsigned long FUNC_Logprintf_03ZR4		= 0x004875F0;
	static constexpr unsigned long FUNC_Logprintf_037		= 0x0048A0B0;
	static constexpr unsigned long FUNC_Logprintf_037_R2_1	= 0x0048C8D0;
#else
	static constexpr unsigned long FUNC_Logprintf_03Z		= 0x080A7440;
	static constexpr unsigned long FUNC_Logprintf_03ZR2_2	= 0x080A77D0;
	static constexpr unsigned long FUNC_Logprintf_03ZR3		= 0x080A78E0;
	static constexpr unsigned long FUNC_Logprintf_03ZR4		= 0x80A7A90;
	static constexpr unsigned long FUNC_Logprintf_037		= 0x080A9000;
	static constexpr unsigned long FUNC_Logprintf_037_R2_1	= 0x080A91D0;
#endif

	static unsigned long			VAR_pRestartWaitTime;
	static unsigned long			VAR_pPosSyncBounds[4];
	static unsigned long			VAR_wRCONUser;
	static unsigned long			ARRAY_ConsoleCommands;

	static unsigned long			FUNC_CConsole__AddStringVariable;
	static unsigned long			FUNC_CConsole__GetStringVariable;
	static unsigned long			FUNC_CConsole__SetStringVariable;
	static unsigned long			FUNC_CConsole__GetIntVariable;
	static unsigned long			FUNC_CConsole__SetIntVariable;
	static unsigned long			FUNC_CConsole__GetBoolVariable;
	static unsigned long			FUNC_CConsole__ModifyVariableFlags;
	static unsigned long			FUNC_CConsole__FindVariable;
	static unsigned long			FUNC_CConsole__SendRules;
	static unsigned long			FUNC_CConsole__Execute;

	static unsigned long			FUNC_CNetGame__SetWeather;
	static unsigned long			FUNC_CNetGame__SetGravity;

	static unsigned long			FUNC_CFilterscripts__LoadFilterscript;
	static unsigned long			FUNC_CFilterscripts__UnLoadFilterscript;
	static unsigned long			FUNC_ContainsInvalidChars;

	static unsigned long			FUNC_CPlayer__SpawnForWorld;
	static unsigned long			FUNC_CVehicle__Respawn;
	static unsigned long			FUNC_CPlayerPool__HandleVehicleRespawn;
	static unsigned long			FUNC_CObject__SpawnForPlayer;

	static unsigned long			FUNC_ProcessQueryPacket;
	static unsigned long			FUNC_Packet_WeaponsUpdate;
	static unsigned long			FUNC_Packet_StatsUpdate;
	static unsigned long			FUNC_format_amxstring;

	static unsigned long			ADDR_GetNetworkStats_VerbosityLevel;
	static unsigned long			ADDR_GetPlayerNetworkStats_VerbosityLevel;

	static unsigned long			ADDR_RecordingDirectory;

	static unsigned long			FUNC_CGameMode__OnPlayerConnect;
	static unsigned long			FUNC_CGameMode__OnPlayerDisconnect;
	static unsigned long			FUNC_CGameMode__OnPlayerSpawn;
	static unsigned long			FUNC_CGameMode__OnPlayerStreamIn;
	static unsigned long			FUNC_CGameMode__OnPlayerStreamOut;
	static unsigned long			FUNC_CGameMode__OnDialogResponse;

	static void	Initialize(void *logprintf_addr);

};
