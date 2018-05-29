#include <math/float3x4.h>
#include <math/float4.h>
#include <math/float3.h>
#include <math/quaternion.h>
#include <math/float3x3.h>
#include <math/float4x4.h>
#include <float.h>
#include <config.h>


#ifdef __ee__
#error Use ps2/float3x4.cpp for VU0-assembly implementation
#endif


BEGIN_NAMESPACE(math) 


float3x4::float3x4( float diagonal )
{
	m[0][0] = diagonal;
	m[0][1] = 0.f;
	m[0][2] = 0.f;
	m[0][3] = 0.f;
	m[1][0] = 0.f;
	m[1][1] = diagonal;
	m[1][2] = 0.f;
	m[1][3] = 0.f;
	m[2][0] = 0.f;
	m[2][1] = 0.f;
	m[2][2] = diagonal;
	m[2][3] = 0.f;
}

float3x4::float3x4( const float3x3& rot, const float3& translation )				
{
	m[0][0] = rot(0,0);
	m[0][1] = rot(0,1);
	m[0][2] = rot(0,2);
	m[0][3] = translation.x;

	m[1][0] = rot(1,0);
	m[1][1] = rot(1,1);
	m[1][2] = rot(1,2);
	m[1][3] = translation.y;

	m[2][0] = rot(2,0);
	m[2][1] = rot(2,1);
	m[2][2] = rot(2,2);
	m[2][3] = translation.z;
}

float3x4::float3x4( const float3x3& rot, const float3& translation, const float3& scale )
{
	m[0][0] = rot(0,0) * scale.x;
	m[0][1] = rot(0,1) * scale.y;
	m[0][2] = rot(0,2) * scale.z;
	m[0][3] = translation.x;

	m[1][0] = rot(1,0) * scale.x;
	m[1][1] = rot(1,1) * scale.y;
	m[1][2] = rot(1,2) * scale.z;
	m[1][3] = translation.y;

	m[2][0] = rot(2,0) * scale.x;
	m[2][1] = rot(2,1) * scale.y;
	m[2][2] = rot(2,2) * scale.z;
	m[2][3] = translation.z;
}

float3x4::float3x4( const quaternion& q, const float3& translation )				
{
	setRotation( q );

	m[0][3] = translation.x;
	m[1][3] = translation.y;
	m[2][3] = translation.z;
}

float3x4::float3x4( const quaternion& q, const float3& translation, const float3& scale )
{
	setRotation( q );

	m[0][0] *= scale.x;
	m[0][1] *= scale.y;
	m[0][2] *= scale.z;
	m[0][3] = translation.x;

	m[1][0] *= scale.x;
	m[1][1] *= scale.y;
	m[1][2] *= scale.z;
	m[1][3] = translation.y;

	m[2][0] *= scale.x;
	m[2][1] *= scale.y;
	m[2][2] *= scale.z;
	m[2][3] = translation.z;
}

float3x4::float3x4( const float4x4& o )
{
	m[0][0] = o(0,0);
	m[0][1] = o(0,1);
	m[0][2] = o(0,2);
	m[0][3] = o(0,3);
	m[1][0] = o(1,0);
	m[1][1] = o(1,1);
	m[1][2] = o(1,2);
	m[1][3] = o(1,3);
	m[2][0] = o(2,0);
	m[2][1] = o(2,1);
	m[2][2] = o(2,2);
	m[2][3] = o(2,3);
}

float3x4 float3x4::operator*( const float3x4& other ) const
{
	float3x4 r;

	#define MATRIX3X4MUL_x(j,i) \
		r.m[j][i] = m[j][0]*other.m[0][i] +	\
					m[j][1]*other.m[1][i] + \
					m[j][2]*other.m[2][i];

	#define MATRIX3X4MUL_3(j,i) \
		r.m[j][i] = m[j][0]*other.m[0][i] +	\
					m[j][1]*other.m[1][i] + \
					m[j][2]*other.m[2][i] +	\
					m[j][3];

	MATRIX3X4MUL_x(0,0)
	MATRIX3X4MUL_x(0,1)
	MATRIX3X4MUL_x(0,2)
	MATRIX3X4MUL_3(0,3)
	MATRIX3X4MUL_x(1,0)
	MATRIX3X4MUL_x(1,1)
	MATRIX3X4MUL_x(1,2)
	MATRIX3X4MUL_3(1,3)
	MATRIX3X4MUL_x(2,0)
	MATRIX3X4MUL_x(2,1)
	MATRIX3X4MUL_x(2,2)
	MATRIX3X4MUL_3(2,3)

	return r;
}

