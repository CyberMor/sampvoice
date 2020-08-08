#pragma once

#include <map>
#include <pawn/amx/amx.h>
#include <raknet/networktypes.h>

#include "utils/cvector.h"
#include "utils/cvector2d.h"

#if !defined PAD
	#define PAD(a, b) char a[b]
#endif

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

enum CON_VARTYPE { CON_VARTYPE_FLOAT, CON_VARTYPE_INT, CON_VARTYPE_BOOL, CON_VARTYPE_STRING };

#define CON_VARFLAG_DEBUG			1
#define CON_VARFLAG_READONLY		2
#define CON_VARFLAG_RULE			4

typedef void(*VARCHANGEFUNC)();

#pragma pack(push, 1)

struct PingAndClockDifferential {
	unsigned short pingTime;
	unsigned int clockDifferential;
};

struct RemoteSystemStruct {
	bool isActive;
	PlayerID playerId;
	PlayerID myExternalPlayerId;
	unsigned char gapD[1895];
	unsigned long dword774;
	unsigned short word778;
	unsigned char gap77A[2];
	unsigned long dword77C;
	unsigned long dword780;
	unsigned char gap784[276];
	unsigned long dword898;
	unsigned char gap89C[16];
	unsigned char byte8AC;
	unsigned char gap8AD[1023];
	unsigned int connectionTime;
	enum ConnectMode {
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
	unsigned char byteAuthTableIndex;
	unsigned char byteAuthType;
	unsigned char byteIsLogon;
};
static_assert(sizeof(RemoteSystemStruct) == 3255, "Invalid RemoteSystemStruct size");

typedef struct _MATRIX4X4  {
	CVector right;
	unsigned long  flags;
	CVector up;
	float  pad_u;
	CVector at;
	float  pad_a;
	CVector pos;
	float  pad_p;
} MATRIX4X4, *PMATRIX4X4;
static_assert(sizeof(_MATRIX4X4) == 64, "Invalid _MATRIX4X4 size");

struct ConsoleVariable_s {
	CON_VARTYPE		VarType;
	unsigned long			VarFlags;
	void*			VarPtr;
	VARCHANGEFUNC	VarChangeFunc;
};
static_assert(sizeof(ConsoleVariable_s) == 16, "Invalid ConsoleVariable_s size");

struct ConsoleCommand_s {
	char			szName[255];
	unsigned long			dwFlags;
	void			(*fptrFunc)();
};
static_assert(sizeof(ConsoleCommand_s) == 263, "Invalid ConsoleCommand_s size");

struct CAimSyncData {
	unsigned char			byteCameraMode;
	CVector					vecFront;
	CVector					vecPosition;
	float					fZAim;
	unsigned char			byteCameraZoom : 6;
	unsigned char			byteWeaponState  : 2;
	unsigned char			byteAspectRatio;
};
static_assert(sizeof(CAimSyncData) == 31, "Invalid CAimSyncData size");

struct CVehicleSyncData {
	unsigned short			wVehicleId;
	unsigned short			wLRAnalog;
	unsigned short			wUDAnalog;
	unsigned short			wKeys;
	float					fQuaternion[4];
	CVector					vecPosition;
	CVector					vecVelocity;
	float					fHealth;
	unsigned char			bytePlayerHealth;
	unsigned char			bytePlayerArmour;
	unsigned char			bytePlayerWeapon : 6;
	unsigned char			unk_2 : 2;
	unsigned char			byteSirenState;
	unsigned char			byteGearState;
	unsigned short			wTrailerID;
    union {
		unsigned short		wHydraReactorAngle[2];                       
		float				fTrainSpeed;
    };
};
static_assert(sizeof(CVehicleSyncData) == 63, "Invalid CVehicleSyncData size");

struct CPassengerSyncData {
	unsigned short			wVehicleId;
	unsigned char			byteSeatFlags : 7;
	unsigned char			byteDriveBy : 1;
	unsigned char			bytePlayerWeapon;
	unsigned char			bytePlayerHealth;
	unsigned char			bytePlayerArmour;
	unsigned short			wLRAnalog;
	unsigned short			wUDAnalog;
	unsigned short			wKeys;
	CVector					vecPosition;
};
static_assert(sizeof(CPassengerSyncData) == 24, "Invalid CPassengerSyncData size");

struct CSyncData {
	unsigned short			wLRAnalog;
	unsigned short			wUDAnalog;
	unsigned short			wKeys;
	CVector					vecPosition;
	float					fQuaternion[4];
	unsigned char			byteHealth;
	unsigned char			byteArmour;
	unsigned char			byteWeapon : 6;
	unsigned char			_unk_ : 2;
	unsigned char			byteSpecialAction;
	CVector					vecVelocity;
	CVector					vecSurfing;
	unsigned short			wSurfingInfo;
	union {
		unsigned long		dwAnimationData;
		struct {
			unsigned short	wAnimIndex;
			unsigned short	wAnimFlags;
		};
	};
};
static_assert(sizeof(CSyncData) == 68, "Invalid CSyncData size");

struct CUnoccupiedSyncData {
	unsigned short			wVehicleID;
	unsigned char			bytePassengerSlot;
	CVector					vecRool;
	CVector					vecDirection;
	CVector					vecPosition;
	CVector					vecVelocity;
	CVector					vecTurnVelocity;
	float					fHealth;
};
static_assert(sizeof(CUnoccupiedSyncData) == 67, "Invalid CUnoccupiedSyncData size");

struct CSpectatingSyncData {
	unsigned short			wLeftRightKeysOnSpectating;
	unsigned short			wUpDownKeysOnSpectating;
	unsigned short			wKeysOnSpectating;
	CVector					vecPosition;
};
static_assert(sizeof(CSpectatingSyncData) == 18, "Invalid CSpectatingSyncData size");

struct CTrailerSyncData {
	unsigned short			wTrailerID;
	CVector					vecPosition;
	float					fQuaternion[4];
	CVector					vecVelocity;
	CVector					vecTurnVelocity;
};
static_assert(sizeof(CTrailerSyncData) == 54, "Invalid CTrailerSyncData size");

typedef struct CTextdraw {
	union {
		unsigned char byteFlags;
		struct {
			unsigned char byteBox : 1;
			unsigned char byteLeft : 1; 
			unsigned char byteRight : 1;
			unsigned char byteCenter : 1;
			unsigned char byteProportional : 1;
			unsigned char bytePadding : 3;
		};
	};
	CVector2D				vecLetter;
	unsigned long			dwLetterColor;
	CVector2D				vecLine;
	unsigned long			dwBoxColor;
	unsigned char			byteShadow;
	unsigned char			byteOutline;
	unsigned long			dwBackgroundColor;
	unsigned char			byteStyle;
	unsigned char			byteSelectable;
	CVector2D				vecPos;
	unsigned short			dwModelIndex;
	CVector					vecRot;
	float					fZoom;
	unsigned short			color1;
	unsigned short			color2;
} _CTextdraw;
static_assert(sizeof(CTextdraw) == 63, "Invalid CTextdraw size");

struct CPlayerTextDraw {
	int						bSlotState[MAX_PLAYER_TEXT_DRAWS];
	CTextdraw				*TextDraw[MAX_PLAYER_TEXT_DRAWS];
	char					*szFontText[MAX_PLAYER_TEXT_DRAWS];
	bool					bHasText[MAX_PLAYER_TEXT_DRAWS];
};
static_assert(sizeof(CPlayerTextDraw) == 2048 + 1024 + 256, "Invalid CPlayerTextDraw size");

struct C3DText {
	char*					szText;
	unsigned long			dwColor;
	CVector					vecPos;
	float					fDrawDistance;
	bool					bLineOfSight;
	int						iWorld;
	unsigned short			wAttachedToPlayerID;
	unsigned short			wAttachedToVehicleID;
};
static_assert(sizeof(C3DText) == 33, "Invalid C3DText size");

struct CPlayerText3DLabels {
	C3DText					TextLabels[MAX_3DTEXT_PLAYER];
	int						isCreated[MAX_3DTEXT_PLAYER];
	unsigned char			unknown9800[MAX_3DTEXT_PLAYER];
	unsigned short			wOwnerID;
};
static_assert(sizeof(CPlayerText3DLabels) == 38914, "Invalid CPlayerText3DLabels size");

struct CAttachedObject {
    int						iModelID;
    int						iBoneiD;
    CVector					vecPos;
    CVector					vecRot;
    CVector					vecScale;
	unsigned long			dwMaterialColor1;
	unsigned long			dwMaterialColor2;
};
static_assert(sizeof(CAttachedObject) == 52, "Invalid CAttachedObject size");

struct CPlayerSpawnInfo {
	unsigned char			byteTeam;
	int						iSkin;
	unsigned char			unk;
	CVector					vecPos;
	float					fRotation;
	int						iSpawnWeapons[3];
	int						iSpawnWeaponsAmmo[3];
};
static_assert(sizeof(CPlayerSpawnInfo) == 46, "Invalid CPlayerSpawnInfo size");

struct CBulletSyncData {
	unsigned char			byteHitType;
	unsigned short			wHitID;
	CVector					vecHitOrigin;
	CVector					vecHitTarget;
	CVector					vecCenterOfHit;
	unsigned char			byteWeaponID;
}; 
static_assert(sizeof(CBulletSyncData) == 40, "Invalid CBulletSyncData size");

struct CPVar {
    char					szVarName[MAX_PVAR_NAME + 1];
    int						bIsReadOnly;
    int						iVarType;
    int						iValue;
    float					fValue;
    char*					szValue;
};
static_assert(sizeof(CPVar) == 61, "Invalid CPVar size");

struct CPlayerVar {
    CPVar					Vars[MAX_PVARS];
	int						bIsPVarActive[MAX_PVARS];
    int						iUpperIndex;
};
static_assert(sizeof(CPlayerVar) == 48800 + 3200 + 4, "Invalid CPlayerVar size");

struct CPlayer {
	CAimSyncData			aimSyncData;
	unsigned short			wCameraObject;
	unsigned short			wCameraVehicle;
	unsigned short			wCameraPlayer;
	unsigned short			wCameraActor;
	CVehicleSyncData		vehicleSyncData;
	CPassengerSyncData		passengerSyncData;
	CSyncData				syncData;
	CUnoccupiedSyncData		unoccupiedSyncData;
	CSpectatingSyncData		spectatingSyncData;
	CTrailerSyncData		trailerSyncData;
	unsigned long			dwPlayerSyncUnused;
	unsigned long			dwVehicleSyncUnused;
	unsigned char			byteStreamedIn[MAX_PLAYERS];
	unsigned char			byteVehicleStreamedIn[MAX_VEHICLES];
	unsigned char			byteSomethingUnused[1000];
	unsigned char			byte3DTextLabelStreamedIn[1024];
	unsigned char			bPickupStreamedIn[MAX_PICKUPS];
	unsigned char			byteActorStreamedIn[MAX_PLAYERS];
	unsigned long			dwStreamedInPlayers;
	unsigned long			dwStreamedInVehicles;
	unsigned long			dwStreamedInSomethingUnused;
	unsigned long			dwStreamedIn3DTextLabels;
	unsigned long			dwStreamedInPickups;
	unsigned long			dwStreamedInActors;
	unsigned long			bHasSetVehiclePos;
	unsigned long			dwSetVehiclePosTick;
	CVector					vecVehicleNewPos;
	int						bCameraTarget;
	unsigned long			bHasSpawnInfo;
	int						bUpdateKeys;
	CVector					vecPosition;
	float					fHealth;
	float					fArmour;
	float					fQuaternion[4];
	float					fAngle;
	CVector					vecVelocity;
	unsigned short			wLRAnalog;
	unsigned short			wUDAnalog;
	unsigned long			dwKeys;
	unsigned long			dwOldKeys;
	int						bEditObject;
	int						bEditAttachedObject;
	unsigned short			wDialogID;
	CPlayerTextDraw*		pTextdraw;
	CPlayerText3DLabels*	p3DText;
	unsigned short			wPlayerId;
	int						iUpdateState;
	CAttachedObject			attachedObject[MAX_PLAYER_ATTACHED_OBJECTS];
	int						attachedObjectSlot[MAX_PLAYER_ATTACHED_OBJECTS];
	int						bHasAimSync;
	int						bHasTrailerSync;
	int						bHasUnoccupiedSync;
	unsigned char			byteState;
	CVector					vecCPPos;
	float					fCPSize;
	int						bIsInCP;
	CVector					vecRaceCPPos;
	CVector					vecRaceCPNextPos;
	unsigned char			byteRaceCPType;
	float					fRaceCPSize;
	int						bIsInRaceCP;
	int						bIsInModShop;
	unsigned short			wSkillLevel[11];
	int						iLastMarkerUpdate;
	CPlayerSpawnInfo		spawn;
	int						bReadyToSpawn;
	unsigned char			byteWantedLevel;
	unsigned char			byteFightingStyle;
	unsigned char			byteSeatId;
	unsigned short			wVehicleId;
	unsigned long			dwNickNameColor;
	int						bShowCheckpoint;
	int						bShowRaceCheckpoint;
	int						iInteriorId;
	unsigned short			wWeaponAmmo[12];
	PAD(pad10, 28);
	unsigned char			byteWeaponId[12];
	unsigned char			byteWeaponID_unknown;
	unsigned char			byteCurrentWeapon;
	unsigned short			wTargetId;
	unsigned short			wTargetActorId;
	unsigned long			dwLastShotTick;
	unsigned char			dwLastShotWeapon;
	CBulletSyncData			bulletSyncData;
	unsigned char			m_byteTime;
	float					m_fGameTime;
	unsigned char			byteSpectateType;
	unsigned long			wSpectateID;
	unsigned long			dwLastStreaming;
	unsigned long			dwNPCRecordingType;
	FILE					*pRecordingFile;
	unsigned long			dwFirstNPCWritingTime;
	PAD(unused, 9);
	CPlayerVar*				pPlayerVars;
};
static_assert(sizeof(CPlayer) == 11486, "Invalid CPlayer size");

struct CPlayerPool {
	unsigned long			dwVirtualWorld[MAX_PLAYERS];
	unsigned long			dwPlayersCount;
	unsigned long			dwlastMarkerUpdate;
	float					fUpdatePlayerGameTimers;
	unsigned long			dwScore[MAX_PLAYERS];
	unsigned long			dwMoney[MAX_PLAYERS];
	unsigned long			dwDrunkLevel[MAX_PLAYERS];
	unsigned long			dwLastScoreUpdate[MAX_PLAYERS];
	char					szSerial[MAX_PLAYERS][101];
	char					szVersion[MAX_PLAYERS][25];
	RemoteSystemStruct		*pRemoteSystem[MAX_PLAYERS];
	int						bIsPlayerConnected[MAX_PLAYERS];
	CPlayer					*pPlayer[MAX_PLAYERS];
	char					szName[MAX_PLAYERS][MAX_PLAYER_NAME + 1];
	int						bIsAnAdmin[MAX_PLAYERS];
	int						bIsNPC[MAX_PLAYERS];
	PAD(pad0, 8000);
	unsigned long			dwConnectedPlayers;
	unsigned long			dwPlayerPoolSize;
	unsigned long			dwUnk;
};
static_assert(sizeof(CPlayerPool) == 199024, "Invalid CPlayerPool size");

struct CVehicleSpawn {
	int						iModelID;
    CVector					vecPos;
    float					fRot;
    int						iColor1;
    int						iColor2;
    int						iRespawnTime;
    int						iInterior;
};
static_assert(sizeof(CVehicleSpawn) == 36, "Invalid CVehicleSpawn size");

struct CVehicleModInfo {
	unsigned char			byteModSlots[14];
    unsigned char			bytePaintJob;
    int						iColor1;
    int						iColor2;
};
static_assert(sizeof(CVehicleModInfo) == 23, "Invalid CVehicleModInfo size");

struct CVehicleParams {
	char					engine;
	char					lights;
	char					alarm;
	char					doors;
	char					bonnet;
	char					boot;
	char					objective;
	char					siren;
	char					door_driver;
	char					door_passenger;
	char					door_backleft;
	char					door_backright;
	char					window_driver;
	char					window_passenger;
	char					window_backleft;
	char					window_backright;
};
static_assert(sizeof(CVehicleParams) == 16, "Invalid CVehicleParams size");

struct CVehicle {
	CVector					vecPosition;
	MATRIX4X4				vehMatrix;
	CVector					vecVelocity;
	CVector					vecTurnSpeed;
	unsigned short			wVehicleID;
	unsigned short			wTrailerID;
	unsigned short			wCabID;
	unsigned short			wLastDriverID;
	unsigned short			vehPassengers[7];
	unsigned long			vehActive;
	unsigned long			vehWasted;
	CVehicleSpawn			customSpawn;
	float					fHealth;
	unsigned long			vehDoorStatus;
	unsigned long			vehPanelStatus;
	unsigned char			vehLightStatus;
	unsigned char			vehTireStatus;
	bool					bDead;
	unsigned short			wKillerID;
	CVehicleModInfo			vehModInfo;
	char					szNumberplate[32 + 1];
	CVehicleParams			vehParamEx;
    unsigned char			bDeathNotification;
    unsigned char			bOccupied;
    unsigned long			vehOccupiedTick;
    unsigned long			vehRespawnTick;
	unsigned char			byteSirenEnabled;
	unsigned char			byteNewSirenState;
};
static_assert(sizeof(CVehicle) == 267, "Invalid CVehicle size");

struct CVehiclePool {
	unsigned char			byteVehicleModelsUsed[212];
	int						iVirtualWorld[MAX_VEHICLES];
	int						bVehicleSlotState[MAX_VEHICLES];
	CVehicle				*pVehicle[MAX_VEHICLES];
	unsigned long			dwVehiclePoolSize;
};
static_assert(sizeof(CVehiclePool) == 24216, "Invalid CVehiclePool size");

struct tPickup {
	int						iModel;
	int						iType;
	CVector					vecPos;
};

struct CPickupPool {
	tPickup					Pickup[MAX_PICKUPS];
	int						bActive[MAX_PICKUPS];
	int						iWorld[MAX_PICKUPS];
	int						iPickupCount;
};

struct CObjectMaterial {
	unsigned char			byteUsed;
	unsigned char			byteSlot;
	unsigned short			wModelID;
	unsigned long			dwMaterialColor;
	char					szMaterialTXD[64 + 1];
	char					szMaterialTexture[64 + 1];
	unsigned char			byteMaterialSize;
	char					szFont[64 + 1];
	unsigned char			byteFontSize;
	unsigned char			byteBold;
	unsigned long			dwFontColor;
	unsigned long			dwBackgroundColor;
	unsigned char			byteAlignment;
};
static_assert(sizeof(CObjectMaterial) == 215, "Invalid CObjectMaterial size");

struct CObject {
	unsigned short			wObjectID;
	int						iModel;
	int						bActive;
	MATRIX4X4				matWorld;
	CVector					vecRot;
	MATRIX4X4				matTarget;
	unsigned char			bIsMoving;
	unsigned char			bNoCameraCol;
	float					fMoveSpeed;
	unsigned long			unk_4;
	float					fDrawDistance;
	unsigned short			wAttachedVehicleID;
	unsigned short			wAttachedObjectID;
	CVector					vecAttachedOffset;
	CVector					vecAttachedRotation;
	unsigned char			byteSyncRot;
	unsigned long			dwMaterialCount;
	CObjectMaterial			Material[MAX_OBJECT_MATERIAL];
	char					*szMaterialText[MAX_OBJECT_MATERIAL];
};
static_assert(sizeof(CObject) == 3701, "Invalid CObject size");

struct CObjectPool {
	int						bPlayerObjectSlotState[MAX_PLAYERS][MAX_OBJECTS];
	int						bPlayersObject[MAX_OBJECTS];
	CObject					*pPlayerObjects[MAX_PLAYERS][MAX_OBJECTS];
	int						bObjectSlotState[MAX_OBJECTS];
	CObject					*pObjects[MAX_OBJECTS];
};

struct MenuInteraction {
	int						Menu;
	int						Row[MAX_ITEMS];
	char					unknown[12];
};
	
struct CMenu {
	unsigned char			menuID;
	char					szTitle[MAX_MENU_TEXT_SIZE];
	char					szItems[MAX_ITEMS][MAX_COLUMNS][MAX_MENU_TEXT_SIZE];
	char					szHeaders[MAX_COLUMNS][MAX_MENU_TEXT_SIZE];
	int						bIsInitiedForPlayer[MAX_PLAYERS];
	MenuInteraction			interaction;
	CVector2D				vecPos;
	float					fColumn1Width;
	float					fColumn2Width;
	unsigned char			byteColumnsNumber;
	unsigned char			byteItemsCount[MAX_COLUMNS];
};

struct CMenuPool {
	CMenu*					pMenu[MAX_MENUS];
	int						bIsCreated[MAX_MENUS];
	int						bPlayerMenu[MAX_PLAYERS];
};

struct CTextDrawPool {
	int						bSlotState[MAX_TEXT_DRAWS];
	CTextdraw*				TextDraw[MAX_TEXT_DRAWS];
	char*					szFontText[MAX_TEXT_DRAWS];
	bool					bHasText[MAX_TEXT_DRAWS][MAX_PLAYERS];
};

class C3DTextPool {
public:
	int						bIsCreated[MAX_3DTEXT_GLOBAL];
	C3DText					TextLabels[MAX_3DTEXT_GLOBAL];
};

struct CSAMPGangZonePool {
	float					fGangZone[MAX_GANG_ZONES][4];
	int						bSlotState[MAX_GANG_ZONES];
};

struct CActorAnim {
	char					szAnimLib[64 + 1];
	char					szAnimName[64 + 1];
	float					fDelta;
	unsigned char			byteLoop;
	unsigned char			byteLockX;
	unsigned char			byteLockY;
	unsigned char			byteFreeze;
	int						iTime;
};
static_assert(sizeof(CActorAnim) == 142, "Invalid CActorAnim size");

struct CActor {
	unsigned char			pad0;				
	int						iSkinID;			
	CVector					vecSpawnPos;	
	float					fSpawnAngle;		
	unsigned long			pad4;				
	unsigned long			pad5;				
	unsigned char			byteLoopAnim;		
	CActorAnim				anim;
	float					fHealth;			
	unsigned long			pad;				
	float					fAngle;			
	CVector					vecPos;	
	unsigned char			pad8[12];			
	unsigned char			byteInvulnerable;	
	unsigned short			wActorID;			
};

struct CActorPool {
	int						iActorVirtualWorld[MAX_ACTORS];
	int						bValidActor[MAX_ACTORS];
	CActor*					pActor[MAX_ACTORS];
	unsigned long			dwActorPoolSize;
};

struct CGameMode {
	AMX						amx;
	bool					bInitialised;
	bool					bSleeping;
	float					fSleepTime;
};

struct CFilterScripts {
	AMX*					pFilterScripts[MAX_FILTER_SCRIPTS];
	char					szFilterScriptName[MAX_FILTER_SCRIPTS][255];
	int						iFilterScriptCount;
};

struct ScriptTimer_s {
	char					szScriptFunc[255];
	int						iTotalTime;
	int						iRemainingTime;
	int						bRepeating;
	int						bKilled;
	AMX*					pAMX;
	int						iParamCount;
	void*					cellParams;
};

typedef std::map<unsigned long, ScriptTimer_s*> DwordTimerMap;

class CScriptTimers {
public:
	DwordTimerMap			Timers;
	unsigned long			dwTimerCount;
};

struct CNetGame {
	CGameMode				*pGameModePool;
	CFilterScripts			*pFilterScriptPool;
	CPlayerPool				*pPlayerPool;
	CVehiclePool			*pVehiclePool;
	CPickupPool				*pPickupPool;
	CObjectPool				*pObjectPool;
	CMenuPool				*pMenuPool;
	CTextDrawPool			*pTextDrawPool;
	C3DTextPool				*p3DTextPool;
	CSAMPGangZonePool		*pGangZonePool;
	CActorPool				*pActorPool;
	int						iCurrentGameModeIndex;
	int						iCurrentGameModeRepeat;
	int						bFirstGameModeLoaded;
	void					*pHttpClient;
	CScriptTimers			*pScriptTimers;
	void					*pRak;
	unsigned long			dwSomethingTick;
	unsigned long			dwUnk;
	unsigned long			dwUnk1;
	int						bLanMode;
	int						bShowPlayerMarkers;
	unsigned char			byteShowNameTags;
	unsigned char			byteWorldTimeHour;
	unsigned char			byteAllowWeapons;
	unsigned char			byteStuntBonus;
	unsigned char			byteDefaultCameraCollision;
	unsigned char			byteWeather;
	int						iGameState;
	float					fGravity;
	int						iDeathDropMoney;
	unsigned char			byteEnableZoneNames;
	unsigned char			byteMode;
	unsigned char			bLimitGlobalChatRadius;
	unsigned char			bUseCJWalk;
	float					fGlobalChatRadius;
	float					fNameTagDrawDistance;
	unsigned char			byteDisableEnterExits;
	unsigned char			byteNameTagLOS;
	unsigned char			bManulVehicleEngineAndLights;
	unsigned char			bLimitPlayerMarkers;
	float					fPlayerMarkesLimit;
	int						bVehicleFriendlyFire;
#ifndef _WIN32
	double					dElapsedTime;
#endif
	int						iSpawnsAvailable;
	CPlayerSpawnInfo		AvailableSpawns[319];
};

#pragma pack(pop)
