#include <gr/Primitive.h>
#include <string.h>
#include <config.h>


BEGIN_NAMESPACE(gr) 


Primitive::Primitive() : 
	ContextObject(CLASSID_PRIMITIVE) 
{
}

void Primitive::setPerspectiveCorrection( int /*quality*/ )
{
}

void Primitive::setWireframeColor( int /*color*/ )
{
}


END_NAMESPACE() // gr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
