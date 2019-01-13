#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <game/CPed.h>
#include <game/CObject.h>
#include <game/CVehicle.h>

// Версия SAMP (SAMP_037/SAMP_03DL)
#define SAMP_037

#if defined(SAMP_037)

#define SAMP_CHAT_INFO_OFFSET					0x21A0E4
#define SAMP_CHAT_INPUT_INFO_OFFSET				0x21A0E8
#define SAMP_KILL_INFO_OFFSET					0x21A0EC
#define SAMP_INFO_OFFSET						0x21A0F8
#define SAMP_MISC_INFO							0x21A10C
#define SAMP_SCOREBOARD_INFO					0x21A0B4
#define SAMP_COLOR_OFFSET						0x216378
#define SAMP_DRAWING_DISTANCE					0xD3DCC

#define SAMP_DRAWTEXTLABELS						0x71471
#define SAMP_ONFOOTSENDRATE						0xEC0A8
#define SAMP_INCARSENDRATE						0xEC0AC
#define SAMP_AIMSENDRATE						0xEC0B0

#define SAMP_FUNC_ADDTOCHATWND					0x64010
#define SAMP_FUNC_SPAWN							0x3AD0
#define SAMP_FUNC_REQUEST_SPAWN					0x3EC0
#define SAMP_FUNC_REQUEST_CLASS					0x56A0
#define SAMP_FUNC_DEATH							0x55E0
#define SAMP_FUNC_SETSPECIALACTION				0x30C0
#define SAMP_FUNC_SAY							0x57F0
#define SAMP_FUNC_SENDCMD						0x65C60
#define SAMP_FUNC_ADDCLIENTCMD					0x65AD0
#define SAMP_FUNC_NAMECHANGE					0xB290
#define SAMP_FUNC_RESTARTGAME					0xA060
#define SAMP_FUNC_ENCRYPT_PORT					0x1BC20
#define SAMP_FUNC_TOGGLECURSOR					0x9BD30
#define SAMP_FUNC_CURSORUNLOCKACTORCAM			0x9BC10
#define SAMP_FUNC_UPDATESCOREBOARDDATA			0x8A10
#define SAMP_FUNC_CNETGAMEDESTRUCTOR			0x9380
#define SAMP_FUNC_SENDINTERIOR					0x5740
#define SAMP_FUNC_DISABLE_SCOREBOARD			0x6A320

#define SAMP_HOOKENTER_STATECHANGE				0x12B82
#define SAMP_HOOKENTER_HANDLE_RPC				0x3743D
#define SAMP_HOOKENTER_HANDLE_RPC2				0x373C9
#define SAMP_HOOKENTER_CNETGAME_DESTR			0xB2893
#define SAMP_HOOKENTER_CNETGAME_DESTR2			0xB3A32
#define SAMP_HOOKEXIT_ANTICARJACK				0x12B93
#define SAMP_HOOKEXIT_HANDLE_RPC				0x37443
#define SAMP_HOOKEXIT_HANDLE_RPC2				0x37451

#define SAMP_HOOK_INIT_SAMP						0x2565E2
#define SAMP_HOOK_INIT_RAKCLIENT				0x2401D3

#define SAMP_NOP_CREATEOBJECT					0x15FB0
#define SAMP_NOP_DESTROYOBJECT					0x16870

#define SAMP_PATCH_DISABLE_NAMETAGS				0x70D40
#define SAMP_PATCH_DISABLE_NAMETAGS_HP			0x6FC30
#define SAMP_PATCH_SKIPSENDINTERIOR				0x7668
#define SAMP_PATCH_SCOREBOARDTOGGLEON			0x6AA10
#define SAMP_PATCH_SCOREBOARDTOGGLEONKEYLOCK	0x6AD30
#define SAMP_PATCH_CHATINPUTADJUST_Y			0x63FE6
#define SAMP_PATCH_CHATINPUTADJUST_X			0x65855

enum DialogStyle {
	DIALOG_STYLE_MSGBOX,
	DIALOG_STYLE_INPUT,
	DIALOG_STYLE_LIST,
	DIALOG_STYLE_PASSWORD,
};

enum Limits {
	SAMP_MAX_ACTORS = 1000,
	SAMP_MAX_PLAYERS = 1004,
	SAMP_MAX_VEHICLES = 2000,
	SAMP_MAX_PICKUPS = 4096,
	SAMP_MAX_OBJECTS = 1000,
	SAMP_MAX_GANGZONES = 1024,
	SAMP_MAX_3DTEXTS = 2048,
	SAMP_MAX_TEXTDRAWS = 2048,
	SAMP_MAX_PLAYERTEXTDRAWS = 256,
	SAMP_MAX_CLIENTCMDS = 144,
	SAMP_MAX_MENUS = 128,
	SAMP_MAX_PLAYER_NAME = 24,
	SAMP_ALLOWED_PLAYER_NAME_LENGTH = 20,
};

enum ChatMessageType {
	CHAT_TYPE_NONE = 0,
	CHAT_TYPE_CHAT = 2,
	CHAT_TYPE_INFO = 4,
	CHAT_TYPE_DEBUG = 8
};

enum ChatDisplayMode {
	CHAT_WINDOW_MODE_OFF = 0,
	CHAT_WINDOW_MODE_LIGHT = 1,
	CHAT_WINDOW_MODE_FULL = 2
};

enum Gamestate {
	GAMESTATE_WAIT_CONNECT = 9,
	GAMESTATE_CONNECTING = 13,
	GAMESTATE_AWAIT_JOIN = 15,
	GAMESTATE_CONNECTED = 14,
	GAMESTATE_RESTARTING = 18
};

enum PlayerState {
	PLAYER_STATE_NONE = 0,
	PLAYER_STATE_ONFOOT = 17,
	PLAYER_STATE_PASSENGER,
	PLAYER_STATE_DRIVER,
	PLAYER_STATE_WASTED = 32,
	PLAYER_STATE_SPAWNED
};

enum MarkersMode {
	PLAYER_MARKERS_MODE_OFF,
	PLAYER_MARKERS_MODE_GLOBAL,
	PLAYER_MARKERS_MODE_STREAMED,
};

enum SpecialAction {
	SPECIAL_ACTION_NONE,
	SPECIAL_ACTION_DUCK,
	SPECIAL_ACTION_USEJETPACK,
	SPECIAL_ACTION_ENTER_VEHICLE,
	SPECIAL_ACTION_EXIT_VEHICLE,
	SPECIAL_ACTION_DANCE1,
	SPECIAL_ACTION_DANCE2,
	SPECIAL_ACTION_DANCE3,
	SPECIAL_ACTION_DANCE4,
	SPECIAL_ACTION_HANDSUP,
	SPECIAL_ACTION_USECELLPHONE,
	SPECIAL_ACTION_SITTING,
	SPECIAL_ACTION_STOPUSECELLPHONE,
	SPECIAL_ACTION_DRINK_BEER = 20,
	SPECIAL_ACTION_SMOKE_CIGGY,
	SPECIAL_ACTION_DRINK_WINE,
	SPECIAL_ACTION_DRINK_SPRUNK,
	SPECIAL_ACTION_CUFFED,
	SPECIAL_ACTION_CARRY,
};

#pragma pack(push, 1)
struct stSAMPPools {
	struct stActorPool		*pActor;
	struct stObjectPool		*pObject;
	struct stGangzonePool	*pGangzone;
	struct stTextLabelPool	*pText3D;
	struct stTextdrawPool	*pTextdraw;
	void					*pPlayerLabels;
	struct stPlayerPool		*pPlayer;
	struct stVehiclePool	*pVehicle;
	struct stPickupPool		*pPickup;
};

struct stSAMP {
	void					*pUnk0;
	struct stServerInfo		*pServerInfo;
	uint8_t					byteSpace[24];
	char					szIP[257];
	char					szHostname[259];
	bool					bNametagStatus;
	uint32_t				ulPort;
	uint32_t				ulMapIcons[100];
	int						iLanMode;
	int						iGameState;
	uint32_t				ulConnectTick;
	struct stServerPresets	*pSettings;
	void					*pRakClientInterface;
	struct stSAMPPools		*pPools;
};

struct stServerInfo {
	uint32_t				uiIP;
	uint16_t				usPort;
};

