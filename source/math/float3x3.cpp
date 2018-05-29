#include <math/float3x3.h>
#include <math/float3.h>
#include <math/quaternion.h>
#include <float.h>
#include <config.h>
#include <lang/assert.h>

BEGIN_NAMESPACE(math) 


float3x3::float3x3( const float3& axis, float angle )
{
	assert( axis.length() > FLT_MIN );

	float3			unitaxis	= normalize( axis );
	const float		halfangle	= angle*.5f;
	const float		s			= sinf(halfangle);
	const float		c			= cosf(halfangle);
	const float		x0			= unitaxis.x * s;
	const float		y0			= unitaxis.y * s;
	const float		z0			= unitaxis.z * s;
	const float		xx			= x0*x0;
	const float		xy 			= y0*x0;
	const float		xz			= z0*x0;
	const float		yy			= y0*y0;
	const float		yz			= z0*y0;
	const float		zz			= z0*z0;
	const float		wx			= c*x0;
	const float		wy			= c*y0;
	const float		wz			= c*z0;

	m[0][0]=1.f-2.f*(yy+zz);	m[0][1]=2.f*(xy-wz);		m[0][2]=2.f*(xz+wy);
	m[1][0]=2.f*(xy+wz);		m[1][1]=1.f-2.f*(xx+zz);	m[1][2]=2.f*(yz-wx);
	m[2][0]=2.f*(xz-wy);		m[2][1]=2.f*(yz+wx);		m[2][2]=1.f-2.f*(xx+yy);
}

float3x3::float3x3(  
	float m00, float m01, float m02, 
	float m10, float m11, float m12,
	float m20, float m21, float m22 )
{
	m[0][0] = m00;
	m[0][1] = m01;
	m[0][2] = m02;
	m[1][0] = m10;
	m[1][1] = m11;
	m[1][2] = m12;
	m[2][0] = m20;
	m[2][1] = m21;
	m[2][2] = m22;
}

float3x3::float3x3( float diagonal )
{
	for ( int j = 0 ; j < ROWS ; ++j )
		for ( int i = 0 ; i < COLUMNS ; ++i )
			m[j][i] = (i == j ? diagonal : 0.f);
}

float3x3::float3x3( const quaternion& q )
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

float3x3 float3x3::operator*( const float3x3& other ) const
{
	float3x3 r;

	for ( int j = 0 ; j < ROWS ; ++j )
	{
		r.m[j][0] = m[j][0]*other.m[0][0] +	 
					m[j][1]*other.m[1][0] + 
					m[j][2]*other.m[2][0];

		r.m[j][1] = m[j][0]*other.m[0][1] +	
					m[j][1]*other.m[1][1] + 
					m[j][2]*other.m[2][1];

		r.m[j][2] = m[j][0]*other.m[0][2] +	
					m[j][1]*other.m[1][2] + 
					m[j][2]*other.m[2][2];
	}

	return r;
}

float float3x3::determinant() const
{
	return 
		m[0][0] * m[1][1] * m[2][2] + 
		m[0][1] * m[1][2] * m[2][0] + 
		m[0][2] * m[1][0] * m[2][1] - 
		m[0][2] * m[1][1] * m[2][0] - 
		m[0][1] * m[1][0] * m[2][2] - 
		m[0][0] * m[1][2] * m[2][1];
}

float3x3 float3x3::transpose() const
{
	float3x3 mt;

	for ( int j = 0 ; j < ROWS ; ++j )
		for ( int i = 0 ; i < COLUMNS ; ++i )
			mt.m[i][j] = m[j][i];

	return mt;
}

