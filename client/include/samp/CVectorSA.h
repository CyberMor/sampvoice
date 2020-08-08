/*
	This is a SAMP API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

SAMP_BEGIN

struct SAMP_API VectorCompressed {

	unsigned short x, y, z;

};

class SAMP_API CVectorSA {
public:

	float x, y, z;

	CVectorSA() { Set(0, 0, 0); }
	CVectorSA(float x, float y, float z) { Set(x, y, z); }

	void Set(float x, float y, float z);
	float GetLength();
	void Normalize();
	float Dot(CVectorSA vec);
	void Cross(CVectorSA vec);
	void ZeroNearZero();

};

SAMP_END
