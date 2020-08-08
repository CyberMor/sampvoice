#pragma once

#define FLOAT_EPSILON 0.0001f

#include <cmath>

class CVector2D {
private:

	bool IsNearZeroFloat(const float param);

public:

	float fX, fY;

	CVector2D();
	CVector2D(float fX, float fY);
	float Length(void) const;
	bool IsNearZero(void);
	void Zero(void);
	void ZeroNearZero(void);
	float GetAngleRadians(void);
	float GetAngleDegrees(void);
	CVector2D operator + (const CVector2D& vecRight) const;
	CVector2D operator - (const CVector2D& vecRight) const;
	CVector2D operator * (const CVector2D& vecRight) const;
	CVector2D operator * (float fRight) const;
	CVector2D operator / (const CVector2D& vecRight) const;
	CVector2D operator / (float fRight) const;
	CVector2D operator - () const;
	void operator += (float fRight);
	void operator += (const CVector2D& vecRight);
	void operator -= (float fRight);
	void operator -= (const CVector2D& vecRight);
	void operator *= (float fRight);
	void operator *= (const CVector2D& vecRight);
	void operator /= (float fRight);
	void operator /= (const CVector2D& vecRight);
	bool operator== (const CVector2D& param) const;
	bool operator!= (const CVector2D& param) const;

};
