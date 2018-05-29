#include <math/float3.h>
#include <math/float2.h>
#include <float.h>
#include <config.h>
#include <lang/assert.h>

BEGIN_NAMESPACE(math) 


float3::float3( const float2& xy0, float z0 ) :
	x(xy0.x),
	y(xy0.y),
	z(z0)
{
}

float3 float3::rotate( const float3& axis, float angle ) const
{
	assert( axis.length() > FLT_MIN ); // ensure that Axis isn't null vector

	const float3			vector		= *this;
	float3					unitaxis	= normalize( axis );
	const float				halfangle	= angle/float(2);
	const float				s			= sinf(halfangle);
	const float				c			= cosf(halfangle);
	const float				x			= unitaxis.x * s;
	const float				y			= unitaxis.y * s;
	const float				z			= unitaxis.z * s;
	const float				w			= c;
	const float				xx			= x*x;
	const float				xy 			= y*x;
	const float				xz			= z*x;
	const float				yy			= y*y;
	const float				yz			= z*y;
	const float				zz			= z*z;
	const float				wx			= w*x;
	const float				wy			= w*y;
	const float				wz			= w*z;
						
	const float				M[3][3]		=
	{
		{float(1)-float(2)*(yy+zz),		float(2)*(xy-wz),				float(2)*(xz+wy)},
		{float(2)*(xy+wz),				float(1)-float(2)*(xx+zz),      float(2)*(yz-wx)},
		{float(2)*(xz-wy),				float(2)*(yz+wx),				float(1)-float(2)*(xx+yy)},
	};

	return float3( 
		vector.x*M[0][0] + vector.y*M[0][1] + vector.z*M[0][2],
		vector.x*M[1][0] + vector.y*M[1][1] + vector.z*M[1][2],
		vector.x*M[2][0] + vector.y*M[2][1] + vector.z*M[2][2] );
}

bool float3::finite() const
{
	return 
		x >= -FLT_MAX && x <= FLT_MAX &&
		y >= -FLT_MAX && y <= FLT_MAX &&
		z >= -FLT_MAX && z <= FLT_MAX;
}

const float2& float3::xy() const
{
	return *reinterpret_cast<const float2*>(&x);
}

float3 normalize0( const float3& v )
{
	const float len = v.length();
	float invlen = 0.f; 
	if ( len >= FLT_MIN ) 
		invlen = 1.f / len; 
	return v * invlen;
}


END_NAMESPACE() // math

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