struct stServerPresets {
	uint8_t					byteCJWalk;
	int						m_iDeathDropMoney;
	float					fWorldBoundaries[4];
	bool					m_bAllowWeapons;
	float					fGravity;
	uint8_t					byteDisableInteriorEnterExits;
	uint32_t				ulVehicleFriendlyFire;
	bool					m_byteHoldTime;
  	bool					m_bInstagib;
  	bool					m_bZoneNames;
  	bool					m_byteFriendlyFire;
	int						iClassesAvailable;
	float					fNameTagsDistance;
	bool					m_bManualVehicleEngineAndLight;
	uint8_t					byteWorldTime_Hour;
	uint8_t					byteWorldTime_Minute;
	uint8_t					byteWeather;
	uint8_t					byteNoNametagsBehindWalls;
	int						iPlayerMarkersMode;
	float					fGlobalChatRadiusLimit;
	uint8_t					byteShowNameTags;
 	bool					m_bLimitGlobalChatRadius;
};

struct stDialogInfo {
	IDirect3DDevice9		*m_pD3DDevice;
	int						iTextPoxX;
	int						iTextPoxY;
	uint32_t				uiDialogSizeX;
	uint32_t				uiDialogSizeY;
	int						iBtnOffsetX;
	int						iBtnOffsetY;
	class _CDXUTDialog		*pDialog;
	class _CDXUTListBox		*pList;
	class _CDXUTIMEEditBox	*pEditBox;
	int						iIsActive;
	int						iType;
	uint32_t				DialogID;
	char					*pText;
	uint32_t				uiTextWidth;
	uint32_t				uiTextHeight;
	char					szCaption[65];
	int						bServerside;
};

struct stTextDrawTransmit {
	union {
		BYTE				byteFlags;
		struct {
			BYTE			byteBox : 1;
			BYTE			byteLeft : 1;
			BYTE			byteRight : 1;
			BYTE			byteCenter : 1;
			BYTE			byteProportional : 1;
			BYTE			bytePadding : 3;
		};
	};
	float					fLetterWidth;
	float					fLetterHeight;
	DWORD					dwLetterColor;
	float					fBoxWidth;
	float					fBoxHeight;
	DWORD					dwBoxColor;
	BYTE					byteShadow;
	BYTE					byteOutline;
	DWORD					dwBackgroundColor;
	BYTE					byteStyle;
	BYTE					byteUNK;
	float					fX;
	float					fY;
	uint16_t				sModel;
	float					fRot[3];
	float					fZoom;
	WORD					sColor[2];
};

struct stTextdraw {
	char					szText[800 + 1];
	char					szString[1600 + 2];
	float					fLetterWidth;
	float					fLetterHeight;
	DWORD					dwLetterColor;
	uint8_t					byte_unk;	// always = 01 (?)
	BYTE					byteCenter;
	BYTE					byteBox;
	float					fBoxSizeX;
	float					fBoxSizeY;
	DWORD					dwBoxColor;
	BYTE					byteProportional;
	DWORD					dwShadowColor;
	BYTE					byteShadowSize;
	BYTE					byteOutline;
	BYTE					byteLeft;
	BYTE					byteRight;
	int						iStyle;		// font style/texture/model
	float					fX;
	float					fY;
	byte					unk[8];
	DWORD					dword99B;	// -1 by default
	DWORD					dword99F;	// -1 by default
	DWORD					index;		// -1 if bad
	BYTE					byte9A7;	// = 1; 0 by default
	uint16_t				sModel;
	float					fRot[3];
	float					fZoom;
	WORD					sColor[2];
	BYTE					f9BE;
	BYTE					byte9BF;
	BYTE					byte9C0;
	DWORD					dword9C1;
	DWORD					dword9C5;
	DWORD					dword9C9;
	DWORD					dword9CD;
	BYTE					byte9D1;
	DWORD					dword9D2;
};

struct stTextdrawPool {
	int						iIsListed[SAMP_MAX_TEXTDRAWS];
	int						iPlayerTextDraw[SAMP_MAX_PLAYERTEXTDRAWS];
	struct stTextdraw		*textdraw[SAMP_MAX_TEXTDRAWS];
	struct stTextdraw		*playerTextdraw[SAMP_MAX_PLAYERTEXTDRAWS];
};

struct stPickup {
	int						iModelID;
	int						iType;
	float					fPosition[3];
};

struct stPickupPool {
	int						iPickupsCount;
	uint32_t				ul_GTA_PickupID[SAMP_MAX_PICKUPS];
	int						iPickupID[SAMP_MAX_PICKUPS];
	int						iTimePickup[SAMP_MAX_PICKUPS];
	uint8_t					unk[SAMP_MAX_PICKUPS * 3];
	struct stPickup			pickup[SAMP_MAX_PICKUPS];
};

struct stPlayerPool {
	uint32_t				ulMaxPlayerID;
	uint16_t				sLocalPlayerID;
	void					*pVTBL_txtHandler;
	std::string				strLocalPlayerName;
	struct stLocalPlayer	*pLocalPlayer;
	int						iLocalPlayerPing;
	int						iLocalPlayerScore;
	struct stRemotePlayer	*pRemotePlayer[SAMP_MAX_PLAYERS];
	int						iIsListed[SAMP_MAX_PLAYERS];
	DWORD					dwPlayerIP[SAMP_MAX_PLAYERS]; // always 0
};

struct stSAMPKeys {
	uint8_t					keys_primaryFire : 1;
	uint8_t					keys_horn__crouch : 1;
	uint8_t					keys_secondaryFire__shoot : 1;
	uint8_t					keys_accel__zoomOut : 1;
	uint8_t					keys_enterExitCar : 1;
	uint8_t					keys_decel__jump : 1;			// on foot: jump or zoom in
	uint8_t					keys_circleRight : 1;
	uint8_t					keys_aim : 1;					// hydra auto aim or on foot aim
	uint8_t					keys_circleLeft : 1;
	uint8_t					keys_landingGear__lookback : 1;
	uint8_t					keys_unknown__walkSlow : 1;
	uint8_t					keys_specialCtrlUp : 1;
	uint8_t					keys_specialCtrlDown : 1;
	uint8_t					keys_specialCtrlLeft : 1;
	uint8_t					keys_specialCtrlRight : 1;
	uint8_t					keys__unused : 1;
};

struct stOnFootData {
	uint16_t				sLeftRightKeys;
	uint16_t				sUpDownKeys;
	union {
		uint16_t			sKeys;
		struct stSAMPKeys	stSampKeys;
	};
	float					fPosition[3];
	float					fQuaternion[4];
	uint8_t					byteHealth;
	uint8_t					byteArmor;
	uint8_t					byteCurrentWeapon;
	uint8_t					byteSpecialAction;
	float					fMoveSpeed[3];
	float					fSurfingOffsets[3];
	uint16_t				sSurfingVehicleID;
	short					sCurrentAnimationID;
	short					sAnimFlags;
};

struct stInCarData {
	uint16_t				sVehicleID;
	uint16_t				sLeftRightKeys;
	uint16_t				sUpDownKeys;
	union {
		uint16_t			sKeys;
		struct stSAMPKeys	stSampKeys;
	};
	float					fQuaternion[4];
	float					fPosition[3];
	float					fMoveSpeed[3];
	float					fVehicleHealth;
	uint8_t					bytePlayerHealth;
	uint8_t					byteArmor;
	uint8_t					byteCurrentWeapon;
	uint8_t					byteSiren;
	uint8_t					byteLandingGearState;
	uint16_t				sTrailerID;
	union {
		uint16_t			HydraThrustAngle[2];
		float				fTrainSpeed;
		float				fBikeSideAngle;
	};
};

struct stAimData {
	BYTE					byteCamMode;
	float					vecAimf1[3];
	float					vecAimPos[3];
	float					fAimZ;
	BYTE					byteCamExtZoom : 6;
	BYTE					byteWeaponState : 2;
	BYTE					byteAspectRatio;
};

struct stTrailerData {
	uint16_t				sTrailerID;
	float					fPosition[3];
	float					fQuaternion[4];
	float					fSpeed[3];
	float					fSpin[3];
};

struct stPassengerData {
	uint16_t				sVehicleID;
	uint8_t					byteSeatID;
	uint8_t					byteCurrentWeapon;
	uint8_t					byteHealth;
	uint8_t					byteArmor;
	uint16_t				sLeftRightKeys;
	uint16_t				sUpDownKeys;
	union
	{
		uint16_t			sKeys;
		struct stSAMPKeys	stSampKeys;
	};
	float					fPosition[3];
};

