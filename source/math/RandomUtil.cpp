#include <math/RandomUtil.h>
#include <math/float3x3.h>
#include <math/float4x4.h>
#include <float.h>
#include <stdlib.h>
#include <config.h>


BEGIN_NAMESPACE(math) 


const float PI = 3.141592653589793f;


float RandomUtil::random()
{
	const float RAND_SCALE = 1.0f / (float(RAND_MAX)+1.0f);
	return float(rand()) * RAND_SCALE;
}

float RandomUtil::getRandom( float begin, float end )
{
	return (end-begin)*random() + begin;
}

float3 RandomUtil::getPointOnDisk( float r1, float r2 )
{
	float x,y;
	getPointOnDisk(r1,r2,&x,&y);
	return float3(x,y,0.f);
}

float3 RandomUtil::getPointOnDisk( const float3& o, const float3& n, float r1, float r2 )
{
	float3x3 rot;
	rot.generateOrthonormalBasisFromZ( normalize0(n) );
	
	float x,y;
	getPointOnDisk(r1,r2,&x,&y);

	return o + rot.getColumn(0)*x + rot.getColumn(1)*y;
}

float3	RandomUtil::getPointInSphere( float r1, float r2 )
{
	float z = (random() - .5f) * 2.f;
	float t = 2.f * PI * random();
	float w = sqrtf( 1.f - z*z );
	float x = w * cosf( t );
	float y = w * sinf( t );
	float u0 = random();
	float u = u0*u0*u0;
	float d = (r1-r2)*u + r2;
	x *= d;
	y *= d;
	z *= d;
	return float3(x,y,z);
}

float3	RandomUtil::getPointOnLine( const float3& p1, const float3& p2 )
{
	return p1 + (p2-p1)*random();
}

float3	RandomUtil::getPointInBox( const float3& p1, const float3& p2 )
{
	return float3( (p2.x-p1.x)*random()+p1.x, (p2.y-p1.y)*random()+p1.y, (p2.z-p1.z)*random()+p1.z );
}

float3	RandomUtil::getPointInCylinder( float len, float r1, float r2 )
{
	float x, y;
	getPointOnDisk( r1, r2, &x, &y );
	float z = random() * len;
	return float3(x,y,z);
}

float3 RandomUtil::getPointInCylinder( const float3& p1, const float3& p2, float r1, float r2 )
{
	float3 lenv = p2 - p1;
	float3 dir = normalize0(lenv);

	float3x3 rot;
	rot.generateOrthonormalBasisFromZ( dir );

	float x, y;
	getPointOnDisk( r1, r2, &x, &y );
	
	return p1 + lenv*random() + rot.getColumn(0)*x + rot.getColumn(1)*y;
}

void RandomUtil::getPointOnDisk( float r1, float r2, float* x, float* y )
{
	assert( r1 >= 0.f );
	assert( r2 >= 0.f );

	float u0 = random();
	float u = u0*u0;
	float d = (r1-r2)*u + r2;
	float t = 2.f * PI * random();
	*x = d * cosf( t );
	*y = d * sinf( t );
}

float3 RandomUtil::getPointOnRectangle( const float3& o, const float3& e1, const float3& e2 )
{
	return o + e1*random() + e2*random();
}

float3 RandomUtil::getPointOnTriangle( const float3& o, const float3& e1, const float3& e2 )
{
	float u = random();
	float v = random();
	if ( u+v >= 1.f )
	{
		u = 1.f - u;
		v = 1.f - v;
	}
	return o + e1*u + e2*v;
}


END_NAMESPACE() // math

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