void float3x4::setTranslation( const float3& t )
{
	m[0][3] = t.x;
	m[1][3] = t.y;
	m[2][3] = t.z;
}

void float3x4::setRotation( const quaternion& q )
{
	float len = q.norm();
	assert( len > FLT_MIN );
	float d = 2.f / len;

    float tx  = d*q.x;
    float ty  = d*q.y;
    float tz  = d*q.z;
    float twx = tx*q.w;
    float twy = ty*q.w;
    float twz = tz*q.w;
    float txx = tx*q.x;
    float txy = ty*q.x;
    float txz = tz*q.x;
    float tyy = ty*q.y;
    float tyz = tz*q.y;
    float tzz = tz*q.z;

    m[0][0] = 1.f-(tyy+tzz);
    m[0][1] = txy-twz;
    m[0][2] = txz+twy;
    m[1][0] = txy+twz;
    m[1][1] = 1.f-(txx+tzz);
    m[1][2] = tyz-twx;
    m[2][0] = txz-twy;
    m[2][1] = tyz+twx;
    m[2][2] = 1.f-(txx+tyy);
}

void float3x4::setRotation( const float3x3& rot )
{
	m[0][0] = rot(0,0);
	m[0][1] = rot(0,1);
	m[0][2] = rot(0,2);

	m[1][0] = rot(1,0);
	m[1][1] = rot(1,1);
	m[1][2] = rot(1,2);

	m[2][0] = rot(2,0);
	m[2][1] = rot(2,1);
	m[2][2] = rot(2,2);
}

void float3x4::getRotation( float3x3* rot ) const
{
	float3x3& d = *rot;

	d.set( 0, 0, m[0][0] );
	d.set( 0, 1, m[0][1] );
	d.set( 0, 2, m[0][2] );

	d.set( 1, 0, m[1][0] );
	d.set( 1, 1, m[1][1] );
	d.set( 1, 2, m[1][2] );

	d.set( 2, 0, m[2][0] );
	d.set( 2, 1, m[2][1] );
	d.set( 2, 2, m[2][2] );
}

float3x3 float3x4::rotation() const
{
	float3x3 d;

	d.set( 0, 0, m[0][0] );
	d.set( 0, 1, m[0][1] );
	d.set( 0, 2, m[0][2] );

	d.set( 1, 0, m[1][0] );
	d.set( 1, 1, m[1][1] );
	d.set( 1, 2, m[1][2] );

	d.set( 2, 0, m[2][0] );
	d.set( 2, 1, m[2][1] );
	d.set( 2, 2, m[2][2] );

	return d;
}

float3	float3x4::scale() const
{
	return float3( getColumn(0).length(), getColumn(1).length(), getColumn(2).length() );
}

float4 float3x4::operator*( const float4& v ) const
{
	float4 v1;
	transform( v, &v1 );
	return v1;
}

float3 float3x4::transform( const float3& v ) const
{
	float3 v1;
	transform( v, &v1 );
	return v1;
}

float4 float3x4::transform( const float4& v ) const
{
	float4 v1;
	transform( v, &v1 );
	return v1;
}

float3 float3x4::rotate( const float3& v ) const
{
	float3 v1;
	rotate( v, &v1 );
	return v1;
}

void float3x4::setRotation( const float3& axis, float angle )
{
	assert( axis.length() > FLT_MIN ); // ensure that Axis isn't null vector

	float3		unitaxis	= normalize( axis );
	const float	halfangle	= angle/2.f;
	const float	s			= sinf(halfangle);
	const float	c			= cosf(halfangle);
	const float	x0			= unitaxis.x * s;
	const float	y0			= unitaxis.y * s;
	const float	z0			= unitaxis.z * s;
	const float	xx			= x0*x0;
	const float	xy 			= y0*x0;
	const float	xz			= z0*x0;
	const float	yy			= y0*y0;
	const float	yz			= z0*y0;
	const float	zz			= z0*z0;
	const float	wx			= c*x0;
	const float	wy			= c*y0;
	const float	wz			= c*z0;

	m[0][0]=1.f-2.f*(yy+zz);	m[0][1]=2.f*(xy-wz);		m[0][2]=2.f*(xz+wy);
	m[1][0]=2.f*(xy+wz);		m[1][1]=1.f-2.f*(xx+zz);	m[1][2]=2.f*(yz-wx);
	m[2][0]=2.f*(xz-wy);		m[2][1]=2.f*(yz+wx);		m[2][2]=1.f-2.f*(xx+yy);
}