struct stDamageData {
	uint16_t				sVehicleID_lastDamageProcessed;
	int						iBumperDamage;
	int						iDoorDamage;
	uint8_t					byteLightDamage;
	uint8_t					byteWheelDamage;
};

struct stSurfData {
	int						iIsSurfing;
	float					fSurfPosition[3];
	int						iUnk0;
	uint16_t				sSurfingVehicleID;
	uint32_t				ulSurfTick;
	struct stSAMPVehicle	*pSurfingVehicle;
	int						iUnk1;
	int						iSurfMode;	//0 = not surfing, 1 = moving (unstable surf), 2 = fixed on vehicle
};

struct stUnoccupiedData {
	int16_t					sVehicleID;
	uint8_t					byteSeatID;
	float					fRoll[3];
	float					fDirection[3];
	float					fPosition[3];
	float					fMoveSpeed[3];
	float					fTurnSpeed[3];
	float					fHealth;
};

struct stBulletData {
	uint8_t					byteType;
	uint16_t				sTargetID;
	float					fOrigin[3];
	float					fTarget[3];
	float					fCenter[3];
	uint8_t					byteWeaponID;
};

struct stSpectatorData {
	uint16_t				sLeftRightKeys;
	uint16_t				sUpDownKeys;
	union {
		uint16_t			sKeys;
		struct stSAMPKeys	stSampKeys;
	};
	float					fPosition[3];
};

struct stStatsData {
	int						iMoney;
	int						iAmmo;
};

struct stHeadSync {
	float					fHeadSync[3];
	int						iHeadSyncUpdateTick;
	int						iHeadSyncLookTick;
};

struct stLocalPlayer {
	struct stSAMPPed		*pSAMP_Actor;
	uint16_t				sCurrentAnimID;
	uint16_t				sAnimFlags;
	uint32_t				ulUnk0;
	int						iIsActive;
	int						iIsWasted;
	uint16_t				sCurrentVehicleID;
	uint16_t				sLastVehicleID;
	struct stOnFootData		onFootData;
	struct stPassengerData	passengerData;
	struct stTrailerData	trailerData;
	struct stInCarData		inCarData;
	struct stAimData		aimData;
	uint8_t					byteTeamID;
	int						iSpawnSkin;
	uint8_t					byteUnk1;
	float					fSpawnPos[3];
	float					fSpawnRot;
	int						iSpawnWeapon[3];
	int						iSpawnAmmo[3];
	int						iIsActorAlive;
	int						iSpawnClassLoaded;
	uint32_t				ulSpawnSelectionTick;
	uint32_t				ulSpawnSelectionStart;
	int						iIsSpectating;
	uint8_t					byteTeamID2;
	uint16_t				usUnk2;
	uint32_t				ulSendTick;
	uint32_t				ulSpectateTick;
	uint32_t				ulAimTick;
	uint32_t				ulStatsUpdateTick;
	uint32_t				ulWeapUpdateTick;
	uint16_t				sAimingAtPid;
	uint16_t				usUnk3;
	uint8_t					byteCurrentWeapon;
	uint8_t					byteWeaponInventory[13];
	int						iWeaponAmmo[13];
	int						iPassengerDriveBy;
	uint8_t					byteCurrentInterior;
	int						iIsInRCVehicle;
	uint16_t				sTargetObjectID;
	uint16_t				sTargetVehicleID;
	uint16_t				sTargetPlayerID;
	struct stHeadSync		headSyncData;
	uint32_t				ulHeadSyncTick;
	BYTE					byteSpace3[260];
	struct stSurfData		surfData;
	int						iClassSelectionOnDeath;
	int						iSpawnClassID;
	int						iRequestToSpawn;
	int						iIsInSpawnScreen;
	uint32_t				ulUnk4;
	uint8_t					byteSpectateMode;		// 3 = vehicle, 4 = player, side = 14, fixed = 15
	uint8_t					byteSpectateType;		// 0 = none, 1 = player, 2 = vehicle
	int						iSpectateID;
	int						iInitiatedSpectating;
	struct stDamageData		vehicleDamageData;
};

struct stRemotePlayerData {
	struct stSAMPPed		*pSAMP_Actor;
	struct stSAMPVehicle	*pSAMP_Vehicle;
	uint8_t					byteTeamID;
	uint8_t					bytePlayerState;
	uint8_t					byteSeatID;
	uint32_t				ulUnk3;
	int						iPassengerDriveBy;
	void					*pUnk0;
	uint8_t					byteUnk1[60];
	float					fSomething[3];
	float					fVehicleRoll[4];
	uint32_t				ulUnk2[3];
	float					fOnFootPos[3];
	float					fOnFootMoveSpeed[3];
	float					fVehiclePosition[3];
	float					fVehicleMoveSpeed[3];
	uint16_t				sPlayerID;
	uint16_t				sVehicleID;
	uint32_t				ulUnk5;
	int						iShowNameTag;
	int						iHasJetPack;
	uint8_t					byteSpecialAction;
	uint32_t				ulUnk4[3];
	struct stOnFootData		onFootData;
	struct stInCarData		inCarData;
	struct stTrailerData	trailerData;
	struct stPassengerData	passengerData;
	struct stAimData		aimData;
	float					fActorArmor;
	float					fActorHealth;
	uint32_t				ulUnk10;
	uint8_t					byteUnk9;
	uint32_t				dwTick;
	uint32_t				dwLastStreamedInTick;	// is 0 when currently streamed in
	uint32_t				ulUnk7;
	int						iAFKState;
	struct stHeadSync		headSyncData;
	int						iGlobalMarkerLoaded;
	int						iGlobalMarkerLocation[3];
	uint32_t				ulGlobalMarker_GTAID;
};

struct stRemotePlayer {
	stRemotePlayerData		*pPlayerData;
	int						iIsNPC;
	void					*pVTBL_txtHandler;
	std::string				strPlayerName;
	int						iScore;
	int						iPing;
};

template <typename T>
struct stSAMPEntity {
	void					*pVTBL;
	uint8_t					byteUnk0[60]; // game CEntity object maybe. always empty.
	T						*pGTAEntity;
	uint32_t				ulGTAEntityHandle;
};

struct stSAMPPed : public stSAMPEntity < CPed > {
	int						usingCellPhone;
	uint8_t					byteUnk0[600];
	CPed					*pGTA_Ped;
	uint8_t					byteUnk1[22];
	uint8_t					byteKeysId;
	uint16_t				ulGTA_UrinateParticle_ID;
	int						DrinkingOrSmoking;
	int						object_in_hand;
	int						drunkLevel;
	uint8_t					byteUnk2[5];
	int						isDancing;
	int						danceStyle;
	int						danceMove;
	uint8_t					byteUnk3[20];
	int						isUrinating;
};

struct stVehiclePool {
	int						iVehicleCount;
	void					*pUnk0;
	uint8_t					byteSpace1[0x112C];
	struct stSAMPVehicle	*pSAMP_Vehicle[SAMP_MAX_VEHICLES];
	int						iIsListed[SAMP_MAX_VEHICLES];
	CVehicle				*pGTA_Vehicle[SAMP_MAX_VEHICLES];
	uint8_t					byteSpace2[SAMP_MAX_VEHICLES * 6];
	uint32_t				ulShit[SAMP_MAX_VEHICLES];
	int						iIsListed2[SAMP_MAX_VEHICLES];
	uint32_t				byteSpace3[SAMP_MAX_VEHICLES * 2];
	float					fSpawnPos[SAMP_MAX_VEHICLES][3];
	int						iInitiated;
};

struct stSAMPVehicle : public stSAMPEntity < CVehicle > {
	uint32_t				bUnk0;
	CVehicle				*pGTA_Vehicle;
	uint8_t					byteUnk1[8];
	int						bIsMotorOn;
	int						iIsLightsOn;
	int						iIsLocked;
	uint8_t					byteIsObjective;
	int						iObjectiveBlipCreated;
	uint8_t					byteUnk2[16];
	uint8_t					byteColor[2];
	int						iColorSync;
	int						iColor_something;
};

