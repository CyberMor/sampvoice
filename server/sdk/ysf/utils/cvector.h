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

#ifndef __CVector_H
#define __CVector_H

#include <cmath>

#define FLOAT_EPSILON 0.0001f

/**
 * CVector Structure used to store a 3D vertex.
 */
class CVector
{
private:
	bool IsNearZeroFloat( const float param )
	{
		//if(!_finite(param))
		//	return 1;
		return (fabs(param) < FLOAT_EPSILON);
	}

public:
	float fX, fY, fZ;
	
	CVector ()
	{
		this->fX = 0;
		this->fY = 0;
		this->fZ = 0;
	};
	
	CVector(CVector const &rhs)
	{
		this->fX = rhs.fX;
		this->fY = rhs.fY;
		this->fZ = rhs.fZ;
	};

	CVector ( float fX, float fY, float fZ) 
	{ 
		this->fX = fX;
		this->fY = fY;
		this->fZ = fZ;
	}
	
    float Normalize ( void ) 
    { 
        double t = sqrt(static_cast<double>(fX*fX + fY*fY + fZ*fZ));
        if ( t > FLOAT_EPSILON )
        {
            double fX2 = fX / t;
            double fY2 = fY / t;
            double fZ2 = fZ / t;
            fX = (float)fX2;
            fY = (float)fY2;
            fZ = (float)fZ2;
        }
        else
            t = 0;
        return static_cast<float>( t );
    }

	float Length ( void ) const
	{
		return sqrt ( (fX*fX) + (fY*fY) + (fZ*fZ) );
	}

    float DotProduct ( const CVector * param ) const
    {
        return fX*param->fX + fY*param->fY + fZ*param->fZ;
    }

    void CrossProduct ( const CVector * param ) 
    { 
        float _fX = fX, _fY = fY, _fZ = fZ;
        fX = _fY * param->fZ - param->fY * _fZ;
        fY = _fZ * param->fX - param->fZ * _fX;
        fZ = _fX * param->fY - param->fX * _fY;
    }

	bool IsNearZero ( void ) 
	{
		//if(!isfinite(fX) || !isfinite(fY) || !isfinite(fZ))
		//	return 1;
		return (IsNearZeroFloat(fX) && IsNearZeroFloat(fY) && IsNearZeroFloat(fZ));
	}

	void Zero ( void )
	{
		fX = 0.0f;
		fY = 0.0f;
		fZ = 0.0f;
	}

	void ZeroNearZero ( void )
	{
		if ( IsNearZeroFloat(fX) )
			fX = 0.0f;
		if ( IsNearZeroFloat(fY) )
			fY = 0.0f;
		if ( IsNearZeroFloat(fZ) )
			fZ = 0.0f;
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

	CVector operator + ( const CVector& vecRight ) const
    {
        return CVector ( fX + vecRight.fX, fY + vecRight.fY, fZ + vecRight.fZ );
    }

    CVector operator - ( const CVector& vecRight ) const
    {
        return CVector ( fX - vecRight.fX, fY - vecRight.fY, fZ - vecRight.fZ );
    }

    CVector operator * ( const CVector& vecRight ) const
    {
        return CVector ( fX * vecRight.fX, fY * vecRight.fY, fZ * vecRight.fZ );
    }

    CVector operator * ( float fRight ) const
    {
        return CVector ( fX * fRight, fY * fRight, fZ * fRight );
    }

    CVector operator / ( const CVector& vecRight ) const
    {
        return CVector ( fX / vecRight.fX, fY / vecRight.fY, fZ / vecRight.fZ );
    }

	CVector operator / ( float fRight ) const
	{
		return CVector ( fX / fRight, fY / fRight, fZ / fRight );
	}

    CVector operator - () const
    {
        return CVector ( -fX, -fY, -fZ );
    }

    void operator += ( float fRight )
    {
        fX += fRight;
        fY += fRight;
        fZ += fRight;
    }

    void operator += ( const CVector& vecRight )
    {
        fX += vecRight.fX;
        fY += vecRight.fY;
        fZ += vecRight.fZ;
    }

    void operator -= ( float fRight )
    {
        fX -= fRight;
        fY -= fRight;
        fZ -= fRight;
    }

    void operator -= ( const CVector& vecRight )
    {
        fX -= vecRight.fX;
        fY -= vecRight.fY;
        fZ -= vecRight.fZ;
    }

    void operator *= ( float fRight )
    {
        fX *= fRight;
        fY *= fRight;
        fZ *= fRight;
    }

    void operator *= ( const CVector& vecRight )
    {
        fX *= vecRight.fX;
        fY *= vecRight.fY;
        fZ *= vecRight.fZ;
    }

    void operator /= ( float fRight )
    {
        fX /= fRight;
        fY /= fRight;
        fZ /= fRight;
    }

    void operator /= ( const CVector& vecRight )
    {
        fX /= vecRight.fX;
        fY /= vecRight.fY;
        fZ /= vecRight.fZ;
    }

    bool operator== ( const CVector& param ) const
    {
        return ( ( fabs ( fX - param.fX ) < FLOAT_EPSILON ) &&
                 ( fabs ( fY - param.fY ) < FLOAT_EPSILON ) &&
                 ( fabs ( fZ - param.fZ ) < FLOAT_EPSILON ) );
    }

    bool operator!= ( const CVector& param ) const
    {
        return ( ( fabs ( fX - param.fX ) >= FLOAT_EPSILON ) ||
                 ( fabs ( fY - param.fY ) >= FLOAT_EPSILON ) ||
                 ( fabs ( fZ - param.fZ ) >= FLOAT_EPSILON ) );
    }
};

// global vector normals
CVector const g_vecFrontNormal ( 1.0f, 0.0f, 0.0f );
CVector const g_vecRightNormal ( 0.0f, 1.0f, 0.0f );
CVector const g_vecUpNormal ( 0.0f, 0.0f, 1.0f );

#endif
