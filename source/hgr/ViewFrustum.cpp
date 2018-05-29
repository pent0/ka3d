#include <hgr/ViewFrustum.h>
#include <lang/Math.h>
#include <lang/Debug.h>
#include <math/float3x4.h>
#include <config.h>


#define MIN_FOV 1.f
#define MAX_FOV 179.f
#define MIN_FRONT 1e-3f
#define MAX_BACK 1e6f


USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(hgr) 


ViewFrustum::ViewFrustum() :
	m_aspect( 1.33f ),
	m_front( 1.f ),
	m_back( 10000.f ),
	m_vertFov( getVerticalFov(1.57f,10000.f,1.33f) )
{
}

void ViewFrustum::setVerticalFov( float fovy )
{
	assert( fovy >= Math::toRadians(MIN_FOV) && fovy <= Math::toRadians(MAX_FOV) );

	m_vertFov = fovy;
}

void ViewFrustum::setHorizontalFov( float fovx )
{
	assert( fovx >= Math::toRadians(MIN_FOV) && fovx <= Math::toRadians(MAX_FOV) );

	m_vertFov = getVerticalFov( fovx, m_front, m_aspect );
}

void ViewFrustum::setFront( float front )
{
	assert( front >= MIN_FRONT && front <= MAX_BACK );
	
	m_front = front;
}

void ViewFrustum::setBack( float back )
{
	assert( back >= MIN_FRONT && back <= MAX_BACK );

	m_back = back;
}

void ViewFrustum::setAspect( float aspect )
{
	assert( aspect > 0.1f && aspect < 10.f );
	m_aspect = aspect;
}

void ViewFrustum::getViewDimensions( float* x, float* y, float* z ) const
{
    float w = 1.f / Math::tan( horizontalFov() * .5f );
	if ( w < 0.f )
		w = -w;

	float vw = 2.f * m_front / w;
	float vh = vw * (1.f/m_aspect);

	*x = vw;
	*y = vh;
	*z = m_front;
}

void ViewFrustum::getPlanes( const float3x4& tm, float4* planes ) const
{
	float halffovhorz = horizontalFov() * 0.5f;
	float halffovvert = verticalFov() * 0.5f;
	float3 rightout = tm.rotate( float3(1,0,0).rotate( float3(0,1,0), halffovhorz ) );
	float3 leftout = tm.rotate( float3(-1,0,0).rotate( float3(0,1,0), -halffovhorz ) );
	float3 topout = tm.rotate( float3(0,1,0).rotate( float3(1,0,0), -halffovvert ) );
	float3 bottomout = tm.rotate( float3(0,-1,0).rotate( float3(1,0,0), halffovvert ) );
	float3 orig = tm.translation();
	float3 z = tm.getColumn(2);

	assert( PLANE_COUNT == 6 );
	planes[PLANE_LEFT].setPlane( leftout, orig );
	planes[PLANE_RIGHT].setPlane( rightout, orig );
	planes[PLANE_NEAR].setPlane( -z, orig+z*m_front );
	planes[PLANE_FAR].setPlane( z, orig+z*m_back );
	planes[PLANE_TOP].setPlane( topout, orig );
	planes[PLANE_BOTTOM].setPlane( bottomout, orig );
}

