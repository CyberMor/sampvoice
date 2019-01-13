#ifndef _ADDRESSES_H_
#define _ADDRESSES_H_

#include "includes/types.h"
#include "utils/memory.h"

#ifdef _WIN32
	#define RAKNET_START_OFFSET							1
	#define RAKNET_SEND_OFFSET							7
	#define RAKNET_RECEIVE_OFFSET						10
	#define RAKNET_DEALLOCATE_PACKET_OFFSET				12
	#define RAKNET_SET_ALLOWED_PLAYERS_OFFSET			13
	#define RAKNET_GET_LAST_PING_OFFSET					19
	#define RAKNET_REGISTER_RPC_OFFSET					29
	#define RAKNET_UNREGISTER_RPC_OFFSET				31
	#define RAKNET_RPC_OFFSET							32
	#define RAKNET_GET_LOCAL_IP_OFFSET					52
	#define RAKNET_GET_INDEX_FROM_PLAYERID_OFFSET		57
	#define RAKNET_GET_PLAYERID_FROM_INDEX_OFFSET		58
	#define RAKNET_ADD_BAN_OFFSET						60
	#define RAKNET_REMOVE_BAN_OFFSET					61
	#define RAKNET_CLEAR_BAN_OFFSET						62
	#define RAKNET_SET_TIMEOUT_OFFSET					65
#else
	#define RAKNET_START_OFFSET							2
	#define RAKNET_SEND_OFFSET							9
	#define RAKNET_RECEIVE_OFFSET						11
	#define RAKNET_DEALLOCATE_PACKET_OFFSET				13
	#define RAKNET_SET_ALLOWED_PLAYERS_OFFSET			14
	#define RAKNET_GET_LAST_PING_OFFSET					20
	#define RAKNET_REGISTER_RPC_OFFSET					30
	#define RAKNET_UNREGISTER_RPC_OFFSET				32
	#define RAKNET_RPC_OFFSET							35
	#define RAKNET_GET_LOCAL_IP_OFFSET					53
	#define RAKNET_GET_INDEX_FROM_PLAYERID_OFFSET		58
	#define RAKNET_GET_PLAYERID_FROM_INDEX_OFFSET		59
	#define RAKNET_ADD_BAN_OFFSET						61
	#define RAKNET_REMOVE_BAN_OFFSET					62
	#define RAKNET_CLEAR_BAN_OFFSET						63
	#define RAKNET_SET_TIMEOUT_OFFSET					65
#endif

enum class SAMPVersion
{
	VERSION_UNKNOWN,
	VERSION_037,
	VERSION_037_R2,
};

class CAddress {
public:

#ifdef _WIN32
	static constexpr DWORD FUNC_Logprintf_03Z		= 0x00486CB0;
	static constexpr DWORD FUNC_Logprintf_03ZR2_2	= 0x00487310;
	static constexpr DWORD FUNC_Logprintf_03ZR3		= 0x00487460;
	static constexpr DWORD FUNC_Logprintf_03ZR4		= 0x004875F0;
	static constexpr DWORD FUNC_Logprintf_037		= 0x0048A0B0;
	static constexpr DWORD FUNC_Logprintf_037_R2_1	= 0x0048C8D0;
#else
	static constexpr DWORD FUNC_Logprintf_03Z		= 0x080A7440;
	static constexpr DWORD FUNC_Logprintf_03ZR2_2	= 0x080A77D0;
	static constexpr DWORD FUNC_Logprintf_03ZR3		= 0x080A78E0;
	static constexpr DWORD FUNC_Logprintf_03ZR4		= 0x80A7A90;
	static constexpr DWORD FUNC_Logprintf_037		= 0x080A9000;
	static constexpr DWORD FUNC_Logprintf_037_R2_1	= 0x080A91D0;
#endif

	static DWORD			VAR_pRestartWaitTime;
	static DWORD			VAR_pPosSyncBounds[4];
	static DWORD			VAR_wRCONUser;
	static DWORD			ARRAY_ConsoleCommands;

