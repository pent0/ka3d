#ifndef _MATH_INTERSECTIONUTIL_H
#define _MATH_INTERSECTIONUTIL_H


#include <lang/pp.h>


BEGIN_NAMESPACE(math) 


class float2;
class float3;
class float4;
class float4x4;


/** 
 * IntersectionUtil testing utility functions.
 * @ingroup math
 */
class IntersectionUtil
{
public:
	/**
	 * Tests if two 2D-triangles overlap.
	 * @param a Vertices (3) of the first triangle.
	 * @param b Vertices (3) of the first triangle.
	 */
	static bool	testTriangleTriangleOverlap( 
		const float2* a, const float2* b );

	/**
	 * Tests if two 3D-triangles overlap.
	 * @param a Vertices (3) of the first triangle.
	 * @param b Vertices (3) of the first triangle.
	 */
	static bool	testTriangleTriangleOverlap( 
		const float3* a, const float3* b );

	/** 
	 * Tests if point is inside oriented bounding box. 
	 * @param point Point position.
	 * @param box Box center transformation.
	 * @param dim Box axis lengths.
	 * @return true if point is inside.
	 */
	static bool testPointBox( const float3& point, 
		const float4x4& box, const float3& dim );

	/** 
	 * Tests if point is inside oriented bounding box. 
	 * @param point Point position.
	 * @param box Box center transformation.
	 * @param dimMin Box axis lengths in negative directions.
	 * @param dimMax Box axis lengths in positive directions.
	 * @return true if point is inside.
	 */
	static bool testPointBox( const float3& point, 
		const float4x4& box, const float3& dimMin, const float3& dimMax );

	/**
	 * Finds ray plane intersection if any.
	 * @param orig Ray origin.
	 * @param dir Ray direction.
	 * @param plane Plane equation. (n.x, n.y, n.z, -p0.dot(n))
	 * @param t [out] Receives length along ray to intersection if any.
	 * @return true if intersection, false otherwise.
	 */
	static bool findRayPlaneIntersection(
		const float3& orig, const float3& dir,
		const float4& plane, float* t );

	/**
	 * Finds ray sphere intersection if any.
	 * @param orig Ray origin.
	 * @param dir Ray direction.
	 * @param center Sphere center point.
	 * @param radius Sphere radius.
	 * @param t [out] Receives length along ray to the first intersection if any. Receives 0 if ray origin is inside sphere.
	 * @return true if intersection, false otherwise.
	 */
	static bool findRaySphereIntersection(
		const float3& orig, const float3& dir,
		const float3& center, float radius,
		float* t );

	/**
	 * Finds ray cylinder intersection if any.
	 * @param orig Ray origin.
	 * @param dir Ray direction.
	 * @param center Cylinder center point.
	 * @param axis Cylinder axis.
	 * @param radius Cylinder radius.
	 * @param halfHeight Half cylinder height (+-length from center point along axis).
	 * @param t [out] Receives length along ray to the first intersection if any. Receives 0 if ray origin is inside cylinder.
	 * @param normal [out] Receives intersection plane normal if any and if ptr not 0.
	 * @return true if intersection, false otherwise.
	 */
	static bool findRayCylinderIntersection(
		const float3& orig, const float3& dir,
		const float3& center, const float3& axis, float radius, float halfHeight,
		float* t, float3* normal );

	/** 
	 * Finds ray triangle intersection if any. 
	 * Assumes clockwise vertex order.
	 * Uses Tomas Moller's algorithm.
	 * @param orig Ray origin.
	 * @param dir Ray direction.

	 * @param vert0 The first vertex of the triangle.
	 * @param vert1 The second vertex of the triangle.
	 * @param vert2 The third vertex of the triangle.
	 * @param t [out] Receives length along ray to the intersection if any.
	 * @return true if intersection, false otherwise.
	 */
	static bool	findRayTriangleIntersection( 
		const float3& orig, const float3& dir,
		const float3& vert0, const float3& vert1, const float3& vert2,
		float* t );

	/**
	 * Finds ray Bezier patch intersection if any.
	 * Uses constant subdivision.
	 * @param orig Ray origin.
	 * @param dir Ray direction.
	 * @param patch Cubic Bezier patch control points.
	 * @param err Error limit. Maximum control polygon vertex distance from a plane.
	 * @param subdiv Subdivision level. For example subdiv=5 results in 5*5*2 tested triangles.
	 * @param t [out] Receives length along ray to the intersection if any.
	 * @param u [out] Receives patch u-coordinate of the intersection if any.
	 * @param v [out] Receives patch v-coordinate of the intersection if any.
	 */
	static bool	findRayCubicBezierPatchIntersection( const float3& orig, 
		const float3& dir, const float3 patch[4][4], int subdiv,
		float* t, float* u, float* v );

