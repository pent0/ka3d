#include <math/IntersectionUtil.h>
#include <math/float4x4.h>
#include <math.h>
#include <float.h>
#include <config.h>


BEGIN_NAMESPACE(math) 


bool IntersectionUtil::findRayPlaneIntersection( const float3& orig, 
	const float3& dir, const float4& plane, float* t )
{
	// orig is on the negative side of the plane?
	float sdist = orig.x*plane.x + orig.y*plane.y + orig.z*plane.z + plane.w;
	if ( sdist <= 0.f )
		return false;

	// ray pointing away from or along the plane?
	float d = dir.x * plane.x + dir.y * plane.y + dir.z * plane.z;
	if ( d > -FLT_MIN )
		return false;

	assert( fabsf(d) >= FLT_MIN );
	*t = -sdist / d;
	return true;
}

bool IntersectionUtil::findRaySphereIntersection(
	const float3& orig, const float3& dir,
	const float3& center, float radius,
	float* t )
{
	float3 origc = orig - center;
	float origDistSqr = dot( origc, origc );
	float radiusSqr = radius*radius;
	if ( origDistSqr <= radiusSqr )
	{
		*t = 0.f;
		return true;
	}

	float a = dot(dir,dir);
	if ( a >= FLT_MIN )
	{
		float b = 2.f * dot(origc,dir);
		float c = origDistSqr - radiusSqr;
		float d = b*b - 4.f*a*c;
		if ( d >= 0.f )
		{
			float dsqrt = sqrtf(d);
			float div = .5f / a;
			float t0 = ( -b - dsqrt )*div;
			//float t1 = ( -b + dsqrt )*div;
			if ( t0 >= 0.f )
			{
				if ( t )
					*t = t0;
				return true;
			}
		}
	}
	return false;
}

bool IntersectionUtil::findRayTriangleIntersection( 
	const float3& orig, const float3& dir,
	const float3& vert0, const float3& vert1, const float3& vert2,
	float* t )
{
	// find vectors for two edges sharing vert0
	float3 edge1 = vert1 - vert0;
	float3 edge2 = vert2 - vert0;

	// begin calculating determinant - also used to calculate U parameter
	float3 pvec = cross( dir, edge2 );

	// if determinant is near zero, ray lies in plane of triangle
	float det = dot( edge1, pvec );

	// the culling branch
	if ( fabsf(det) <= FLT_MIN )
		return false;
	float invdet = 1.f / det;

	// calculate distance from vert0 to ray orig
	float3 tvec = orig - vert0;

	// calculate U parameter and test bounds
	float u = dot(tvec,pvec) * invdet;
	if ( u < 0.f || u > 1.f )
		return false;

	// prepare to test V parameter
	float3 qvec = cross( tvec, edge1 );

	// calculate V parameter and test bounds
	float v = dot(dir,qvec) * invdet;
	if ( v < 0.f || u + v > 1.f )
		return false;

	// calculate s, ray intersects triangle
	float s = dot(edge2,qvec) * invdet;
	if ( s < 0.f )
		return false;

	*t = s;
	return true;
}

bool IntersectionUtil::testPointBox( const float3& point, const float4x4& box, const float3& dim )
{
	float3 d = point - box.translation();
	for ( int i = 0 ; i < 3 ; ++i )
	{
		float ad = dot( d, float3(box(0,i),box(1,i),box(2,i)) );
		if ( fabsf(ad) > dim[i] )
			return false;
	}
	return true;
}

bool IntersectionUtil::testPointBox( const float3& point, const float4x4& box, const float3& dimMin, const float3& dimMax )
{
	float3 d = point - box.translation();
	for ( int i = 0 ; i < 3 ; ++i )
	{
		float ad = dot( d, float3(box(0,i),box(1,i),box(2,i)) );
		if ( ad < dimMin[i] || ad > dimMax[i] )
			return false;
	}
	return true;
}

