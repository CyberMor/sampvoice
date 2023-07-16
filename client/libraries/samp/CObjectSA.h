/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#include "CEntitySA.h"
#include "CVehicleSA.h"
#include "CVectorSA.h"
#include "CMatrixSA.h"

class CSprite2d;
struct RwTexture;

SAMP_BEGIN

class SAMP_API CObjectSA : public CEntitySA {
public:

	// void **m_lpVtbl = 0xDA84C;

	char pad_0[6];
	int							m_nModel;
	char pad_1;
	bool						m_bDontCollideWithCamera;
	float						m_fDrawDistance;
	float field_0;
	CVectorSA					m_position;
	float						m_fDistanceToCamera;
	bool						m_bDrawLast;
	char pad_2[64];
	CVectorSA					m_rotation;
	char pad_3[5];
	ID							m_nAttachedToVehicle;
	ID							m_nAttachedToObject;
	CVectorSA					m_attachOffset;
	CVectorSA					m_attachRotation;
	char field_1;
	CMatrixSA					m_targetMatrix;
	char pad_4[148];
	char						m_bMoving;
	float						m_fSpeed;
	char pad_5[99];
	
	struct SAMP_API {

		union {

			::CSprite2d*		m_pSprite[16];
			::RwTexture*		m_pTextBackground[16];

		};
		
		D3DCOLOR				m_color[16];
		char pad_6[68];
		int						m_nType[16];
		
		struct SAMP_API {

			BOOL				m_bTextureWasCreated[16];

			struct SAMP_API {

				char			m_nMaterialIndex;
				char pad_0[137];
				char			m_nMaterialSize;
				char			m_szFont[65];
				char			m_nFontSize;
				bool			m_bBold;
				D3DCOLOR		m_fontColor;
				D3DCOLOR		m_backgroundColor;
				char			m_align;

			}					m_textInfo[16];

			char*				m_szData[16];
			IDirect3DTexture9*	m_pBackgroundTexture[16];
			IDirect3DTexture9*	m_pTexture[16];

		}						m_text;

	}							m_material;

	BOOL						m_bHasCustomMaterial;
	char pad_9[10];

	enum Material { None = 0, Texture = 1, Text = 2 };
	
	enum MaterialSize {

		x32y32 = 10,
		x64y32 = 20,
		x64y64 = 30,
		x128y32 = 40,
		x128y64 = 50,
		x128y123 = 60,
		x256y32 = 70,
		x256y64 = 80,
		x256y128 = 90,
		x256y256 = 100,
		x512y64 = 110,
		x512y128 = 120,
		x512y256 = 130,
		x512y512 = 140

	};

	CObjectSA() { *(void**)this = (void*)SAMP_ADDROF(0xDA84C); }
	CObjectSA(int nModel, CVectorSA position, CVectorSA rotation, float fDrawDistance, int arg5);

	virtual ~CObjectSA() SAMP_VIRTUAL;
	virtual void Add() SAMP_VIRTUAL;
	virtual void Remove() SAMP_VIRTUAL;

	float GetDistance(const CMatrixSA* pMatrix);
	void Stop();
	void SetRotation(const CVectorSA* pVector);
	void SetAttachedToVehicle(ID nId, const CVectorSA* pOffset, const CVectorSA* pRotation);
	void SetAttachedToObject(ID nId, const CVectorSA* pOffset, const CVectorSA* pRotation, char a5);
	void AttachToVehicle(CVehicleSA* pVehicle);
	void AttachToObject(CObjectSA* pObject);
	void Rotate(CVectorSA vector);
	BOOL AttachedToMovingEntity();
	void SetMaterial(int nModel, int nIndex, const char* szTxd, const char* szTexture, D3DCOLOR color);
	void SetMaterialText(int nIndex, const char* szText, char nMaterialSize, const char* szFont, char nFontSize, bool bBold, D3DCOLOR fontColor, D3DCOLOR backgroundColor, char align);
	bool GetMaterialSize(int nSize, int* x, int* y);
	void Render();
	void Process(float fElapsedTime);
	void ConstructMaterialText();
	void Draw();
	void ShutdownMaterialText();

};

