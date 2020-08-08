/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#include "Animation.h"
#include "ControllerState.h"
#include "CVectorSA.h"

#define SAMP_COMPRESS_ASPECT_RATIO(_VALUE) (_VALUE * 255)
#define SAMP_COMPRESS_EXT_ZOOM(_VALUE) (_VALUE * 63)

#define SAMP_DECOMPRESS_ASPECT_RATIO(_VALUE) (_VALUE / 255)
#define SAMP_DECOMPRESS_EXT_ZOOM(_VALUE) (_VALUE / 63)

#define SAMP_DEFAULT_PACKET_CTOR(_TYPE) inline _TYPE() { for (int i = 0; i < sizeof(*this); i++) ((unsigned char *)this)[i] = 0; }
#define SAMP_PACKET_ID(_ID) static const unsigned char ID = _ID;

SAMP_BEGIN

namespace PACKET {

	struct SAMP_API OnfootData {

		ControllerState		m_controllerState;
		CVectorSA			m_position;
		float				m_fQuaternion[4];
		unsigned char		m_nHealth;
		unsigned char		m_nArmor;
		unsigned char		m_nCurrentWeapon;
		unsigned char		m_nSpecialAction;
		CVectorSA			m_speed;
		CVectorSA			m_surfingOffset;
		ID					m_nSurfingVehicleId;
		Animation			m_animation;
		
		SAMP_PACKET_ID(207);

		SAMP_DEFAULT_PACKET_CTOR(OnfootData);
	};

	struct SAMP_API IncarData {

		ID					m_nVehicle;
		ControllerState		m_controllerState;
		float				m_fQuaternion[4];
		CVectorSA			m_position;
		CVectorSA			m_speed;
		float				m_fHealth;
		unsigned char		m_nDriverHealth;
		unsigned char		m_nDriverArmor;
		unsigned char		m_nCurrentWeapon;
		bool				m_bSirenEnabled;
		bool				m_bLandingGear;
		ID					m_nTrailerId;

		union {

			unsigned short	m_aHydraThrustAngle[2];
			float			m_fTrainSpeed;

		};

		SAMP_PACKET_ID(200);

		SAMP_DEFAULT_PACKET_CTOR(IncarData);
	};

	struct SAMP_API AimData {

		unsigned char		m_nCameraMode;
		CVectorSA			m_aimf1;
		CVectorSA			m_aimPos;
		float				m_fAimZ;
		unsigned char		m_nCameraExtZoom : 6;
		unsigned char		m_nWeaponState : 2;
		char				m_nAspectRatio;
		
		enum WeaponState { None, LastBullet, MoreBullets, Reloading };

		SAMP_PACKET_ID(203);

		SAMP_DEFAULT_PACKET_CTOR(AimData);
	};

	struct SAMP_API TrailerData {

		ID					m_nId;
		CVectorSA			m_position;
		float				m_fQuaternion[4];
		CVectorSA			m_speed;
		CVectorSA			m_turnSpeed;

		SAMP_PACKET_ID(210);

		SAMP_DEFAULT_PACKET_CTOR(TrailerData);
	};

	struct SAMP_API PassengerData {

		ID					m_nVehicleId;
		unsigned char		m_nSeatId; // flags
		unsigned char		m_nCurrentWeapon;
		unsigned char		m_nHealth;
		unsigned char		m_nArmor;
		ControllerState		m_controllerState;
		CVectorSA			m_position;

		SAMP_PACKET_ID(211);

		SAMP_DEFAULT_PACKET_CTOR(PassengerData);
	};

	struct SAMP_API UnoccupiedData {

		ID					m_nVehicleId;
		unsigned char		m_nSeatId;
		CVectorSA			m_roll;
		CVectorSA			m_direction;
		CVectorSA			m_position;
		CVectorSA			m_speed;
		CVectorSA			m_turnSpeed;
		float				m_fHealth;
		
		SAMP_PACKET_ID(209);

		SAMP_DEFAULT_PACKET_CTOR(UnoccupiedData);
	};

	struct SAMP_API BulletData {

		unsigned char		m_nTargetType;
		ID					m_nTargetId;
		CVectorSA			m_origin;
		CVectorSA			m_target;
		CVectorSA			m_center;
		unsigned char		m_nWeapon;

