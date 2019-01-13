#ifndef _STRUCTS_H_
#define _STRUCTS_H_

#include <map>
#include <pawn/amx/amx.h>
#include <raknet/networktypes.h>

#include "utils/cvector.h"
#include "utils/cvector2d.h"
#include "includes/types.h"

#if !defined PAD
	#define PAD(a, b) char a[b]
#endif

/* -------------------------------------------------------- */

// Defines from SAMPGDK
#define MAX_PLAYER_NAME (24)
#define MAX_CLIENT_MESSAGE (144)
#define MAX_PLAYERS (1000)
#define MAX_VEHICLES (2000)
#define MAX_ACTORS (1000)
#define INVALID_PLAYER_ID (0xFFFF)
#define INVALID_VEHICLE_ID (0xFFFF)
#define INVALID_ACTOR_ID (0xFFFF)
#define NO_TEAM (255)
#define MAX_OBJECTS (1000)
#define INVALID_OBJECT_ID (0xFFFF)
#define MAX_GANG_ZONES (1024)
#define MAX_TEXT_DRAWS (2048)
#define MAX_PLAYER_TEXT_DRAWS (256)
#define MAX_MENUS (128)
#define MAX_3DTEXT_GLOBAL (1024)
#define MAX_3DTEXT_PLAYER (1024)
#define MAX_PICKUPS (4096)
#define INVALID_MENU (0xFF)
#define INVALID_TEXT_DRAW (0xFFFF)
#define INVALID_GANG_ZONE (-1)
#define INVALID_3DTEXT_ID (0xFFFF)
#define SERVER_VARTYPE_NONE (0)
#define SERVER_VARTYPE_INT (1)
#define SERVER_VARTYPE_STRING (2)
#define SERVER_VARTYPE_FLOAT (3)
#define TEXT_DRAW_FONT_SPRITE_DRAW (4)
#define TEXT_DRAW_FONT_MODEL_PREVIEW (5)
#define DIALOG_STYLE_MSGBOX (0)
#define DIALOG_STYLE_INPUT (1)
#define DIALOG_STYLE_LIST (2)
#define DIALOG_STYLE_PASSWORD (3)
#define DIALOG_STYLE_TABLIST (4)
#define DIALOG_STYLE_TABLIST_HEADERS (5)
#define PLAYER_STATE_NONE (0)
#define PLAYER_STATE_ONFOOT (1)
#define PLAYER_STATE_DRIVER (2)
#define PLAYER_STATE_PASSENGER (3)
#define PLAYER_STATE_EXIT_VEHICLE (4)
#define PLAYER_STATE_ENTER_VEHICLE_DRIVER (5)
#define PLAYER_STATE_ENTER_VEHICLE_PASSENGER (6)
#define PLAYER_STATE_WASTED (7)
#define PLAYER_STATE_SPAWNED (8)
#define PLAYER_STATE_SPECTATING (9)
#define PLAYER_MARKERS_MODE_OFF (0)
#define PLAYER_MARKERS_MODE_GLOBAL (1)
#define PLAYER_MARKERS_MODE_STREAMED (2)
#define WEAPON_BRASSKNUCKLE (1)
#define WEAPON_GOLFCLUB (2)
#define WEAPON_NITESTICK (3)
#define WEAPON_KNIFE (4)
#define WEAPON_BAT (5)
#define WEAPON_SHOVEL (6)
#define WEAPON_POOLSTICK (7)
#define WEAPON_KATANA (8)
#define WEAPON_CHAINSAW (9)
#define WEAPON_DILDO (10)
#define WEAPON_DILDO2 (11)
#define WEAPON_VIBRATOR (12)
#define WEAPON_VIBRATOR2 (13)
#define WEAPON_FLOWER (14)
#define WEAPON_CANE (15)
#define WEAPON_GRENADE (16)
#define WEAPON_TEARGAS (17)
#define WEAPON_MOLTOV (18)
#define WEAPON_COLT45 (22)
#define WEAPON_SILENCED (23)
#define WEAPON_DEAGLE (24)
#define WEAPON_SHOTGUN (25)
#define WEAPON_SAWEDOFF (26)
#define WEAPON_SHOTGSPA (27)
#define WEAPON_UZI (28)
#define WEAPON_MP5 (29)
#define WEAPON_AK47 (30)
#define WEAPON_M4 (31)
#define WEAPON_TEC9 (32)
#define WEAPON_RIFLE (33)
#define WEAPON_SNIPER (34)
#define WEAPON_ROCKETLAUNCHER (35)
#define WEAPON_HEATSEEKER (36)
#define WEAPON_FLAMETHROWER (37)
#define WEAPON_MINIGUN (38)
#define WEAPON_SATCHEL (39)
#define WEAPON_BOMB (40)
#define WEAPON_SPRAYCAN (41)
#define WEAPON_FIREEXTINGUISHER (42)
#define WEAPON_CAMERA (43)
#define WEAPON_NIGHTVISION (44)
#define WEAPON_INFRARED (45)
#define WEAPON_PARACHUTE (46)
#define WEAPON_VEHICLE (49)
#define WEAPON_DROWN (53)
#define WEAPON_COLLISION (54)
#define KEY_ACTION (1)
#define KEY_CROUCH (2)
#define KEY_FIRE (4)
#define KEY_SPRINT (8)
#define KEY_SECONDARY_ATTACK (16)
#define KEY_JUMP (32)
#define KEY_LOOK_RIGHT (64)
#define KEY_HANDBRAKE (128)
#define KEY_LOOK_LEFT (256)
#define KEY_SUBMISSION (512)
#define KEY_LOOK_BEHIND (512)
#define KEY_WALK (1024)
#define KEY_ANALOG_UP (2048)
#define KEY_ANALOG_DOWN (4096)
#define KEY_ANALOG_LEFT (8192)
#define KEY_ANALOG_RIGHT (16384)
#define KEY_YES (65536)
#define KEY_NO (131072)
#define KEY_CTRL_BACK (262144)
#define KEY_UP (-128)
#define KEY_DOWN (128)
#define KEY_LEFT (-128)
#define KEY_RIGHT (128)
#define BODY_PART_TORSO (3)
#define BODY_PART_GROIN (4)
#define BODY_PART_LEFT_ARM (5)
#define BODY_PART_RIGHT_ARM (6)
#define BODY_PART_LEFT_LEG (7)
#define BODY_PART_RIGHT_LEG (8)
#define BODY_PART_HEAD (9)
#define CLICK_SOURCE_SCOREBOARD (0)
#define EDIT_RESPONSE_CANCEL (0)
#define EDIT_RESPONSE_FINAL (1)
#define EDIT_RESPONSE_UPDATE (2)
#define SELECT_OBJECT_GLOBAL_OBJECT (1)
#define SELECT_OBJECT_PLAYER_OBJECT (2)
#define BULLET_HIT_TYPE_NONE (0)
#define BULLET_HIT_TYPE_PLAYER (1)
#define BULLET_HIT_TYPE_VEHICLE (2)
#define BULLET_HIT_TYPE_OBJECT (3)
#define BULLET_HIT_TYPE_PLAYER_OBJECT (4)

