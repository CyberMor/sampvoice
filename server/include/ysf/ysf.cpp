#include "utils/cvector.h"
#include "utils/cvector2d.h"
#include "utils/memory.h"

#include "addresses.h"
#include "globals.h"

// CVector
// ------------------------------------------------------

bool CVector::IsNearZeroFloat(const float param) {
	return (fabs(param) < FLOAT_EPSILON);
}

CVector::CVector() {
	this->fX = 0;
	this->fY = 0;
	this->fZ = 0;
};

CVector::CVector(CVector const &rhs) {
	this->fX = rhs.fX;
	this->fY = rhs.fY;
	this->fZ = rhs.fZ;
};

CVector::CVector(float fX, float fY, float fZ) {
	this->fX = fX;
	this->fY = fY;
	this->fZ = fZ;
}

float CVector::Normalize(void) {
	double t = sqrt(static_cast<double>(fX * fX + fY * fY + fZ * fZ));
	if (t > FLOAT_EPSILON) {
		double fX2 = fX / t;
		double fY2 = fY / t;
		double fZ2 = fZ / t;
		fX = (float)fX2;
		fY = (float)fY2;
		fZ = (float)fZ2;
	} else t = 0;
	return static_cast<float>(t);
}

float CVector::Length(void) const {
	return sqrt((fX*fX) + (fY*fY) + (fZ*fZ));
}

float CVector::DotProduct(const CVector * param) const {
	return fX * param->fX + fY * param->fY + fZ * param->fZ;
}

void CVector::CrossProduct(const CVector * param) {
	float _fX = fX, _fY = fY, _fZ = fZ;
	fX = _fY * param->fZ - param->fY * _fZ;
	fY = _fZ * param->fX - param->fZ * _fX;
	fZ = _fX * param->fY - param->fX * _fY;
}

bool CVector::IsNearZero(void) {
	return (IsNearZeroFloat(fX) && IsNearZeroFloat(fY) && IsNearZeroFloat(fZ));
}

void CVector::Zero(void) {
	fX = 0.0f;
	fY = 0.0f;
	fZ = 0.0f;
}

void CVector::ZeroNearZero(void) {
	if (IsNearZeroFloat(fX)) fX = 0.0f;
	if (IsNearZeroFloat(fY)) fY = 0.0f;
	if (IsNearZeroFloat(fZ)) fZ = 0.0f;
}

float CVector::GetAngleRadians(void) {
	return -atan2(fY, -fX);
}

float CVector::GetAngleDegrees(void) {
	static float radtodeg = 57.324840764331210191082802547771f; // 180/pi
	float ret = (atan2(fY, -fX) * radtodeg) + 270.0f;
	if (ret >= 360.0f) ret -= 360.0f;
	return ret;
}

CVector CVector::operator + (const CVector& vecRight) const {
	return CVector(fX + vecRight.fX, fY + vecRight.fY, fZ + vecRight.fZ);
}

CVector CVector::operator - (const CVector& vecRight) const {
	return CVector(fX - vecRight.fX, fY - vecRight.fY, fZ - vecRight.fZ);
}

CVector CVector::operator * (const CVector& vecRight) const {
	return CVector(fX * vecRight.fX, fY * vecRight.fY, fZ * vecRight.fZ);
}

CVector CVector::operator * (float fRight) const {
	return CVector(fX * fRight, fY * fRight, fZ * fRight);
}

CVector CVector::operator / (const CVector& vecRight) const {
	return CVector(fX / vecRight.fX, fY / vecRight.fY, fZ / vecRight.fZ);
}

CVector CVector::operator / (float fRight) const {
	return CVector(fX / fRight, fY / fRight, fZ / fRight);
}

CVector CVector::operator - () const {
	return CVector(-fX, -fY, -fZ);
}

void CVector::operator += (float fRight) {
	fX += fRight;
	fY += fRight;
	fZ += fRight;
}

void CVector::operator += (const CVector& vecRight) {
	fX += vecRight.fX;
	fY += vecRight.fY;
	fZ += vecRight.fZ;
}

