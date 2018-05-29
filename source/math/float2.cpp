#include <math/float2.h>
#include <float.h>
#include <config.h>


BEGIN_NAMESPACE(math) 


bool float2::finite() const
{
	return 
		x >= -FLT_MAX && x <= FLT_MAX &&
		y >= -FLT_MAX && y <= FLT_MAX;
}

float2 normalize0( const float2& v )
{
	const float len = v.length();
	float invlen = 0.f; 
	if ( len >= FLT_MIN ) 
		invlen = 1.f / len; 
	return v * invlen;
}


END_NAMESPACE() // math

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