// Additional Defines
#define MAX_OBJECT_MATERIAL			16
#define MAX_PLAYER_ATTACHED_OBJECTS 10
#define	MAX_FILTER_SCRIPTS			16

#define MAX_PVARS					800
#define MAX_PVAR_NAME				40

#define MAX_MENU_TEXT_SIZE			32
#define MAX_ITEMS					12
#define MAX_COLUMNS					2

#define GAMESTATE_STOPPED			0
#define GAMESTATE_RUNNING			1
#define GAMESTATE_RESTARTING		2
/* -------------------------------------------------------- */

// Server rules special
enum CON_VARTYPE { CON_VARTYPE_FLOAT, CON_VARTYPE_INT, CON_VARTYPE_BOOL, CON_VARTYPE_STRING };

#define CON_VARFLAG_DEBUG			1
#define CON_VARFLAG_READONLY		2
#define CON_VARFLAG_RULE			4	// Gets sent with a RULES query responce

typedef void(*VARCHANGEFUNC)();

/* -------------------------------------------------------- */

// RakNet Remote system
struct PingAndClockDifferential
{
	unsigned short pingTime;
	unsigned int clockDifferential;
};

struct RemoteSystemStruct
{
	bool isActive;
	PlayerID playerId;
	PlayerID myExternalPlayerId;
	BYTE gapD[1895];
	DWORD dword774;
	WORD word778;
	BYTE gap77A[2];
	DWORD dword77C;
	DWORD dword780;
	BYTE gap784[276];
	DWORD dword898;
	BYTE gap89C[16];
	BYTE byte8AC;
	BYTE gap8AD[1023];
	unsigned int connectionTime;
	enum ConnectMode
	{
		NO_ACTION,
		DISCONNECT_ASAP,
		DISCONNECT_ASAP_SILENTLY,
		DISCONNECT_ON_NO_ACK,
		REQUESTED_CONNECTION,
		HANDLING_CONNECTION_REQUEST,
		UNVERIFIED_SENDER,
		SET_ENCRYPTION_ON_MULTIPLE_16_BYTE_PACKET,
		CONNECTED
	} connectMode;
	BYTE byteAuthTableIndex;
	BYTE byteAuthType;
	BYTE byteIsLogon;
};
static_assert(sizeof(RemoteSystemStruct) == 3255, "Invalid RemoteSystemStruct size");

/* -------------------------------------------------------- */
typedef struct _MATRIX4X4 
{
	CVector right;
	DWORD  flags;
	CVector up;
	float  pad_u;
	CVector at;
	float  pad_a;
	CVector pos;
	float  pad_p;
} MATRIX4X4, *PMATRIX4X4;
static_assert(sizeof(_MATRIX4X4) == 64, "Invalid _MATRIX4X4 size");

struct ConsoleVariable_s
{
	CON_VARTYPE		VarType;
	DWORD			VarFlags;
	void*			VarPtr;
	VARCHANGEFUNC	VarChangeFunc;
};
static_assert(sizeof(ConsoleVariable_s) == 16, "Invalid ConsoleVariable_s size");

struct ConsoleCommand_s
{
	char			szName[255];
	DWORD			dwFlags;
	void			(*fptrFunc)();
};
static_assert(sizeof(ConsoleCommand_s) == 263, "Invalid ConsoleCommand_s size");

/* -------------------------------------------------------- */
// CPlayer
/* -------------------------------------------------------- */

