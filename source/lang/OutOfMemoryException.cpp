#include <lang/OutOfMemoryException.h>
#include <config.h>


BEGIN_NAMESPACE(lang) 


void throw_OutOfMemoryException()
{
	throwError( OutOfMemoryException() );
}
	
OutOfMemoryException::OutOfMemoryException()
{
}


END_NAMESPACE() // lang

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
