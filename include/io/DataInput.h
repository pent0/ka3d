#ifndef _IO_DATAINPUT_H
#define _IO_DATAINPUT_H


#include <lang/String.h>
#include <stdint.h>


BEGIN_NAMESPACE(io) 


/**
 * Interface for reading primitive types from the input in portable way.
 * 
 * @ingroup io
 */
class DataInput
{
public:
	/**
	 * Tries to skip over n bytes from the stream.
	 *
	 * @return Number of bytes actually skipped.
	 * @exception IOException
	 */
	virtual int skip( int n ) = 0;

	/**
	 * Tries to read specified number of bytes from the source stream.
	 * Doesn't block the caller if specified number of bytes isn't available.
	 *
	 * @return Number of bytes actually read.
	 * @exception IOException
	 */
	virtual int read( void* data, int size ) = 0;

	/**
	 * Reads specified number of bytes from the stream.
	 *
	 * @exception EOFException
	 * @exception IOException
	 */
	virtual void readFully( void* data, int size ) = 0;

	/**
	 * Reads boolean from the stream.
	 *
	 * @exception EOFException
	 * @exception IOException
	 */
	virtual bool readBoolean() = 0;

	/**
	 * Reads 8-bit signed integer from the stream.
	 *
	 * @exception EOFException
	 * @exception IOException
	 */
	virtual uint8_t readByte() = 0;

	/**
	 * Reads character from the stream.
	 *
	 * @exception EOFException
	 * @exception IOException
	 */
	virtual char readChar() = 0;

	/**
	 * Reads character sequence from the stream.
	 *
	 * @param n Number of characters to read.
	 * @exception EOFException
	 * @exception IOException
	 */
	virtual NS(lang,String) readChars( int n ) = 0;

	/**
	 * Reads double value from the stream.
	 *
	 * @exception EOFException
	 * @exception IOException
	 */
	virtual double readDouble() = 0;

	/**
	 * Reads float from the stream.
	 *
	 * @exception EOFException
	 * @exception IOException
	 */
	virtual float readFloat() = 0;

	/**
	 * Reads 32-bit signed integer from the stream.
	 *
	 * @exception EOFException
	 * @exception IOException
	 */
	virtual int readInt() = 0;

	/**
	 * Reads 16-bit signed integer from the stream.
	 *
	 * @exception EOFException
	 * @exception IOException
	 */
	virtual int readShort() = 0;

	/**
	 * Reads string encoded in UTF-8 from the stream.
	 *
	 * @exception EOFException
	 * @exception IOException
	 */
	virtual NS(lang,String) readUTF() = 0;

	/**
	 * Reads string encoded in UTF-8 from the stream.
	 *
	 * @param buf [out] Receives 0-terminated UTF-8 string.
	 * @param bufsize Maximum size for the string. IOException is thrown if buffer too small.
	 * @exception EOFException
	 * @exception IOException
	 */
	virtual void readUTF( char* buf, int bufsize ) = 0;
};


END_NAMESPACE() // io


#endif // _IO_DATAINPUT_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
