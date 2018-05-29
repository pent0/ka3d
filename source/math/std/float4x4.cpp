#include <math/float4x4.h>
#include <math/float4.h>
#include <math/float3.h>
#include <math/float3x3.h>
#include <math/float3x4.h>
#include <float.h>
#include <config.h>


#ifdef __ee__
#error Use ps2/float4x4.cpp (VU0-assembly implementation)
#endif


BEGIN_NAMESPACE(math) 


float4x4::float4x4( float diagonal )
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
	m[3][0] = 0.f;
	m[3][1] = 0.f;
	m[3][2] = 0.f;
	m[3][3] = diagonal;
}

float4x4::float4x4( const float3x4& other )
{
	for ( int j = 0 ; j < 3 ; ++j )
		for ( int i = 0 ; i < 4 ; ++i )
			m[j][i] = other(j,i);
	m[3][0] = m[3][1] = m[3][2] = 0.f; m[3][3] = 1.f;
}

float4x4::float4x4( const float3x3& rotation, const float3& translation )				
{
	setRotation( rotation ); 
	setTranslation( translation ); 
	m[3][0] = m[3][1] = m[3][2] = 0.f; m[3][3] = 1.f;
}

void float4x4::setPerspectiveProjection( float fovHorz, float front, float back, float aspect )
{
	assert( (back-front) > FLT_MIN );

	// 'w-friendly' projection matrix
    float w = 1.f / tanf( fovHorz * .5f );
	if ( w < 0.f )
		w = -w;

	float vw = 2.f*front / w;
	float vh = vw / aspect;
	float h = 2.f*front / vh;
    float q = back / (back-front);
	
	m[0][0] = w;
	m[0][1] = 0.f;
	m[0][2] = 0.f;
	m[0][3] = 0.f;
	
	m[1][0] = 0.f;
    m[1][1] = h;
	m[1][2] = 0.f;
	m[1][3] = 0.f;
	
	m[2][0] = 0.f;
	m[2][1] = 0.f;
    m[2][2] = q;
    m[2][3] = -q*front;
	
	m[3][0] = 0.f;
	m[3][1] = 0.f;
    m[3][2] = 1.f;
    m[3][3] = 0.f;
}

void float4x4::setScreenTransform( float screenz, float scalex, float scaley, 
	float zmin, float zmax, float cx, float cy )
{
	m[0][0] = screenz * scalex;
	m[0][1] = 0.f;
	m[0][2] = 0.f;
	m[0][3] = cx;

	m[1][0] = 0.f;
	m[1][1] = screenz * scaley;
	m[1][2] = 0.f;
	m[1][3] = cy;

	m[2][0] = 0.f;
	m[2][1] = 0.f;
	m[2][2] = (zmax - zmin);
	m[2][3] = zmin;

	m[3][0] = 0.f;
	m[3][1] = 0.f;
	m[3][2] = 0.f;
	m[3][3] = 1.f;
}


void float4x4::setPointPlaneProjection( const float3& l,
	const float3& n, const float3& p )
{
	float d		= -dot(p,n);
	float nl	= dot(n,l);

	m[0][0]		= nl + d - l.x*n.x;
	m[0][1]		= -l.x*n.y;
	m[0][2]		= -l.x*n.z;
	m[0][3]		= -l.x*d;

	m[1][0]		= -l.y*n.x;
	m[1][1]		= nl + d - l.y*n.y;
	m[1][2]		= -l.y*n.z;
	m[1][3]		= -l.y*d;

	m[2][0]		= -l.z*n.x;
	m[2][1]		= -l.z*n.y;
	m[2][2]		= nl + d - l.z*n.z;
	m[2][3]		= -l.z*d;

	m[3][0]		= -n.x;
	m[3][1]		= -n.y;
	m[3][2]		= -n.z;
	m[3][3]		= nl;
}

void float4x4::setDirectPlaneProjection( const float3& l,
	const float3& n, const float3& p )
{
	float np = dot(n,p);
	float nl = dot(n,l);

	m[0][0] = l.x*n.x -nl;
	m[0][1] = l.x*n.y;
	m[0][2] = l.x*n.z;
	m[0][3] = -l.x*np;

	m[1][0] = l.y*n.x;
	m[1][1] = l.y*n.y -nl;
	m[1][2] = l.y*n.z;
	m[1][3] = -l.y*np;

	m[2][0] = l.z*n.x;
	m[2][1] = l.z*n.y;
	m[2][2] = l.z*n.z -nl;
	m[2][3] = -l.z*np;

	m[3][0] = 0;
	m[3][1] = 0;
	m[3][2] = 0;
	m[3][3] = -nl;
}