// Big thanks to OrMisicL
struct CAimSyncData
{
	BYTE			byteCameraMode;			// 0
	CVector			vecFront;				// 1 - 13
	CVector			vecPosition;			// 13 - 25
	float			fZAim;					// 25 - 29
	BYTE			byteCameraZoom : 6;		// 29
	BYTE			byteWeaponState  : 2;	// 29
	BYTE			byteAspectRatio;					// 30 - 31
};
static_assert(sizeof(CAimSyncData) == 31, "Invalid CAimSyncData size");

struct CVehicleSyncData
{
	WORD			wVehicleId;				// 0x001F - 0x0021
	WORD			wLRAnalog;				// 0x0021 - 0x0023
	WORD			wUDAnalog;				// 0x0023 - 0x0025
	WORD			wKeys;					// 0x0025 - 0x0027
	float			fQuaternion[4];			// 0x002B - 0x0037
	CVector			vecPosition;			// 0x0037 - 0x0043
	CVector			vecVelocity;			// 0x0043 - 0x004F
	float			fHealth;				// 0x004F - 0x0053
	BYTE			bytePlayerHealth;		// 0x0053 - 0x0054
	BYTE			bytePlayerArmour;		// 0x0054 - 0x0055
	BYTE			bytePlayerWeapon : 6;	// 0x0055 - 0x0056
	BYTE			unk_2 : 2;				// 0x0055 - 0x0056
	BYTE			byteSirenState;			// 0x0056 - 0x0057
	BYTE			byteGearState;			// 0x0057 -	0x0058
	WORD			wTrailerID;				// 0x0058 - 0x005A
    union									// 
    {
            WORD			wHydraReactorAngle[2];                       
            float           fTrainSpeed;
    };
};
static_assert(sizeof(CVehicleSyncData) == 63, "Invalid CVehicleSyncData size");

struct CPassengerSyncData
{
	WORD			wVehicleId;				// 0x005E - 0x0060
	BYTE			byteSeatFlags : 7;
	BYTE			byteDriveBy : 1;
	BYTE			bytePlayerWeapon;		// 0x0061 - 0x0062
	BYTE			bytePlayerHealth;		// 0x0062 - 0x0063
	BYTE			bytePlayerArmour;		// 0x0063 - 0x0064
	WORD			wLRAnalog;				// 0x0064 - 0x0066
	WORD			wUDAnalog;				// 0x0066 - 0x0068
	WORD			wKeys;					// 0x0068 - 0x006A
	CVector			vecPosition;			// 0x006A - 0x0076
};
static_assert(sizeof(CPassengerSyncData) == 24, "Invalid CPassengerSyncData size");

struct CSyncData
{
	WORD			wLRAnalog;				// 0x0076 - 0x0078
	WORD			wUDAnalog;				// 0x0078 - 0x007A
	WORD			wKeys;					// 0x007A - 0x007C
	CVector			vecPosition;			// 0x007C - 0x0088
	float			fQuaternion[4];			// 0x0088 - 0x008C
	BYTE			byteHealth;				// 0x0098 - 0x0099
	BYTE			byteArmour;				// 0x0099 - 0x009A
	BYTE			byteWeapon : 6;				// 0x009A - 0x009B
	BYTE			_unk_ : 2;
	BYTE			byteSpecialAction;		// 0x009B - 0x009C
	CVector			vecVelocity;			// 0x009C - 0x00A8
	CVector			vecSurfing;				// 0x00A8 - 0x00B4
	WORD			wSurfingInfo;			// 0x00B4 - 0x00B6
	union 
	{
		DWORD		dwAnimationData;		// 0x00B6 - 0x00BA
		struct 
		{
			WORD	wAnimIndex;
			WORD	wAnimFlags;
		};
	};
};
static_assert(sizeof(CSyncData) == 68, "Invalid CSyncData size");

struct CUnoccupiedSyncData
{
	WORD			wVehicleID;				// + 0x0000
	BYTE			bytePassengerSlot;			// + 0x0002
	CVector			vecRool;				// + 0x0003
	CVector			vecDirection;			// + 0x000F
	CVector			vecPosition;			// + 0x001B
	CVector			vecVelocity;			// + 0x0027
	CVector			vecTurnVelocity;		// + 0x0033
	float			fHealth;					// + 0x003F
};
static_assert(sizeof(CUnoccupiedSyncData) == 67, "Invalid CUnoccupiedSyncData size");

struct CSpectatingSyncData
{
	WORD			wLeftRightKeysOnSpectating;				// + 0x0000
	WORD			wUpDownKeysOnSpectating;				// + 0x0002
	WORD			wKeysOnSpectating;						// + 0x0004
	CVector			vecPosition;							// + 0x0006
};
static_assert(sizeof(CSpectatingSyncData) == 18, "Invalid CSpectatingSyncData size");

struct CTrailerSyncData
{
	WORD			wTrailerID;			// + 0x0000
	CVector			vecPosition;			// + 0x0002
	float			fQuaternion[4];			// + 0x000E
	CVector			vecVelocity;			// + 0x001E
	CVector			vecTurnVelocity;		// + 0x002A
};
static_assert(sizeof(CTrailerSyncData) == 54, "Invalid CTrailerSyncData size");

