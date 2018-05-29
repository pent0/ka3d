#include <math/toString.h>
#include <math/float2.h>
#include <math/float3.h>
#include <math/float4.h>
#include <math/float3x3.h>
#include <math/float3x4.h>
#include <math/float4x4.h>
#include <stdio.h>
#include <config.h>


USING_NAMESPACE(lang)


BEGIN_NAMESPACE(math) 


String toString( const float2& v )
{
	char buf[256];
	sprintf( buf, "(%g, %g)", v.x, v.y );
	return buf;
}

String toString( const float3& v )
{
	char buf[256];
	sprintf( buf, "(%g, %g, %g)", v.x, v.y, v.z );
	return buf;
}

String toString( const float4& v )
{
	char buf[256];
	sprintf( buf, "(%g, %g, %g, %g)", v.x, v.y, v.z, v.w );
	return buf;
}

String toString( const float3x3& v )
{
	char buf[512];
	sprintf( buf, "/%10g, %10g, %10g\\\n|%10g, %10g, %10g|\n\\%10g, %10g, %10g/",
		v(0,0), v(0,1), v(0,2),
		v(1,0), v(1,1), v(1,2),
		v(2,0), v(2,1), v(2,2) );
	return buf;
}

String toString( const float3x4& v )
{
	char buf[512];
	sprintf( buf, "/%10g, %10g, %10g, %10g\\\n|%10g, %10g, %10g, %10g|\n\\%10g, %10g, %10g, %10g/",
		v(0,0), v(0,1), v(0,2), v(0,3),
		v(1,0), v(1,1), v(1,2), v(1,3),
		v(2,0), v(2,1), v(2,2), v(2,3) );
	return buf;
}

String toString( const float4x4& v )
{
	char buf[512];
	sprintf( buf, "/%10g, %10g, %10g, %10g\\\n|%10g, %10g, %10g, %10g|\n|%10g, %10g, %10g, %10g|\n\\%10g, %10g, %10g, %10g/",
		v(0,0), v(0,1), v(0,2), v(0,3),
		v(1,0), v(1,1), v(1,2), v(1,3),
		v(2,0), v(2,1), v(2,2), v(2,3),
		v(3,0), v(3,1), v(3,2), v(3,3) );
	return buf;
}


END_NAMESPACE() // math

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