struct stObject : public stSAMPEntity < CObject > {
	uint8_t					byteUnk0[2];
	uint32_t				ulUnk1;
	int						iModel;
	uint16_t				byteUnk2;
	float					fDrawDistance;
	float					fUnk;
	float					fPos[3];
	uint8_t					byteUnk3[68];
	uint8_t					byteUnk4;
	float					fRot[3];
};

struct stObjectPool {
	int						iObjectCount;
	int						iIsListed[SAMP_MAX_OBJECTS];
	struct stObject			*object[SAMP_MAX_OBJECTS];
};

struct stGangzone {
	float					fPosition[4];
	DWORD					dwColor;
	DWORD					dwAltColor;
};

struct stGangzonePool {
	struct stGangzone		*pGangzone[SAMP_MAX_GANGZONES];
	int						iIsListed[SAMP_MAX_GANGZONES];
};

struct stTextLabel {
	char					*pText;
	DWORD					color;
	float					fPosition[3];
	float					fMaxViewDistance;
	uint8_t					byteShowBehindWalls;
	uint16_t				sAttachedToPlayerID;
	uint16_t				sAttachedToVehicleID;
};

struct stTextLabelPool {
	struct stTextLabel		textLabel[SAMP_MAX_3DTEXTS];
	int						iIsListed[SAMP_MAX_3DTEXTS];
};

struct stChatEntry {
	uint32_t				SystemTime;
	char					szPrefix[28];
	char					szText[144];
	uint8_t					unknown[64];
	int						iType;			// 2 - text + prefix, 4 - text (server msg), 8 - text (debug)
	D3DCOLOR				clTextColor;
	D3DCOLOR				clPrefixColor;	// or textOnly colour
};

struct stFontRenderer {
	ID3DXFont				*m_pChatFont;
	ID3DXFont				*m_pLittleFont;
	ID3DXFont				*m_pChatShadowFont;
	ID3DXFont				*m_pLittleShadowFont;
	ID3DXFont				*m_pCarNumberFont;
	ID3DXSprite				*m_pTempSprite;
	IDirect3DDevice9		*m_pD3DDevice;
	char					*m_pszTextBuffer;
};

struct stChatInfo {
	int						pagesize;
	char					*pLastMsgText;
	int						iChatWindowMode;
	uint8_t					bTimestamps;
	uint32_t				m_iLogFileExist;
	char					logFilePathChatLog[MAX_PATH + 1];
	void					*pGameUI; // CDXUTDialog
	void					*pEditBackground; // CDXUTEditBox
	void					*pDXUTScrollBar;
	D3DCOLOR				clTextColor;
	D3DCOLOR				clInfoColor;
	D3DCOLOR				clDebugColor;
	DWORD					m_lChatWindowBottom;
	struct stChatEntry		chatEntry[100];
	stFontRenderer			*m_pFontRenderer;
	ID3DXSprite				*m_pChatTextSprite;
	ID3DXSprite				*m_pSprite;
	IDirect3DDevice9		*m_pD3DDevice;
	int						m_iRenderMode; // 0 - Direct Mode (slow), 1 - Normal mode
	ID3DXRenderToSurface	*pID3DXRenderToSurface;
	IDirect3DTexture9		*m_pTexture;
	IDirect3DSurface9		*pSurface;
	D3DDISPLAYMODE			*pD3DDisplayMode;
	int						iUnk1[3];
	int						iUnk2; // smth related to drawing in direct mode
	int						m_iRedraw;
	int						m_nPrevScrollBarPosition;
	int						m_iFontSizeY;
	int						m_iTimestampWidth;
};

struct stInputBox {
	void					*pUnknown;
	uint8_t					bIsChatboxOpen;
	uint8_t					bIsMouseInChatbox;
	uint8_t					bMouseClick_related;
	uint8_t					unk;
	DWORD					dwPosChatInput[2];
	uint8_t					unk2[263];
	int						iCursorPosition;
	uint8_t					unk3;
	int						iMarkedText_startPos; // Highlighted text between this and iCursorPosition
	uint8_t					unk4[20];
	int						iMouseLeftButton;
};

typedef void(__cdecl *CMDPROC) (PCHAR);
struct stInputInfo {
	void					*pD3DDevice;
	void					*pDXUTDialog;
	stInputBox				*pDXUTEditBox;
	CMDPROC					pCMDs[SAMP_MAX_CLIENTCMDS];
	char					szCMDNames[SAMP_MAX_CLIENTCMDS][33];
	int						iCMDCount;
	int						iInputEnabled;
	char					szInputBuffer[129];
	char					szRecallBufffer[10][129];
	char					szCurrentBuffer[129];
	int						iCurrentRecall;
	int						iTotalRecalls;
	CMDPROC					pszDefaultCMD;
};

struct stKillEntry {
	char					szKiller[25];
	char					szVictim[25];
	D3DCOLOR				clKillerColor;
	D3DCOLOR				clVictimColor;
	uint8_t					byteType;
};

struct stKillInfo {
	int						iEnabled;
	struct stKillEntry		killEntry[5];
	int 					iLongestNickLength;
  	int						iOffsetX;
  	int 					iOffsetY;
	ID3DXFont				*pD3DFont;
	ID3DXFont				*pWeaponFont1;
	ID3DXFont				*pWeaponFont2;
	ID3DXSprite				*pSprite;
	IDirect3DDevice9		*pD3DDevice;
	int						iAuxFontInited;
  	ID3DXFont 				*pAuxFont1;
  	ID3DXFont 				*pAuxFont2;
};

struct stChatPlayer {
	int						iCreated;
	char					probablyTheText[256];
	DWORD					dwTickCreated;
	DWORD					dwLiveLength;
	DWORD					dwColor;
	float					fDrawDistance;
	DWORD					dwUnknown;
};

struct stAudio {
	int						iSoundState; // 0 - Finished, 1 - Loaded, 2 - Playing
	bool					bStopInteriourAmbientSounds;
};

struct stGameInfo {
	struct stCamera {
		void*				pEntity; // attached entity
		class CMatrix_Padded*	matrix;
	};
	stAudio*				pAudio;
	stCamera*				pCamera;
	stSAMPPed*				pLocalPlayerPed;
	float					fCheckpointPos[3];
	float					fCheckpointExtent[3];
	int						bCheckpointsEnabled;

	// not tested
	DWORD					dwCheckpointMarker;
	float					fRaceCheckpointPos[3];
	float					fRaceCheckpointNext[3];
	float					m_fRaceCheckpointSize;
	uint8_t					byteRaceType;
	
	int						bRaceCheckpointsEnabled;

	// not tested
	DWORD					dwRaceCheckpointMarker;
	DWORD					dwRaceCheckpointHandle;

	int						iCursorMode;
	uint32_t				ulUnk1;
	int						bClockEnabled;
	uint32_t				ulUnk2;
	int						bHeadMove;
	uint32_t				ulFpsLimit;
	uint8_t					byteUnk3;
	uint8_t					byteVehicleModels[212];
};

struct stScoreboardInfo {
	int						iIsEnabled;
	int						iPlayersCount;
	float					fTextOffset[2];
	float					fScalar;
	float					fSize[2];
	float					fUnk0[5];
	IDirect3DDevice9		*pDirectDevice;
	class _CDXUTDialog		*pDialog;
	class _CDXUTListBox		*pList;
	int						iOffset;
	int						iIsSorted;
};

struct stActorPool {
	int						iLastActorID;
	stSAMPEntity<void>		*pActor[SAMP_MAX_ACTORS];
	int						iIsListed[SAMP_MAX_ACTORS];
	CPed					*pGTAPed[SAMP_MAX_ACTORS];
	uint32_t				ulUnk0[SAMP_MAX_ACTORS];
	uint32_t				ulUnk1[SAMP_MAX_ACTORS];
};

struct stChatBubbleInfo {
	struct stChatPlayer		chatBubble[SAMP_MAX_PLAYERS];
};

struct stStreamedOutPlayerInfo {
	int						iPlayerID[SAMP_MAX_PLAYERS];
	float					fPlayerPos[SAMP_MAX_PLAYERS][3];
};
#pragma pack(pop)

#elif defined(SAMP_03DL)

