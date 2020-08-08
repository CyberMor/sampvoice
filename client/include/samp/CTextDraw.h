/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#include "CVectorSA.h"

SAMP_BEGIN

class SAMP_API CTextDraw {
public:

	struct SAMP_API Transmit {

		union {

			struct {

				unsigned char	m_bBox : 1;
				unsigned char	m_bLeft : 1;
				unsigned char	m_bRight : 1;
				unsigned char	m_bCenter : 1;
				unsigned char	m_bProportional : 1;

			};

			unsigned char		m_nFlags;

		};

		float					m_fLetterWidth;
		float					m_fLetterHeight;
		D3DCOLOR				m_letterColor;
		float					m_fBoxWidth;
		float					m_fBoxHeight;
		D3DCOLOR				m_boxColor;
		unsigned char			m_nShadow;
		bool					m_bOutline;
		D3DCOLOR				m_backgroundColor;
		unsigned char			m_nStyle;
		unsigned char unknown;
		float					m_fX;
		float					m_fY;
		unsigned short			m_nModel;
		CVectorSA				m_rotation;
		float					m_fZoom;
		unsigned short			m_aColor[2];

	};

	struct SAMP_API Data {

		float					m_fLetterWidth;
		float					m_fLetterHeight;
		D3DCOLOR				m_letterColor;
		unsigned char unknown;
		unsigned char			m_bCenter;
		unsigned char			m_bBox;
		float					m_fBoxSizeX;
		float					m_fBoxSizeY;
		D3DCOLOR				m_boxColor;
		unsigned char			m_nProportional;
		D3DCOLOR				m_backgroundColor;
		unsigned char			m_nShadow;
		unsigned char			m_nOutline;
		unsigned char			m_bLeft;
		unsigned char			m_bRight;
		int						m_nStyle;
		float					m_fX;
		float					m_fY;
		unsigned char pad_[8];
		unsigned long field_99B;
		unsigned long field_99F;
		unsigned long			m_nIndex;
		unsigned char field_9A7;
		unsigned short			m_nModel;
		CVectorSA				m_rotation;
		float					m_fZoom;
		unsigned short			m_aColor[2];
		unsigned char field_9BE;
		unsigned char field_9BF;
		unsigned char field_9C0;
		unsigned long field_9C1;
		unsigned long field_9C5;
		unsigned long field_9C9;
		unsigned long field_9CD;
		unsigned char field_9D1;
		unsigned long field_9D2;

	};

	char						m_szText[801];
	char						m_szString[1602];
	Data						m_data;

	CTextDraw(Transmit* pTransmit, const char* szText);
	~CTextDraw();

	void SetText(const char* szText);
	void Draw();

};

SAMP_END

#elif defined(SAMP_R3)

#include "CVectorSA.h"

SAMP_BEGIN

class SAMP_API CTextDraw {
public:

	struct SAMP_API Transmit {

		union {

			struct {

				unsigned char	m_bBox : 1;
				unsigned char	m_bLeft : 1;
				unsigned char	m_bRight : 1;
				unsigned char	m_bCenter : 1;
				unsigned char	m_bProportional : 1;

			};

			unsigned char		m_nFlags;

		};

		float					m_fLetterWidth;
		float					m_fLetterHeight;
		D3DCOLOR				m_letterColor;
		float					m_fBoxWidth;
		float					m_fBoxHeight;
		D3DCOLOR				m_boxColor;
		unsigned char			m_nShadow;
		bool					m_bOutline;
		D3DCOLOR				m_backgroundColor;
		unsigned char			m_nStyle;
		unsigned char unknown;
		float					m_fX;
		float					m_fY;
		unsigned short			m_nModel;
		CVectorSA				m_rotation;
		float					m_fZoom;
		unsigned short			m_aColor[2];

	};

	struct SAMP_API Data {

		float					m_fLetterWidth;
		float					m_fLetterHeight;
		D3DCOLOR				m_letterColor;
		unsigned char unknown;
		unsigned char			m_bCenter;
		unsigned char			m_bBox;
		float					m_fBoxSizeX;
		float					m_fBoxSizeY;
		D3DCOLOR				m_boxColor;
		unsigned char			m_nProportional;
		D3DCOLOR				m_backgroundColor;
		unsigned char			m_nShadow;
		unsigned char			m_nOutline;
		unsigned char			m_bLeft;
		unsigned char			m_bRight;
		int						m_nStyle;
		float					m_fX;
		float					m_fY;
		unsigned char pad_[8];
		unsigned long field_99B;
		unsigned long field_99F;
		unsigned long			m_nIndex;
		unsigned char field_9A7;
		unsigned short			m_nModel;
		CVectorSA				m_rotation;
		float					m_fZoom;
		unsigned short			m_aColor[2];
		unsigned char field_9BE;
		unsigned char field_9BF;
		unsigned char field_9C0;
		unsigned long field_9C1;
		unsigned long field_9C5;
		unsigned long field_9C9;
		unsigned long field_9CD;
		unsigned char field_9D1;
		unsigned long field_9D2;

	};

	char						m_szText[801];
	char						m_szString[1602];
	Data						m_data;

	CTextDraw(Transmit* pData, const char* szText);
	~CTextDraw();

	void SetText(const char* szText);
	void Draw();

};

SAMP_END

#endif