typedef struct CTextdraw
{
	union
	{
		BYTE byteFlags;			// 25
		struct
		{
			BYTE byteBox : 1;
			BYTE byteLeft : 1; 
			BYTE byteRight : 1;
			BYTE byteCenter : 1;
			BYTE byteProportional : 1;
			BYTE bytePadding : 3;
		};
	};
	CVector2D		vecLetter;	// 1 - 9
	DWORD			dwLetterColor;		// 9
	CVector2D		vecLine;
	DWORD			dwBoxColor;			// 21
	BYTE			byteShadow; // 26
	BYTE			byteOutline; // 27
	DWORD			dwBackgroundColor; // 31
	BYTE			byteStyle; // 32
	BYTE			byteSelectable; // 32
	CVector2D		vecPos;
	WORD			dwModelIndex; // 41 - 43
	CVector			vecRot;  // 43 - 55
	float			fZoom; // 55 - 59
	WORD			color1; // 59 - 61
	WORD			color2; // 61 - 63
} _CTextdraw;
static_assert(sizeof(CTextdraw) == 63, "Invalid CTextdraw size");

struct CPlayerTextDraw
{
	BOOL			bSlotState[MAX_PLAYER_TEXT_DRAWS];
	CTextdraw	    *TextDraw[MAX_PLAYER_TEXT_DRAWS];
	char			*szFontText[MAX_PLAYER_TEXT_DRAWS];
	bool			bHasText[MAX_PLAYER_TEXT_DRAWS];
};
static_assert(sizeof(CPlayerTextDraw) == 2048 + 1024 + 256, "Invalid CPlayerTextDraw size");

struct C3DText
{
	char*			szText;					// + 0x00
	DWORD			dwColor;				// + 0x04
	CVector			vecPos;
	float			fDrawDistance;			// + 0x14
	bool			bLineOfSight;           // + 0x18
	int				iWorld;                 // + 0x19
	WORD			wAttachedToPlayerID;    // + 0x1D
	WORD			wAttachedToVehicleID;   // + 0x1F
};
static_assert(sizeof(C3DText) == 33, "Invalid C3DText size");

struct CPlayerText3DLabels
{
	C3DText			TextLabels[ MAX_3DTEXT_PLAYER ];	// + 0x0000
	BOOL			isCreated[ MAX_3DTEXT_PLAYER ];	// + 0x8400
	BYTE			unknown9800[MAX_3DTEXT_PLAYER];				// + 0x9400
	WORD			wOwnerID;
};
static_assert(sizeof(CPlayerText3DLabels) == 38914, "Invalid CPlayerText3DLabels size");

struct CAttachedObject
{
    int				iModelID;
    int				iBoneiD;
    CVector         vecPos;
    CVector         vecRot;
    CVector         vecScale;
	DWORD			dwMaterialColor1;
	DWORD			dwMaterialColor2;
};
static_assert(sizeof(CAttachedObject) == 52, "Invalid CAttachedObject size");

struct CPlayerSpawnInfo
{
	BYTE			byteTeam;				// 0 - 1
	int				iSkin;					// 1 - 5
	BYTE			unk;					// 5 - 6
	CVector			vecPos;				// 6 - 18
	float			fRotation;			// 18 - 22
	int				iSpawnWeapons[3];		// 22 - 34
	int				iSpawnWeaponsAmmo[3];	// 34 - 46
};
static_assert(sizeof(CPlayerSpawnInfo) == 46, "Invalid CPlayerSpawnInfo size");

struct CBulletSyncData
{
	BYTE			byteHitType;
	WORD			wHitID;
	CVector			vecHitOrigin;
	CVector			vecHitTarget;
	CVector			vecCenterOfHit;
	BYTE			byteWeaponID;
}; 
static_assert(sizeof(CBulletSyncData) == 40, "Invalid CBulletSyncData size");

struct CPVar
{
    char			szVarName[MAX_PVAR_NAME + 1];
    BOOL			bIsReadOnly;
    int				iVarType;
    int				iValue;
    float			fValue;
    char*			szValue;
};
static_assert(sizeof(CPVar) == 61, "Invalid CPVar size");

struct CPlayerVar
{
    CPVar			Vars[MAX_PVARS];
	BOOL			bIsPVarActive[MAX_PVARS];
    int				iUpperIndex;
};
static_assert(sizeof(CPlayerVar) == 48800 + 3200 + 4, "Invalid CPlayerVar size");