SAMP_END

#elif defined(SAMP_R3)

#include "CVectorSA.h"
#include "CMatrixSA.h"
#include "CEntitySA.h"

class CSprite2d;
struct RwTexture;

SAMP_BEGIN

class CVehicleSA;

class SAMP_API CObjectSA : public CEntitySA {
public:

	// void **m_lpVtbl = 0xECD1C;

	char pad_0[6];
	int							m_nModel;
	char pad_1;
	bool						m_bDontCollideWithCamera;
	float						m_fDrawDistance;
	float field_0;
	CVectorSA					m_position;
	float						m_fDistanceToCamera;
	bool						m_bDrawLast;
	char pad_2[64];
	CVectorSA					m_rotation;
	char pad_3[5];
	ID							m_nAttachedToVehicle;
	ID							m_nAttachedToObject;
	CVectorSA					m_attachOffset;
	CVectorSA					m_attachRotation;
	char field_1;
	CMatrixSA					m_targetMatrix;
	char pad_4[148];
	char						m_bMoving;
	float						m_fSpeed;
	char pad_5[99];

	struct SAMP_API {

		union {

			::CSprite2d*		m_pSprite[16];
			::RwTexture*		m_pTextBackground[16];

		};

		D3DCOLOR				m_color[16];
		char pad_6[68];
		int						m_nType[16];

		struct SAMP_API {

			BOOL				m_bTextureWasCreated[16];

			struct SAMP_API {

				char			m_nMaterialIndex;
				char pad_0[137];
				char			m_nMaterialSize;
				char			m_szFont[65];
				char			m_nFontSize;
				bool			m_bBold;
				D3DCOLOR		m_fontColor;
				D3DCOLOR		m_backgroundColor;
				char			m_align;

			}					m_textInfo[16];

			char*				m_szData[16];
			IDirect3DTexture9*	m_pBackgroundTexture[16];
			IDirect3DTexture9*	m_pTexture[16];

		}						m_text;

	}							m_material;

	BOOL						m_bHasCustomMaterial;
	char pad_9[13];

	enum Material { None = 0, Texture = 1, Text = 2 };

	enum MaterialSize {

		x32y32 = 10,
		x64y32 = 20,
		x64y64 = 30,
		x128y32 = 40,
		x128y64 = 50,
		x128y123 = 60,
		x256y32 = 70,
		x256y64 = 80,
		x256y128 = 90,
		x256y256 = 100,
		x512y64 = 110,
		x512y128 = 120,
		x512y256 = 130,
		x512y512 = 140

	};

	virtual ~CObjectSA() SAMP_VIRTUAL;
	virtual void Add() SAMP_VIRTUAL;
	virtual void Remove() SAMP_VIRTUAL;

	CObjectSA() { *(void**)this = (void*)SAMP_ADDROF(0xECD1C); }
	CObjectSA(int nModel, CVectorSA position, CVectorSA rotation, float fDrawDistance, int a10, char a11, char a12);

	float GetDistance(const CMatrixSA* pMatrix);
	void Stop();
	void SetRotation(const CVectorSA* pVector);
	void SetAttachedToVehicle(ID nId, const CVectorSA* pOffset, const CVectorSA* pRotation);
	void SetAttachedToObject(ID nId, const CVectorSA* pOffset, const CVectorSA* pRotation, char a5);
	void AttachToVehicle(CVehicleSA* pVehicle);
	void AttachToObject(CObjectSA* pObject);
	void Rotate(CVectorSA vector);
	BOOL AttachedToMovingEntity();
	void SetMaterial(int nModel, int nIndex, const char* szTxd, const char* szTexture, D3DCOLOR color);
	void SetMaterialText(int nIndex, const char* szText, char nMaterialSize, const char* szFont, char nFontSize, bool bBold, D3DCOLOR fontColor, D3DCOLOR backgroundColor, char align);
	bool GetMaterialSize(int nSize, int* x, int* y);
	void Render();
	void Process(float fElapsedTime);
	void ConstructMaterialText();
	void Draw();
	void ShutdownMaterialText();

};

SAMP_END

#endif
