#ifndef _MATH_RANDOMUTIL_H
#define _MATH_RANDOMUTIL_H


#include <lang/pp.h>
#include <math/float3.h>


BEGIN_NAMESPACE(math) 


/**
 * Utility functions for pseudo-random number generation
 * inside various domains.
 * @ingroup math
 */
class RandomUtil
{
public:
	/**
	 * Returns random number between 0 (inclusive) and 1 (exclusive).
	 */
	static float	random();

	/**
	 * Returns random number in given range.
	 * @param begin Range start (inclusive).
	 * @param end Range end (exclusive).
	 */
	static float	getRandom( float begin, float end );

	/**
	 * Returns random point on origin centered XY-plane disk.
	 * @param r1 Radius inside which there are no points (inclusive).
	 * @param r2 Radius outside which there are no points (exclusive).
	 */
	static float3	getPointOnDisk( float r1, float r2 );

	/**
	 * Returns random point inside disk.
	 * @param o Origin of disk.
	 * @param n Normal of disk plane.
	 * @param r1 Radius inside which there are no points (inclusive).
	 * @param r2 Radius outside which there are no points (exclusive).
	 */
	static float3	getPointOnDisk( const float3& o, const float3& n, float r1, float r2 );

	/**
	 * Randomizes point inside origin centered sphere.
	 * @param r1 Radius inside which there are no points (inclusive).
	 * @param r2 Radius outside which there are no points (exclusive).
	 */
	static float3	getPointInSphere( float r1, float r2 );

	/**
	 * Randomizes point on line.
	 * @param p1 Start of line (inclusive).
	 * @param p2 End of line (exclusive).
	 */
	static float3	getPointOnLine( const float3& p1, const float3& p2 );

	/**
	 * Randomizes point inside box.
	 * @param p1 Min corner of box (inclusive).
	 * @param p2 Max corner of box (exclusive).
	 */
	static float3	getPointInBox( const float3& p1, const float3& p2 );

	/**
	 * Randomizes point inside cylinder which starts at origin and extends along +Z.
	 * @param len Length of cylinder along Z+ axis.
	 * @param r1 Radius inside which there are no points (inclusive).
	 * @param r2 Radius outside which there are no points (exclusive).
	 */
	static float3	getPointInCylinder( float len, float r1, float r2 );

	/**
	 * Randomizes point inside cylinder.
	 * @param p1 Start of cylinder (inclusive).
	 * @param p2 End of cylinder (exclusive).
	 * @param r1 Radius inside which there are no points (inclusive).
	 * @param r2 Radius outside which there are no points (exclusive).
	 */
	static float3	getPointInCylinder( const float3& p1, const float3& p2, float r1, float r2 );

	/**
	 * Randomizes point on rectangle.
	 * @param o Origin.
	 * @param e1 Edge 1-2, from origin.
	 * @param e2 Edge 1-4, from origin.
	 */
	static float3	getPointOnRectangle( const float3& o, const float3& e1, const float3& e2 );

	/**
	 * Randomizes point on triangle.
	 * @param o Origin of triangle.
	 * @param e1 Edge 1-2, from origin.
	 * @param e2 Edge 1-3, from origin.
	 */
	static float3	getPointOnTriangle( const float3& o, const float3& e1, const float3& e2 );

private:
	static void		getPointOnDisk( float r1, float r2, float* x, float* y );
};


END_NAMESPACE() // math


#endif // _MATH_RANDOMUTIL_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
