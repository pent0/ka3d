#ifndef _IO_BYTEARRAYOUTPUTSTREAM_H
#define _IO_BYTEARRAYOUTPUTSTREAM_H


#include <io/OutputStream.h>
#include <lang/Array.h>
#include <stdint.h>


BEGIN_NAMESPACE(io) 


/**
 * ByteArrayOutputStream writes bytes to a memory buffer.
 * 
 * @ingroup io
 */
class ByteArrayOutputStream :
	public OutputStream
{
public:
	/** 
	 * Creates an output stream to a memory buffer of specified initial capacity.
	 */
	explicit ByteArrayOutputStream( int size=0 );

	/** 
	 * Creates an output stream to a user defined memory buffer.
	 * Note that user defined memory buffer is NOT deleted when ByteArrayOutputStream is deleted.
	 */
	explicit ByteArrayOutputStream( NS(lang,Array)<uint8_t>* buffer );

	///
	~ByteArrayOutputStream();

	/** Discards all written bytes and resets write pointer. */
	void			reset();

	/**
	 * Writes specified number of bytes to the stream.
	 */
	void			write( const void* data, int size );

	/**
	 * Returns pointer to the memory buffer used by the stream.
	 */
	const NS(lang,Array)<uint8_t>&	toByteArray() const							{return *m_buffer;}

	/**
	 * Returns number of bytes written to the stream.
	 */
	int				size() const;

	/** Returns byte array identifier. */
	NS(lang,String)	toString() const;

private:
	NS(lang,Array)<uint8_t>*	m_buffer;
	bool						m_userDefined;

	ByteArrayOutputStream( const ByteArrayOutputStream& );
	ByteArrayOutputStream& operator=( const ByteArrayOutputStream& );
};


END_NAMESPACE() // io


#endif // _IO_BYTEARRAYOUTPUTSTREAM_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