bool IntersectionUtil::testLineBox( const float3& orig, const float3& delta, 
	const float4x4& box, const float3& dim )
{
	if ( testPointBox(orig,box,dim) )
		return true;

	float3 segdir = delta*.5f;
	float3 segmid = orig + segdir;
	float3 diff = segmid - box.translation();

	float w[3], d[3];
	for ( int i = 0 ; i < 3 ; ++i )
	{
		float3 axis( box(0,i), box(1,i), box(2,i) );
		w[i] = fabsf( dot(segdir,axis) );
		d[i] = fabsf( dot(diff,axis) );
		float r = dim[i] + w[i];
		if ( d[i] > r )
			return false;
	}

	float wb[3] = {w[2], w[2], w[2]};
	float wc[3] = {w[1], w[0], w[0]};
	float3 wxd = cross( segdir, diff );
	for ( int i = 0 ; i < 3 ; ++i )
	{
		float3 axis( box(0,i), box(1,i), box(2,i) );
		float d = fabsf( dot(wxd,axis) );
		float r = dim[1]*wb[2] + dim[2]*wc[1];
		if ( d > r )
			return false;
	}

	return true;
}

bool IntersectionUtil::testBoxBox( const float3& a,
	const float4x4& B, const float3& b )
{
	const float3 T( B(0,3), B(1,3), B(2,3) );
	float3x3 Bf;
	for ( int i = 0 ; i < 3 ; ++i )
		for ( int j = 0 ; j < 3 ; ++j )
			Bf(i,j) = fabsf( B(i,j) );

	// A1 x A2 = A0
	float t = fabsf( T[0] );
	if ( t > a[0] + b[0] * Bf(0,0) + b[1] * Bf(0,1) + b[2] * Bf(0,2) )
		return false;

	// B1 x B2 = B0
	float s = T[0]*B(0,0) + T[1]*B(1,0) + T[2]*B(2,0);
	t = fabsf(s);
	if ( t > b[0] + a[0] * Bf(0,0) + a[1] * Bf(1,0) + a[2] * Bf(2,0) )
		return false;
  
	// A2 x A0 = A1
	t = fabsf(T[1]);
	if ( t > a[1] + b[0] * Bf(1,0) + b[1] * Bf(1,1) + b[2] * Bf(1,2) )
		return false;

	// A0 x A1 = A2
	t = fabsf(T[2]);
	if ( t > a[2] + b[0] * Bf(2,0) + b[1] * Bf(2,1) + b[2] * Bf(2,2) )
		return false;

	// B2 x B0 = B1
	s = T[0]*B(0,1) + T[1]*B(1,1) + T[2]*B(2,1);
	t = fabsf(s);
	if ( t > b[1] + a[0] * Bf(0,1) + a[1] * Bf(1,1) + a[2] * Bf(2,1) )
		return false;

	// B0 x B1 = B2
	s = T[0]*B(0,2) + T[1]*B(1,2) + T[2]*B(2,2);
	t = fabsf(s);
	if ( t > b[2] + a[0] * Bf(0,2) + a[1] * Bf(1,2) + a[2] * Bf(2,2) )
		return false;

	// A0 x B0
	s = T[2] * B(1,0) - T[1] * B(2,0);
	t = fabsf(s);
	if ( t > a[1] * Bf(2,0) + a[2] * Bf(1,0) + b[1] * Bf(0,2) + b[2] * Bf(0,1) )
		return false;

	// A0 x B1
	s = T[2] * B(1,1) - T[1] * B(2,1);
	t = fabsf(s);
	if ( t > a[1] * Bf(2,1) + a[2] * Bf(1,1) + b[0] * Bf(0,2) + b[2] * Bf(0,0) )
		return false;

	// A0 x B2
	s = T[2] * B(1,2) - T[1] * B(2,2);
	t = fabsf(s);
	if ( t > a[1] * Bf(2,2) + a[2] * Bf(1,2) + b[0] * Bf(0,1) + b[1] * Bf(0,0) )
		return false;

	// A1 x B0
	s = T[0] * B(2,0) - T[2] * B(0,0);
	t = fabsf(s);
	if ( t > a[0] * Bf(2,0) + a[2] * Bf(0,0) + b[1] * Bf(1,2) + b[2] * Bf(1,1) )
		return false;

	// A1 x B1
	s = T[0] * B(2,1) - T[2] * B(0,1);
	t = fabsf(s);
	if ( t > a[0] * Bf(2,1) + a[2] * Bf(0,1) + b[0] * Bf(1,2) + b[2] * Bf(1,0) )
		return false;

	// A1 x B2
	s = T[0] * B(2,2) - T[2] * B(0,2);
	t = fabsf(s);
	if ( t > a[0] * Bf(2,2) + a[2] * Bf(0,2) + b[0] * Bf(1,1) + b[1] * Bf(1,0) )
		return false;

	// A2 x B0
	s = T[1] * B(0,0) - T[0] * B(1,0);
	t = fabsf(s);
	if ( t > a[0] * Bf(1,0) + a[1] * Bf(0,0) + b[1] * Bf(2,2) + b[2] * Bf(2,1) )
		return false;

	// A2 x B1
	s = T[1] * B(0,1) - T[0] * B(1,1);
	t = fabsf(s);
	if ( t > a[0] * Bf(1,1) + a[1] * Bf(0,1) + b[0] * Bf(2,2) + b[2] * Bf(2,0) )
		return false;

	// A2 x B2
	s = T[1] * B(0,2) - T[0] * B(1,2);
	t = fabsf(s);
	if ( t > a[0] * Bf(1,2) + a[1] * Bf(0,2) + b[0] * Bf(2,1) + b[1] * Bf(2,0) )
		return false;

	return true;
}

