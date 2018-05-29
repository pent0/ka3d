#ifndef _MATH_FLOAT3_H
#define _MATH_FLOAT3_H


#include <lang/pp.h>
#include <math/float.h>
#include <xmath.h>


BEGIN_NAMESPACE(math) 


class float2;


/**
 * 3-vector of scalar type float.
 *
 * NOTE: The class behaves like C basic types what it comes to initialization
 * so default constructor leaves the components to unspecified value.
 * Alerting user is the reason also for that the name starts with lower-case letter.
 * 
 * @ingroup math
 */
class float3
{
public:
	/** Component [0] of the vector. */
	float	x;
	
	/** Component [1] of the vector. */
	float	y;
	
	/** Component [2] of the vector. */
	float	z;

#ifdef _DEBUG
	/** Constructs an undefined vector. */
	float3()																		{z=y=x=getNaN();}
#else																				
	float3()																		{}
#endif																				
																					
	/**	Constructs vector from 2-vector and scalar. */
	float3( const float2& xy0, float z0 );

	/**	Constructs vector from scalar triplet. */
	float3( float x0, float y0, float z0 )											: x(x0), y(y0), z(z0) {}
		
	/** Component-wise addition of vectors. */
	float3&	operator+=( const float3& other )										{x+=other.x; y+=other.y; z+=other.z; return *this;}
																					
	/** Component-wise subtraction of vectors. */
	float3&	operator-=( const float3& other )										{x-=other.x; y-=other.y; z-=other.z; return *this;}

	/** Component-wise scalar multiplication. */
	float3&	operator*=( float s )													{x*=s; y*=s; z*=s; return *this;}

	/** Component-wise multiplication. */
	float3&	operator*=( const float3& other )										{x*=other.x; y*=other.y; z*=other.z; return *this;}

	/** Access to ith component of the vector. */
	float&		operator[]( int index )												{return *((&x)+index);}
																					
	/** Returns a random-access iterator to the first component. */					
	float*		begin()																{return &x;}
	
	/** Returns a random-access iterator that points one beyond the last component. */
	float*		end()																{return &x + 3;}

	/** Returns component-wise addition of vectors. */
	float3		operator+( const float3& other ) const								{return float3(x+other.x,y+other.y,z+other.z);}
	
	/** Returns component-wise subtraction of vectors. */
	float3		operator-( const float3& other ) const								{return float3(x-other.x,y-other.y,z-other.z);}

	/** Returns component-wise multiplication of vectors. */
	float3		operator*( const float3& other ) const								{return float3(x*other.x,y*other.y,z*other.z);}

	/** Returns component-wise negation. */
	float3		operator-() const													{return float3(-x,-y,-z);}
	
	/** Returns vector multiplied by given scalar */
	float3		operator*( float s ) const											{return float3(x*s,y*s,z*s);}
	
	/** Returns ith component of the vector. */
	const float&	operator[]( int index ) const									{return *((&x)+index);}

	/** Component-wise equality. */
	bool		operator==( const float3& other ) const								{return x==other.x && y==other.y && z==other.z;}
	
	/** Component-wise inequality. */
	bool		operator!=( const float3& other ) const								{return x!=other.x || y!=other.y || z!=other.z;}

	/** Return first 2-vector part. */
	const float2& xy() const;

	/** Returns length of the vector */
	float		length() const														{return ::sqrtf( x*x + y*y + z*z );}
	
	/** Returns length of the vector squared */
	float		lengthSquared() const												{return x*x + y*y + z*z;}
	
	/** Returns a random-access iterator to the first component. */
	const float*	begin() const													{return &x;}
	
	/** Returns a random-access iterator that points one beyond the last component. */
	const float*	end() const														{return &x + 3;}

	/** Returns the vector rotated about specified axis by angle. */
	float3		rotate( const float3& axis, float angle ) const;

	/** Returns true if all components are in finite range. */
	bool		finite() const;
};


/** 
 * Scalar multiply
 */
inline float3 operator*( float s, const float3& v )
{
	return float3( v.x*s, v.y*s, v.z*s );
}

/** 
 * Returns cross product of two vectors.
 */
inline float3 cross( const float3& a, const float3& b )
{
	return float3( a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x );
}

/** 
 * Returns dot product of two vectors. 
 */
inline float dot( const float3& a, const float3& b )
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

/** 
 * Returns the vector scaled to unit length. This vector can't be 0-vector. 
 */
inline float3 normalize( const float3& v )
{
	return v * (1.f / ::sqrtf( v.x*v.x + v.y*v.y + v.z*v.z ));
}

/** 
 * Returns vector clamped to [0,1]. 
 */
inline float3 saturate( const float3& v )
{
	return float3( 
		v.x < 0.f ? 0.f : (v.x > 1.f ? 1.f : v.x),
		v.y < 0.f ? 0.f : (v.y > 1.f ? 1.f : v.y),
		v.z < 0.f ? 0.f : (v.z > 1.f ? 1.f : v.z) );
}

/** 
 * Returns vector clamped to [min,max] range. 
 */
inline float3 clamp( const float3& v, const float3& minv, const float3& maxv )
{
	return float3( 
		v.x < minv.x ? minv.x : (v.x > maxv.x ? maxv.x : v.x),
		v.y < minv.y ? minv.y : (v.y > maxv.y ? maxv.y : v.y),
		v.z < minv.z ? minv.z : (v.z > maxv.z ? maxv.z : v.z) );
}

/** 
 * Returns vector absolute value. 
 */
inline float3 abs( const float3& v )
{
	return float3( 
		v.x < 0.f ? -v.x : v.x,
		v.y < 0.f ? -v.y : v.y,
		v.z < 0.f ? -v.z : v.z );
}

/** 
 * Returns the vector scaled to unit length. Returns (0,0,0) if input 0-vector. 
 */
float3 normalize0( const float3& v );


END_NAMESPACE() // math


#endif // _MATH_FLOAT3_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