	static DWORD			FUNC_CConsole__AddStringVariable;
	static DWORD			FUNC_CConsole__GetStringVariable;
	static DWORD			FUNC_CConsole__SetStringVariable;
	static DWORD			FUNC_CConsole__GetIntVariable;
	static DWORD			FUNC_CConsole__SetIntVariable;
	static DWORD			FUNC_CConsole__GetBoolVariable;
	static DWORD			FUNC_CConsole__ModifyVariableFlags;
	static DWORD			FUNC_CConsole__FindVariable;
	static DWORD			FUNC_CConsole__SendRules;
	static DWORD			FUNC_CConsole__Execute;

	static DWORD			FUNC_CNetGame__SetWeather;
	static DWORD			FUNC_CNetGame__SetGravity;

	static DWORD			FUNC_CFilterscripts__LoadFilterscript;
	static DWORD			FUNC_CFilterscripts__UnLoadFilterscript;
	static DWORD			FUNC_ContainsInvalidChars;

	static DWORD			FUNC_CPlayer__SpawnForWorld;
	static DWORD			FUNC_CVehicle__Respawn;
	static DWORD			FUNC_CPlayerPool__HandleVehicleRespawn;
	static DWORD			FUNC_CObject__SpawnForPlayer;

	static DWORD			FUNC_ProcessQueryPacket;
	static DWORD			FUNC_Packet_WeaponsUpdate;
	static DWORD			FUNC_Packet_StatsUpdate;
	static DWORD			FUNC_format_amxstring;

	static DWORD			ADDR_GetNetworkStats_VerbosityLevel;
	static DWORD			ADDR_GetPlayerNetworkStats_VerbosityLevel;

	static DWORD			ADDR_RecordingDirectory;

	static DWORD			FUNC_CGameMode__OnPlayerConnect;
	static DWORD			FUNC_CGameMode__OnPlayerDisconnect;
	static DWORD			FUNC_CGameMode__OnPlayerSpawn;
	static DWORD			FUNC_CGameMode__OnPlayerStreamIn;
	static DWORD			FUNC_CGameMode__OnPlayerStreamOut;
	static DWORD			FUNC_CGameMode__OnDialogResponse;

