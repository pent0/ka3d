#ifndef _IO_IOEXCEPTION_H
#define _IO_IOEXCEPTION_H


#include <lang/Exception.h>


BEGIN_NAMESPACE(io) 


/**
 * Thrown if input/output operation fails.
 * 
 * @ingroup io
 */
class IOException :
	public NS(lang,Exception)
{
public:
	IOException( const NS(lang,Format)& msg )											: Exception(msg) {}
};


END_NAMESPACE() // io


#endif // _IO_IOEXCEPTION_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