		SAMP_PACKET_ID(206);

		SAMP_DEFAULT_PACKET_CTOR(BulletData);
	};

	struct SAMP_API SpectatorData {

		ControllerState		m_controllerState;
		CVectorSA			m_position;

		SAMP_PACKET_ID(212);

		SAMP_DEFAULT_PACKET_CTOR(SpectatorData);
	};

	struct SAMP_API StatsData {

		int					m_nMoney;
		int					m_nDrunkLevel;

		SAMP_PACKET_ID(205);

		SAMP_DEFAULT_PACKET_CTOR(StatsData);
	};

	/*
		a structure in this block has dynamic size
		
		struct SAMP_API WeaponsData {
			ID m_nAimedPlayer;
			ID m_nAimedActor;
			struct {
				char m_nSlot;
				char m_nWeapon;
				unsigned short m_nAmmo;
			} m_aWeapons[n]; // 0 < n < 14
			SAMP_PACKET_ID(204)
			SAMP_DEFAULT_PACKET_CTOR(WeaponsData)
		};
		struct SAMP_API RconCommand {
			unsigned long m_nTextLen;
			char m_szText[m_nTextLen];
			SAMP_PACKET_ID(201)
			SAMP_DEFAULT_PACKET_CTOR(RconCommand)
		};
		struct SAMP_API MarkersData {
			bool m_bCreate; // create(1)/remove(0)
			int m_nCount;
			struct {
				ID m_nPlayer;
				VectorCompressed m_vPos;
			} m_aMarkers[m_nCount];
			SAMP_PACKET_ID(208)
			SAMP_DEFAULT_PACKET_CTOR(MarkersData)
		};
	*/

#if defined(__RAK_CLIENT_H)
	template<class T>
	void Send(T packet) {

		RakNet::BitStream bs;

		bs.Write(T::ID);
		bs.Write(packet);

		pNetGame->m_pRakClient->Send(&bs, HIGH_PRIORITY, RELIABLE, 0);

	}
#endif

}

SAMP_END

#elif defined(SAMP_R3)

#include "CVectorSA.h"
#include "Animation.h"
#include "ControllerState.h"

#define SAMP_COMPRESS_ASPECT_RATIO(_VALUE) (_VALUE * 255)
#define SAMP_COMPRESS_EXT_ZOOM(_VALUE) (_VALUE * 63)

#define SAMP_DECOMPRESS_ASPECT_RATIO(_VALUE) (_VALUE / 255)
#define SAMP_DECOMPRESS_EXT_ZOOM(_VALUE) (_VALUE / 63)

#define SAMP_DEFAULT_PACKET_CTOR(_TYPE) inline _TYPE() { for (int i = 0; i < sizeof(*this); i++) ((unsigned char *)this)[i] = 0; }
#define SAMP_PACKET_ID(_ID) static const unsigned char ID = _ID;

SAMP_BEGIN

namespace PACKET {

	struct SAMP_API OnfootData {

		ControllerState		m_controllerState;
		CVectorSA			m_position;
		float				m_fQuaternion[4];
		unsigned char		m_nHealth;
		unsigned char		m_nArmor;
		unsigned char		m_nCurrentWeapon;
		unsigned char		m_nSpecialAction;
		CVectorSA			m_speed;
		CVectorSA			m_surfingOffset;
		ID					m_nSurfingVehicleId;
		Animation			m_animation;

		SAMP_PACKET_ID(207);

		SAMP_DEFAULT_PACKET_CTOR(OnfootData);
	};

	struct SAMP_API IncarData {
		ID					m_nVehicle;
		ControllerState		m_controllerState;
		float				m_fQuaternion[4];
		CVectorSA			m_position;
		CVectorSA			m_speed;
		float				m_fHealth;
		unsigned char		m_nDriverHealth;
		unsigned char		m_nDriverArmor;
		unsigned char		m_nCurrentWeapon;
		bool				m_bSirenEnabled;
		bool				m_bLandingGear;
		ID					m_nTrailerId;

		union {

			unsigned short	m_aHydraThrustAngle[2];
			float			m_fTrainSpeed;

		};