float4x4 float4x4::operator*( const float4x4& other ) const
{
	float4x4 r;

	for ( int j = 0 ; j < ROWS ; ++j )
	{
		r.m[j][0] = m[j][0]*other.m[0][0] +	
					m[j][1]*other.m[1][0] + 
					m[j][2]*other.m[2][0] +	
					m[j][3]*other.m[3][0];

		r.m[j][1] = m[j][0]*other.m[0][1] +	
					m[j][1]*other.m[1][1] + 
					m[j][2]*other.m[2][1] +	
					m[j][3]*other.m[3][1];

		r.m[j][2] = m[j][0]*other.m[0][2] +	
					m[j][1]*other.m[1][2] + 
					m[j][2]*other.m[2][2] +	
					m[j][3]*other.m[3][2];

		r.m[j][3] = m[j][0]*other.m[0][3] +	
					m[j][1]*other.m[1][3] + 
					m[j][2]*other.m[2][3] +	
					m[j][3]*other.m[3][3];
	}

		
	return r;
}

float4x4 float4x4::transpose() const
{
	float4x4 mt;

	for ( int j = 0 ; j < ROWS ; ++j )
		for ( int i = 0 ; i < COLUMNS ; ++i )
			mt.m[i][j] = m[j][i];
		
	return mt;
}

void float4x4::setTranslation( const float3& t )
{
	m[0][3] = t.x;
	m[1][3] = t.y;
	m[2][3] = t.z;
}

void float4x4::setRotation( const float3x3& rot )
{
	for ( int j = 0 ; j < 3 ; ++j )
		for ( int i = 0 ; i < 3 ; ++i )
			m[j][i] = rot.get(j,i);
}

float3x3 float4x4::rotation() const
{
	float3x3 d;
	for ( int j = 0 ; j < 3 ; ++j )
		for ( int i = 0 ; i < 3 ; ++i )
			d.set( j, i, m[j][i] );
	return d;
}

float3 float4x4::translation() const
{
	return float3( m[0][3], m[1][3], m[2][3] );
}

float float4x4::determinant() const
{
	return 
			m[0][0] * (m[1][1] * m[2][2] * m[3][3] + 
			m[1][2] * m[2][3] * m[3][1] + 
			m[1][3] * m[2][1] * m[3][2] - 
			m[1][3] * m[2][2] * m[3][1] - 
			m[1][2] * m[2][1] * m[3][3] - 
			m[1][1] * m[2][3] * m[3][2])
		-	m[0][1] * (m[1][0] * m[2][2] * m[3][3] + 
			m[1][2] * m[2][3] * m[3][0] + 
			m[1][3] * m[2][0] * m[3][2] - 
			m[1][3] * m[2][2] * m[3][0] - 
			m[1][2] * m[2][0] * m[3][3] - 
			m[1][0] * m[2][3] * m[3][2])
		+	m[0][2] * (m[1][0] * m[2][1] * m[3][3] + 
			m[1][1] * m[2][3] * m[3][0] + 
			m[1][3] * m[2][0] * m[3][1] - 
			m[1][3] * m[2][1] * m[3][0] - 
			m[1][1] * m[2][0] * m[3][3] - 
			m[1][0] * m[2][3] * m[3][1])
		-	m[0][3] * (m[1][0] * m[2][1] * m[3][2] + 
			m[1][1] * m[2][2] * m[3][0] + 
			m[1][2] * m[2][0] * m[3][1] - 
			m[1][2] * m[2][1] * m[3][0] - 
			m[1][1] * m[2][0] * m[3][2] - 
			m[1][0] * m[2][2] * m[3][1]);
}

float4x4 float4x4::inverse() const
{
	const float det = determinant();
	assert( det > FLT_MIN || det < -FLT_MIN ); // singular

	float invdet = 1.f / det;
	float invdet0 = invdet;
	float4x4 invm;
	for ( int j = 0 ; j < 4 ; ++j )
	{
		int j0 = (j+3)&3;
		int j1 = (j+1)&3;
		int j2 = (j+2)&3;
		invdet = invdet0;
		invdet0 = -invdet0;

		for ( int i = 0 ; i < 4 ; ++i )
		{
			int i0 = (i+3)&3;
			int i1 = (i+1)&3;
			int i2 = (i+2)&3;
			
			float v = invdet *
				(m[j0][i0] * m[j1][i1] * m[j2][i2] + 
				m[j0][i1] * m[j1][i2] * m[j2][i0] + 
				m[j0][i2] * m[j1][i0] * m[j2][i1] - 
				m[j0][i2] * m[j1][i1] * m[j2][i0] - 
				m[j0][i1] * m[j1][i0] * m[j2][i2] - 
				m[j0][i0] * m[j1][i2] * m[j2][i1]);

			invm.m[i][j] = v;
			invdet = -invdet;
		}
	}

	return invm;
}

