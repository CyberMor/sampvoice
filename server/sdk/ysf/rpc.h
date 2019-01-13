#ifndef _RPCS_H_
#define _RPCS_H_

#include "includes/types.h"

class RPCID
{
	static BYTE tmp;
	BYTE val;
public:
	constexpr RPCID(int value) : val(value) {}
	constexpr operator BYTE() const
	{ return val; }
	BYTE* operator&() const
	{ return &(tmp = val); }
	BYTE* operator&()
	{ return &val; }
};

BYTE RPCID::tmp;

constexpr RPCID RPC_ClientConnect			= 0x19; // 25
constexpr RPCID RPC_SelectObject			= 0x1B; // 27
constexpr RPCID RPC_Spawn					= 0x34; // 52
constexpr RPCID RPC_Death					= 0x35; // 53
constexpr RPCID RPC_Chat					= 0x65; // 101
constexpr RPCID RPC_ClientCheck				= 0x67; // 103
constexpr RPCID RPC_PickedUpPickup			= 0x83; // 131
constexpr RPCID RPC_UpdateScoresPingsIPs	= 0x9B; // 155

constexpr RPCID RPC_SetPlayerName			= 0x0B; // 11
constexpr RPCID RPC_WorldPlayerAdd			= 0x20; // 32
constexpr RPCID RPC_CreateObject			= 0x2C; // 44
constexpr RPCID RPC_SetObjectPos			= 0x2D; // 45
constexpr RPCID RPC_SetObjectRot			= 0x2E; // 46
constexpr RPCID RPC_DestroyObject			= 0x2F; // 47
constexpr RPCID RPC_ChatBubble				= 0x3B; // 59
constexpr RPCID RPC_DestroyPickup			= 0x3F; // 63
constexpr RPCID RPC_SetPlayerTeam			= 0x45; // 69
constexpr RPCID RPC_SetPlayerColor			= 0x48; // 72
constexpr RPCID RPC_DisplayGameText			= 0x49; // 73
constexpr RPCID RPC_AttachObject			= 0x4B; // 75
constexpr RPCID RPC_AttachCameraToObject	= 0x51; // 81
constexpr RPCID RPC_SetObjectMaterial		= 0x54; // 84
constexpr RPCID RPC_StopFlashGangZone		= 0x55; // 85
constexpr RPCID RPC_ApplyAnimation			= 0x56; // 86
constexpr RPCID RPC_SetFightingStyle		= 0x59; // 89
constexpr RPCID RPC_ClientMessage			= 0x5D; // 93
constexpr RPCID RPC_CreatePickup			= 0x5F; // 95
constexpr RPCID RPC_ScmEvent				= 0x60; // 96
constexpr RPCID RPC_MoveObject				= 0x63; // 99
constexpr RPCID RPC_SetTextDrawString		= 0x69; // 105
constexpr RPCID RPC_ShowGangZone			= 0x6C; // 108
constexpr RPCID RPC_Widescreen				= 0x6F; // 111
constexpr RPCID RPC_SetPlayerAttachedObject	= 0x71; // 113
constexpr RPCID RPC_EditObject				= 0x75; // 117
constexpr RPCID RPC_HideGangZone			= 0x78; // 120
constexpr RPCID RPC_FlashGangZone			= 0x79; // 121
constexpr RPCID RPC_StopObject				= 0x74; // 122
constexpr RPCID RPC_ServerJoin				= 0x89; // 137
constexpr RPCID RPC_ServerQuit				= 0x8A; // 138
constexpr RPCID RPC_InitGame				= 0x8B; // 139
constexpr RPCID RPC_SetGravity				= 0x92; // 146
constexpr RPCID RPC_SetPlayerSkin			= 0x99; // 153
constexpr RPCID RPC_WorldPlayerRemove		= 0xA3; // 163
constexpr RPCID RPC_WorldVehicleAdd			= 0xA4; // 164
constexpr RPCID RPC_WorldVehicleRemove		= 0xA5; // 165
constexpr RPCID RPC_DeathBroadcast			= 0xA6; // 166

enum PacketEnumeration : unsigned char {
	ID_TIMESTAMP = 40,
	ID_VEHICLE_SYNC = 200,
	ID_AIM_SYNC = 203,
	ID_BULLET_SYNC = 206,
	ID_PLAYER_SYNC = 207,
	ID_MARKERS_SYNC,
	ID_UNOCCUPIED_SYNC = 209, 
	ID_TRAILER_SYNC = 210,
	ID_PASSENGER_SYNC = 211,
	ID_SPECTATOR_SYNC = 212,
	ID_RCON_COMMAND = 201,
	ID_RCON_RESPONCE,
	ID_WEAPONS_UPDATE = 204,
	ID_STATS_UPDATE = 205,
};

#endif