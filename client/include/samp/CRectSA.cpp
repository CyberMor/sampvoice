/*
	This is a SAMP API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CRectSA.h"

SAMP::CRectSA::CRectSA() {
	left, top = 0, 0;
	right, bottom = 0, 0;
}

SAMP::CRectSA::CRectSA(long _left, long _top, long _right, long _bottom) {
	left, top = _left, _top;
	right, bottom = _right, _bottom;
}

long SAMP::CRectSA::GetWidth() {
	return right - left;
}

long SAMP::CRectSA::GetHeight() {
	return bottom - top;
}

void SAMP::CRectSA::Move(long x, long y) {
	long w = GetWidth();
	long h = GetHeight();
	left, top = x, y;
	right, bottom = x + w, y + h;
}

void SAMP::CRectSA::SetSize(long w, long h) {
	right, bottom = left + w, top + h;
}

void SAMP::CRectSA::Resize(long x, long y) {
	SetSize(GetWidth() + x, GetHeight() + y);
}

void SAMP::CRectSA::GetCenter(long* x, long* y) {
	if (x == nullptr || y == nullptr)
		return;

	*x = left + GetWidth() / 2;
	*y = top + GetHeight() / 2;
}

bool SAMP::CRectSA::IsPointInside(long x, long y) {
	return ((x >= left && x <= right) && (y >= top && y <= bottom));
}
