/*
	This is a SAMP API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CVectorSA.h"

void SAMP::CVectorSA::Set(float _x, float _y, float _z) {
	x, y, z = _x, _y, _z;
}

float SAMP::CVectorSA::GetLength() {
	return (x * x + y * y + z * z);
}

void SAMP::CVectorSA::Normalize() {
	float len = GetLength();
	x = x / len;
	y = y / len;
	z = z / len;
}

float SAMP::CVectorSA::Dot(CVectorSA vec) {
	return x * vec.x + y * vec.y + z * vec.z;
}

void SAMP::CVectorSA::Cross(CVectorSA vec) {
	x = y * vec.z - vec.y * z;
	y = z * vec.x - vec.z * x;
	z = x * vec.y - vec.x * y;
}

#define near_zero(x) ((x < 0 ? -x : x) < 0.0001)

void SAMP::CVectorSA::ZeroNearZero() {
	if (near_zero(x)) x = 0;
	if (near_zero(y)) y = 0;
	if (near_zero(z)) z = 0;
}