	/**
	 * Finds line plane intersection if any.
	 * @param orig Line start.
	 * @param delta Vector from line start to end.
	 * @param plane Plane equation. (n.x, n.y, n.z, -p0.dot(n))
	 * @param t [out] Receives length along line to intersection if any.
	 * @return true if intersection, false otherwise.
	 */
	static bool findLinePlaneIntersection(
		const float3& orig, const float3& delta,
		const float4& plane, float* t );

	/** 
	 * Finds line triangle intersection if any. 
	 * Assumes clockwise vertex order.
	 * Uses Tomas Moller's algorithm.
	 * @param orig Line origin.
	 * @param delta Line delta, vector from start to end.
	 * @param vert0 The first vertex of the triangle.
	 * @param vert1 The second vertex of the triangle.
	 * @param vert2 The third vertex of the triangle.
	 * @param t [out] Receives length along ray to the intersection if any.
	 * @return true if intersection, false otherwise.
	 */
	static bool	findLineTriangleIntersection( 
		const float3& orig, const float3& delta,
		const float3& vert0, const float3& vert1, const float3& vert2,
		float* t );

	/**
	 * Finds line sphere intersection if any.
	 * @param orig Line origin.
	 * @param delta Vector from line start to end.
	 * @param center Sphere center point.
	 * @param radius Sphere radius.
	 * @param t [out] Receives length along line to the first intersection if any. Receives 0 if line origin is inside sphere.
	 * @return true if intersection, false otherwise.
	 */
	static bool findLineSphereIntersection(
		const float3& orig, const float3& delta,
		const float3& center, float radius,
		float* t );

	/**
	 * Finds line cylinder intersection if any.
	 * @param orig Line start.
	 * @param delta Vector from line start to end.
	 * @param center Cylinder center point.
	 * @param axis Cylinder axis.
	 * @param radius Cylinder radius.
	 * @param halfHeight Half cylinder height (+-length from center point along axis).
	 * @param t [out] Receives length along line to the first intersection if any. Receives 0 if line origin is inside cylinder.
	 * @param normal [out] Receives intersection plane normal if any and if ptr not 0.
	 * @return true if intersection, false otherwise.
	 */
	static bool findLineCylinderIntersection(
		const float3& orig, const float3& delta,
		const float3& center, const float3& axis, float radius, float halfHeight,
		float* t, float3* normal );

	/**
	 * Computes line segment intersection against oriented bounding box if any.
	 * If line start point is inside then intersection is returned at t=0 and normal is set to zero.
	 * @param orig Line segment origin.
	 * @param delta Line segment direction and length.
	 * @param box Box center transformation.
	 * @param dim Box axis lengths.
	 * @param t [out] Receives relative length [0,1] of intersection along line segment.
	 * @param normal [out] Receives intersection point normal if intersection.
	 * @return true if intersection.
	 */
	static bool findLineBoxIntersection( const float3& orig, const float3& delta,
		const float4x4& box, const float3& dim, float* t, float3* normal = 0 );

	/**
	 * Computes line segment intersection against oriented bounding box if any.
	 * If line start point is inside then intersection is returned at t=0 and normal is set to zero.
	 * @param orig Line segment origin.
	 * @param delta Line segment direction and length.
	 * @param box Box center transformation.
	 * @param dimMin Box axis lengths in negative directions.
	 * @param dimMax Box axis lengths in positive directions.
	 * @param t [out] Receives relative length [0,1] of intersection along line segment.
	 * @param normal [out] Receives intersection point normal if intersection.
	 * @return true if intersection.
	 */
	static bool findLineBoxIntersection( const float3& orig, const float3& delta,
		const float4x4& box, const float3& dimMin, const float3& dimMax, float* t, float3* normal = 0 );

	/**
	 * Tests line segment against oriented bounding box.
	 * @param orig Line segment origin.
	 * @param delta Line segment direction and length.
	 * @param box Box center transformation.
	 * @param dim Box axis lengths.
	 * @return true if intersection.
	 */
	static bool	testLineBox( const float3& orig, const float3& delta, 
		const float4x4& box, const float3& dim );

	/**
	 * Tests if two oriented boxes overlap.
	 * Uses Stefan Gottchalk's separating axis algorithm.
	 * @param dim Dimensions of the first OBB.
	 * @param otherTm Transformation of the second OBB in the first OBB space.
	 * @param otherDim Dimensions of the second OBB.
	 * @return true if the OBBs overlap, false otherwise.
	 */
	static bool testBoxBox( const float3& dim,
		const float4x4& otherTm, const float3& otherDim );

	/**
	 * Tests if two vertical cylinders overlap.
	 * @param bot1 bottom vertex of first cylinder
	 * @param height1 height of first cylinder
	 * @param radius1 radius of first cylinder
	 * @param bot2 bottom vertex of second cylinder
	 * @param height2 height of second cylinder
	 * @param radius2 radius of second cylinder
	 */
	static bool testVerticalCylinderCylinder( const float3& bot1, float height1, float radius1, 
		const float3& bot2, float height2, float radius2 );
};


END_NAMESPACE() // math


#endif // _MATH_INTERSECTIONUTIL_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
