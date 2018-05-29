#ifndef _LANG_OUTOFMEMORYEXCEPTION_H
#define _LANG_OUTOFMEMORYEXCEPTION_H


#include <lang/pp.h>
#include <lang/Exception.h>


BEGIN_NAMESPACE(lang) 


/** 
 * Thrown if application rans out of memory. 
 * @ingroup lang
 */
class OutOfMemoryException :
	public Exception
{
public:
	OutOfMemoryException();
};


/*
 * Throws OutOfMemoryException. (used to avoid dependencies to header OutOfMemoryException.h) 
 */
void throw_OutOfMemoryException();


END_NAMESPACE() // lang


#endif // _LANG_OUTOFMEMORYEXCEPTION_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