struct CPlayer
{
	CAimSyncData			aimSyncData;			// 0 - 31
	WORD				wCameraObject;			// 31 - 33
	WORD				wCameraVehicle;			// 33 - 35
	WORD				wCameraPlayer;			// 35 - 37
	WORD				wCameraActor;			// 37 - 39
	CVehicleSyncData		vehicleSyncData;		// 39 -
	CPassengerSyncData		passengerSyncData;		//
	CSyncData				syncData;				// 126 - 194
	CUnoccupiedSyncData		unoccupiedSyncData;		// 194 - 261
	CSpectatingSyncData		spectatingSyncData;		// 261 - 279
	CTrailerSyncData		trailerSyncData;		// 279 - 333
	DWORD					dwPlayerSyncUnused;		// 333 - 337
	DWORD					dwVehicleSyncUnused;	// 337 - 341
	BYTE					byteStreamedIn[MAX_PLAYERS];				// 341 - 1341
	BYTE					byteVehicleStreamedIn[MAX_VEHICLES];		// 1341 - 3341
	BYTE					byteSomethingUnused[1000];					// 3341 - 4341
	BYTE					byte3DTextLabelStreamedIn[1024];			// 4341  - 5365
	BYTE					bPickupStreamedIn[MAX_PICKUPS];				// 5365 - 9461
	BYTE					byteActorStreamedIn[MAX_PLAYERS];			// 9461 - 10461
	DWORD					dwStreamedInPlayers;						// 10461 - 10465
	DWORD					dwStreamedInVehicles;						// 10465 - 10469
	DWORD					dwStreamedInSomethingUnused;				// 10469 - 10473
	DWORD					dwStreamedIn3DTextLabels;					// 10479 - 10477
	DWORD					dwStreamedInPickups;						// 10477 - 10481
	DWORD					dwStreamedInActors;							// 10481 - 10485
	DWORD					bHasSetVehiclePos;	// 10485 - 10489
	DWORD					dwSetVehiclePosTick;// 10489 - 10493
	CVector					vecVehicleNewPos;	// 10493 - 10505
	BOOL					bCameraTarget;		// 10505
	DWORD					bHasSpawnInfo;		// 10509
	BOOL					bUpdateKeys;		// 10513
	CVector					vecPosition;		// 10517
	float					fHealth;			// 10529 - 10533
	float					fArmour;			// 10533 - 10537
	float					fQuaternion[4];		// 10537 - 10553
	float					fAngle;				// 10553 - 10557
	CVector					vecVelocity;		// 10557 - 10569
	WORD					wLRAnalog;			// 10569
	WORD					wUDAnalog;			// 10571
	DWORD					dwKeys;				// 10573 - 10577
	DWORD					dwOldKeys;			// 10577 - 10581
	BOOL					bEditObject;		// 10581 - 10585
	BOOL					bEditAttachedObject;// 10585 - 10589
	WORD					wDialogID;			// 10589 - 10591
	CPlayerTextDraw*		pTextdraw;			// 10591 - 10595
	CPlayerText3DLabels*	p3DText;			// 10595 - 10599
	WORD					wPlayerId;			// 10599 - 10601
	int						iUpdateState;		// 10601 - 10605
	//DWORD					dwLastSyncTick;		// 10605 - 10609
	CAttachedObject			attachedObject[MAX_PLAYER_ATTACHED_OBJECTS]; // 10605 - 11125
	BOOL					attachedObjectSlot[MAX_PLAYER_ATTACHED_OBJECTS]; // 11125 - 11165
	BOOL					bHasAimSync;		// 11165 - 11169
	BOOL					bHasTrailerSync;	// 11169 - 11173
	BOOL					bHasUnoccupiedSync;	// 11173 - 11177
	BYTE					byteState;			// 11177 - 11178
	CVector					vecCPPos;			// 11178 - 11190
	float					fCPSize;			// 11190 - 11194
	BOOL					bIsInCP;			// 11194 - 11198
	CVector					vecRaceCPPos;		// 11198 - 11210
	CVector					vecRaceCPNextPos;	// 11210 - 11222 
	BYTE					byteRaceCPType;		// 11222 - 11223 // TODO -> replace
	float					fRaceCPSize;		// 11223 - 11227
	BOOL					bIsInRaceCP;		// 11227 - 11231
	BOOL					bIsInModShop;		// 11231 - 11235
	WORD					wSkillLevel[11];	// 11235 - 11257
	int						iLastMarkerUpdate;	// 11257 - 11261
	CPlayerSpawnInfo		spawn;				// 11261 - 11307
	BOOL					bReadyToSpawn;		// 11307 - 11311
	BYTE					byteWantedLevel;	// 11311 - 11312
	BYTE					byteFightingStyle;  // 11312 - 11313
	BYTE					byteSeatId;			// 11313 - 11314
	WORD					wVehicleId;			// 11314 - 11316
	DWORD					dwNickNameColor;	// 11316 - 11320
	BOOL					bShowCheckpoint;	// 11320 - 11324
	BOOL					bShowRaceCheckpoint;// 11324 - 11328
	int						iInteriorId;		// 11328 - 11332
	WORD					wWeaponAmmo[12];	// 11332 - 11356
	PAD(pad10, 28);								// 11356 - 11384
	BYTE					byteWeaponId[12];	// 11384 - 11396
	BYTE					byteWeaponID_unknown;// 11396 - 11397
	BYTE					byteCurrentWeapon;	// 11397 - 11398
	WORD					wTargetId;			// 11398 - 11400
	WORD					wTargetActorId;		// 11400 - 11402
	DWORD					dwLastShotTick;		// 11402 - 11406
	BYTE					dwLastShotWeapon;	// 11406 - 11407
	CBulletSyncData			bulletSyncData;		// 11407 - 11447	
	BYTE					m_byteTime;			// 11447 - 11448
	float					m_fGameTime;		// 11448 - 11452
	BYTE					byteSpectateType;	// 11452 - 11453
	DWORD					wSpectateID;		// 11453 - 11457
	DWORD					dwLastStreaming;	// 11457 - 11461
	DWORD					dwNPCRecordingType;	// 11461 - 11465
	FILE					*pRecordingFile;	// 11465 - 11469
	DWORD					dwFirstNPCWritingTime; // 11469 - 11473 
	PAD(unused, 9);								// 11473 - 11482
	CPlayerVar*				pPlayerVars;		// 11482 - 11486
};
static_assert(sizeof(CPlayer) == 11486, "Invalid CPlayer size");