#define SAMP_CHAT_INFO_OFFSET					0x2ACA10
#define SAMP_CHAT_INPUT_INFO_OFFSET				0x2ACA14
#define SAMP_KILL_INFO_OFFSET					0x2ACA18
#define SAMP_INFO_OFFSET						0x2ACA24
#define SAMP_MISC_INFO							0x2ACA3C
#define SAMP_SCOREBOARD_INFO					0x2AC9DC
#define SAMP_COLOR_OFFSET						0x18F6C0
#define SAMP_DRAWING_DISTANCE					0x11812C

#define SAMP_DRAWTEXTLABELS						0x754F1
#define SAMP_ONFOOTSENDRATE						0x13C0A8
#define SAMP_INCARSENDRATE						0x13C0AC
#define SAMP_AIMSENDRATE						0x13C0B0

#define SAMP_FUNC_ADDTOCHATWND					0x67650
#define SAMP_FUNC_SPAWN							0x3B20
#define SAMP_FUNC_REQUEST_SPAWN					0x3F40
#define SAMP_FUNC_REQUEST_CLASS					0x5720
#define SAMP_FUNC_DEATH							0x5660
#define SAMP_FUNC_SETSPECIALACTION				0x3110
#define SAMP_FUNC_SAY							0x5860
#define SAMP_FUNC_SENDCMD						0x69340
#define SAMP_FUNC_ADDCLIENTCMD					0x691B0
#define SAMP_FUNC_NAMECHANGE					0xB490
#define SAMP_FUNC_RESTARTGAME					0xA230
#define SAMP_FUNC_ENCRYPT_PORT					0x1F1D0
#define SAMP_FUNC_TOGGLECURSOR					0xA0530
#define SAMP_FUNC_CURSORUNLOCKACTORCAM			0xA0410
#define SAMP_FUNC_UPDATESCOREBOARDDATA			0x8C00
#define SAMP_FUNC_CNETGAMEDESTRUCTOR			0x9570
#define SAMP_FUNC_SENDINTERIOR					0x57C0
#define SAMP_FUNC_DISABLE_SCOREBOARD			0x6E410

#define SAMP_HOOKENTER_STATECHANGE				0x15FAB
#define SAMP_HOOKENTER_HANDLE_RPC				0x3A9ED
#define SAMP_HOOKENTER_HANDLE_RPC2				0x3A979
#define SAMP_HOOKENTER_CNETGAME_DESTR			0xC5533
#define SAMP_HOOKENTER_CNETGAME_DESTR2			0xC6736
#define SAMP_HOOKEXIT_ANTICARJACK				0x15FBC
#define SAMP_HOOKEXIT_HANDLE_RPC				0x3A9F3
#define SAMP_HOOKEXIT_HANDLE_RPC2				0x3AA01

#define SAMP_HOOK_INIT_SAMP						0xC6614
#define SAMP_HOOK_INIT_RAKCLIENT				0xB678

#define SAMP_PATCH_DISABLE_NAMETAGS				0x74DC0
#define SAMP_PATCH_DISABLE_NAMETAGS_HP			0x73CB0
#define SAMP_PATCH_SKIPSENDINTERIOR				0x76D9
#define SAMP_PATCH_SCOREBOARDTOGGLEON			0x6EAF0
#define SAMP_PATCH_SCOREBOARDTOGGLEONKEYLOCK	0x6EE10
#define SAMP_PATCH_CHATINPUTADJUST_Y			0x67626
#define SAMP_PATCH_CHATINPUTADJUST_X			0x68F35

enum DialogStyle {
	DIALOG_STYLE_MSGBOX,
	DIALOG_STYLE_INPUT,
	DIALOG_STYLE_LIST,
	DIALOG_STYLE_PASSWORD,
	DIALOG_STYLE_TABLIST,
	DIALOG_STYLE_TABLIST_HEADERS
};

enum Limits {
	SAMP_MAX_ACTORS = 1000,
	SAMP_MAX_PLAYERS = 1004,
	SAMP_MAX_VEHICLES = 2000,
	SAMP_MAX_PICKUPS = 4096,
	SAMP_MAX_OBJECTS = 2100,
	SAMP_MAX_GANGZONES = 1024,
	SAMP_MAX_3DTEXTS = 2048,
	SAMP_MAX_TEXTDRAWS = 2048,
	SAMP_MAX_PLAYERTEXTDRAWS = 256,
	SAMP_MAX_CLIENTCMDS = 144,
	SAMP_MAX_MENUS = 128,
	SAMP_MAX_PLAYER_NAME = 24,
	SAMP_ALLOWED_PLAYER_NAME_LENGTH = 20,
	SAMP_MAX_MAPICONS = 100,
};

enum ChatMessageType
{
	CHAT_TYPE_NONE = 0,
	CHAT_TYPE_CHAT = 2,
	CHAT_TYPE_INFO = 4,
	CHAT_TYPE_DEBUG = 8
};

enum ChatDisplayMode
{
	CHAT_WINDOW_MODE_OFF = 0,
	CHAT_WINDOW_MODE_LIGHT = 1,
	CHAT_WINDOW_MODE_FULL = 2
};

enum Gamestate
{
	GAMESTATE_WAIT_CONNECT = 9,
	GAMESTATE_CONNECTING = 13,
	GAMESTATE_AWAIT_JOIN = 15,
	GAMESTATE_CONNECTED = 14,
	GAMESTATE_RESTARTING = 18
};

enum PlayerState
{
	PLAYER_STATE_NONE = 0,
	PLAYER_STATE_ONFOOT = 17,
	PLAYER_STATE_PASSENGER,
	PLAYER_STATE_DRIVER,
	PLAYER_STATE_WASTED = 32,
	PLAYER_STATE_SPAWNED
};

enum MarkersMode
{
	PLAYER_MARKERS_MODE_OFF,
	PLAYER_MARKERS_MODE_GLOBAL,
	PLAYER_MARKERS_MODE_STREAMED,
};

enum SpecialAction
{
	SPECIAL_ACTION_NONE,
	SPECIAL_ACTION_DUCK,
	SPECIAL_ACTION_USEJETPACK,
	SPECIAL_ACTION_ENTER_VEHICLE,
	SPECIAL_ACTION_EXIT_VEHICLE,
	SPECIAL_ACTION_DANCE1,
	SPECIAL_ACTION_DANCE2,
	SPECIAL_ACTION_DANCE3,
	SPECIAL_ACTION_DANCE4,
	SPECIAL_ACTION_HANDSUP,
	SPECIAL_ACTION_USECELLPHONE,
	SPECIAL_ACTION_SITTING,
	SPECIAL_ACTION_STOPUSECELLPHONE,
	SPECIAL_ACTION_DRINK_BEER = 20,
	SPECIAL_ACTION_SMOKE_CIGGY,
	SPECIAL_ACTION_DRINK_WINE,
	SPECIAL_ACTION_DRINK_SPRUNK,
	SPECIAL_ACTION_CUFFED,
	SPECIAL_ACTION_CARRY,
};

#pragma pack(push, 1)
struct stSAMPPools {
	void					*pMenu;
	struct stActorPool		*pActor;
	struct stPlayerPool		*pPlayer;
	struct stVehiclePool	*pVehicle;
	struct stPickupPool		*pPickup;
	struct stObjectPool		*pObject;
	struct stGangzonePool	*pGangzone;
	struct stTextLabelPool	*pText3D;
	struct stTextdrawPool	*pTextdraw;
};

struct stSAMP {
	uint8_t					_pad0[20];
	void					*pUnk0;
	struct stServerInfo		*pServerInfo;
	uint8_t					_pad1[16];
	void					*pRakClientInterface;
	char					szIP[256 + 1];
	char					szHostname[256 + 1];
	uint8_t					_pad2;
	bool					m_bUpdateCameraTarget;
	bool					m_bNoNameTagStatus;
	uint32_t				ulPort;
	BOOL					m_bLanMode;
	uint32_t				ulMapIcons[SAMP_MAX_MAPICONS];
	Gamestate				iGameState;
	uint32_t				ulConnectTick;
	struct stServerPresets	*pSettings;
	uint8_t					_pad3[5];
	struct stSAMPPools		*pPools;
};

struct stServerInfo {
	uint32_t 				uiIP;
	uint16_t 				usPort;
};

