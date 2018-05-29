#ifndef _IO_FILTEROUTPUTSTREAM_H
#define _IO_FILTEROUTPUTSTREAM_H


#include <io/OutputStream.h>


BEGIN_NAMESPACE(io) 


/**
 * FilterOutputStream overrides all methods of OutputStream with 
 * versions that pass all requests to the target output stream.
 * The target stream must be exist as long as FilterOutputStream is used.
 * 
 * @ingroup io
 */
class FilterOutputStream :
	public OutputStream
{
public:
	/** 
	 * Forwards all operations to specified target stream.
	 */
	explicit FilterOutputStream( OutputStream* target );

	/**
	 * Writes specified number of bytes to the target stream.
	 *
	 * @exception IOException
	 */
	 void	write( const void* data, int size );

	 /** Returns name of the target stream. */
	NS(lang,String)	toString() const;

	/** Returns number of bytes written to the stream. */
	int				bytesWritten() const;

private:
	OutputStream*	m_target;
	int				m_bytesWritten;

	FilterOutputStream();
	FilterOutputStream( const FilterOutputStream& );
	FilterOutputStream& operator=( const FilterOutputStream& );
};


END_NAMESPACE() // io


#endif // _IO_FILTEROUTPUTSTREAM_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