float4 float4x4::operator*( const float4& v ) const
{
	float4 v1;
	transform( v, &v1 );
	return v1;
}

float4 float4x4::transform( const float4& v ) const
{
	float4 v1;
	transform( v, &v1 );
	return v1;
}

float3 float4x4::transform( const float3& v ) const
{
	float3 v1;
	transform( v, &v1 );
	return v1;
}

float3 float4x4::rotate( const float3& v ) const
{
	float3 v1;
	rotate( v, &v1 );
	return v1;
}

void float4x4::setRotation( const float3& axis, float angle )
{
	assert( axis.length() > FLT_MIN ); // ensure that Axis isn't null vector

	float3			unitaxis		= normalize( axis );
	const float		halfangle		= angle/2.f;
	const float		s				= sinf(halfangle);
	const float		c				= cosf(halfangle);
	const float		x0				= unitaxis.x * s;
	const float		y0				= unitaxis.y * s;
	const float		z0				= unitaxis.z * s;
	const float		w				= c;
	const float		xx				= x0*x0;
	const float		xy 				= y0*x0;
	const float		xz				= z0*x0;
	const float		yy				= y0*y0;
	const float		yz				= z0*y0;
	const float		zz				= z0*z0;
	const float		wx				= w*x0;
	const float		wy				= w*y0;
	const float		wz				= w*z0;

	m[0][0]=1.f-2.f*(yy+zz);	m[0][1]=2.f*(xy-wz);		m[0][2]=2.f*(xz+wy);
	m[1][0]=2.f*(xy+wz);		m[1][1]=1.f-2.f*(xx+zz);	m[1][2]=2.f*(yz-wx);
	m[2][0]=2.f*(xz-wy);		m[2][1]=2.f*(yz+wx);		m[2][2]=1.f-2.f*(xx+yy);
}

void float4x4::transform( const float3& v, float3* v1 ) const
{
	assert( &v != v1 );
	float* d = &v1->x;
	d[0] = m[0][0]*v[0] + m[0][1]*v[1] + m[0][2]*v[2] + m[0][3];
	d[1] = m[1][0]*v[0] + m[1][1]*v[1] + m[1][2]*v[2] + m[1][3];
	d[2] = m[2][0]*v[0] + m[2][1]*v[1] + m[2][2]*v[2] + m[2][3];
}

void float4x4::transform( const float4& v, float4* v1 ) const
{
	assert( &v != v1 );
	float* d = &v1->x;
	d[0] = m[0][0]*v[0] + m[0][1]*v[1] + m[0][2]*v[2] + m[0][3]*v[3];
	d[1] = m[1][0]*v[0] + m[1][1]*v[1] + m[1][2]*v[2] + m[1][3]*v[3];
	d[2] = m[2][0]*v[0] + m[2][1]*v[1] + m[2][2]*v[2] + m[2][3]*v[3];
	d[3] = m[3][0]*v[0] + m[3][1]*v[1] + m[3][2]*v[2] + m[3][3]*v[3];
}

void float4x4::rotate( const float3& v, float3* v1 ) const
{
	assert( &v != v1 );
	float* d = &v1->x;
	d[0] = m[0][0]*v[0] + m[0][1]*v[1] + m[0][2]*v[2];
	d[1] = m[1][0]*v[0] + m[1][1]*v[1] + m[1][2]*v[2];
	d[2] = m[2][0]*v[0] + m[2][1]*v[1] + m[2][2]*v[2];
}

bool float4x4::finite() const
{
	for ( int j = 0 ; j < ROWS ; ++j )
		for ( int i = 0 ; i < COLUMNS ; ++i )
			if ( !(m[j][i] >= -FLT_MAX && m[j][i] <= FLT_MAX) )
				return false;
	return true;
}

float4x4 float4x4::operator*( const float3x4& other ) const
{
	float4x4 r;

	const float other_m3[4] = {0.f,0.f,0.f,1.f};
	for ( int j = 0 ; j < ROWS ; ++j )
		for ( int i = 0 ; i < COLUMNS ; ++i )
			r.m[j][i] = m[j][0]*other(0,i) +	
						m[j][1]*other(1,i) + 
						m[j][2]*other(2,i) +	
						m[j][3]*other_m3[i];

		
	return r;
}


END_NAMESPACE() // math

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