struct stServerPresets {
	uint8_t					byteCJWalk;
	int						m_iDeathDropMoney;
	float					fWorldBoundaries[4];
	bool					m_bAllowWeapons;
	float					fGravity;
	uint8_t					byteDisableInteriorEnterExits;
	uint32_t				ulVehicleFriendlyFire;
	bool					m_byteHoldTime;
	bool					m_bInstagib;
	bool					m_bZoneNames;
	bool					m_byteFriendlyFire;
	int						iClassesAvailable;
	float					fNameTagsDistance;
	bool					m_bManualVehicleEngineAndLight;
	uint8_t					byteWorldTime_Hour;
	uint8_t					byteWorldTime_Minute;
	uint8_t					byteWeather;
	uint8_t					byteNoNametagsBehindWalls;
	int						iPlayerMarkersMode;
	float					fGlobalChatRadiusLimit;
	uint8_t					byteShowNameTags;
	bool					m_bLimitGlobalChatRadius;
};

struct stDialogInfo {
	IDirect3DDevice9		*m_pD3DDevice;
	int						iTextPoxX;
	int						iTextPoxY;
	uint32_t				uiDialogSizeX;
	uint32_t				uiDialogSizeY;
	int						iBtnOffsetX;
	int						iBtnOffsetY;
	class _CDXUTDialog		*pDialog;
	class _CDXUTListBox		*pList;
	class _CDXUTIMEEditBox	*pEditBox;
	int						iIsActive;
	int						iType;
	uint32_t				DialogID;
	char					*pText;
	uint32_t				uiTextWidth;
	uint32_t				uiTextHeight;
	char					szCaption[65];
	int						bServerside;
};

struct stTextDrawTransmit {
	union {
		BYTE				byteFlags;
		struct {
			BYTE			byteBox : 1;
			BYTE			byteLeft : 1;
			BYTE			byteRight : 1;
			BYTE			byteCenter : 1;
			BYTE			byteProportional : 1;
			BYTE			bytePadding : 3;
		};
	};
	float					fLetterWidth;
	float					fLetterHeight;
	DWORD					dwLetterColor;
	float					fBoxWidth;
	float					fBoxHeight;
	DWORD					dwBoxColor;
	BYTE					byteShadow;
	BYTE					byteOutline;
	DWORD					dwBackgroundColor;
	BYTE					byteStyle;
	BYTE					byteUNK;
	float					fX;
	float					fY;
	uint16_t				sModel;
	float					fRot[3];
	float					fZoom;
	WORD					sColor[2];
};

struct stTextdraw {
	char					szText[800 + 1];
	char					szString[1600 + 2];
	float					fLetterWidth;
	float					fLetterHeight;
	DWORD					dwLetterColor;
	uint8_t					byte_unk;	// always = 01 (?)
	BYTE					byteCenter;
	BYTE					byteBox;
	float					fBoxSizeX;
	float					fBoxSizeY;
	DWORD					dwBoxColor;
	BYTE					byteProportional;
	DWORD					dwShadowColor;
	BYTE					byteShadowSize;
	BYTE					byteOutline;
	BYTE					byteLeft;
	BYTE					byteRight;
	int						iStyle;		// font style/texture/model
	float					fX;
	float					fY;
	byte					unk[8];
	DWORD					dword99B;	// -1 by default
	DWORD					dword99F;	// -1 by default
	DWORD					index;		// -1 if bad
	BYTE					byte9A7;	// = 1; 0 by default
	uint16_t				sModel;
	float					fRot[3];
	float					fZoom;
	WORD					sColor[2];
	BYTE					f9BE;
	BYTE					byte9BF;
	BYTE					byte9C0;
	DWORD					dword9C1;
	DWORD					dword9C5;
	DWORD					dword9C9;
	DWORD					dword9CD;
	BYTE					byte9D1;
	DWORD					dword9D2;
};

struct stTextdrawPool {
	int						iIsListed[SAMP_MAX_TEXTDRAWS];
	int						iPlayerTextDraw[SAMP_MAX_PLAYERTEXTDRAWS];
	struct stTextdraw		*textdraw[SAMP_MAX_TEXTDRAWS];
	struct stTextdraw		*playerTextdraw[SAMP_MAX_PLAYERTEXTDRAWS];
};

struct stPickup {
	int						iModelID;
	int						iType;
	float					fPosition[3];
};

struct stPickupPool {
	int						iPickupsCount;
	uint32_t				ul_GTA_PickupID[SAMP_MAX_PICKUPS];
	int						iPickupID[SAMP_MAX_PICKUPS];
	int						iTimePickup[SAMP_MAX_PICKUPS];
	uint8_t					unk[SAMP_MAX_PICKUPS * 3];
	struct stPickup			pickup[SAMP_MAX_PICKUPS];
};

struct stPlayerPool {
	uint16_t				sLocalPlayerID;
	void					*pVTBL_txtHandler;
	std::string				strLocalPlayerName;
	struct stLocalPlayer	*pLocalPlayer;
	uint32_t				ulMaxPlayerID;
	struct stRemotePlayer	*pRemotePlayer[SAMP_MAX_PLAYERS];
	int						iIsListed[SAMP_MAX_PLAYERS];
	BOOL					bSavedCheckCollision[SAMP_MAX_PLAYERS];
	int						iLocalPlayerPing;
	int						iLocalPlayerScore;
};

struct stSAMPKeys {
	uint8_t					keys_primaryFire : 1;
	uint8_t					keys_horn__crouch : 1;
	uint8_t					keys_secondaryFire__shoot : 1;
	uint8_t					keys_accel__zoomOut : 1;
	uint8_t					keys_enterExitCar : 1;
	uint8_t					keys_decel__jump : 1;			// on foot: jump or zoom in
	uint8_t					keys_circleRight : 1;
	uint8_t					keys_aim : 1;					// hydra auto aim or on foot aim
	uint8_t					keys_circleLeft : 1;
	uint8_t					keys_landingGear__lookback : 1;
	uint8_t					keys_unknown__walkSlow : 1;
	uint8_t					keys_specialCtrlUp : 1;
	uint8_t					keys_specialCtrlDown : 1;
	uint8_t					keys_specialCtrlLeft : 1;
	uint8_t					keys_specialCtrlRight : 1;
	uint8_t					keys__unused : 1;
};

struct stOnFootData {
	uint16_t				sLeftRightKeys;
	uint16_t				sUpDownKeys;
	union {
		uint16_t			sKeys;
		struct stSAMPKeys	stSampKeys;
	};
	float					fPosition[3];
	float					fQuaternion[4];
	uint8_t					byteHealth;
	uint8_t					byteArmor;
	uint8_t					byteCurrentWeapon;
	uint8_t					byteSpecialAction;
	float					fMoveSpeed[3];
	float					fSurfingOffsets[3];
	uint16_t				sSurfingVehicleID;
	short					sCurrentAnimationID;
	short					sAnimFlags;
};

struct stInCarData {
	uint16_t				sVehicleID;
	uint16_t				sLeftRightKeys;
	uint16_t				sUpDownKeys;
	union {
		uint16_t			sKeys;
		struct stSAMPKeys	stSampKeys;
	};
	float					fQuaternion[4];
	float					fPosition[3];
	float					fMoveSpeed[3];
	float					fVehicleHealth;
	uint8_t					bytePlayerHealth;
	uint8_t					byteArmor;
	uint8_t					byteCurrentWeapon;
	uint8_t					byteSiren;
	uint8_t					byteLandingGearState;
	uint16_t				sTrailerID;
	union {
		uint16_t			HydraThrustAngle[2];	//nearly same value
		float				fTrainSpeed;
		float				fBikeSideAngle;
	};
};

struct stAimData {
	BYTE					byteCamMode;
	float					vecAimf1[3];
	float					vecAimPos[3];
	float					fAimZ;
	BYTE					byteCamExtZoom : 6;		// 0-63 normalized
	BYTE					byteWeaponState : 2;	// see eWeaponState
	BYTE					byteAspectRatio;
};

struct stTrailerData {
	uint16_t				sTrailerID;
	float					fPosition[3];
	float					fQuaternion[4];
	float					fSpeed[3];
	float					fSpin[3];
};

struct stPassengerData {
	uint16_t				sVehicleID;
	uint8_t					byteSeatID;
	uint8_t					byteCurrentWeapon;
	uint8_t					byteHealth;
	uint8_t					byteArmor;
	uint16_t				sLeftRightKeys;
	uint16_t				sUpDownKeys;
	union {
		uint16_t			sKeys;
		struct stSAMPKeys	stSampKeys;
	};
	float					fPosition[3];
};

