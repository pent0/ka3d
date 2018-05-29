#ifndef _IO_FILTERINPUTSTREAM_H
#define _IO_FILTERINPUTSTREAM_H


#include <io/InputStream.h>


BEGIN_NAMESPACE(io) 


/**
 * FilterInputStream overrides all methods of InputStream with 
 * versions that pass all requests to the source input stream.
 * The source stream must be exist as long as FilterInputStream is used.
 * 
 * @ingroup io
 */
class FilterInputStream :
	public InputStream
{
public:
	/** 
	 * Forwards all operations to specified source stream.
	 */
	explicit FilterInputStream( InputStream* source );

	/**
	 * Tries to read specified number of bytes from the source stream.
	 * Doesn't block the caller if specified number of bytes isn't available.
	 *
	 * @return Number of bytes actually read.
	 * @exception IOException
	 */
	int		read( void* data, int size );

	/**
	 * Tries to skip over n bytes from the source stream.
	 *
	 * @return Number of bytes actually skipped.
	 * @exception IOException
	 */
	int		skip( int n );

	/** 
	 * Returns the number of bytes that can be read from the source stream without blocking.
	 *
	 * @exception IOException
	 */
	int		available() const;

	/** 
	 * Returns number of bytes read from the stream. 
	 */
	int		bytesRead() const;

	/**
	 * Returns string description of the stream.
	 */
	NS(lang,String)	toString() const;

private:
	InputStream*	m_source;
	int				m_bytesRead;

	FilterInputStream();
	FilterInputStream( const FilterInputStream& );
	FilterInputStream& operator=( const FilterInputStream& );
};


END_NAMESPACE() // io


#endif // _IO_FILTERINPUTSTREAM_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
