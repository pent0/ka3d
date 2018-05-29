#ifndef _MATH_ALL_H
#define _MATH_ALL_H


/**
 * @defgroup math Mathematical support classes
 * 
 * The main feature of the library is to provide support for various linear algebra classes like float2, float3, float4, float3x3, float3x4, float4x4 and quaternion.
 *
 * The library also provides interpolation support with end-behavior specifications, which are useful while animating for example characters.
 *
 * Random numbers are frequently used to assist for example special effects like particles and non-deterministic behavior of AI. Support for various random variables, like randomizing point inside sphere, is provided by RandomUtil class.
 *
 * @{
 */

#include <math/InterpolationUtil.h>
#include <math/float3x3.h>
#include <math/float3x4.h>
#include <math/float4x4.h>
#include <math/toString.h>
#include <math/quaternion.h>
#include <math/RandomUtil.h>
#include <math/float2.h>
#include <math/float3.h>
#include <math/float4.h>

/** @} */


#endif // _MATH_ALL_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
