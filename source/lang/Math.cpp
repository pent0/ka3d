#include <lang/Math.h>
#include <stdlib.h>
#include <config.h>


BEGIN_NAMESPACE(lang) 


const float		Math::E		= 2.718281828459045f;
const float		Math::PI	= 3.141592653589793f;

	
float Math::random()
{
	const float RAND_SCALE = 1.f / ( float(RAND_MAX) + 1.f );
	return RAND_SCALE * float(rand());
}


END_NAMESPACE() // lang

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
