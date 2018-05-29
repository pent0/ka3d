#include <lang/internalError.h>
#include <lang/Debug.h>
#include <lang/Exception.h>
#include <config.h>


BEGIN_NAMESPACE(lang) 


void internalError( const char* filename, int line, const char* expr )
{
	Debug::printf( "INTERNAL ERROR: %s(%d): %s\n", filename, line, expr );
	throwError( Exception( Format("Internal error at {0}({1}): {2}", filename, line, expr) ) );
}


END_NAMESPACE() // lang

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