void CVector::operator -= (float fRight) {
	fX -= fRight;
	fY -= fRight;
	fZ -= fRight;
}

void CVector::operator -= (const CVector& vecRight) {
	fX -= vecRight.fX;
	fY -= vecRight.fY;
	fZ -= vecRight.fZ;
}

void CVector::operator *= (float fRight) {
	fX *= fRight;
	fY *= fRight;
	fZ *= fRight;
}

void CVector::operator *= (const CVector& vecRight) {
	fX *= vecRight.fX;
	fY *= vecRight.fY;
	fZ *= vecRight.fZ;
}

void CVector::operator /= (float fRight) {
	fX /= fRight;
	fY /= fRight;
	fZ /= fRight;
}

void CVector::operator /= (const CVector& vecRight) {
	fX /= vecRight.fX;
	fY /= vecRight.fY;
	fZ /= vecRight.fZ;
}

bool CVector::operator== (const CVector& param) const {
	return ((fabs(fX - param.fX) < FLOAT_EPSILON) &&
		(fabs(fY - param.fY) < FLOAT_EPSILON) &&
		(fabs(fZ - param.fZ) < FLOAT_EPSILON));
}

bool CVector::operator!= (const CVector& param) const {
	return ((fabs(fX - param.fX) >= FLOAT_EPSILON) ||
		(fabs(fY - param.fY) >= FLOAT_EPSILON) ||
		(fabs(fZ - param.fZ) >= FLOAT_EPSILON));
}

// CVector2D
// ------------------------------------------------------

bool CVector2D::IsNearZeroFloat(const float param) {
	return (fabs(param) < FLOAT_EPSILON);
}

CVector2D::CVector2D() {
	this->fX = 0;
	this->fY = 0;
};

CVector2D::CVector2D(float fX, float fY) {
	this->fX = fX;
	this->fY = fY;
}

float CVector2D::Length(void) const {
	return sqrt((fX*fX) + (fY*fY));
}

bool CVector2D::IsNearZero(void) {
	return (IsNearZeroFloat(fX) && IsNearZeroFloat(fY));
}

void CVector2D::Zero(void) {
	fX = 0.0f;
	fY = 0.0f;
}

void CVector2D::ZeroNearZero(void) {
	if (IsNearZeroFloat(fX)) fX = 0.0f;
	if (IsNearZeroFloat(fY)) fY = 0.0f;
}

float CVector2D::GetAngleRadians(void) {
	return -atan2(fY, -fX);
}

float CVector2D::GetAngleDegrees(void) {
	static float radtodeg = 57.324840764331210191082802547771f; // 180/pi
	float ret = (atan2(fY, -fX) * radtodeg) + 270.0f;
	if (ret >= 360.0f) ret -= 360.0f;
	return ret;
}

CVector2D CVector2D::operator + (const CVector2D& vecRight) const {
	return CVector2D(fX + vecRight.fX, fY + vecRight.fY);
}

CVector2D CVector2D::operator - (const CVector2D& vecRight) const {
	return CVector2D(fX - vecRight.fX, fY - vecRight.fY);
}

CVector2D CVector2D::operator * (const CVector2D& vecRight) const {
	return CVector2D(fX * vecRight.fX, fY * vecRight.fY);
}

CVector2D CVector2D::operator * (float fRight) const {
	return CVector2D(fX * fRight, fY * fRight);
}

CVector2D CVector2D::operator / (const CVector2D& vecRight) const {
	return CVector2D(fX / vecRight.fX, fY / vecRight.fY);
}

CVector2D CVector2D::operator / (float fRight) const {
	return CVector2D(fX / fRight, fY / fRight);
}

CVector2D CVector2D::operator - () const {
	return CVector2D(-fX, -fY);
}

void CVector2D::operator += (float fRight) {
	fX += fRight;
	fY += fRight;
}

void CVector2D::operator += (const CVector2D& vecRight) {
	fX += vecRight.fX;
	fY += vecRight.fY;
}

void CVector2D::operator -= (float fRight) {
	fX -= fRight;
	fY -= fRight;
}

