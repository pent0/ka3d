#ifndef _MATH_QUATERNION_H
#define _MATH_QUATERNION_H


#include <lang/pp.h>
#include <math/float4.h>


BEGIN_NAMESPACE(math) 


class float3;
class float3x3;


/**
 * Quaternion of scalar type float.
 *
 * NOTE: The class behaves like C basic types what it comes to initialization
 * so default constructor leaves the components to unspecified value.
 * Alerting user is the reason also for that the name starts with lower-case letter.
 * 
 * @ingroup math
 */
class quaternion
{
public:
	enum
	{ 
		/** Number of components in a quaternion. */
		SIZE = 4
	};

	/** X-component of the imaginary vector part of the quaternion. */
	float		x MATH_ALIGN_128;

	/** Y-component of the imaginary vector part of the quaternion. */
	float		y;

	/** Z-component of the imaginary vector part of the quaternion. */
	float		z;
	
	/** Real part of the quaternion. */
	float		w;

#ifdef _DEBUG
	/** Constructs undefined quaternion. */
	quaternion()																	{w=z=y=x=getNaN();}
#else
	quaternion()																	{}
#endif
	
	/** Constructs the quaternion from quadruple. */
	quaternion( float x0, float y0, float z0, float w0 );
	
	/** quaternion from rotation about Axis by Angle. */
	quaternion( const float3& axis, float angle );
	
	/** quaternion from rotation matrix. */
	quaternion( const float3x3& rot );

	/** Component-wise addition. */
	quaternion&		operator+=( const quaternion& other )							{for ( int i = 0 ; i < SIZE ; ++i ) (&x)[i] += (&other.x)[i]; return *this;}
	
	/** Component-wise subtraction. */
	quaternion&		operator-=( const quaternion& other )							{for ( int i = 0 ; i < SIZE ; ++i ) (&x)[i] -= (&other.x)[i]; return *this;}
	
	/** Component-wise scalar multiplication. */
	quaternion&		operator*=( float s )											{for ( int i = 0 ; i < SIZE ; ++i ) (&x)[i] *= s; return *this;}
	
	/** quaternion multiplication. */
	quaternion&		operator*=( const quaternion& other )							{*this = *this * other; return *this;}

	/** Returns norm of quaternion. */
	float			norm() const													{return ::sqrtf( normSquared() );}
	
	/** Returns squared norm. */
	float			normSquared() const												{return dot(*this);}

	/** Component-wise equality. */
	bool			operator==( const quaternion& other ) const						{for ( int i = 0 ; i < SIZE ; ++i ) if ( (&x)[i] != (&other.x)[i] ) return false; return true;}
	
	/** Component-wise inequality. */
	bool			operator!=( const quaternion& other ) const						{for ( int i = 0 ; i < SIZE ; ++i ) if ( (&x)[i] != (&other.x)[i] ) return true; return false;}

	/** Returns component-wise addition. */
	quaternion		operator+( const quaternion& other ) const						{quaternion v; for ( int i = 0 ; i < SIZE ; ++i ) (&v.x)[i] = (&x)[i] + (&other.x)[i]; return v;}
	
	/** Returns component-wise subtraction of vectors. */
	quaternion		operator-( const quaternion& other ) const						{quaternion v; for ( int i = 0 ; i < SIZE ; ++i ) (&v.x)[i] = (&x)[i] - (&other.x)[i]; return v;}
	
	/** Returns component-wise negation. */
	quaternion		operator-() const												{quaternion v; for ( int i = 0 ; i < SIZE ; ++i ) (&v.x)[i] = -(&x)[i]; return v;}
	
	/** Returns dot product of this and other quaternion (viewed as 4-vector). */
	float			dot( const quaternion& other ) const							{float d = 0.f; for ( int i = 0 ; i < SIZE ; ++i ) d += (&x)[i] * (&other.x)[i]; return d;}
	
	/** Returns quaternion multiplied by given scalar. */
	quaternion		operator*( float s ) const										{quaternion v; for ( int i = 0 ; i < SIZE ; ++i ) (&v.x)[i] = (&x)[i] * s; return v;}
	
	/** quaternion multiplication. */
	quaternion		operator*( const quaternion& other ) const;

	/** Returns quaternion of unit length. */
	quaternion 		normalize() const;

	/** Returns conjugate of quaternion. */
	quaternion		conjugate() const;

	/** Returns inverse of quaternion. */
	quaternion		inverse() const;

	/** Returns exponent of unit quaternion. */
	quaternion		exp() const;

	/** Returns logarithm of unit quaternion. */
	quaternion		log() const;

	/** Returns unit quaternion raised to power. */
	quaternion		pow( float t ) const;

	/** 
	 * Returns spherical linear interpolation for unit quaternions. 
	 *
	 * @param t Interpolation phase [0,1]
	 * @param q quaternion [n+1]
	 * @return Interpolated quaternion.
	 */
	quaternion		slerp( float t, const quaternion& q ) const;

	/** 
	 * Returns spherical cubic interpolation for unit quaternions. 
	 * This quaternion is used as element [n-1] in the interpolation.
	 *
	 * @param t Interpolation phase [0,1]
	 * @param a quaternion [n]
	 * @param b quaternion [n+1]
	 * @param q quaternion [n+2]
	 * @return Interpolated quaternion.
	 */
	quaternion		squad( float t, const quaternion& a, const quaternion& b, const quaternion& q ) const;
};


END_NAMESPACE() // math


#endif // _MATH_QUATERNION_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