bool ViewFrustum::testOBox( const float3x4& boxtm,
	const float3& boxmin, const float3& boxmax, const float4* planes, int& hint )
{
	if ( hint < 0 || hint >= PLANE_COUNT )
		hint = 0;

	float4 points[8] =
	{
		// boxmin boxmin boxmin
		float4( 
			boxtm(0,3) + boxtm(0,0) * boxmin.x + boxtm(0,1) * boxmin.y + boxtm(0,2) * boxmin.z,
			boxtm(1,3) + boxtm(1,0) * boxmin.x + boxtm(1,1) * boxmin.y + boxtm(1,2) * boxmin.z,
			boxtm(2,3) + boxtm(2,0) * boxmin.x + boxtm(2,1) * boxmin.y + boxtm(2,2) * boxmin.z,
			1.f ),

		// boxmin boxmin boxmax
		float4(
			boxtm(0,3) + boxtm(0,0) * boxmin.x + boxtm(0,1) * boxmin.y + boxtm(0,2) * boxmax.z,
			boxtm(1,3) + boxtm(1,0) * boxmin.x + boxtm(1,1) * boxmin.y + boxtm(1,2) * boxmax.z,
			boxtm(2,3) + boxtm(2,0) * boxmin.x + boxtm(2,1) * boxmin.y + boxtm(2,2) * boxmax.z,
			1.f ),

		// boxmin boxmax boxmin
		float4( 
			boxtm(0,3) + boxtm(0,0) * boxmin.x + boxtm(0,1) * boxmax.y + boxtm(0,2) * boxmin.z,
			boxtm(1,3) + boxtm(1,0) * boxmin.x + boxtm(1,1) * boxmax.y + boxtm(1,2) * boxmin.z,
			boxtm(2,3) + boxtm(2,0) * boxmin.x + boxtm(2,1) * boxmax.y + boxtm(2,2) * boxmin.z,
			1.f ),

		// boxmin boxmax boxmax
		float4( 
			boxtm(0,3) + boxtm(0,0) * boxmin.x + boxtm(0,1) * boxmax.y + boxtm(0,2) * boxmax.z,
			boxtm(1,3) + boxtm(1,0) * boxmin.x + boxtm(1,1) * boxmax.y + boxtm(1,2) * boxmax.z,
			boxtm(2,3) + boxtm(2,0) * boxmin.x + boxtm(2,1) * boxmax.y + boxtm(2,2) * boxmax.z,
			1.f ),

		// boxmax boxmin boxmin
		float4( 
			boxtm(0,3) + boxtm(0,0) * boxmax.x + boxtm(0,1) * boxmin.y + boxtm(0,2) * boxmin.z,
			boxtm(1,3) + boxtm(1,0) * boxmax.x + boxtm(1,1) * boxmin.y + boxtm(1,2) * boxmin.z,
			boxtm(2,3) + boxtm(2,0) * boxmax.x + boxtm(2,1) * boxmin.y + boxtm(2,2) * boxmin.z,
			1.f ),

		// boxmax boxmin boxmax
		float4( 
			boxtm(0,3) + boxtm(0,0) * boxmax.x + boxtm(0,1) * boxmin.y + boxtm(0,2) * boxmax.z,
			boxtm(1,3) + boxtm(1,0) * boxmax.x + boxtm(1,1) * boxmin.y + boxtm(1,2) * boxmax.z,
			boxtm(2,3) + boxtm(2,0) * boxmax.x + boxtm(2,1) * boxmin.y + boxtm(2,2) * boxmax.z,
			1.f ),

		// boxmax boxmax boxmin
		float4( 
			boxtm(0,3) + boxtm(0,0) * boxmax.x + boxtm(0,1) * boxmax.y + boxtm(0,2) * boxmin.z,
			boxtm(1,3) + boxtm(1,0) * boxmax.x + boxtm(1,1) * boxmax.y + boxtm(1,2) * boxmin.z,
			boxtm(2,3) + boxtm(2,0) * boxmax.x + boxtm(2,1) * boxmax.y + boxtm(2,2) * boxmin.z,
			1.f ),

		// boxmax boxmax boxmax
		float4( 
			boxtm(0,3) + boxtm(0,0) * boxmax.x + boxtm(0,1) * boxmax.y + boxtm(0,2) * boxmax.z,
			boxtm(1,3) + boxtm(1,0) * boxmax.x + boxtm(1,1) * boxmax.y + boxtm(1,2) * boxmax.z,
			boxtm(2,3) + boxtm(2,0) * boxmax.x + boxtm(2,1) * boxmax.y + boxtm(2,2) * boxmax.z,
			1.f ),
	};

	for ( int i = hint ; i < PLANE_COUNT ; ++i )
	{
		int out = 0;
		for ( int k = 0 ; k < 8 ; ++k )
			out += ( dot(points[k],planes[i]) > 0.f ? 1 : 0 );
		if ( 8 == out )
		{
			hint = i;
			return false;
		}
	}
	for ( int i = 0 ; i < hint ; ++i )
	{
		int out = 0;
		for ( int k = 0 ; k < 8 ; ++k )
			out += ( dot(points[k],planes[i]) > 0.f ? 1 : 0 );
		if ( 8 == out )
		{
			hint = i;
			return false;
		}
	}
	return true;
}