float3x3 float3x3::inverse() const
{
	float det = determinant();
	assert( det > FLT_MIN || det < -FLT_MIN ); 
	float invdet = 1.f / det;

	float3x3 inv;
	inv.m[0][0] = invdet * (m[1][1]*m[2][2] - m[1][2]*m[2][1]);
	inv.m[0][1] = invdet * (m[2][1]*m[0][2] - m[2][2]*m[0][1]);
	inv.m[0][2] = invdet * (m[0][1]*m[1][2] - m[0][2]*m[1][1]);
	inv.m[1][0] = invdet * (m[1][2]*m[2][0] - m[1][0]*m[2][2]);
	inv.m[1][1] = invdet * (m[2][2]*m[0][0] - m[2][0]*m[0][2]);
	inv.m[1][2] = invdet * (m[0][2]*m[1][0] - m[0][0]*m[1][2]);
	inv.m[2][0] = invdet * (m[1][0]*m[2][1] - m[1][1]*m[2][0]);
	inv.m[2][1] = invdet * (m[2][0]*m[0][1] - m[2][1]*m[0][0]);
	inv.m[2][2] = invdet * (m[0][0]*m[1][1] - m[0][1]*m[1][0]);
	return inv;
}

float3x3 float3x3::orthonormalize() const
{
	assert( cross(getColumn(0),getColumn(1)).length() > FLT_MIN );
	assert( cross(getColumn(1),getColumn(2)).length() > FLT_MIN );
	assert( cross(getColumn(0),getColumn(2)).length() > FLT_MIN );

	// Gram-Schmidt orthogonalization
	float3 x = normalize( getColumn(0) );
	float3 y = getColumn(1);
	y = normalize( y - x*dot(y,x) );
	float3 z = getColumn(2);
	z = normalize( z - x*dot(z,x) - y*dot(z,y) );

	float3x3 n;
	n.setColumn( 0, x );
	n.setColumn( 1, y );
	n.setColumn( 2, z );
	return n;
}

float3 float3x3::operator*( const float3& v ) const
{
	float3 v1;
	rotate( v, &v1 );
	return v1;
}

void float3x3::generateOrthonormalBasisFromZ( const float3& axis )
{
	generateOrthonormalBasisFromZ( axis, float3(0,1,0) );
}

void float3x3::generateOrthonormalBasisFromZ( const float3& axis, const float3& up )
{
	assert( fabsf(axis.length()-1.f) < 1e-3f ); // axis must be unit vector

	float3 xaxis, yaxis;
	const float3 nextaxis[] = { up, float3(1,0,0), float3(0,0,1), float3(0,1,0) };
	int axisindex = 0;
	float len2;
	do
	{
		yaxis = nextaxis[axisindex++];
		xaxis = cross( yaxis, axis );
		len2 = dot(xaxis,xaxis);
	} while ( len2 < 1e-10f );

	xaxis *= 1.f/sqrtf(len2);
	float3 zaxis = axis;
	yaxis = cross( zaxis, xaxis );
	setColumn( 0, xaxis );
	setColumn( 1, yaxis );
	setColumn( 2, zaxis );
}

void float3x3::rotate( const float3& v, float3* v1 ) const
{
	assert( &v != v1 );
	float* d = &v1->x;
	d[0] = m[0][0]*v[0] + m[0][1]*v[1] + m[0][2]*v[2];
	d[1] = m[1][0]*v[0] + m[1][1]*v[1] + m[1][2]*v[2];
	d[2] = m[2][0]*v[0] + m[2][1]*v[1] + m[2][2]*v[2];
}

float3 float3x3::rotate( const float3& v ) const
{
	float3 v1;
	float* d = &v1.x;
	d[0] = m[0][0]*v[0] + m[0][1]*v[1] + m[0][2]*v[2];
	d[1] = m[1][0]*v[0] + m[1][1]*v[1] + m[1][2]*v[2];
	d[2] = m[2][0]*v[0] + m[2][1]*v[1] + m[2][2]*v[2];
	return v1;
}

bool float3x3::finite() const
{
	for ( int j = 0 ; j < ROWS ; ++j )
		for ( int i = 0 ; i < COLUMNS ; ++i )
			if ( !(m[j][i] >= -FLT_MAX && m[j][i] <= FLT_MAX) )
				return false;
	return true;
}


END_NAMESPACE() // math

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