void float3x4::transform( const float3& v, float3* v1 ) const
{
	assert( &v != v1 );
	float* d = &v1->x;
	d[0] = m[0][0]*v[0] + m[0][1]*v[1] + m[0][2]*v[2] + m[0][3];
	d[1] = m[1][0]*v[0] + m[1][1]*v[1] + m[1][2]*v[2] + m[1][3];
	d[2] = m[2][0]*v[0] + m[2][1]*v[1] + m[2][2]*v[2] + m[2][3];
}

void float3x4::transform( const float4& v, float4* v1 ) const
{
	assert( &v != v1 );
	float* d = &v1->x;
	d[0] = m[0][0]*v[0] + m[0][1]*v[1] + m[0][2]*v[2] + m[0][3]*v[3];
	d[1] = m[1][0]*v[0] + m[1][1]*v[1] + m[1][2]*v[2] + m[1][3]*v[3];
	d[2] = m[2][0]*v[0] + m[2][1]*v[1] + m[2][2]*v[2] + m[2][3]*v[3];
	d[3] = v[3];
}

void float3x4::rotate( const float3& v, float3* v1 ) const
{
	assert( &v != v1 );
	float* d = &v1->x;
	d[0] = m[0][0]*v[0] + m[0][1]*v[1] + m[0][2]*v[2];
	d[1] = m[1][0]*v[0] + m[1][1]*v[1] + m[1][2]*v[2];
	d[2] = m[2][0]*v[0] + m[2][1]*v[1] + m[2][2]*v[2];
}

void float3x4::setInverseOrthonormalTransform( const float3x3& rot, const float3& t )
{
	m[0][0] = rot(0,0);	m[0][1] = rot(1,0);	m[0][2] = rot(2,0);
	m[1][0] = rot(0,1);	m[1][1] = rot(1,1);	m[1][2] = rot(2,1);
	m[2][0] = rot(0,2);	m[2][1] = rot(1,2);	m[2][2] = rot(2,2);

	m[0][3] = -( t[0]*m[0][0] + t[1]*m[0][1] + t[2]*m[0][2] );
	m[1][3] = -( t[0]*m[1][0] + t[1]*m[1][1] + t[2]*m[1][2] );
	m[2][3] = -( t[0]*m[2][0] + t[1]*m[2][1] + t[2]*m[2][2] );
}

bool float3x4::finite() const
{
	for ( int j = 0 ; j < ROWS ; ++j )
		for ( int i = 0 ; i < COLUMNS ; ++i )
			if ( !(m[j][i] >= -FLT_MAX && m[j][i] <= FLT_MAX) )
				return false;
	return true;
}

float float3x4::determinant3() const
{
	return 
		m[0][0] * m[1][1] * m[2][2] + 
		m[0][1] * m[1][2] * m[2][0] + 
		m[0][2] * m[1][0] * m[2][1] - 
		m[0][2] * m[1][1] * m[2][0] - 
		m[0][1] * m[1][0] * m[2][2] - 
		m[0][0] * m[1][2] * m[2][1];
}

float3x4 float3x4::inverse() const
{
	const float det = determinant3();
	assert( det > FLT_MIN || det < -FLT_MIN ); 
	const float invdet = 1.f / det;

	float3x4 inv;
	inv.m[0][0] = invdet * (m[1][1]*m[2][2] - m[1][2]*m[2][1]);
	inv.m[0][1] = invdet * (m[2][1]*m[0][2] - m[2][2]*m[0][1]);
	inv.m[0][2] = invdet * (m[0][1]*m[1][2] - m[0][2]*m[1][1]);
	inv.m[1][0] = invdet * (m[1][2]*m[2][0] - m[1][0]*m[2][2]);
	inv.m[1][1] = invdet * (m[2][2]*m[0][0] - m[2][0]*m[0][2]);
	inv.m[1][2] = invdet * (m[0][2]*m[1][0] - m[0][0]*m[1][2]);
	inv.m[2][0] = invdet * (m[1][0]*m[2][1] - m[1][1]*m[2][0]);
	inv.m[2][1] = invdet * (m[2][0]*m[0][1] - m[2][1]*m[0][0]);
	inv.m[2][2] = invdet * (m[0][0]*m[1][1] - m[0][1]*m[1][0]);
	inv.m[0][3] = -( m[0][3]*inv.m[0][0] + m[1][3]*inv.m[0][1] + m[2][3]*inv.m[0][2] );
	inv.m[1][3] = -( m[0][3]*inv.m[1][0] + m[1][3]*inv.m[1][1] + m[2][3]*inv.m[1][2] );
	inv.m[2][3] = -( m[0][3]*inv.m[2][0] + m[1][3]*inv.m[2][1] + m[2][3]*inv.m[2][2] );

	return inv;
}


END_NAMESPACE() // math

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
