#ifndef _MATH_TOSTRING_H
#define _MATH_TOSTRING_H


#include <lang/String.h>


BEGIN_NAMESPACE(math) 


class float2;
class float3;
class float4;
class float3x3;
class float3x4;
class float4x4;


/**
 * @defgroup mathstr String formatting support for mathematical classes
 * @{
 * @ingroup math
 */

/**
 * Converts object to string representation.
 * Not part of vector class since that
 * way user doesnt come dependent on NS(lang,String)
 * if he only needs the vector class and no string conversion.
 * @ingroup mathstr
 */
NS(lang,String)	toString( const float2& v );

/**
 * Converts object to string representation.
 * Not part of vector class since that
 * way user doesnt come dependent on NS(lang,String)
 * if he only needs the vector class and no string conversion.
 * @ingroup mathstr
 */
NS(lang,String)	toString( const float3& v );

/**
 * Converts object to string representation.
 * Not part of vector class since that
 * way user doesnt come dependent on NS(lang,String)
 * if he only needs the vector class and no string conversion.
 * @ingroup mathstr
 */
NS(lang,String)	toString( const float4& v );

/**
 * Converts object to string representation.
 * Not part of matrix class since that
 * way user doesnt come dependent on NS(lang,String)
 * if he only needs the matrix class and no string conversion.
 * @ingroup mathstr
 */
NS(lang,String)	toString( const float3x3& v );

/**
 * Converts object to string representation.
 * Not part of matrix class since that
 * way user doesnt come dependent on NS(lang,String)
 * if he only needs the matrix class and no string conversion.
 * @ingroup mathstr
 */
NS(lang,String)	toString( const float3x4& v );

/**
 * Converts object to string representation.
 * Not part of matrix class since that
 * way user doesnt come dependent on NS(lang,String)
 * if he only needs the matrix class and no string conversion.
 * @ingroup mathstr
 */
NS(lang,String)	toString( const float4x4& v );

/** @} */


END_NAMESPACE() // math


#endif // _MATH_TOSTRING_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