bool IntersectionUtil::findLineBoxIntersection( const float3& orig, const float3& delta,
	const float4x4& box, const float3& dim, float* t, float3* normal )
{
	if ( testPointBox(orig,box,dim) )
	{
		if ( normal )
			*normal = float3(0,0,0);

		if ( t )
			*t = 0.f;

		return true;
	}

	float3 nx = float3(box(0,0),box(1,0),box(2,0));
	float3 ny = float3(box(0,1),box(1,1),box(2,1));
	float3 nz = float3(box(0,2),box(1,2),box(2,2));
	float3 dx = nx * dim[0];
	float3 dy = ny * dim[1];
	float3 dz = nz * dim[2];
	float3 c = box.translation();

	float3 v[8] =
	{
		c-dx+dy-dz,
		c+dx+dy-dz,
		c+dx-dy-dz,
		c-dx-dy-dz,
		c-dx+dy+dz,
		c+dx+dy+dz,
		c+dx-dy+dz,
		c-dx-dy+dz,
	};

	float3 faces[24] =
	{
		// front
		v[0], v[1], v[2], v[3],
		// back
		v[5], v[4], v[7], v[6], 
		// right
		v[1], v[5], v[6], v[2],
		// left
		v[4], v[0], v[3], v[7],
		// top
		v[4], v[5], v[1], v[0],
		// bottom
		v[3], v[2], v[6], v[7]
	};

	float3 normals[6] =
	{
		-nz,
		nz,
		nx,
		-nx,
		ny,
		-ny,
	};

	float umin = 1.f;
	for ( int i = 0 ; i < 24 ; i += 4 )
	{
		float u = 0.f;
		if ( ( findRayTriangleIntersection( orig, delta, faces[i], faces[i+1], faces[i+2], &u ) && u < umin ) ||
			( findRayTriangleIntersection( orig, delta, faces[i], faces[i+2], faces[i+3], &u ) && u < umin ) )
		{
			umin = u;
			if ( normal )
				*normal = normals[i/4];
		}
	}

	if ( t )
		*t = umin;
	return umin < 1.f;
}

bool IntersectionUtil::findLineBoxIntersection( const float3& orig, const float3& delta,
	const float4x4& box, const float3& dimMin, const float3& dimMax, float* t, float3* normal )
{
	if ( testPointBox(orig,box,dimMin,dimMax) )
	{
		if ( normal )
			*normal = float3(0,0,0);

		if ( t )
			*t = 0.f;

		return true;
	}

	float3 nx = float3(box(0,0),box(1,0),box(2,0));
	float3 ny = float3(box(0,1),box(1,1),box(2,1));
	float3 nz = float3(box(0,2),box(1,2),box(2,2));
	float3 dxn = nx * dimMin[0];
	float3 dyn = ny * dimMin[1];
	float3 dzn = nz * dimMin[2];
	float3 dxp = nx * dimMax[0];
	float3 dyp = ny * dimMax[1];
	float3 dzp = nz * dimMax[2];
	float3 c = box.translation();

	float3 v[8] =
	{
		c+dxn+dyp+dzn,
		c+dxp+dyp+dzn,
		c+dxp+dyn+dzn,
		c+dxn+dyn+dzn,
		c+dxn+dyp+dzp,
		c+dxp+dyp+dzp,
		c+dxp+dyn+dzp,
		c+dxn+dyn+dzp,
	};

	float3 faces[24] =
	{
		// front
		v[0], v[1], v[2], v[3],
		// back
		v[5], v[4], v[7], v[6], 
		// right
		v[1], v[5], v[6], v[2],
		// left
		v[4], v[0], v[3], v[7],
		// top
		v[4], v[5], v[1], v[0],
		// bottom
		v[3], v[2], v[6], v[7]
	};

	float3 normals[6] =
	{
		-nz,
		nz,
		nx,
		-nx,
		ny,
		-ny,
	};

	float umin = 1.f;
	for ( int i = 0 ; i < 24 ; i += 4 )
	{
		float u = 0.f;
		if ( ( findRayTriangleIntersection( orig, delta, faces[i], faces[i+1], faces[i+2], &u ) && u < umin ) ||
			( findRayTriangleIntersection( orig, delta, faces[i], faces[i+2], faces[i+3], &u ) && u < umin ) )
		{
			umin = u;
			if ( normal )
				*normal = normals[i/4];
		}
	}

	if ( t )
		*t = umin;
	return umin < 1.f;
}