struct CPlayerPool
{
	DWORD					dwVirtualWorld[MAX_PLAYERS];				// 0 - 4000
	DWORD					dwPlayersCount;								// 4000 - 4004
	DWORD					dwlastMarkerUpdate;							// 4004 - 4008
	float					fUpdatePlayerGameTimers;					// 4008 - 4012
	DWORD					dwScore[MAX_PLAYERS];						// 4012 - 8012
	DWORD					dwMoney[MAX_PLAYERS];						// 8012 - 12012
	DWORD					dwDrunkLevel[MAX_PLAYERS];					// 12012 - 16012
	DWORD					dwLastScoreUpdate[MAX_PLAYERS];				// 16012 - 20012
	char					szSerial[MAX_PLAYERS][101];					// 20012 - 121012				
	char					szVersion[MAX_PLAYERS][25];					// 121012 - 146012
	RemoteSystemStruct		*pRemoteSystem[MAX_PLAYERS];				// 146012 - 150012
	BOOL					bIsPlayerConnected[MAX_PLAYERS];			// 150012 - 154012
	CPlayer					*pPlayer[MAX_PLAYERS];						// 154012 - 158012
	char					szName[MAX_PLAYERS][MAX_PLAYER_NAME + 1];	// 158012 - 183012
	BOOL					bIsAnAdmin[MAX_PLAYERS];					// 183012 - 187012
	BOOL					bIsNPC[MAX_PLAYERS];						// 187012 - 191012
	PAD(pad0, 8000);													// 191012 - 199012
	DWORD					dwConnectedPlayers;							// 199012 - 199016
	DWORD					dwPlayerPoolSize;							// 199016 - 199020
	DWORD					dwUnk;										// 199020 - 199024
};
static_assert(sizeof(CPlayerPool) == 199024, "Invalid CPlayerPool size");

/* -------------------------------------------------------- */
// CVehicle
/* -------------------------------------------------------- */

struct CVehicleSpawn
{
	int				iModelID;
    CVector			vecPos;   
    float			fRot;
    int				iColor1;  
    int				iColor2;  
    int				iRespawnTime;
    int				iInterior;
};
static_assert(sizeof(CVehicleSpawn) == 36, "Invalid CVehicleSpawn size");

struct CVehicleModInfo
{
	BYTE			byteModSlots[14];                // + 0x0000
    BYTE			bytePaintJob;                    // + 0x000E
    int				iColor1;                             // + 0x000F
    int				iColor2;                             // + 0x0010
};
static_assert(sizeof(CVehicleModInfo) == 23, "Invalid CVehicleModInfo size");

struct CVehicleParams
{
	char engine;
	char lights;
	char alarm;
	char doors;
	char bonnet;
	char boot;
	char objective; // 6
	char siren; // 7
	char door_driver; // 8
	char door_passenger;
	char door_backleft;
	char door_backright; // 11
	char window_driver; // 12
	char window_passenger;
	char window_backleft;
	char window_backright; // 15 - 16
};
static_assert(sizeof(CVehicleParams) == 16, "Invalid CVehicleParams size");

struct CVehicle
{
	CVector			vecPosition;		// 0 - 12
	MATRIX4X4		vehMatrix;			// 12 - 76
	CVector			vecVelocity;		// 76 - 88
	CVector			vecTurnSpeed;		// 88 - 100
	WORD			wVehicleID;			// 100 - 102
	WORD			wTrailerID;			// 102 - 104
	WORD			wCabID;				// 104 - 106
	WORD			wLastDriverID;		// 106 - 108
	WORD			vehPassengers[7];	// 108 - 122
	DWORD			vehActive;			// 122 - 126
	DWORD			vehWasted;			// 126 - 130	
	CVehicleSpawn	customSpawn;		// 130 - 166
	float			fHealth;			// 166 - 170
	DWORD			vehDoorStatus;		// 170 - 174
	DWORD			vehPanelStatus;		// 174 - 178
	BYTE			vehLightStatus;		// 178 - 179
	BYTE			vehTireStatus;		// 179 - 180
	bool			bDead;				// 180 - 181
	WORD			wKillerID;			// 181 - 183
	CVehicleModInfo vehModInfo;			// 183 - 206
	char			szNumberplate[32 + 1]; // 206 - 239
	CVehicleParams	vehParamEx;			// 239 - 255
    BYTE			bDeathNotification; // 255 - 256
    BYTE			bOccupied;			// 256 - 257
    DWORD			vehOccupiedTick;	// 257 - 261
    DWORD			vehRespawnTick;		// 261 - 265
	BYTE			byteSirenEnabled;	// 265 - 266
	BYTE			byteNewSirenState;	// 266 - 267 : passed to OnVehicleSirenStateChange
};
static_assert(sizeof(CVehicle) == 267, "Invalid CVehicle size");

struct CVehiclePool
{
	BYTE			byteVehicleModelsUsed[212];			// 0 - 212
	int				iVirtualWorld[MAX_VEHICLES];		// 212 - 8212
	BOOL			bVehicleSlotState[MAX_VEHICLES];	// 8212 - 16212
	CVehicle		*pVehicle[MAX_VEHICLES];			// 16212 - 24212
	DWORD			dwVehiclePoolSize;					// 24212 - 24216
};
static_assert(sizeof(CVehiclePool) == 24216, "Invalid CVehiclePool size");

