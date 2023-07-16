/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CHttpClient.h"

#if defined(SAMP_R1)

SAMP::CHttpClient::CHttpClient() {
	((void(__thiscall*)(CHttpClient*))SAMP_ADDROF(0x22D0))(this);
}

SAMP::CHttpClient::~CHttpClient() {
	((void(__thiscall*)(CHttpClient*))SAMP_ADDROF(0x2330))(this);
}

bool SAMP::CHttpClient::GetHeaderValue(const char* szHeaderName, char* szBuffer, int nBufferLen) {
	return ((bool(__thiscall*)(CHttpClient*, const char*, char*, int))SAMP_ADDROF(0x23A0))(this, szHeaderName, szBuffer, nBufferLen);
}

void SAMP::CHttpClient::InitializeRequest(int nType, const char* szUrl, const char* szPostData, const char* szReferer) {
	((void(__thiscall*)(CHttpClient*, int, const char*, const char*, const char*))SAMP_ADDROF(0x24A0))(this, nType, szUrl, szPostData, szReferer);
}

void SAMP::CHttpClient::HandleEntity() {
	((void(__thiscall*)(CHttpClient*))SAMP_ADDROF(0x2670))(this);
}

bool SAMP::CHttpClient::Connect(const char* szHost, int nPort) {
	return ((bool(__thiscall*)(CHttpClient*, const char*, int))SAMP_ADDROF(0x2990))(this, szHost, nPort);
}

void SAMP::CHttpClient::Process() {
	((void(__thiscall*)(CHttpClient*))SAMP_ADDROF(0x2A50))(this);
}

void SAMP::CHttpClient::Disconnect() {
	((void(__thiscall*)(CHttpClient*))SAMP_ADDROF(0x2430))(this);
}

SAMP::CHttpClient::Error SAMP::CHttpClient::ProcessUrl(int nType, const char* szUrl, const char* szPostData, const char* szReferer) {
	return ((Error(__thiscall*)(CHttpClient*, int, const char*, const char*, const char*))SAMP_ADDROF(0x2C30))(this, nType, szUrl, szPostData, szReferer);
}

bool SAMP::CHttpClient::Send(const char* szBuffer) {
	return ((bool(__thiscall*)(CHttpClient*, const char*))SAMP_ADDROF(0x2440))(this, szBuffer);
}

int SAMP::CHttpClient::Receive(char* szBuffer, int nBufferLen) {
	return ((int(__thiscall*)(CHttpClient*, char*, int))SAMP_ADDROF(0x2480))(this, szBuffer, nBufferLen);
}

#elif defined(SAMP_R3)

SAMP::CHttpClient::CHttpClient() {
	((void(__thiscall*)(CHttpClient*))SAMP_ADDROF(0x22C0))(this);
}

SAMP::CHttpClient::~CHttpClient() {
	((void(__thiscall*)(CHttpClient*))SAMP_ADDROF(0x2320))(this);
}

bool SAMP::CHttpClient::GetHeaderValue(const char* szHeaderName, char* szBuffer, int nBufferLen) {
	return ((bool(__thiscall*)(CHttpClient*, const char*, char*, int))SAMP_ADDROF(0x2390))(this, szHeaderName, szBuffer, nBufferLen);
}

void SAMP::CHttpClient::InitializeRequest(int nType, const char* szUrl, const char* szPostData, const char* szReferer) {
	((void(__thiscall*)(CHttpClient*, int, const char*, const char*, const char*))SAMP_ADDROF(0x2490))(this, nType, szUrl, szPostData, szReferer);
}

void SAMP::CHttpClient::HandleEntity() {
	((void(__thiscall*)(CHttpClient*))SAMP_ADDROF(0x2660))(this);
}

bool SAMP::CHttpClient::Connect(const char* szHost, int nPort) {
	return ((bool(__thiscall*)(CHttpClient*, const char*, int))SAMP_ADDROF(0x2980))(this, szHost, nPort);
}

void SAMP::CHttpClient::Process() {
	((void(__thiscall*)(CHttpClient*))SAMP_ADDROF(0x2A40))(this);
}

void SAMP::CHttpClient::Disconnect() {
	((void(__thiscall*)(CHttpClient*))SAMP_ADDROF(0x2420))(this);
}

SAMP::CHttpClient::Error SAMP::CHttpClient::ProcessUrl(int nType, const char* szUrl, const char* szPostData, const char* szReferer) {
	return ((Error(__thiscall*)(CHttpClient*, int, const char*, const char*, const char*))SAMP_ADDROF(0))(this, nType, szUrl, szPostData, szReferer);
}

bool SAMP::CHttpClient::Send(const char* szBuffer) {
	return ((bool(__thiscall*)(CHttpClient*, const char*))SAMP_ADDROF(0x2430))(this, szBuffer);
}

int SAMP::CHttpClient::Receive(char* szBuffer, int nBufferLen) {
	return ((int(__thiscall*)(CHttpClient*, char*, int))SAMP_ADDROF(0x2470))(this, szBuffer, nBufferLen);
}

#endif
