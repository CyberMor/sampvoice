/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#define SAMP_HTTP_USERAGENT "SAMP/0.2.0"
#define SAMP_HTTP_GET "GET %s HTTP/1.0\r\nAccept: */*\r\nUser-Agent: %s\r\nReferer: http://%s\r\nHost: %s\r\n\r\n"
#define SAMP_HTTP_HEAD "HEAD %s HTTP/1.0\r\nAccept: */*\r\nUser-Agent: %s\r\nReferer: http://%s\r\nHost: %s\r\n\r\n"
#define SAMP_HTTP_POST "POST %s HTTP/1.0\r\nAccept: */*\r\nUser-Agent: %s\r\nReferer: http://%s\r\nHost: %s\r\nContent-type: application/x-www-form-urlencoded\r\nContent-length: %u\r\n\r\n%s"

#define SAMP_HTTP_RECEIVE_BUFFER_SIZE 4096

SAMP_BEGIN

class SAMP_API CHttpClient {
public:

	struct SAMP_API Request {

		unsigned short	m_nPort;
		int				m_nType;
		char*			m_szHost;
		char*			m_szFile;
		char*			m_szData;
		char*			m_szReferer;

		// request types
		enum { Get = 1, Post, Head };

	};

	struct SAMP_API Response {

		char*			m_szHeader;
		char*			m_szResponse;
		unsigned int	m_nHeaderLen;
		unsigned int	m_nResponseLen;
		unsigned int	m_nResponseCode;
		unsigned int	m_nContentType;

		// content types
		enum { Unknown, Text, HTML };

	};

	enum Error { NoError, BadHost, NoSocket, CannotConnect, CannotWrite, TooBigContent, IncorrectResponce };

	int			m_nSocket;
	Request		m_request;
	Response	m_response;
	Error		m_error;

	CHttpClient();
	~CHttpClient();

	bool GetHeaderValue(const char* szHeaderName, char* szBuffer, int nBufferLen);
	void InitializeRequest(int nType, const char* szUrl, const char* szPostData, const char* szReferer);
	void HandleEntity();
	bool Connect(const char* szHost, int nPort);
	void Process();
	void Disconnect();
	Error ProcessUrl(int nType, const char* szUrl, const char* szPostData, const char* szReferer);
	bool Send(const char* szBuffer);
	int Receive(char* szBuffer, int nBufferLen);

};

SAMP_END

#elif defined(SAMP_R3)

#define SAMP_HTTP_USERAGENT "SAMP/0.2.0"
#define SAMP_HTTP_GET "GET %s HTTP/1.0\r\nAccept: */*\r\nUser-Agent: %s\r\nReferer: http://%s\r\nHost: %s\r\n\r\n"
#define SAMP_HTTP_HEAD "HEAD %s HTTP/1.0\r\nAccept: */*\r\nUser-Agent: %s\r\nReferer: http://%s\r\nHost: %s\r\n\r\n"
#define SAMP_HTTP_POST "POST %s HTTP/1.0\r\nAccept: */*\r\nUser-Agent: %s\r\nReferer: http://%s\r\nHost: %s\r\nContent-type: application/x-www-form-urlencoded\r\nContent-length: %u\r\n\r\n%s"

#define SAMP_HTTP_RECEIVE_BUFFER_SIZE 4096

SAMP_BEGIN

class SAMP_API CHttpClient {
public:

	struct SAMP_API Request {

		unsigned short	m_nPort;
		int				m_nType;
		char*			m_szHost;
		char*			m_szFile;
		char*			m_szData;
		char*			m_szReferer;

		// request types
		enum { Get = 1, Post, Head };
	};

	struct SAMP_API Response {
		char*			m_szHeader;
		char*			m_szResponse;
		unsigned int	m_nHeaderLen;
		unsigned int	m_nResponseLen;
		unsigned int	m_nResponseCode;
		unsigned int	m_nContentType;

		// content types
		enum { Unknown, Text, HTML };
	};

	enum Error { NoError, BadHost, NoSocket, CannotConnect, CannotWrite, TooBigContent, IncorrectResponce };

	int					m_nSocket;
	Request				m_request;
	Response			m_response;
	Error				m_error;

	CHttpClient();
	~CHttpClient();

	bool GetHeaderValue(const char* szHeaderName, char* szBuffer, int nBufferLen);
	void InitializeRequest(int nType, const char* szUrl, const char* szPostData, const char* szReferer);
	void HandleEntity();
	bool Connect(const char* szHost, int nPort);
	void Process();
	void Disconnect();
	Error ProcessUrl(int nType, const char* szUrl, const char* szPostData, const char* szReferer);
	bool Send(const char* szBuffer);
	int Receive(char* szBuffer, int nBufferLen);

};

SAMP_END

#endif
