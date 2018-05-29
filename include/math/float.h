#ifndef _MATH_FLOAT_H
#define _MATH_FLOAT_H


#include <lang/pp.h>


#undef abs


BEGIN_NAMESPACE(math) 


/** 
 * Returns scalar clamped to [0,1]. 
 */
inline float saturate( float v )
{
	return v < 0.f ? 0.f : (v > 1.f ? 1.f : v);
}

/** 
 * Returns scalar clamped to [min,max] range. 
 */
inline float clamp( float v, float minv, float maxv )
{
	return v < minv ? minv : (v > maxv ? maxv : v);
}

#ifdef PLATFORM_SUPPORTS_NAMESPACES
/** 
 * Returns scalar absolute value. 
 */
inline float abs( float v )
{
	return v < 0.f ? -v : v;
}
#endif // PLATFORM_SUPPORTS_NAMESPACES

/** 
 * Linear interpolation in range [a,b] by s.
 */
template <class T> inline T lerp( const T& a, const T& b, float s )
{
	return a + (b-a)*s;
}

/** 
 * Returns not-a-number value (sqrt of -1). 
 */
inline static float getNaN()
{
	const int x = 0xFFC00000; 
	return *(const float*)&x;
}


END_NAMESPACE() // math


#endif // _MATH_FLOAT_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