struct stDamageData {
	uint16_t				sVehicleID_lastDamageProcessed;
	int						iBumperDamage;
	int						iDoorDamage;
	uint8_t					byteLightDamage;
	uint8_t					byteWheelDamage;
};

struct stSurfData {
	int						iIsSurfing;
	float					fSurfPosition[3];
	int						iUnk0;
	uint16_t				sSurfingVehicleID;
	uint32_t				ulSurfTick;
	struct stSAMPVehicle	*pSurfingVehicle;
	int						iUnk1;
	int						iSurfMode;	//0 = not surfing, 1 = moving (unstable surf), 2 = fixed on vehicle
};

struct stUnoccupiedData {
	int16_t					sVehicleID;
	uint8_t					byteSeatID;
	float					fRoll[3];
	float					fDirection[3];
	float					fPosition[3];
	float					fMoveSpeed[3];
	float					fTurnSpeed[3];
	float					fHealth;
};

struct stBulletData {
	uint8_t					byteType;
	uint16_t				sTargetID;
	float					fOrigin[3];
	float					fTarget[3];
	float					fCenter[3];
	uint8_t					byteWeaponID;
};

struct stSpectatorData {
	uint16_t				sLeftRightKeys;
	uint16_t				sUpDownKeys;
	union {
		uint16_t			sKeys;
		struct stSAMPKeys	stSampKeys;
	};
	float					fPosition[3];
};

struct stStatsData {
	int						iMoney;
	int						iAmmo;
};

struct stHeadSync {
	float					fHeadSync[3];
	int						iHeadSyncUpdateTick;
	int						iHeadSyncLookTick;
};

struct CAMERA_TARGET_AIM {
	uint16_t				ObjectID;
	uint16_t				VehicleID;
	uint16_t				PlayerID;
	uint16_t				ActorID;
};

struct PLAYER_SPAWN_INFO {
	BYTE					byteTeam;
	int						iSkin;
	int						iCustomModel;
	BYTE					unk;
	CVector					vecPos;
	float					fRotation;
	int						iSpawnWeapons[3];
	int						iSpawnWeaponsAmmo[3];
};

struct stLocalPlayer {
	struct stSAMPPed		*pSAMP_Actor;
	struct stTrailerData	trailerData;
	struct stOnFootData		onFootData;
	struct stPassengerData	passengerData;
	struct stInCarData		inCarData;
	struct stAimData		aimData;
	int						iIsActive;
	int						iIsWasted;
	uint16_t				sCurrentVehicleID;
	uint16_t				sLastVehicleID;
	uint16_t				sCurrentAnimID;
	uint16_t				sAnimFlags;
	uint32_t				ulUnk0;
	CAMERA_TARGET_AIM		cameraTarget;
	uint32_t				ulCameraTargetTick;
	struct stHeadSync		headSyncData;
	uint32_t				ulHeadSyncTick;
	int						iIsSpectating;
	uint8_t					byteTeamID2;
	uint16_t				usUnk2;
	uint32_t				ulSendTick;
	uint32_t				ulSpectateTick;
	uint32_t				ulAimTick;
	uint32_t				ulStatsUpdateTick;
	int						iSpawnClassLoaded;
	uint32_t				ulSpawnSelectionTick;
	uint32_t				ulSpawnSelectionStart;
	PLAYER_SPAWN_INFO		spawnInfo;
	int						iIsActorAlive;
	uint32_t				ulWeapUpdateTick;
	uint16_t				sAimingAtPlayerID;
	uint16_t				sAimingAtActorID;
	uint8_t					byteCurrentWeapon;
	uint8_t					byteWeaponInventory[13];
	int						iWeaponAmmo[13];
	int						iPassengerDriveBy;
	uint8_t					byteCurrentInterior;
	int						iIsInRCVehicle;
	char					szPlayerName[256];
	struct stSurfData		surfData;
	int						iClassSelectionOnDeath;
	int						iSpawnClassID;
	int						iRequestToSpawn;
	int						iIsInSpawnScreen;
	uint32_t				ulDisplayZoneTick;
	uint8_t					byteSpectateMode;		// 3 = vehicle, 4 = player, side = 14, fixed = 15
	uint8_t					byteSpectateType;		// 0 = none, 1 = player, 2 = vehicle
	int						iSpectateID;
	int						iInitiatedSpectating;
	struct stDamageData		vehicleDamageData;
};

struct stRemotePlayerData {
	uint16_t				sPlayerID;
	uint16_t				sVehicleID;
	struct stSAMPPed		*pSAMP_Actor;
	struct stSAMPVehicle	*pSAMP_Vehicle;
	int						iHasJetPack;
	int						iShowNameTag;
	BOOL					bUsingJetPack;
	uint8_t					byteSpecialAction;
	uint8_t					byteTeamID;
	uint8_t					bytePlayerState;
	uint8_t					byteSeatID;
	BOOL					bIsNpc;
	int						iPassengerDriveBy;
	struct stPassengerData	passengerData;
	struct stOnFootData		onFootData;
	struct stInCarData		inCarData;
	struct stTrailerData	trailerData;
	struct stAimData		aimData;
	uint32_t				ulUnk0[3];
	float					fOnFootPos[3];
	float					fOnFootMoveSpeed[3];
	float					fVehiclePosition[3];
	float					fVehicleMoveSpeed[3];
	void					*pUnk0;
	uint8_t					byteUnk1[60];
	CVector					vecVehiclePosOffset;
	float					fVehicleRoll[4];
	float					fActorArmor;
	float					fActorHealth;
	uint32_t				ulUnk1[3];
	int						iLastAnimationID;
	uint8_t					byteUpdateFromNetwork;
	uint32_t				dwTick;
	uint32_t				dwLastStreamedInTick;	// is 0 when currently streamed in
	uint32_t				ulUnk2;
	int						iAFKState;
	struct stHeadSync		headSyncData;
	int						iGlobalMarkerLoaded;
	int						iGlobalMarkerLocation[3];
	uint32_t				ulGlobalMarker_GTAID;
};

struct stRemotePlayer {
	int						iScore;
	int						iIsNPC;
	stRemotePlayerData		*pPlayerData;
	int						iPing;
	void					*pVTBL_txtHandler;
	std::string				strPlayerName;
};

template <typename T>
struct stSAMPEntity {
	void					*pVTBL;
	uint8_t					byteUnk0[60]; // game CEntity object maybe. always empty.
	T						*pGTAEntity;
	uint32_t				ulGTAEntityHandle;
};

struct stSAMPPed : public stSAMPEntity < CPed > {
	int						usingCellPhone;
	uint8_t					byteUnk0[600]; // attached objects info - (state + info + ptr) * 10
	CPed					*pGTA_Ped;
	uint8_t					byteUnk1[22];
	uint8_t					byteKeysId;
	uint16_t				ulGTA_UrinateParticle_ID;
	int						DrinkingOrSmoking;
	int						object_in_hand;
	int						drunkLevel;
	uint8_t					byteUnk2[5];
	int						isDancing;
	int						danceStyle;
	int						danceMove;
	uint8_t					byteUnk3[20];
	int						isUrinating;
};

struct stVehiclePool {
	int						iVehicleCount;
	void					*pUnk0;
	uint8_t					byteSpace1[0x112C];
	struct stSAMPVehicle	*pSAMP_Vehicle[SAMP_MAX_VEHICLES];
	int						iIsListed[SAMP_MAX_VEHICLES];
	CVehicle				*pGTA_Vehicle[SAMP_MAX_VEHICLES];
	uint8_t					byteSpace2[SAMP_MAX_VEHICLES * 6];
	uint32_t				ulShit[SAMP_MAX_VEHICLES];
	int						iIsListed2[SAMP_MAX_VEHICLES];
	uint32_t				byteSpace3[SAMP_MAX_VEHICLES * 2];
	float					fSpawnPos[SAMP_MAX_VEHICLES][3];
	int						iInitiated;
};

struct stSAMPVehicle : public stSAMPEntity < CVehicle > {
	uint32_t				bUnk0;
	CVehicle				*pGTA_Vehicle;
	uint8_t					byteUnk1[8];
	int						bIsMotorOn;
	int						iIsLightsOn;
	int						iIsLocked;
	uint8_t					byteIsObjective;
	int						iObjectiveBlipCreated;
	uint8_t					byteUnk2[16];
	uint8_t					byteColor[2];
	int						iColorSync;
	int						iColor_something;
};