	static void	Initialize(void *logprintf_addr) {
		
		DWORD dwTemp; 
	#ifdef _WIN32
		dwTemp = FindPattern("\xD9\x15\x00\x00\x00\x00\xD8\x1D\x00\x00\x00\x00\xDF\xE0\xF6\xC4\x41\x75\x07", "xx????xx????xxxxxxx") + 6;
		VAR_pRestartWaitTime = *(DWORD*)(dwTemp + 2);
		
		dwTemp = FindPattern("Shot out of bounds.\0", "xxxxxxxxxxxxxxxxxxxx");
		if (dwTemp)
		{
			VAR_pPosSyncBounds[0] = dwTemp + 20;
			VAR_pPosSyncBounds[1] = VAR_pPosSyncBounds[0] + 4;
		}
		dwTemp = FindPattern("Offset out of bounds.\0\0\0", "xxxxxxxxxxxxxxxxxxxxxxxx");
		if (dwTemp)
		{
			VAR_pPosSyncBounds[2] = dwTemp + 24;
		}
		VAR_pPosSyncBounds[3] =						FindPattern("\x00\x50\x43\x48\x00\x00\xC8\xC2\x00\x00\x00\x3F\xB2\x00\x00\x00\xAD\x00\x00\x00\xAE\x00\x00\x00maxnpc", "????xxxxxxxxxxxxxxxxxxxxxxxxxx");

		VAR_wRCONUser =								0x004E5874;
		ARRAY_ConsoleCommands =						FindPattern("echo", "xxxx");

		FUNC_CConsole__AddStringVariable =			FindPattern("\x53\x56\x57\x8B\x7C\x24\x18\x85\xFF", "xxxxxxxxx");
		FUNC_CConsole__GetStringVariable =			FindPattern("\x8B\x44\x24\x04\x50\xE8\x00\x00\x00\x00\x85\xC0\x74\x0B", "xxxxxx????xxxx");
		FUNC_CConsole__SetStringVariable =			FindPattern("\x8B\x44\x24\x04\x53\x50\xE8\xD5\xFE\xFF\xFF\x8B\xD8\x85\xDB", "xxxxxxx???xxxx");
		FUNC_CConsole__GetIntVariable =				FindPattern("\x8B\x44\x24\x04\x50\xE8\x00\x00\x00\x00\x85\xC0\x74\x0D\x83\x38\x01\x75\x08", "xxxxxx????xxxxxxxxx");
		FUNC_CConsole__SetIntVariable =				FindPattern("\x8B\x44\x24\x04\x50\xE8\xF6\xFD\xFF\xFF\x85\xC0\x74\xE0\x83\x38\x01", "xxxxxx????xx??xxx") + 0x20;
		FUNC_CConsole__GetBoolVariable =			FindPattern("\x8B\x44\x24\x04\x50\xE8\x00\x00\x00\x00\x85\xC0\x74\x0D", "xxxxxx????xxxx") + 0x90;
		FUNC_CConsole__ModifyVariableFlags =		FindPattern("\x8B\x44\x24\x04\x50\xE8\x16\xFF\xFF\xFF\x85\xC0\x74\x07", "xxxxxx????xxxx");
		FUNC_CConsole__FindVariable =				FindPattern("\x8B\x84\x24\x30\x01\x00\x00\x53\x56\x57", "xxxxxxxxxx") - 0x1B;
		FUNC_CConsole__SendRules =					FindPattern("\x81\xEC\x08\x04\x00\x00\x53\x55\x56\x57\x8B\xF9\x8B\x77\x04", "xx????xxxxxxxxx");
		FUNC_CConsole__Execute =					FindPattern("\x55\x8B\xEC\x83\xE4\xF8\x81\xEC\x0C\x01\x00\x00", "xxxxxxxxxxxx");

		FUNC_CNetGame__SetWeather =					FindPattern("\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x81\xEC\x98\x01\x00\x00\x56\x8B\xF1", "xxx????xxxxxxxxxxxxxxxxx?xxxxx"); // 00490A40
		FUNC_CNetGame__SetGravity =					FUNC_CNetGame__SetWeather + 0xD0; // 0x00490B10;

		FUNC_CFilterscripts__LoadFilterscript =		FindPattern("\x8B\x44\x24\x04\x81\xEC\x04\x01\x00\x00", "xxxxxxxxxx");
		FUNC_CFilterscripts__UnLoadFilterscript =	FindPattern("\xCC\x51\x53\x8B\x5C\x24\x0C\x55\x56\x57\x89", "xxxxxxxxxxx") + 0x1;

		FUNC_ContainsInvalidChars =					FindPattern("\x8B\x4C\x24\x04\x8A\x01\x84\xC0", "xxxxxxxx");

		FUNC_CPlayer__SpawnForWorld =				FindPattern("\x56\x8B\xF1\x8B\x86\x3B\x26\x00\x00\x85\xC0\x0F\x84", "xxxxx????xxxx");
		FUNC_CVehicle__Respawn =					FindPattern("\x53\x33\xC0\x56\x8B\xF1\x57\xB9\x10\x00\x00\x00\x8D\x7E\x0C", "xxxxxxxxx???xxx");
		FUNC_CPlayerPool__HandleVehicleRespawn =	FindPattern("\x53\x55\x56\x8B\xF1\x8B\xAE\x68\x09", "xxxxxx?xx");
		FUNC_CObject__SpawnForPlayer =				0x00497790;

		FUNC_ProcessQueryPacket =					FindPattern("\x83\xEC\x24\x53\x55\x56\x57\x8B\x7C\x24", "xxxxxxxxxx");
		FUNC_Packet_WeaponsUpdate =					FindPattern("\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x0\x0\x0\x0\x50\x64\x89\x25\x0\x0\x0\x0\x81\xEC\x2C\x01\x00\x00\x55\x56", "xx????xx????xxxx????xxxx??xx");
		FUNC_Packet_StatsUpdate =					FindPattern("\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x81\xEC\x20\x01\x00\x00\x56\x57", "xxx????xx????xxxxxxxxxxx???xx");
		FUNC_format_amxstring =						FindPattern("\x8B\x54\x24\x08\x56\x8B\x74\x24\x08\x57\x33\xC0", "xxxxxxxxxxxx");

		ADDR_GetNetworkStats_VerbosityLevel =		FindPattern("\x6A\x01\x8D\x4C\x0C", "xxxxx"); // 0x0047362A
		ADDR_GetPlayerNetworkStats_VerbosityLevel = FindPattern("\x6A\x01\x8D\x44\x04", "xxxxx"); // 0x004730B9;

		ADDR_RecordingDirectory =					FindPattern("\x75\xDF\x8D\x44\x24\x18\x50\x8D\x8C\x24", "xxxxx?xxxx") + 14; // 0x00482265

		FUNC_CGameMode__OnPlayerConnect =			FindPattern("\x83\xEC\x08\x56\x8B\xF1\x8A\x46\x68", "xxxxxxxxx"); // 0x0046D910
		FUNC_CGameMode__OnPlayerDisconnect =		FUNC_CGameMode__OnPlayerConnect + 0x60; // 0x0046D970;
		FUNC_CGameMode__OnPlayerSpawn =				FUNC_CGameMode__OnPlayerDisconnect + 0x60; // 0x0046D9D0;
		FUNC_CGameMode__OnPlayerStreamIn =			FUNC_CGameMode__OnPlayerSpawn + 0xF10; // 0x0046E8E0
		FUNC_CGameMode__OnPlayerStreamOut =			FUNC_CGameMode__OnPlayerStreamIn + 0x70; // 0x0046E950;
		FUNC_CGameMode__OnDialogResponse =			FUNC_CGameMode__OnPlayerStreamOut + 0x230;  // 0x0046EB80;

		#else

		// Thx for Mellnik
		VAR_pRestartWaitTime = 						NULL;
		VAR_wRCONUser = NULL;
		ARRAY_ConsoleCommands =						FindPattern("echo", "xxxx");


		FUNC_CConsole__AddStringVariable = 			FindPattern("\x55\x89\xE5\x56\x53\x83\xEC\x00\x8B\x75\x00\x85\xF6\x74\x00\x89\x34\x24", "xxxxxxx?xx?xxx?xxx");
		FUNC_CConsole__GetStringVariable =			FUNC_CConsole__AddStringVariable - 0x760;//0x80A0190;
		FUNC_CConsole__SetStringVariable =			FindPattern("\x55\x89\xE5\x83\xEC\x00\x89\x75\x00\x8B\x45\x00\x89\x7D\x00\x8B\x7D\x00\x89\x5D\x00\x89\x44\x24\x00\x8B\x45\x00", "xxxxx?xx?xx?xx?xx?xx?xxx?xx?");
		FUNC_CConsole__SetIntVariable =				FindPattern("\x83\x38\x00\x74\x00\xC9\xC3\x8B\x50\x00\x8B\x45\x00", "xx?x?xxxx?xx?") - 0x1C;
		FUNC_CConsole__GetIntVariable =				FUNC_CConsole__SetIntVariable + 0x30; // 0x80A0070;
		FUNC_CConsole__GetBoolVariable =			FUNC_CConsole__SetIntVariable - 0x30;//0x80A0010;
		FUNC_CConsole__ModifyVariableFlags =		FindPattern("\x89\x04\x24\xE8\x00\x00\x00\x00\x85\xC0\x89\xC2\x74\x00\x8B\x45\x00", "xxxx????xxxxx?xx?") - 0x10;	
		FUNC_CConsole__FindVariable =				FindPattern("\xB9\xFF\x00\x00\x00\x89\xE5\x81\xEC\x68\x01\x00\x00", "xxxxxxxxxxxxx") - 0x1;
		FUNC_CConsole__SendRules =					FindPattern("\x55\x31\xD2\x89\xE5\x57\x56\x53\x81\xEC\x4C\x04", "xxxxxxxxxxxx");
		FUNC_CConsole__Execute =					FindPattern("\x55\x89\xE5\x57\x56\x53\x81\xEC\x3C\x01\x00\x00\x8B\x45\x0C", "xxxxxxxxxxxxxxx");

		FUNC_CNetGame__SetWeather = 0x80AE6D0;
		FUNC_CNetGame__SetGravity = 0x80AE7D0;

		FUNC_CFilterscripts__LoadFilterscript =		FindPattern("\x89\x7D\x00\x8B\x45\x00\x8B\x7D\x00\x89\x5D\x00\x89\x44\x24\x00", "xx?xx?xx?xx?xxx?") - 0x9;
		FUNC_CFilterscripts__UnLoadFilterscript =	FindPattern("\x31\xF6\x53\x83\xEC\x00\x8B\x45\x00\x8B\x7D\x00\x89\xC3", "xxxxx?xx?xx?xx") - 0x5;

		FUNC_ContainsInvalidChars =					FindPattern("\x53\x8B\x5D\x00\x0F\xB6\x0B\x84\xC9\x74\x00\x66\x90", "xxx?xxxxxx?xx") - 0x3;

		FUNC_CPlayer__SpawnForWorld =				FindPattern("\x55\x89\xE5\x56\x53\x83\xEC\x00\x8B\x75\x00\xA1\x00\x00\x00\x00", "xxxxxxx?xx?x????");
		FUNC_CObject__SpawnForPlayer =				0x80C8080;

		DWORD addr = FindPattern("\x55\x31\xD2\x89\xE5\x57\xB9\x40\x00\x00\x00", "xxxxxxxxxxx");
		
		FUNC_ProcessQueryPacket =					FindPattern("\x81\xEC\xA8\x00\x00\x00\x89\x5D\xF4\x8B\x5D\x14\x89\x75\xF8", "xxxxxxxxxxxxxxx") - 0x3;
		FUNC_Packet_WeaponsUpdate =					FindPattern("\x55\x31\xC0\x89\xE5\x81\xEC\x58\x01\x00\x00", "xxxxxxxxxxx");
		FUNC_Packet_StatsUpdate =					FindPattern("\x55\x31\xD2\x89\xE5\x81\xEC\x58\x01\x00\x00\x89\x5D\xF4", "xxxxxxxxxxxxxx"); // 80AD430
		FUNC_format_amxstring = FindPattern("\x55\xB8\x00\x10\x00\x00\x89\xE5\x56\x53\x83\xEC\x20", "xxxxxxxxxxxxx");

		ADDR_GetNetworkStats_VerbosityLevel =		FindPattern("\xB8\x01\x00\x00\x00\x83\xD9\x03", "xxxxxxxx");
		ADDR_GetPlayerNetworkStats_VerbosityLevel = FindPattern("\xBB\x01\x00\x00\x00\x83\xD9\x03", "xxxxxxxx"); // 080DD7FA
		
		FUNC_CGameMode__OnPlayerConnect = 0x80A5160;
		FUNC_CGameMode__OnPlayerDisconnect = 0x80A51D0;
		FUNC_CGameMode__OnPlayerSpawn = 0x80A5250;
		FUNC_CGameMode__OnPlayerStreamIn = 0x80A6450;
		FUNC_CGameMode__OnPlayerStreamOut = 0x80A64D0;
		FUNC_CGameMode__OnDialogResponse = 0x80A6750;

		switch((DWORD)(logprintf_addr))
		{
			case CAddress::FUNC_Logprintf_037:
			{
				VAR_pRestartWaitTime =						0x081A0840;
				break;
			}
			case CAddress::FUNC_Logprintf_037_R2_1:
			{
				VAR_pRestartWaitTime =						0x0815A528; // 12.0
				VAR_pPosSyncBounds[0] =						0x08150710;
				VAR_pPosSyncBounds[1] =						0x0815070C;
				VAR_pPosSyncBounds[2] =						0x08150718;
				VAR_pPosSyncBounds[3] =						0x08150714;
				ADDR_RecordingDirectory =					0x080CC7D1;
				FUNC_CVehicle__Respawn =					0x814B4C0;
				FUNC_CPlayerPool__HandleVehicleRespawn =	0x80D1480;
				VAR_wRCONUser =								0x08197DF0;
				ARRAY_ConsoleCommands =						0x08196920;
				break;
			}
		}
		#endif
	
		if (VAR_pRestartWaitTime)
			Unlock((void*)VAR_pRestartWaitTime, 4);

		for (BYTE i = 0; i < 4; i++)
		{
			if (VAR_pPosSyncBounds[i])
				Unlock((void*)VAR_pPosSyncBounds[i], 4);
		}

		if(ADDR_GetNetworkStats_VerbosityLevel)
		{
	#ifdef __WIN32
			Unlock((void*)ADDR_GetNetworkStats_VerbosityLevel, 2);
	#else
			Unlock((void*)ADDR_GetNetworkStats_VerbosityLevel, 5);
	#endif
		}

		if(ADDR_GetPlayerNetworkStats_VerbosityLevel)
		{
	#ifdef __WIN32
			Unlock((void*)ADDR_GetPlayerNetworkStats_VerbosityLevel, 2);
	#else
			Unlock((void*)ADDR_GetPlayerNetworkStats_VerbosityLevel, 5);
	#endif
		}
		
	}

};

