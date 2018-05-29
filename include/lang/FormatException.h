#ifndef _LANG_FORMATEXCEPTION_H
#define _LANG_FORMATEXCEPTION_H


#include <lang/pp.h>
#include <lang/Exception.h>


BEGIN_NAMESPACE(lang) 


/**
 * Exception thrown when string formatting error occurs.
 * 
 * @ingroup lang
 */
class FormatException :
	public Exception
{
public:
	/** Creates an exception with the specified error description. */
	FormatException( const NS(lang,Format)& msg )						: Exception(msg) {}
};


END_NAMESPACE() // lang


#endif // _LANG_FORMATEXCEPTION_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