bool IntersectionUtil::findRayCylinderIntersection(
	const float3& orig, const float3& dir,
	const float3& center, const float3& axis, float radius, float halfHeight,
	float* t, float3* normal )
{
	assert( orig.finite() );
	assert( dir.finite() );
	assert( axis.finite() && fabs(axis.length()-1.f) < 1e-3f );

	// solution type
	enum SolType
	{
		SOL_NONE,
		SOL_INSIDE,
		SOL_CAP,
		SOL_WALL,
	};

	// setup
	const float eps = 1e-10f;
	float radiusSqr = radius * radius;
	float sol[2] = {-1.f,-1.f}; // solutions relative to world space dir length
	SolType solType[2] = {SOL_NONE,SOL_NONE};
	int solCount = 0;

	// generate cylinder coordinate space
	float3x3 cylRot;
	cylRot.generateOrthonormalBasisFromZ( axis );
	//float4x4 cylTmInv; cylTmInv.setInverseOrthonormalTransform( cylRot, center );
	float4x4 cylTm( 1.f ); cylTm.setRotation( cylRot ); cylTm.setTranslation( center ); float4x4 cylTmInv = cylTm.inverse();

	// transform ray to cylinder space
	float3 origC = cylTmInv.transform( orig );
	float3 dirC = cylTmInv.rotate( dir );

	// start point inside cylinder?
	bool origInInfCyl = origC.x*origC.x + origC.y*origC.y <= radiusSqr;
	bool origBetweenCaps = fabsf(origC.z) <= halfHeight;
	if ( origBetweenCaps && origInInfCyl )
	{
		// case 1: startpoint inside
		solType[solCount] = SOL_INSIDE;
		sol[solCount++] = 0.f;
	}
	else
	{
		// valid direction?
		float dirLen = dirC.length();
		if ( dirLen >= eps )
		{
			float dirLenInv = 1.f / dirLen;
			dirC *= dirLenInv;

			// direction parallel to cylinder?
			if ( fabsf(dirC.z) >= 1.f-eps )
			{
				if ( origInInfCyl )
				{
					// case 2: intersects cap(s), direction parallel
					float tmp = dirLenInv / dirC.z;
					solType[solCount] = SOL_CAP;
					sol[solCount++] = (halfHeight - origC.z) * tmp;
					solType[solCount] = SOL_CAP;
					sol[solCount++] = (-halfHeight - origC.z) * tmp;
				}
			}
			else
			{
				// direction perpendicular to cylinder?
				if ( fabsf(dirC.z) <= eps )
				{
					// inside caps?
					if ( origBetweenCaps )
					{
						// intersects origin centered circle?
						float a = dirC.x*dirC.x + dirC.y*dirC.y;
						float b = 2.f * (origC.x*dirC.x + origC.y*dirC.y);
						float c = origC.x*origC.x + origC.y*origC.y - radiusSqr;
						float discr = b*b - 4.f*a*c;
						if ( discr > FLT_MIN )
						{
							// case 3: intersects wall(s), direction perpendicular
							float root = sqrtf( discr );
							float tmp = dirLenInv / (2.f*a);
							solType[solCount] = SOL_WALL;
							sol[solCount++] = (-b + root) * tmp;
							solType[solCount] = SOL_WALL;
							sol[solCount++] = (-b - root) * tmp;
						}
					}
				}
				else
				{
					// test cap plane intersections
					assert( 0 == solCount );
					float inv = 1.f / dirC.z;
					float u0 = (+halfHeight - origC.z)*inv;
					float tmp0 = origC.x + u0*dirC.x;
					float tmp1 = origC.y + u0*dirC.y;
					if ( tmp0*tmp0 + tmp1*tmp1 <= radiusSqr )
					{
						// case 4: intersects cap(s)
						solType[solCount] = SOL_CAP;
						sol[solCount++] = u0*dirLenInv;
					}

					float u1 = (-halfHeight - origC.z)*inv;
					tmp0 = origC.x + u1*dirC.x;
					tmp1 = origC.y + u1*dirC.y;
					if ( tmp0*tmp0 + tmp1*tmp1 <= radiusSqr )
					{
						// case 4: intersects cap(s)
						solType[solCount] = SOL_CAP;
						sol[solCount++] = u1*dirLenInv;
					}

					// ray does not intersects both end caps?
					if ( solCount < 2 )
					{
						// intersects origin centered circle?
						float a = dirC.x*dirC.x + dirC.y*dirC.y;
						float b = origC.x*dirC.x + origC.y*dirC.y;
						float c = origC.x*origC.x + origC.y*origC.y - radiusSqr;
						float discr = b*b - a*c;
						if ( discr > FLT_MIN )
						{
							// case 5: intersects wall(s)
							float root = sqrtf( discr );
							float tmp = 1.f / a;
							float u = (-b - root) * tmp;
							if ( u0 <= u1 )
							{
								if ( u0 <= u && u <= u1 )
								{
									solType[solCount] = SOL_WALL;
									sol[solCount++] = u * dirLenInv;
								}
							}
							else
							{
								if ( u1 <= u && u <= u0 )
								{
									solType[solCount] = SOL_WALL;
									sol[solCount++] = u * dirLenInv;
								}
							}

							// max 2 intersections, solve second only if it is possible
							if ( solCount < 2 )
							{
								float u = (-b + root) * tmp;
								if ( u0 <= u1 )
								{
									if ( u0 <= u && u <= u1 )
									{
										solType[solCount] = SOL_WALL;
										sol[solCount++] = u * dirLenInv;
									}
								}
								else
								{
									if ( u1 <= u && u <= u0 )
									{
										solType[solCount] = SOL_WALL;
										sol[solCount++] = u * dirLenInv;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// store closest intersection
	float u = FLT_MAX; // relative to world space dir
	SolType intersectionType = SOL_NONE;
	for ( int i = 0 ; i < solCount ; ++i )
	{
		if ( sol[i] >= 0.f )
		{
			if ( sol[i] < u )
			{
				u = sol[i];
				intersectionType = solType[i];
			}
		}
	}

	// compute normal at intersection point
	if ( normal && intersectionType != SOL_NONE )
	{
		float3 ipointC = origC + cylTmInv.rotate( dir*u );
		SolType itype = intersectionType;

		// inside -> wall or cap normal, which one is closer
		if ( itype == SOL_INSIDE )
		{
			float capDist = fabsf( fabsf(ipointC.z) - halfHeight );
			float cylDist = fabsf( sqrtf(ipointC.x*ipointC.x + ipointC.y*ipointC.y) - radius );
			if ( capDist < cylDist )
				itype = SOL_CAP;
			else
				itype = SOL_WALL;
		}
		
		if ( itype == SOL_CAP )
		{
			if ( ipointC.z < 0.f )
				*normal = -axis;
			else
				*normal = axis;
		}
		else if ( itype == SOL_WALL )
		{
			float3 inormal = float3(ipointC.x,ipointC.y,0);
			float inormalLen = inormal.length();
			if ( inormalLen < FLT_MIN )
				*normal = axis;
			else
				*normal = ( cylRot * inormal ) * (1.f/inormalLen);
		}
	}

	// store ray length to closest intersection
	if ( t )
		*t = u;
	return intersectionType != SOL_NONE;
}

bool IntersectionUtil::findLineCylinderIntersection(
	const float3& orig, const float3& delta,
	const float3& center, const float3& axis, float radius, float halfHeight,
	float* t, float3* normal )
{
	float u = 1.f;
	bool intersects = findRayCylinderIntersection( orig, delta, center, axis, radius, halfHeight, &u, normal );
	intersects = ( intersects && u < 1.f );
	if ( intersects && t )
		*t = u;
	return intersects;
}

bool IntersectionUtil::findLinePlaneIntersection(
	const float3& orig, const float3& delta,
	const float4& plane, float* t )
{
	float u = 1.f;
	bool intersects = findRayPlaneIntersection( orig, delta, plane, &u );
	intersects = ( intersects && u < 1.f );
	if ( intersects && t )
		*t = u;
	return intersects;
}

bool IntersectionUtil::findLineTriangleIntersection(
	const float3& orig, const float3& delta,
	const float3& vert0, const float3& vert1, const float3& vert2,
	float* t )
{
	float u = 1.f;
	bool intersects = findRayTriangleIntersection( orig, delta, vert0, vert1, vert2, &u );
	intersects = ( intersects && u < 1.f );
	if ( intersects && t )
		*t = u;
	return intersects;
}

bool IntersectionUtil::findLineSphereIntersection(
	const float3& orig, const float3& delta,
	const float3& center, float radius,
	float* t )
{
	float u = 1.f;
	bool intersects = findRaySphereIntersection( orig, delta, center, radius, &u );
	intersects = ( intersects && u < 1.f );
	if ( intersects && t )
		*t = u;
	return intersects;
}

/*
	// test line segment against radius shifted polygon
	Vector3 planeNormal( plane.x, plane.y, plane.z );
	Vector3 shiftDelta = planeNormal * -r;
	Vector3 startShifted = start + shiftDelta;
	float u;
	if ( IntersectionUtil::findLinePlaneIntersection(start, shiftDelta, plane, &u) )
	{
		// already inside polygon
		Vector3 intersectpoint = start + shiftDelta*u;
		if ( poly->isPointInPolygon(intersectpoint) )
		{
			setMovingSphereCollisionInfo( start, delta, 0.f, poly, planeNormal, intersectpoint, cinfo, t );
			continue;
		}
	}
	else if ( IntersectionUtil::findLinePlaneIntersection(startShifted, delta, plane, &u) && u <= *t )
	{
		Vector3 intersectpoint = startShifted + delta*u;
		if ( poly->isPointInPolygon(intersectpoint) )
		{
			setMovingSphereCollisionInfo( start, delta, u, poly, planeNormal, intersectpoint, cinfo, t );
			continue;
		}
	}

	// test line segment against vertex r-spheres
	for ( int i = 0 ; i < poly->vertices() ; ++i )
	{
		const Vector3& vert = poly->getVertex(i);
		if ( IntersectionUtil::findLineSphereIntersection(start, delta, vert, r, &u) && u <= *t )
		{
			Vector3 iposition = start + delta*u;
			Vector3 inormal = (iposition - vert).normalize();
			Vector3 intersectpoint; 
			if ( u == 0.f )
				intersectpoint = vert;
			else
				intersectpoint = iposition - inormal*r;
			setMovingSphereCollisionInfo( start, delta, u, poly, inormal, intersectpoint, cinfo, t );
		}
	}

	// test line segment against edge (k,i) r-cylinders
	const float MIN_EDGE_LEN = 1e-12f;
	int k = poly->vertices() - 1;
	for ( int i = 0 ; i < poly->vertices() ; k = i++ )
	{
		Vector3 e0 = poly->getVertex(k);
		Vector3 e1 = poly->getVertex(i);
		Vector3 cylCenter = (e0+e1)*.5f;
		Vector3 cylAxis (e1-e0);
		float cylLen = cylAxis.length();
		if ( cylLen > MIN_EDGE_LEN )
		{
			Vector3 inormal(0,0,0);
			cylAxis *= 1.f / cylLen;
			if ( IntersectionUtil::findLineCylinderIntersection(start, delta, cylCenter, cylAxis, r, cylLen*.5f, &u, &inormal) && u <= *t )
			{ 
				Vector3 iposition = start + delta*u;
				Vector3 intersectpoint;
				float ndotp;
				if ( u == 0.f )
				{
					ndotp = inormal.dot(iposition-cylCenter);
					intersectpoint = iposition - inormal*ndotp;
				}
				else
				{
					intersectpoint = iposition - inormal*r;
				}
				setMovingSphereCollisionInfo( start, delta, u, poly, inormal, intersectpoint, cinfo, t );
			}
		}
	}
*/

END_NAMESPACE() // math
