/*
	This is a SAMP API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"
#include "CVectorSA.h"

SAMP_BEGIN

class SAMP_API CMatrixSA {
public:

	CVectorSA right;
	unsigned long flags;
	CVectorSA up;
	float pad_u;
	CVectorSA at;
	float pad_a;
	CVectorSA pos;
	float pad_p;

};

SAMP_END
