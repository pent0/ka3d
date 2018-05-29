#ifndef _MATH_FLOAT2_H
#define _MATH_FLOAT2_H


#include <math/float.h>
#include <xmath.h>


BEGIN_NAMESPACE(math) 


/** 
 * 2-vector of scalar type float. 
 *
 * NOTE: The class behaves like C basic types what it comes to initialization
 * so default constructor leaves the components to unspecified value.
 * Alerting user is the reason also for that the name starts with lower-case letter.
 * 
 * @ingroup math
 */
class float2
{
public:
	/** Component [0] of the vector. */
	float		x;
		
	/** Component [1] of the vector. */
	float		y;
		
#ifdef _DEBUG
	/** Constructs undefined vector. */
	float2()																		{y=x=getNaN();}
#else
	float2()																		{}
#endif
			
	/** Constructs vector from scalar pair. */
	float2( float X, float Y )														: x(X), y(Y) {}
		
	/** Component-wise addition of vectors. */
	float2&	operator+=( const float2& other )										{x+=other.x; y+=other.y; return *this;}
																							
	/** Component-wise subtraction of vectors. */
	float2&	operator-=( const float2& other )										{x-=other.x; y-=other.y; return *this;}
			
	/** Component-wise scalar multiplication. */
	float2&	operator*=( float s )													{x*=s; y*=s; return *this;}

	/** Component-wise multiplication. */
	float2&	operator*=( const float2& other )										{x*=other.x; y*=other.y; return *this;}

	/** Access to ith component of the vector. */
	float&		operator[]( int index )												{return *((&x)+index);}
		
	/** Returns a random-access iterator to the first component. */
	float*		begin()																{return &x;}
			
	/** Returns a random-access iterator that points one beyond the last component. */
	float*		end()																{return &x + 2;}
		
	/** Returns component-wise addition of vectors. */
	float2		operator+( const float2& other ) const								{float2 v; for ( int i = 0 ; i < 2 ; ++i ) (&v.x)[i] = (&x)[i] + (&other.x)[i]; return v;}
			
	/** Returns component-wise subtraction of vectors. */
	float2		operator-( const float2& other ) const								{float2 v; for ( int i = 0 ; i < 2 ; ++i ) (&v.x)[i] = (&x)[i] - (&other.x)[i]; return v;}

	/** Returns component-wise multiplication of vectors. */
	float2		operator*( const float2& other ) const								{float2 v; for ( int i = 0 ; i < 2 ; ++i ) (&v.x)[i] = (&x)[i] * (&other.x)[i]; return v;}

	/** Returns component-wise negation. */
	float2		operator-() const													{float2 v; for ( int i = 0 ; i < 2 ; ++i ) (&v.x)[i] = -(&x)[i]; return v;}
				
	/** Returns vector multiplied by given scalar */
	float2		operator*( float s ) const											{float2 v; for ( int i = 0 ; i < 2 ; ++i ) (&v.x)[i] = (&x)[i] * s; return v;}
																								
	/** Returns ith component of the vector. */
	const float&	operator[]( int index ) const									{/*assert(index>=0&&index<2);*/ return *((&x)+index);}
																								
	/** Component-wise equality. */
	bool		operator==( const float2& other ) const								{for ( int i = 0 ; i < 2 ; ++i ) if ( (&x)[i] != (&other.x)[i] ) return false; return true;}
	
	/** Component-wise inequality. */
	bool		operator!=( const float2& other ) const								{for ( int i = 0 ; i < 2 ; ++i ) if ( (&x)[i] != (&other.x)[i] ) return true; return false;}

	/** Returns length of the vector. */											
	float		length() const														{return ::sqrtf( lengthSquared() );}
																					
	/** Returns length of the vector squared. */									
	float		lengthSquared() const												{return x*x+y*y;}
																					
	/** Returns a random-access iterator to the first component. */					
	const float*	begin() const													{return &x;}
	
	/** Returns a random-access iterator that points one beyond the last component. */
	const float*	end() const														{return &x + 2;}

	/** Returns true if all components are in finite range. */
	bool		finite() const;
};


/** 
 * Scalar multiply
 */
inline float2 operator*( float s, const float2& v )
{
	return float2( v.x*s, v.y*s );
}


/** 
 * Returns dot product of two vectors. 
 */
inline float dot( const float2& a, const float2& b )
{
	return a.x*b.x + a.y*b.y;
}

/** 
 * Returns the vector scaled to unit length. This vector can't be 0-vector. 
 */
inline float2 normalize( const float2& v )
{
	return v * (1.f / ::sqrtf( v.x*v.x + v.y*v.y ));
}

/** 
 * Returns vector clamped to [0,1]. 
 */
inline float2 saturate( const float2& v )
{
	return float2( 
		v.x < 0.f ? 0.f : (v.x > 1.f ? 1.f : v.x),
		v.y < 0.f ? 0.f : (v.y > 1.f ? 1.f : v.y) );
}

/** 
 * Returns vector clamped to [min,max] range. 
 */
inline float2 clamp( const float2& v, const float2& minv, const float2& maxv )
{
	return float2( 
		v.x < minv.x ? minv.x : (v.x > maxv.x ? maxv.x : v.x),
		v.y < minv.y ? minv.y : (v.y > maxv.y ? maxv.y : v.y) );
}

/** 
 * Returns vector absolute value. 
 */
inline float2 abs( const float2& v )
{
	return float2( 
		v.x < 0.f ? -v.x : v.x,
		v.y < 0.f ? -v.y : v.y );
}

/** 
 * Returns the vector scaled to unit length. Returns (0,0) if input 0-vector. 
 */
float2 normalize0( const float2& v );


END_NAMESPACE() // math


#endif // _MATH_FLOAT2_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