void CVector2D::operator -= (const CVector2D& vecRight) {
	fX -= vecRight.fX;
	fY -= vecRight.fY;
}

void CVector2D::operator *= (float fRight) {
	fX *= fRight;
	fY *= fRight;
}

void CVector2D::operator *= (const CVector2D& vecRight) {
	fX *= vecRight.fX;
	fY *= vecRight.fY;
}

void CVector2D::operator /= (float fRight) {
	fX /= fRight;
	fY /= fRight;
}

void CVector2D::operator /= (const CVector2D& vecRight) {
	fX /= vecRight.fX;
	fY /= vecRight.fY;
}

bool CVector2D::operator== (const CVector2D& param) const {
	return ((fabs(fX - param.fX) < FLOAT_EPSILON) &&
		(fabs(fY - param.fY) < FLOAT_EPSILON));
}

bool CVector2D::operator!= (const CVector2D& param) const {
	return ((fabs(fX - param.fX) >= FLOAT_EPSILON) ||
		(fabs(fY - param.fY) >= FLOAT_EPSILON));
}

// Memory
// ------------------------------------------------------

bool Unlock(void *address, size_t len) {
#ifdef _WIN32
	unsigned long oldp;
	return !!VirtualProtect(address, len, PAGE_EXECUTE_READWRITE, &oldp);
#else
	size_t iPageSize = getpagesize(), iAddr = ((reinterpret_cast <uint32_t>(address) / iPageSize) * iPageSize);
	return !mprotect(reinterpret_cast <void*>(iAddr), len, PROT_READ | PROT_WRITE | PROT_EXEC);
#endif
}

bool memory_compare(const unsigned char *data, const unsigned char *pattern, const char *mask) {
	for (; *mask; ++mask, ++data, ++pattern)
		if (*mask == 'x' && *data != *pattern) return false;
	return (*mask) == NULL;
}

unsigned long FindPattern(const char *pattern, const char *mask) {
	unsigned long i, size, address;
#ifdef _WIN32
	MODULEINFO info = { 0 };
	address = (unsigned long)GetModuleHandle(NULL);
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &info, sizeof(MODULEINFO));
	size = (unsigned long)info.SizeOfImage;
#else
	address = 0x804b480;
	size = 0x8128B80 - address;
#endif
	for (i = 0; i < size; ++i) if (memory_compare((unsigned char*)(address + i), (unsigned char*)pattern, mask))
		return (unsigned long)(address + i);
	return 0;
}

// Addresses
// ------------------------------------------------------