		SAMP_PACKET_ID(200);

		SAMP_DEFAULT_PACKET_CTOR(IncarData);
	};

	struct SAMP_API AimData {
		unsigned char		m_nCameraMode;
		CVectorSA			m_aimf1;
		CVectorSA			m_aimPos;
		float				m_fAimZ;
		unsigned char		m_nCameraExtZoom : 6;
		unsigned char		m_nWeaponState : 2;
		char				m_nAspectRatio;

		enum WeaponState { None, LastBullet, MoreBullets, Reloading };

		SAMP_PACKET_ID(203);

		SAMP_DEFAULT_PACKET_CTOR(AimData);
	};

	struct SAMP_API TrailerData {
		ID					m_nId;
		CVectorSA			m_position;
		float				m_fQuaternion[4];
		CVectorSA			m_speed;
		CVectorSA			m_turnSpeed;

		SAMP_PACKET_ID(210);

		SAMP_DEFAULT_PACKET_CTOR(TrailerData);
	};

	struct SAMP_API PassengerData {
		ID					m_nVehicleId;
		unsigned char		m_nSeatId; // flags
		unsigned char		m_nCurrentWeapon;
		unsigned char		m_nHealth;
		unsigned char		m_nArmor;
		ControllerState		m_controllerState;
		CVectorSA			m_position;

		SAMP_PACKET_ID(211);

		SAMP_DEFAULT_PACKET_CTOR(PassengerData);
	};

	struct SAMP_API UnoccupiedData {
		ID					m_nVehicleId;
		unsigned char		m_nSeatId;
		CVectorSA			m_roll;
		CVectorSA			m_direction;
		CVectorSA			m_position;
		CVectorSA			m_speed;
		CVectorSA			m_turnSpeed;
		float				m_fHealth;

		SAMP_PACKET_ID(209);

		SAMP_DEFAULT_PACKET_CTOR(UnoccupiedData);
	};

	struct SAMP_API BulletData {
		unsigned char		m_nTargetType;
		ID					m_nTargetId;
		CVectorSA			m_origin;
		CVectorSA			m_target;
		CVectorSA			m_center;
		unsigned char		m_nWeapon;

		SAMP_PACKET_ID(206);

		SAMP_DEFAULT_PACKET_CTOR(BulletData);
	};

	struct SAMP_API SpectatorData {
		ControllerState		m_controllerState;
		CVectorSA			m_position;

		SAMP_PACKET_ID(212);

		SAMP_DEFAULT_PACKET_CTOR(SpectatorData);
	};

	struct SAMP_API StatsData {
		int					m_nMoney;
		int					m_nDrunkLevel;

		SAMP_PACKET_ID(205);

		SAMP_DEFAULT_PACKET_CTOR(StatsData);
	};

	/*
		a structure in this block has dynamic size

		struct SAMP_API WeaponsData {
			ID m_nAimedPlayer;
			ID m_nAimedActor;

			struct {
				char m_nSlot;
				char m_nWeapon;
				unsigned short m_nAmmo;
			} m_aWeapons[n]; // 0 < n < 14

			SAMP_PACKET_ID(204)
			SAMP_DEFAULT_PACKET_CTOR(WeaponsData)
		};

		struct SAMP_API RconCommand {
			unsigned long m_nTextLen;
			char m_szText[m_nTextLen];

			SAMP_PACKET_ID(201)
			SAMP_DEFAULT_PACKET_CTOR(RconCommand)
		};

		struct SAMP_API MarkersData {
			bool m_bCreate; // create(1)/remove(0)
			int m_nCount;

			struct {
				ID m_nPlayer;
				VectorCompressed m_vPos;
			} m_aMarkers[m_nCount];

			SAMP_PACKET_ID(208)
			SAMP_DEFAULT_PACKET_CTOR(MarkersData)
		};
	*/

#if defined(__RAK_CLIENT_H)

	template<class T>
	void Send(T packet) {
		RakNet::BitStream bs;

		bs.Write(T::ID);
		bs.Write(packet);

		pNetGame->m_pRakClient->Send(&bs, HIGH_PRIORITY, RELIABLE, 0);
	}

#endif

}

SAMP_END

#endif
