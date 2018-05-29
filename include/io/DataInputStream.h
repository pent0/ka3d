#ifndef _IO_DATAINPUTSTREAM_H
#define _IO_DATAINPUTSTREAM_H


#include <io/DataInput.h>
#include <io/FilterInputStream.h>
#include <lang/Array.h>
#include <lang/String.h>
#include <stdint.h>


BEGIN_NAMESPACE(io) 


/**
 * Class for reading primitive types from the input stream in portable way.
 * 
 * @ingroup io
 */
class DataInputStream :
	public FilterInputStream,
	public DataInput
{
public:
	///
	explicit DataInputStream( InputStream* in );

	///
	~DataInputStream();

	/**
	 * Tries to skip over n bytes from the stream.
	 *
	 * @return Number of bytes actually skipped.
	 * @exception IOException
	 */
	int skip( int n );

	/**
	 * Tries to read specified number of bytes from the source stream.
	 * Doesn't block the caller if specified number of bytes isn't available.
	 *
	 * @return Number of bytes actually read.
	 * @exception IOException
	 */
	int read( void* data, int size );

	/**
	 * Reads specified number of bytes from the stream.
	 *
	 * @exception IOException
	 */
	void readFully( void* data, int size );

	/**
	 * Reads boolean from the stream.
	 *
	 * @exception IOException
	 */
	bool readBoolean();

	/**
	 * Reads 8-bit signed integer from the stream.
	 *
	 * @exception IOException
	 */
	uint8_t readByte();

	/**
	 * Reads character from the stream.
	 *
	 * @exception IOException
	 */
	char 	readChar();

	/**
	 * Reads character sequence from the stream.
	 *
	 * @param n Number of characters to read.
	 * @exception IOException
	 */
	NS(lang,String) readChars( int n );

	/**
	 * Reads double value from the stream.
	 *
	 * @exception IOException
	 */
	double readDouble();

	/**
	 * Reads float from the stream.
	 *
	 * @exception IOException
	 */
	float readFloat();

	/**
	 * Reads 32-bit signed integer from the stream.
	 *
	 * @exception IOException
	 */
	int readInt();

	/**
	 * Reads 16-bit signed integer from the stream.
	 *
	 * @exception IOException
	 */
	int readShort();

	/**
	 * Reads string encoded in UTF-8 from the stream.
	 *
	 * @exception EOFException
	 * @exception IOException
	 */
	NS(lang,String) readUTF();

	/**
	 * Reads string encoded in UTF-8 from the stream.
	 *
	 * @param buf [out] Receives 0-terminated UTF-8 string.
	 * @param bufsize Maximum size for the string. IOException is thrown if buffer too small.
	 * @exception EOFException
	 * @exception IOException
	 */
	void readUTF( char* buf, int bufsize );

	/**
	 * Reads string encoded in UTF-8 from the stream. Buffer size is adjusted accordingly.
	 *
	 * @param buf [out] Receives 0-terminated UTF-8 string.
	 * @exception EOFException
	 * @exception IOException
	 */
	void readUTF( NS(lang,Array)<char>& buf );

private:
	NS(lang,Array)<uint8_t> m_buf;

	void	readBE( void* data, int size );

	DataInputStream();
	DataInputStream( const DataInputStream& );
	DataInputStream& operator=( const DataInputStream& );
};


END_NAMESPACE() // io


#endif // _IO_DATAINPUTSTREAM_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