/* -------------------------------------------------------- */
// CPickup
/* -------------------------------------------------------- */

struct tPickup // size 0x14
{
	int				iModel;
	int				iType;
	CVector			vecPos;
};

struct CPickupPool
{
	tPickup			Pickup[MAX_PICKUPS];			// + 0x0000
	BOOL			bActive[MAX_PICKUPS];			// + 0xA000
	int				iWorld[MAX_PICKUPS];		// + 0xC000
	int				iPickupCount;
};

/* -------------------------------------------------------- */
// CObject
/* -------------------------------------------------------- */

struct CObjectMaterial
{
	BYTE			byteUsed;				// 197 - 198
	BYTE			byteSlot;				// 198 - 199
	WORD			wModelID;				// 199 - 201
	DWORD			dwMaterialColor;		// 201 - 205
	char			szMaterialTXD[64 + 1];	// 205 - 270
	char			szMaterialTexture[64 + 1]; // 270 - 335
	BYTE			byteMaterialSize;		// 335 - 336
	char			szFont[64 + 1];			// 336 - 401
	BYTE			byteFontSize;			// 401 - 402
	BYTE			byteBold;				// 402 - 403
	DWORD			dwFontColor;			// 403 - 407
	DWORD			dwBackgroundColor;		// 407 - 411
	BYTE			byteAlignment;			// 411 - 412
};
static_assert(sizeof(CObjectMaterial) == 215, "Invalid CObjectMaterial size");

struct CObject
{
	WORD			wObjectID;			// 0 - 2
	int				iModel;				// 2 - 6
	BOOL			bActive;			// 6 - 10
	MATRIX4X4		matWorld;			// 10 - 74 - pos - Object position
	CVector			vecRot; 			// 74 - 86 - Object rotation
	MATRIX4X4		matTarget;			// 86 - 150	- 
	BYTE			bIsMoving;			// 150 - 151
	BYTE			bNoCameraCol;		// 151 - 152
	float			fMoveSpeed;			// 152 - 156
	DWORD			unk_4;				// 156 -160
	float			fDrawDistance;		// 160 - 164
	WORD			wAttachedVehicleID;	// 164 - 166
	WORD			wAttachedObjectID;	// 166 - 168
	CVector			vecAttachedOffset;	// 168 - 180
	CVector			vecAttachedRotation;// 180 - 192
	BYTE			byteSyncRot;		// 192 - 193
	DWORD			dwMaterialCount;	// 193 - 197
	CObjectMaterial	Material[MAX_OBJECT_MATERIAL];		// 197 - 3637
	char			*szMaterialText[MAX_OBJECT_MATERIAL];// 3637 - 3653
};
static_assert(sizeof(CObject) == 3701, "Invalid CObject size");

struct CObjectPool
{
	BOOL			bPlayerObjectSlotState[MAX_PLAYERS][MAX_OBJECTS];	// 0 
	BOOL			bPlayersObject[MAX_OBJECTS];							// 4.000.000
	CObject			*pPlayerObjects[MAX_PLAYERS][MAX_OBJECTS];		// 4.004.000
	BOOL			bObjectSlotState[MAX_OBJECTS];						// 8.004.000
	CObject			*pObjects[MAX_OBJECTS];							// 8.008.000
};

/* -------------------------------------------------------- */
// CMenu
/* -------------------------------------------------------- */

struct MenuInteraction
{
	BOOL			Menu;
	BOOL			Row[MAX_ITEMS];
	char			unknown[12];
};
	
struct CMenu	// size 0xB84
{
	BYTE			menuID;														// + 0x0000
	char			szTitle[ MAX_MENU_TEXT_SIZE ];								// + 0x0001
	char			szItems[ MAX_ITEMS ][ MAX_COLUMNS ][ MAX_MENU_TEXT_SIZE ];	// + 0x0021
	char			szHeaders[MAX_COLUMNS][MAX_MENU_TEXT_SIZE];					// + 0x0321
	BOOL			bIsInitiedForPlayer[MAX_PLAYERS];							// + 0x0361
	MenuInteraction interaction;												// + 0x0B31
	CVector2D		vecPos;
	float			fColumn1Width;												// + 0x0B79
	float			fColumn2Width;												// + 0x0B7D
	BYTE			byteColumnsNumber;											// + 0x0B81
	BYTE			byteItemsCount[ MAX_COLUMNS ];								// + 0x0B82
};

struct CMenuPool
{
	CMenu*			pMenu[ MAX_MENUS ];			//	+ 0x0000
	BOOL			bIsCreated[ MAX_MENUS ];		//	+ 0x0200
	BOOL			bPlayerMenu[MAX_PLAYERS];	//	+ 0x0400
};

/* -------------------------------------------------------- */
// CTextDraw
/* -------------------------------------------------------- */
struct CTextDrawPool
{
	BOOL			bSlotState[MAX_TEXT_DRAWS];
	CTextdraw*		TextDraw[MAX_TEXT_DRAWS];
	char*			szFontText[MAX_TEXT_DRAWS];
	bool			bHasText[MAX_TEXT_DRAWS][MAX_PLAYERS];
};

/* -------------------------------------------------------- */
// C3DText
/* -------------------------------------------------------- */

