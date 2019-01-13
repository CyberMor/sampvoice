/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/CVector.h
*  PURPOSE:		3D vector math implementation
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CVector2D_H
#define __CVector2D_H

#define FLOAT_EPSILON 0.0001f

#include <cmath>

/**
 * CVector Structure used to store a 3D vertex.
 */
class CVector2D
{
private:
	bool IsNearZeroFloat( const float param )
	{
		//if(!_finite(param))
		//	return 1;
		return (fabs(param) < FLOAT_EPSILON);
	}

public:
	float fX, fY;
	
	CVector2D()
	{
		this->fX = 0;
		this->fY = 0;
	};
	
	CVector2D( float fX, float fY)
	{ 
		this->fX = fX;
		this->fY = fY;
	}
	

	float Length ( void ) const
	{
		return sqrt ( (fX*fX) + (fY*fY) );
	}

 
	bool IsNearZero ( void ) 
	{
		//if(!isfinite(fX) || !isfinite(fY) || !isfinite(fZ))
		//	return 1;
		return (IsNearZeroFloat(fX) && IsNearZeroFloat(fY));
	}

	void Zero ( void )
	{
		fX = 0.0f;
		fY = 0.0f;
	}

	void ZeroNearZero ( void )
	{
		if ( IsNearZeroFloat(fX) )
			fX = 0.0f;
		if ( IsNearZeroFloat(fY) )
			fY = 0.0f;
	}

	float GetAngleRadians ( void )
	{
		return -atan2(fY, -fX);
	}

	float GetAngleDegrees ( void )
	{
		static float radtodeg = 57.324840764331210191082802547771f; // 180/pi
		float ret = (atan2(fY, -fX) * radtodeg) + 270.0f;
		if (ret >= 360.0f) ret -= 360.0f;
		return ret;
	}
	
	CVector2D operator + ( const CVector2D& vecRight ) const
    {
        return CVector2D ( fX + vecRight.fX, fY + vecRight.fY);
    }

	CVector2D operator - ( const CVector2D& vecRight ) const
    {
        return CVector2D( fX - vecRight.fX, fY - vecRight.fY);
    }

	CVector2D operator * ( const CVector2D& vecRight ) const
    {
        return CVector2D( fX * vecRight.fX, fY * vecRight.fY );
    }

	CVector2D operator * ( float fRight ) const
    {
        return CVector2D( fX * fRight, fY * fRight);
    }

	CVector2D operator / ( const CVector2D& vecRight ) const
    {
        return CVector2D( fX / vecRight.fX, fY / vecRight.fY);
    }

	CVector2D operator / ( float fRight ) const
	{
		return CVector2D( fX / fRight, fY / fRight);
	}

	CVector2D operator - () const
    {
        return CVector2D( -fX, -fY );
    }

    void operator += ( float fRight )
    {
        fX += fRight;
        fY += fRight;
    }

    void operator += ( const CVector2D& vecRight )
    {
        fX += vecRight.fX;
        fY += vecRight.fY;
    }

    void operator -= ( float fRight )
    {
        fX -= fRight;
        fY -= fRight;
    }

    void operator -= ( const CVector2D& vecRight )
    {
        fX -= vecRight.fX;
        fY -= vecRight.fY;
    }

    void operator *= ( float fRight )
    {
        fX *= fRight;
        fY *= fRight;
    }

    void operator *= ( const CVector2D& vecRight )
    {
        fX *= vecRight.fX;
        fY *= vecRight.fY;
    }

    void operator /= ( float fRight )
    {
        fX /= fRight;
        fY /= fRight;
    }

    void operator /= ( const CVector2D& vecRight )
    {
        fX /= vecRight.fX;
        fY /= vecRight.fY;
    }

    bool operator== ( const CVector2D& param ) const
    {
		return ((fabs(fX - param.fX) < FLOAT_EPSILON) &&
			(fabs(fY - param.fY) < FLOAT_EPSILON));
    }

    bool operator!= ( const CVector2D& param ) const
    {
		return ((fabs(fX - param.fX) >= FLOAT_EPSILON) ||
			(fabs(fY - param.fY) >= FLOAT_EPSILON));
    }
};

#endif
