#ifndef _IO_FILENOTFOUNDEXCEPTION_H
#define _IO_FILENOTFOUNDEXCEPTION_H


#include <io/IOException.h>


BEGIN_NAMESPACE(io) 


/**
 * Thrown if specified file or path is not found or 
 * if the file cannot be opened for some other reason.
 * (for example if the file tried to be opened is a directory)
 * 
 * @ingroup io
 */
class FileNotFoundException :
	public IOException
{
public:
	///
	FileNotFoundException( const NS(lang,Format)& msg )								: IOException(msg) {}
};


END_NAMESPACE() // io


#endif // _IO_FILENOTFOUNDEXCEPTION_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