class C3DTextPool
{
public:
	BOOL			bIsCreated[MAX_3DTEXT_GLOBAL]; // 0 - 4096 <- OK
	C3DText			TextLabels[MAX_3DTEXT_GLOBAL];
};

/* -------------------------------------------------------- */
// CGangZone
/* -------------------------------------------------------- */

struct CSAMPGangZonePool
{
	float			fGangZone[MAX_GANG_ZONES][4];
	BOOL			bSlotState[MAX_GANG_ZONES];
};

/* -------------------------------------------------------- */
// CActor
/* -------------------------------------------------------- */

struct CActorAnim // 140
{
	char			szAnimLib[64 + 1]; // 0 - 64
	char			szAnimName[64 + 1]; // 64 - 128
	float			fDelta;		// 128 - 132
	BYTE			byteLoop;		// 132 - 133
	BYTE			byteLockX;			// 133 - 134
	BYTE			byteLockY;			// 134 - 135
	BYTE			byteFreeze;		// 135 - 136
	int				iTime;				//  136 - 140
};
static_assert(sizeof(CActorAnim) == 142, "Invalid CActorAnim size");

struct CActor
{
	BYTE			pad0;				
	int				iSkinID;			
	CVector			vecSpawnPos;	
	float			fSpawnAngle;		
	DWORD			pad4;				
	DWORD			pad5;				
	BYTE			byteLoopAnim;		
	CActorAnim		anim;
	float			fHealth;			
	DWORD			pad;				
	float			fAngle;			
	CVector			vecPos;	
	BYTE			pad8[12];			
	BYTE			byteInvulnerable;	
	WORD			wActorID;			
};
//static_assert(sizeof(CActor) == 211, "Invalid CActor size");

struct CActorPool
{
	int				iActorVirtualWorld[MAX_ACTORS];
	BOOL			bValidActor[MAX_ACTORS];
	CActor*			pActor[MAX_ACTORS];
	DWORD			dwActorPoolSize;
};

struct CGameMode
{
	AMX				amx;
	bool			bInitialised;
	bool			bSleeping;
	float			fSleepTime;
};

struct CFilterScripts
{
	AMX*			pFilterScripts[MAX_FILTER_SCRIPTS];
	char			szFilterScriptName[MAX_FILTER_SCRIPTS][255];
	int				iFilterScriptCount;
};

struct ScriptTimer_s // sizeof = 0x11B (283)
{
	char szScriptFunc[255];
	int iTotalTime;
	int iRemainingTime;
	BOOL bRepeating;
	BOOL bKilled;
	AMX* pAMX;
	int iParamCount;
	void* cellParams;
};

typedef std::map<DWORD, ScriptTimer_s*> DwordTimerMap;

class CScriptTimers
{
public:
	DwordTimerMap			Timers;
	DWORD					dwTimerCount;
};

struct CNetGame
{
	CGameMode				*pGameModePool;			// 0
	CFilterScripts			*pFilterScriptPool;		// 4
	CPlayerPool				*pPlayerPool;			// 8
	CVehiclePool			*pVehiclePool;			// 12
	CPickupPool				*pPickupPool;			// 16
	CObjectPool				*pObjectPool;			// 20
	CMenuPool				*pMenuPool;				// 24
	CTextDrawPool			*pTextDrawPool;			// 28
	C3DTextPool				*p3DTextPool;			// 32
	CSAMPGangZonePool		*pGangZonePool;			// 36 
	CActorPool				*pActorPool;			// 40 
	int						iCurrentGameModeIndex;	// 44
	int						iCurrentGameModeRepeat;	// 48
	BOOL					bFirstGameModeLoaded;	// 52
	void					*pHttpClient;					// 56
	CScriptTimers			*pScriptTimers;			// 60
	void					*pRak;					// 64
	DWORD					dwSomethingTick;
	DWORD					dwUnk;
	DWORD					dwUnk1;
	BOOL					bLanMode;				// 
	BOOL					bShowPlayerMarkers;		// 84
	BYTE					byteShowNameTags;		// 
	BYTE					byteWorldTimeHour;		// 
	BYTE					byteAllowWeapons;		// 
	BYTE					byteStuntBonus;			// 91 - 92
	BYTE					byteDefaultCameraCollision; // 92 - 93
	BYTE					byteWeather;			// 93 - 94
	int						iGameState;				// 94 - 98
	float					fGravity;				// 98 - 102
	int						iDeathDropMoney;		// 102 - 106
	BYTE					byteEnableZoneNames;				// 106 - 107
	BYTE					byteMode;				// 107 - 108
	BYTE					bLimitGlobalChatRadius;	// 108 - 109
	BYTE					bUseCJWalk;				// 109 - 110
	float					fGlobalChatRadius;		// 110 - 114
	float					fNameTagDrawDistance;	// 114 - 118
	BYTE					byteDisableEnterExits;	// 118 - 119
	BYTE					byteNameTagLOS;			// 119 - 120
	BYTE					bManulVehicleEngineAndLights; // 120 - 121
	BYTE					bLimitPlayerMarkers;	// 121 - 122
	float					fPlayerMarkesLimit;		// 122 - 126 
	BOOL					bVehicleFriendlyFire;	// 126 - 130
#ifndef _WIN32
	double					dElapsedTime;			// size = 8
#endif
	int						iSpawnsAvailable;		// 130 - 134
	CPlayerSpawnInfo		AvailableSpawns[319];	// 129 - 14803
};

#endif