struct stObject : public stSAMPEntity < CObject > {
	uint8_t					byteUnk0[2];
	uint32_t				ulUnk1;
	int						iModel;
	uint16_t				byteUnk2;
	float					fDrawDistance;
	float					fUnk;
	float					fPos[3];
	uint8_t					byteUnk3[68];
	uint8_t					byteUnk4;
	float					fRot[3];
};

struct stObjectPool {
	int						iObjectCount;
	int						iIsListed[SAMP_MAX_OBJECTS];
	struct stObject			*object[SAMP_MAX_OBJECTS];
};

struct stGangzone {
	float					fPosition[4];
	DWORD					dwColor;
	DWORD					dwAltColor;
};

struct stGangzonePool {
	struct stGangzone		*pGangzone[SAMP_MAX_GANGZONES];
	int						iIsListed[SAMP_MAX_GANGZONES];
};

struct stTextLabel {
	char					*pText;
	DWORD					color;
	float					fPosition[3];
	float					fMaxViewDistance;
	uint8_t					byteShowBehindWalls;
	uint16_t				sAttachedToPlayerID;
	uint16_t				sAttachedToVehicleID;
};

struct stTextLabelPool {
	struct stTextLabel		textLabel[SAMP_MAX_3DTEXTS];
	int						iIsListed[SAMP_MAX_3DTEXTS];
};

struct stChatEntry {
	uint32_t				SystemTime;
	char					szPrefix[28];
	char					szText[144];
	uint8_t					unknown[64];
	int						iType;			// 2 - text + prefix, 4 - text (server msg), 8 - text (debug)
	D3DCOLOR				clTextColor;
	D3DCOLOR				clPrefixColor;	// or textOnly colour
};

struct stFontRenderer {
	ID3DXFont				*m_pChatFont;
	ID3DXFont				*m_pLittleFont;
	ID3DXFont				*m_pChatShadowFont;
	ID3DXFont				*m_pLittleShadowFont;
	ID3DXFont				*m_pCarNumberFont;
	ID3DXSprite				*m_pTempSprite;
	IDirect3DDevice9		*m_pD3DDevice;
	char					*m_pszTextBuffer;
};

struct stChatInfo {
	int						pagesize;
	char					*pLastMsgText;
	int						iChatWindowMode;
	uint8_t					bTimestamps;
	uint32_t				m_iLogFileExist;
	char					logFilePathChatLog[MAX_PATH + 1];
	void					*pGameUI; // CDXUTDialog
	void					*pEditBackground; // CDXUTEditBox
	void					*pDXUTScrollBar;
	D3DCOLOR				clTextColor;
	D3DCOLOR				clInfoColor;
	D3DCOLOR				clDebugColor;
	DWORD					m_lChatWindowBottom;
	struct stChatEntry		chatEntry[100];
	stFontRenderer			*m_pFontRenderer;
	ID3DXSprite				*m_pChatTextSprite;
	ID3DXSprite				*m_pSprite;
	IDirect3DDevice9		*m_pD3DDevice;
	int						m_iRenderMode; // 0 - Direct Mode (slow), 1 - Normal mode
	ID3DXRenderToSurface	*pID3DXRenderToSurface;
	IDirect3DTexture9		*m_pTexture;
	IDirect3DSurface9		*pSurface;
	D3DDISPLAYMODE			*pD3DDisplayMode;
	int						iUnk1[3];
	int						iUnk2; // smth related to drawing in direct mode
	int						m_iRedraw;
	int						m_nPrevScrollBarPosition;
	int						m_iFontSizeY;
	int						m_iTimestampWidth;
};

struct stInputBox {
	void					*pUnknown;
	uint8_t					bIsChatboxOpen;
	uint8_t					bIsMouseInChatbox;
	uint8_t					bMouseClick_related;
	uint8_t					unk;
	DWORD					dwPosChatInput[2];
	uint8_t					unk2[263];
	int						iCursorPosition;
	uint8_t					unk3;
	int						iMarkedText_startPos; // Highlighted text between this and iCursorPosition
	uint8_t					unk4[20];
	int						iMouseLeftButton;
};

typedef void(__cdecl *CMDPROC) (PCHAR);
struct stInputInfo {
	void					*pD3DDevice;
	void					*pDXUTDialog;
	stInputBox				*pDXUTEditBox;
	CMDPROC					pCMDs[SAMP_MAX_CLIENTCMDS];
	char					szCMDNames[SAMP_MAX_CLIENTCMDS][33];
	int						iCMDCount;
	int						iInputEnabled;
	char					szInputBuffer[129];
	char					szRecallBufffer[10][129];
	char					szCurrentBuffer[129];
	int						iCurrentRecall;
	int						iTotalRecalls;
	CMDPROC					pszDefaultCMD;
};

struct stKillEntry {
	char					szKiller[25];
	char					szVictim[25];
	D3DCOLOR				clKillerColor;
	D3DCOLOR				clVictimColor;
	uint8_t					byteType;
};

struct stKillInfo {
	int						iEnabled;
	struct stKillEntry		killEntry[5];
	int						iLongestNickLength;
	int						iOffsetX;
	int						iOffsetY;
	ID3DXFont				*pD3DFont;
	ID3DXFont				*pWeaponFont1;
	ID3DXFont				*pWeaponFont2;
	ID3DXSprite				*pSprite;
	IDirect3DDevice9		*pD3DDevice;
	int						iAuxFontInited;
	ID3DXFont				*pAuxFont1;
	ID3DXFont				*pAuxFont2;
};

struct stChatPlayer {
	int						iCreated;
	char					probablyTheText[256];
	DWORD					dwTickCreated;
	DWORD					dwLiveLength;
	DWORD					dwColor;
	float					fDrawDistance;
	DWORD					dwUnknown;
};

struct stAudio {
	int						iSoundState; // 0 - Finished, 1 - Loaded, 2 - Playing
	bool					bStopInteriourAmbientSounds;
};

struct stGameInfo {
	struct stCamera {
		void*				pEntity; // attached entity
		class CMatrix_Padded*	matrix;
	};
	stAudio*				pAudio;
	stCamera*				pCamera;
	stSAMPPed*				pLocalPlayerPed;
	float					fRaceCheckpointPos[3];
	float					fRaceCheckpointNext[3];
	float					m_fRaceCheckpointSize;
	uint8_t					byteRaceType;
	int						bRaceCheckpointsEnabled;
	DWORD					dwRaceCheckpointMarker;
	DWORD					dwRaceCheckpointHandle;
	float					fCheckpointPos[3];
	float					fCheckpointExtent[3];
	int						bCheckpointsEnabled;
	DWORD					dwCheckpointMarker;
	uint32_t				ulUnk2;
	int						bHeadMove;
	uint32_t				ulFpsLimit;
	int						iCursorMode;
	uint32_t				ulUnk1;
	int						bClockEnabled;
	uint8_t					byteUnk3;
	uint8_t					byteVehicleModels[212];
};

struct stScoreboardInfo {
	int						iIsEnabled;
	int						iPlayersCount;
	float					fTextOffset[2];
	float					fScalar;
	float					fSize[2];
	float					fUnk0[5];
	IDirect3DDevice9		*pDirectDevice;
	class _CDXUTDialog		*pDialog;
	class _CDXUTListBox		*pList;
	int						iOffset;
	int						iIsSorted;
};

struct stActorPool {
	int						iLastActorID;
	stSAMPEntity<void>		*pActor[SAMP_MAX_ACTORS];
	int						iIsListed[SAMP_MAX_ACTORS];
	CPed					*pGTAPed[SAMP_MAX_ACTORS];
	uint32_t				ulUnk0[SAMP_MAX_ACTORS];
	uint32_t				ulUnk1[SAMP_MAX_ACTORS];
};

struct stChatBubbleInfo {
	struct stChatPlayer		chatBubble[SAMP_MAX_PLAYERS];
};

struct stStreamedOutPlayerInfo {
	int						iPlayerID[SAMP_MAX_PLAYERS];
	float					fPlayerPos[SAMP_MAX_PLAYERS][3];
};
#pragma pack(pop)

#endif