void CAddress::Initialize(void *logprintf_addr) {

	unsigned long dwTemp;
#ifdef _WIN32
	dwTemp = FindPattern("\xD9\x15\x00\x00\x00\x00\xD8\x1D\x00\x00\x00\x00\xDF\xE0\xF6\xC4\x41\x75\x07", "xx????xx????xxxxxxx") + 6;
	VAR_pRestartWaitTime = *(unsigned long*)(dwTemp + 2);

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
	VAR_pPosSyncBounds[3] = FindPattern("\x00\x50\x43\x48\x00\x00\xC8\xC2\x00\x00\x00\x3F\xB2\x00\x00\x00\xAD\x00\x00\x00\xAE\x00\x00\x00maxnpc", "????xxxxxxxxxxxxxxxxxxxxxxxxxx");

	VAR_wRCONUser = 0x004E5874;
	ARRAY_ConsoleCommands = FindPattern("echo", "xxxx");

	FUNC_CConsole__AddStringVariable = FindPattern("\x53\x56\x57\x8B\x7C\x24\x18\x85\xFF", "xxxxxxxxx");
	FUNC_CConsole__GetStringVariable = FindPattern("\x8B\x44\x24\x04\x50\xE8\x00\x00\x00\x00\x85\xC0\x74\x0B", "xxxxxx????xxxx");
	FUNC_CConsole__SetStringVariable = FindPattern("\x8B\x44\x24\x04\x53\x50\xE8\xD5\xFE\xFF\xFF\x8B\xD8\x85\xDB", "xxxxxxx???xxxx");
	FUNC_CConsole__GetIntVariable = FindPattern("\x8B\x44\x24\x04\x50\xE8\x00\x00\x00\x00\x85\xC0\x74\x0D\x83\x38\x01\x75\x08", "xxxxxx????xxxxxxxxx");
	FUNC_CConsole__SetIntVariable = FindPattern("\x8B\x44\x24\x04\x50\xE8\xF6\xFD\xFF\xFF\x85\xC0\x74\xE0\x83\x38\x01", "xxxxxx????xx??xxx") + 0x20;
	FUNC_CConsole__GetBoolVariable = FindPattern("\x8B\x44\x24\x04\x50\xE8\x00\x00\x00\x00\x85\xC0\x74\x0D", "xxxxxx????xxxx") + 0x90;
	FUNC_CConsole__ModifyVariableFlags = FindPattern("\x8B\x44\x24\x04\x50\xE8\x16\xFF\xFF\xFF\x85\xC0\x74\x07", "xxxxxx????xxxx");
	FUNC_CConsole__FindVariable = FindPattern("\x8B\x84\x24\x30\x01\x00\x00\x53\x56\x57", "xxxxxxxxxx") - 0x1B;
	FUNC_CConsole__SendRules = FindPattern("\x81\xEC\x08\x04\x00\x00\x53\x55\x56\x57\x8B\xF9\x8B\x77\x04", "xx????xxxxxxxxx");
	FUNC_CConsole__Execute = FindPattern("\x55\x8B\xEC\x83\xE4\xF8\x81\xEC\x0C\x01\x00\x00", "xxxxxxxxxxxx");

	FUNC_CNetGame__SetWeather = FindPattern("\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x81\xEC\x98\x01\x00\x00\x56\x8B\xF1", "xxx????xxxxxxxxxxxxxxxxx?xxxxx"); // 00490A40
	FUNC_CNetGame__SetGravity = FUNC_CNetGame__SetWeather + 0xD0; // 0x00490B10;

	FUNC_CFilterscripts__LoadFilterscript = FindPattern("\x8B\x44\x24\x04\x81\xEC\x04\x01\x00\x00", "xxxxxxxxxx");
	FUNC_CFilterscripts__UnLoadFilterscript = FindPattern("\xCC\x51\x53\x8B\x5C\x24\x0C\x55\x56\x57\x89", "xxxxxxxxxxx") + 0x1;

	FUNC_ContainsInvalidChars = FindPattern("\x8B\x4C\x24\x04\x8A\x01\x84\xC0", "xxxxxxxx");

	FUNC_CPlayer__SpawnForWorld = FindPattern("\x56\x8B\xF1\x8B\x86\x3B\x26\x00\x00\x85\xC0\x0F\x84", "xxxxx????xxxx");
	FUNC_CVehicle__Respawn = FindPattern("\x53\x33\xC0\x56\x8B\xF1\x57\xB9\x10\x00\x00\x00\x8D\x7E\x0C", "xxxxxxxxx???xxx");
	FUNC_CPlayerPool__HandleVehicleRespawn = FindPattern("\x53\x55\x56\x8B\xF1\x8B\xAE\x68\x09", "xxxxxx?xx");
	FUNC_CObject__SpawnForPlayer = 0x00497790;

	FUNC_ProcessQueryPacket = FindPattern("\x83\xEC\x24\x53\x55\x56\x57\x8B\x7C\x24", "xxxxxxxxxx");
	FUNC_Packet_WeaponsUpdate = FindPattern("\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x0\x0\x0\x0\x50\x64\x89\x25\x0\x0\x0\x0\x81\xEC\x2C\x01\x00\x00\x55\x56", "xx????xx????xxxx????xxxx??xx");
	FUNC_Packet_StatsUpdate = FindPattern("\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x81\xEC\x20\x01\x00\x00\x56\x57", "xxx????xx????xxxxxxxxxxx???xx");
	FUNC_format_amxstring = FindPattern("\x8B\x54\x24\x08\x56\x8B\x74\x24\x08\x57\x33\xC0", "xxxxxxxxxxxx");

	ADDR_GetNetworkStats_VerbosityLevel = FindPattern("\x6A\x01\x8D\x4C\x0C", "xxxxx"); // 0x0047362A
	ADDR_GetPlayerNetworkStats_VerbosityLevel = FindPattern("\x6A\x01\x8D\x44\x04", "xxxxx"); // 0x004730B9;

	ADDR_RecordingDirectory = FindPattern("\x75\xDF\x8D\x44\x24\x18\x50\x8D\x8C\x24", "xxxxx?xxxx") + 14; // 0x00482265

	FUNC_CGameMode__OnPlayerConnect = FindPattern("\x83\xEC\x08\x56\x8B\xF1\x8A\x46\x68", "xxxxxxxxx"); // 0x0046D910
	FUNC_CGameMode__OnPlayerDisconnect = FUNC_CGameMode__OnPlayerConnect + 0x60; // 0x0046D970;
	FUNC_CGameMode__OnPlayerSpawn = FUNC_CGameMode__OnPlayerDisconnect + 0x60; // 0x0046D9D0;
	FUNC_CGameMode__OnPlayerStreamIn = FUNC_CGameMode__OnPlayerSpawn + 0xF10; // 0x0046E8E0
	FUNC_CGameMode__OnPlayerStreamOut = FUNC_CGameMode__OnPlayerStreamIn + 0x70; // 0x0046E950;
	FUNC_CGameMode__OnDialogResponse = FUNC_CGameMode__OnPlayerStreamOut + 0x230;  // 0x0046EB80;

#else

	// Thx for Mellnik
	VAR_pRestartWaitTime = NULL;
	VAR_wRCONUser = NULL;
	ARRAY_ConsoleCommands = FindPattern("echo", "xxxx");


	FUNC_CConsole__AddStringVariable = FindPattern("\x55\x89\xE5\x56\x53\x83\xEC\x00\x8B\x75\x00\x85\xF6\x74\x00\x89\x34\x24", "xxxxxxx?xx?xxx?xxx");
	FUNC_CConsole__GetStringVariable = FUNC_CConsole__AddStringVariable - 0x760;//0x80A0190;
	FUNC_CConsole__SetStringVariable = FindPattern("\x55\x89\xE5\x83\xEC\x00\x89\x75\x00\x8B\x45\x00\x89\x7D\x00\x8B\x7D\x00\x89\x5D\x00\x89\x44\x24\x00\x8B\x45\x00", "xxxxx?xx?xx?xx?xx?xx?xxx?xx?");
	FUNC_CConsole__SetIntVariable = FindPattern("\x83\x38\x00\x74\x00\xC9\xC3\x8B\x50\x00\x8B\x45\x00", "xx?x?xxxx?xx?") - 0x1C;
	FUNC_CConsole__GetIntVariable = FUNC_CConsole__SetIntVariable + 0x30; // 0x80A0070;
	FUNC_CConsole__GetBoolVariable = FUNC_CConsole__SetIntVariable - 0x30;//0x80A0010;
	FUNC_CConsole__ModifyVariableFlags = FindPattern("\x89\x04\x24\xE8\x00\x00\x00\x00\x85\xC0\x89\xC2\x74\x00\x8B\x45\x00", "xxxx????xxxxx?xx?") - 0x10;
	FUNC_CConsole__FindVariable = FindPattern("\xB9\xFF\x00\x00\x00\x89\xE5\x81\xEC\x68\x01\x00\x00", "xxxxxxxxxxxxx") - 0x1;
	FUNC_CConsole__SendRules = FindPattern("\x55\x31\xD2\x89\xE5\x57\x56\x53\x81\xEC\x4C\x04", "xxxxxxxxxxxx");
	FUNC_CConsole__Execute = FindPattern("\x55\x89\xE5\x57\x56\x53\x81\xEC\x3C\x01\x00\x00\x8B\x45\x0C", "xxxxxxxxxxxxxxx");

	FUNC_CNetGame__SetWeather = 0x80AE6D0;
	FUNC_CNetGame__SetGravity = 0x80AE7D0;

	FUNC_CFilterscripts__LoadFilterscript = FindPattern("\x89\x7D\x00\x8B\x45\x00\x8B\x7D\x00\x89\x5D\x00\x89\x44\x24\x00", "xx?xx?xx?xx?xxx?") - 0x9;
	FUNC_CFilterscripts__UnLoadFilterscript = FindPattern("\x31\xF6\x53\x83\xEC\x00\x8B\x45\x00\x8B\x7D\x00\x89\xC3", "xxxxx?xx?xx?xx") - 0x5;

	FUNC_ContainsInvalidChars = FindPattern("\x53\x8B\x5D\x00\x0F\xB6\x0B\x84\xC9\x74\x00\x66\x90", "xxx?xxxxxx?xx") - 0x3;

	FUNC_CPlayer__SpawnForWorld = FindPattern("\x55\x89\xE5\x56\x53\x83\xEC\x00\x8B\x75\x00\xA1\x00\x00\x00\x00", "xxxxxxx?xx?x????");
	FUNC_CObject__SpawnForPlayer = 0x80C8080;

	unsigned long addr = FindPattern("\x55\x31\xD2\x89\xE5\x57\xB9\x40\x00\x00\x00", "xxxxxxxxxxx");

	FUNC_ProcessQueryPacket = FindPattern("\x81\xEC\xA8\x00\x00\x00\x89\x5D\xF4\x8B\x5D\x14\x89\x75\xF8", "xxxxxxxxxxxxxxx") - 0x3;
	FUNC_Packet_WeaponsUpdate = FindPattern("\x55\x31\xC0\x89\xE5\x81\xEC\x58\x01\x00\x00", "xxxxxxxxxxx");
	FUNC_Packet_StatsUpdate = FindPattern("\x55\x31\xD2\x89\xE5\x81\xEC\x58\x01\x00\x00\x89\x5D\xF4", "xxxxxxxxxxxxxx"); // 80AD430
	FUNC_format_amxstring = FindPattern("\x55\xB8\x00\x10\x00\x00\x89\xE5\x56\x53\x83\xEC\x20", "xxxxxxxxxxxxx");

	ADDR_GetNetworkStats_VerbosityLevel = FindPattern("\xB8\x01\x00\x00\x00\x83\xD9\x03", "xxxxxxxx");
	ADDR_GetPlayerNetworkStats_VerbosityLevel = FindPattern("\xBB\x01\x00\x00\x00\x83\xD9\x03", "xxxxxxxx"); // 080DD7FA

	FUNC_CGameMode__OnPlayerConnect = 0x80A5160;
	FUNC_CGameMode__OnPlayerDisconnect = 0x80A51D0;
	FUNC_CGameMode__OnPlayerSpawn = 0x80A5250;
	FUNC_CGameMode__OnPlayerStreamIn = 0x80A6450;
	FUNC_CGameMode__OnPlayerStreamOut = 0x80A64D0;
	FUNC_CGameMode__OnDialogResponse = 0x80A6750;

	switch ((unsigned long)(logprintf_addr))
	{
	case CAddress::FUNC_Logprintf_037:
	{
		VAR_pRestartWaitTime = 0x081A0840;
		break;
	}
	case CAddress::FUNC_Logprintf_037_R2_1:
	{
		VAR_pRestartWaitTime = 0x0815A528; // 12.0
		VAR_pPosSyncBounds[0] = 0x08150710;
		VAR_pPosSyncBounds[1] = 0x0815070C;
		VAR_pPosSyncBounds[2] = 0x08150718;
		VAR_pPosSyncBounds[3] = 0x08150714;
		ADDR_RecordingDirectory = 0x080CC7D1;
		FUNC_CVehicle__Respawn = 0x814B4C0;
		FUNC_CPlayerPool__HandleVehicleRespawn = 0x80D1480;
		VAR_wRCONUser = 0x08197DF0;
		ARRAY_ConsoleCommands = 0x08196920;
		break;
	}
	}
#endif

	if (VAR_pRestartWaitTime)
		Unlock((void*)VAR_pRestartWaitTime, 4);

	for (unsigned char i = 0; i < 4; i++)
	{
		if (VAR_pPosSyncBounds[i])
			Unlock((void*)VAR_pPosSyncBounds[i], 4);
	}

	if (ADDR_GetNetworkStats_VerbosityLevel)
	{
#ifdef __WIN32
		Unlock((void*)ADDR_GetNetworkStats_VerbosityLevel, 2);
#else
		Unlock((void*)ADDR_GetNetworkStats_VerbosityLevel, 5);
#endif
	}

	if (ADDR_GetPlayerNetworkStats_VerbosityLevel)
	{
#ifdef __WIN32
		Unlock((void*)ADDR_GetPlayerNetworkStats_VerbosityLevel, 2);
#else
		Unlock((void*)ADDR_GetPlayerNetworkStats_VerbosityLevel, 5);
#endif
	}

}