DWORD CAddress::VAR_pRestartWaitTime = NULL;
DWORD CAddress::VAR_pPosSyncBounds[4];
DWORD CAddress::VAR_wRCONUser = NULL;
DWORD CAddress::ARRAY_ConsoleCommands = NULL;
DWORD CAddress::FUNC_CConsole__AddStringVariable = NULL;
DWORD CAddress::FUNC_CConsole__GetStringVariable = NULL;
DWORD CAddress::FUNC_CConsole__SetStringVariable = NULL;
DWORD CAddress::FUNC_CConsole__GetIntVariable = NULL;
DWORD CAddress::FUNC_CConsole__SetIntVariable = NULL;
DWORD CAddress::FUNC_CConsole__GetBoolVariable = NULL;
DWORD CAddress::FUNC_CConsole__ModifyVariableFlags = NULL;
DWORD CAddress::FUNC_CConsole__FindVariable = NULL;
DWORD CAddress::FUNC_CConsole__SendRules = NULL;
DWORD CAddress::FUNC_CConsole__Execute = NULL;
DWORD CAddress::FUNC_CNetGame__SetWeather = NULL;
DWORD CAddress::FUNC_CNetGame__SetGravity = NULL;
DWORD CAddress::FUNC_CFilterscripts__LoadFilterscript = NULL;
DWORD CAddress::FUNC_CFilterscripts__UnLoadFilterscript = NULL;
DWORD CAddress::FUNC_ContainsInvalidChars = NULL;
DWORD CAddress::FUNC_CPlayer__SpawnForWorld = NULL;
DWORD CAddress::FUNC_CVehicle__Respawn = NULL;
DWORD CAddress::FUNC_CPlayerPool__HandleVehicleRespawn = NULL;
DWORD CAddress::FUNC_CObject__SpawnForPlayer = NULL;
DWORD CAddress::FUNC_ProcessQueryPacket = NULL;
DWORD CAddress::FUNC_Packet_WeaponsUpdate = NULL;
DWORD CAddress::FUNC_Packet_StatsUpdate = NULL;
DWORD CAddress::FUNC_format_amxstring = NULL;
DWORD CAddress::ADDR_GetNetworkStats_VerbosityLevel = NULL;
DWORD CAddress::ADDR_GetPlayerNetworkStats_VerbosityLevel = NULL;
DWORD CAddress::ADDR_RecordingDirectory = NULL;
DWORD CAddress::FUNC_CGameMode__OnPlayerConnect = NULL;
DWORD CAddress::FUNC_CGameMode__OnPlayerDisconnect = NULL;
DWORD CAddress::FUNC_CGameMode__OnPlayerSpawn = NULL;
DWORD CAddress::FUNC_CGameMode__OnPlayerStreamIn = NULL;
DWORD CAddress::FUNC_CGameMode__OnPlayerStreamOut = NULL;
DWORD CAddress::FUNC_CGameMode__OnDialogResponse = NULL;

#endif