bool ViewFrustum::testSphere( const float3x4& spheretm, float radius,
	const float4* planes, int& hint )
{
	float scale = dot( spheretm.getColumn(0), spheretm.getColumn(0) );
	float scale2 = dot( spheretm.getColumn(1), spheretm.getColumn(1) );
	float scale3 = dot( spheretm.getColumn(2), spheretm.getColumn(2) );
	if ( fabsf(scale-1.f) > 1e-5f || fabsf(scale2-1.f) > 1e-5f || fabsf(scale3-1.f) > 1e-5f )
	{
		if ( scale2 > scale )
			scale = scale2;
		if ( scale3 > scale )
			scale = scale3;
		scale = Math::sqrt( scale );
		radius *= scale;
	}

	float3 cp = spheretm.translation();

	for ( int i = hint ; i < PLANE_COUNT ; ++i )
	{
		const float4& pl = planes[i];
		if ( cp.x*pl.x+cp.y*pl.y+cp.z*pl.z+pl.w > radius )
		{
			hint = i;
			return false;
		}
	}
	for ( int i = 0 ; i < hint ; ++i )
	{
		const float4& pl = planes[i];
		if ( cp.x*pl.x+cp.y*pl.y+cp.z*pl.z+pl.w > radius )
		{
			hint = i;
			return false;
		}
	}
	return true;
}

bool ViewFrustum::testAABox( const float3& boxmin, const float3& boxmax, 
	const float4* planes )
{
	// TODO: optimize
	for ( int i = 0 ; i < PLANE_COUNT ; ++i )
	{
		float4 p; p.w = 1.f;
		int out = 0;

		// boxmin boxmin boxmin
		p.x = boxmin.x;
		p.y = boxmin.y;
		p.z = boxmin.z;
		out += ( dot(p,planes[i]) > 0.f ? 1 : 0 );

		// boxmin boxmin boxmax
		p.x = boxmin.x;
		p.y = boxmin.y;
		p.z = boxmax.z;
		out += ( dot(p,planes[i]) > 0.f ? 1 : 0 );

		// boxmin boxmax boxmin
		p.x = boxmin.x;
		p.y = boxmax.y;
		p.z = boxmin.z;
		out += ( dot(p,planes[i]) > 0.f ? 1 : 0 );

		// boxmin boxmax boxmax
		p.x = boxmin.x;
		p.y = boxmax.y;
		p.z = boxmax.z;
		out += ( dot(p,planes[i]) > 0.f ? 1 : 0 );

		// boxmax boxmin boxmin
		p.x = boxmax.x;
		p.y = boxmin.y;
		p.z = boxmin.z;
		out += ( dot(p,planes[i]) > 0.f ? 1 : 0 );

		// boxmax boxmin boxmax
		p.x = boxmax.x;
		p.y = boxmin.y;
		p.z = boxmax.z;
		out += ( dot(p,planes[i]) > 0.f ? 1 : 0 );

		// boxmax boxmax boxmin
		p.x = boxmax.x;
		p.y = boxmax.y;
		p.z = boxmin.z;
		out += ( dot(p,planes[i]) > 0.f ? 1 : 0 );

		// boxmax boxmax boxmax
		p.x = boxmax.x;
		p.y = boxmax.y;
		p.z = boxmax.z;
		out += ( dot(p,planes[i]) > 0.f ? 1 : 0 );

		if ( 8 == out )
			return false;
	}
	return true;
}

float ViewFrustum::getVerticalFov( float fovx, float front, float aspect )
{
    float w = 1.f / Math::tan( fovx * .5f );
	if ( w < 0.f )
		w = -w;
	float vx = 2.f * front / w;
	float vy = vx * (1.f/aspect);

	float tana = (vy*.5f) / front;
	return 2.f * Math::atan( tana );
}

float ViewFrustum::getHorizontalFov( float fovy, float front, float aspect )
{
	assert( front > 0.f );
	assert( aspect > 0.f );
	assert( fovy > 0.f );

	float h = 1.f / Math::tan( fovy * .5f );
	if ( h < 0.f )
		h = -h;
	float vh = 2.f * front / h;
	float vw = vh * aspect;

	float tana = (vw*.5f) / front;
	return 2.f * Math::atan( tana );
}

float ViewFrustum::horizontalFov() const
{
	return getHorizontalFov( m_vertFov, m_front, m_aspect );
}

float ViewFrustum::verticalFov() const
{
	return m_vertFov;
}


END_NAMESPACE() // hgr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