unsigned long CAddress::VAR_pRestartWaitTime(NULL);
unsigned long CAddress::VAR_pPosSyncBounds[4];
unsigned long CAddress::VAR_wRCONUser(NULL);
unsigned long CAddress::ARRAY_ConsoleCommands(NULL);
unsigned long CAddress::FUNC_CConsole__AddStringVariable(NULL);
unsigned long CAddress::FUNC_CConsole__GetStringVariable(NULL);
unsigned long CAddress::FUNC_CConsole__SetStringVariable(NULL);
unsigned long CAddress::FUNC_CConsole__GetIntVariable(NULL);
unsigned long CAddress::FUNC_CConsole__SetIntVariable(NULL);
unsigned long CAddress::FUNC_CConsole__GetBoolVariable(NULL);
unsigned long CAddress::FUNC_CConsole__ModifyVariableFlags(NULL);
unsigned long CAddress::FUNC_CConsole__FindVariable(NULL);
unsigned long CAddress::FUNC_CConsole__SendRules(NULL);
unsigned long CAddress::FUNC_CConsole__Execute(NULL);
unsigned long CAddress::FUNC_CNetGame__SetWeather(NULL);
unsigned long CAddress::FUNC_CNetGame__SetGravity(NULL);
unsigned long CAddress::FUNC_CFilterscripts__LoadFilterscript(NULL);
unsigned long CAddress::FUNC_CFilterscripts__UnLoadFilterscript(NULL);
unsigned long CAddress::FUNC_ContainsInvalidChars(NULL);
unsigned long CAddress::FUNC_CPlayer__SpawnForWorld(NULL);
unsigned long CAddress::FUNC_CVehicle__Respawn(NULL);
unsigned long CAddress::FUNC_CPlayerPool__HandleVehicleRespawn(NULL);
unsigned long CAddress::FUNC_CObject__SpawnForPlayer(NULL);
unsigned long CAddress::FUNC_ProcessQueryPacket(NULL);
unsigned long CAddress::FUNC_Packet_WeaponsUpdate(NULL);
unsigned long CAddress::FUNC_Packet_StatsUpdate(NULL);
unsigned long CAddress::FUNC_format_amxstring(NULL);
unsigned long CAddress::ADDR_GetNetworkStats_VerbosityLevel(NULL);
unsigned long CAddress::ADDR_GetPlayerNetworkStats_VerbosityLevel(NULL);
unsigned long CAddress::ADDR_RecordingDirectory(NULL);
unsigned long CAddress::FUNC_CGameMode__OnPlayerConnect(NULL);
unsigned long CAddress::FUNC_CGameMode__OnPlayerDisconnect(NULL);
unsigned long CAddress::FUNC_CGameMode__OnPlayerSpawn(NULL);
unsigned long CAddress::FUNC_CGameMode__OnPlayerStreamIn(NULL);
unsigned long CAddress::FUNC_CGameMode__OnPlayerStreamOut(NULL);
unsigned long CAddress::FUNC_CGameMode__OnDialogResponse(NULL);

// Globals
// ------------------------------------------------------

logprintf_t logprintf(nullptr);
void** ppPluginData(nullptr);
CNetGame* pNetGame(nullptr);
