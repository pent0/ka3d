#include <math/float4.h>
#include <math/float3.h>
#include <math/float2.h>
#include <float.h>
#include <config.h>


BEGIN_NAMESPACE(math) 


float4::float4( const float3& xyz0, float w0 ) :
	x(xyz0.x),
	y(xyz0.y),
	z(xyz0.z),
	w(w0)
{
}

float4::float4( const float2& xy0, float z0, float w0 ) :
	x(xy0.x),
	y(xy0.y),
	z(z0),
	w(w0)
{
}

void float4::setPlane( const float3& normal, const float3& point )
{
	x = normal.x;
	y = normal.y;
	z = normal.z;
	w = -dot( normal, point );
}

bool float4::finite() const
{
	return 
		x >= -FLT_MAX && x <= FLT_MAX &&
		y >= -FLT_MAX && y <= FLT_MAX &&
		z >= -FLT_MAX && z <= FLT_MAX &&
		w >= -FLT_MAX && w <= FLT_MAX;
}

const float3& float4::xyz() const
{
	return *reinterpret_cast<const float3*>(&x);
}

const float2& float4::xy() const
{
	return *reinterpret_cast<const float2*>(&x);
}

float4 normalize0( const float4& v )
{
	const float len = v.length();
	float invlen = 0.f; 
	if ( len >= FLT_MIN ) 
		invlen = 1.f / len; 
	return v * invlen;
}


END_NAMESPACE() // math

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
