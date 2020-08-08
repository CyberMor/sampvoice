#pragma once

#include <cmath>

#define FLOAT_EPSILON 0.0001f

class CVector {
private:

	bool IsNearZeroFloat(const float param);

public:

	float fX, fY, fZ;
	
	CVector();
	CVector(CVector const &rhs);
	CVector(float fX, float fY, float fZ);
	float Normalize(void);
	float Length(void) const;
	float DotProduct(const CVector * param) const;
	void CrossProduct(const CVector * param);
	bool IsNearZero(void);
	void Zero(void);
	void ZeroNearZero(void);
	float GetAngleRadians(void);
	float GetAngleDegrees(void);
	CVector operator + (const CVector& vecRight) const;
	CVector operator - (const CVector& vecRight) const;
	CVector operator * (const CVector& vecRight) const;
	CVector operator * (float fRight) const;
	CVector operator / (const CVector& vecRight) const;
	CVector operator / (float fRight) const;
	CVector operator - () const;
	void operator += (float fRight);
	void operator += (const CVector& vecRight);
	void operator -= (float fRight);
	void operator -= (const CVector& vecRight);
	void operator *= (float fRight);
	void operator *= (const CVector& vecRight);
	void operator /= (float fRight);
	void operator /= (const CVector& vecRight);
	bool operator== (const CVector& param) const;
	bool operator!= (const CVector& param) const;

};
