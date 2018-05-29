#ifndef _IO_OUTPUTSTREAM_H
#define _IO_OUTPUTSTREAM_H


#include <lang/Object.h>
#include <lang/String.h>


BEGIN_NAMESPACE(lang) 
	class String;END_NAMESPACE()


BEGIN_NAMESPACE(io) 


/**
 * Base class of all classes writing stream of bytes.
 * 
 * @ingroup io
 */
class OutputStream :
	public NS(lang,Object)
{
public:
	/**
	 * Writes specified number of bytes to the stream.
	 * @exception IOException
	 */
	virtual void 			write( const void* data, int size ) = 0;

	/** Returns name of the stream. */
	virtual NS(lang,String)	toString() const = 0;
};


END_NAMESPACE() // io


#endif // _IO_OUTPUTSTREAM_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
