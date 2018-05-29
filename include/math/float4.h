#ifndef _MATH_FLOAT4_H
#define _MATH_FLOAT4_H


#include <lang/pp.h>
#include <math/float.h>
#include <xmath.h>


#ifdef PLATFORM_PS2
	/** Align 4-vectors to 128 bit boundary. */
	#define MATH_ALIGN_128 __attribute__ ((aligned(16)))
#else
	#define MATH_ALIGN_128
#endif


BEGIN_NAMESPACE(math) 


class float2;
class float3;


/**
 * 4-vector of scalar type float.
 *
 * NOTE: The class behaves like C basic types what it comes to initialization
 * so default constructor leaves the components to unspecified value.
 * Alerting user is the reason also for that the name starts with lower-case letter.
 * 
 * @ingroup math
 */
class float4
{
public:
	/** Component [0] of the vector. */
	MATH_ALIGN_128 float		x;
	
	/** Component [1] of the vector. */
	float		y;
	
	/** Component [2] of the vector. */
	float		z;
	
	/** Component [3] of the vector. */
	float		w;

#ifdef _DEBUG
	/** Constructs undefined vector. */
	float4()																		{w=z=y=x=getNaN();}
#else
	float4()																		{}
#endif
	
	/** Constructs vector from scalar quadruple. */
	float4( float x0, float y0, float z0, float w0 )								: x(x0), y(y0), z(z0), w(w0) {}

	/** Constructs vector from 3-vector and scalar. */
	float4( const float3& xyz0, float w0 );

	/** Constructs vector from 2-vector and 2 scalars. */
	float4( const float2& xy0, float z0, float w0 );

	/** Component-wise addition. */
	float4&	operator+=( const float4& other )										{x+=other.x; y+=other.y; z+=other.z; w+=other.w; return *this;}
	
	/** Component-wise subtraction. */
	float4&	operator-=( const float4& other )										{x-=other.x; y-=other.y; z-=other.z; w-=other.w; return *this;}
	
	/** Component-wise scalar multiplication. */
	float4&	operator*=( float s )													{x*=s; y*=s; z*=s; w*=s; return *this;}
	
	/** Component-wise multiplication. */
	float4&	operator*=( const float4& other )										{x*=other.x; y*=other.y; z*=other.z; w*=other.w; return *this;}

	/** Returns ith component of the vector. */
	float&		operator[]( int index )										{return *((&x)+index);}

	/** Returns a random-access iterator to the first component. */
	float*		begin()																{return &x;}
	
	/** Returns a random-access iterator that points just beyond the last component. */
	float*		end()																{return &x + 4;}

	/** 
	 * Define plane by 4-vector which consists of the plane normal 
	 * (x,y,z) and the distance (w) to origin from the plane along the normal.
	 * Plane equation is Ax+By+Cz+D==0, where D=-Ax-By-Cz.
	 * Point (x,y,z,1) is on the 'front' side of a plane if dot product between
	 * the point and the plane is positive. To check if a point is on the plane
	 * is is adivisable to use some an error threshold, since with floating
	 * point arithmetic dot product is rarely equal to zero, even
	 * thought the point would practically be on the plane.
	 */
	void		setPlane( const float3& normal, const float3& point );

	/** Returns component-wise addition of vectors. */
	float4		operator+( const float4& other ) const								{return float4(x+other.x,y+other.y,z+other.z,w+other.w);}
	
	/** Returns component-wise subtraction of vectors. */
	float4		operator-( const float4& other ) const								{return float4(x-other.x,y-other.y,z-other.z,w-other.w);}

	/** Returns component-wise multiplication of vectors. */
	float4		operator*( const float4& other ) const								{return float4(x*other.x,y*other.y,z*other.z,w*other.w);}

	/** Returns component-wise negation. */
	float4		operator-() const													{return float4(-x,-y,-z,-w);}
	
	/** Returns vector multiplied by given scalar */
	float4		operator*( float s ) const											{return float4(x*s,y*s,z*s,w*s);}
	
	/** Access to ith component of the vector. */
	const float&	operator[]( int index ) const									{return *((&x)+index);}

	/** Component-wise equality. */
	bool		operator==( const float4& other ) const								{return x==other.x && y==other.y && z==other.z && w==other.w;}
	
	/** Component-wise inequality. */
	bool		operator!=( const float4& other ) const								{return x!=other.x || y!=other.y || z!=other.z || w!=other.w;}

	/** Return first 3-vector part. */
	const float3& xyz() const;

	/** Return first 2-vector part. */
	const float2& xy() const;

	/** Returns length of the vector. */
	float		length() const														{return ::sqrtf( lengthSquared() );}
	
	/** Returns length of the vector squared. */
	float		lengthSquared() const												{return x*x+y*y+z*z+w*w;}

	/** Returns a random-access iterator to the first component. */
	const float*	begin() const													{return &x;}
	
	/** Returns a random-access iterator that points one beyond the last component. */
	const float*	end() const														{return &x + 4;}

	/** Returns true if all components are in finite range. */
	bool			finite() const;
};


/** 
 * Scalar multiply
 */
inline float4 operator*( float s, const float4& v )
{
	return float4( v.x*s, v.y*s, v.z*s, v.w*s );
}

/** 
 * Returns dot product of two vectors. 
 */
inline float dot( const float4& a, const float4& b )
{
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

/** 
 * Returns the vector scaled to unit length. This vector can't be 0-vector. 
 */
inline float4 normalize( const float4& v )
{
	return v * (1.f / ::sqrtf( v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w ));
}

/** 
 * Returns vector clamped to [0,1]. 
 */
inline float4 saturate( const float4& v )
{
	return float4( 
		v.x < 0.f ? 0.f : (v.x > 1.f ? 1.f : v.x),
		v.y < 0.f ? 0.f : (v.y > 1.f ? 1.f : v.y),
		v.z < 0.f ? 0.f : (v.z > 1.f ? 1.f : v.z),
		v.w < 0.f ? 0.f : (v.w > 1.f ? 1.f : v.w) );
}

/** 
 * Returns vector clamped to [min,max] range. 
 */
inline float4 clamp( const float4& v, const float4& minv, const float4& maxv )
{
	return float4( 
		v.x < minv.x ? minv.x : (v.x > maxv.x ? maxv.x : v.x),
		v.y < minv.y ? minv.y : (v.y > maxv.y ? maxv.y : v.y),
		v.z < minv.z ? minv.z : (v.z > maxv.z ? maxv.z : v.z),
		v.w < minv.w ? minv.w : (v.w > maxv.w ? maxv.w : v.w) );
}

/** 
 * Returns vector absolute value. 
 */
inline float4 abs( const float4& v )
{
	return float4( 
		v.x < 0.f ? -v.x : v.x,
		v.y < 0.f ? -v.y : v.y,
		v.z < 0.f ? -v.z : v.z,
		v.w < 0.f ? -v.w : v.w );
}

/** 
 * Returns the vector scaled to unit length. Returns (0,0,0,0) if input 0-vector. 
 */
float4 normalize0( const float4& v );


END_NAMESPACE() // math


#endif // _MATH_FLOAT4_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
