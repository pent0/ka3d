#ifndef _IO_DATAOUTPUTSTREAM_H
#define _IO_DATAOUTPUTSTREAM_H


#include <io/DataOutput.h>
#include <io/FilterOutputStream.h>
#include <lang/String.h>


BEGIN_NAMESPACE(io) 


/**
 * Class for writing primitive types to the output stream in portable way.
 * 
 * @ingroup io
 */
class DataOutputStream :
	public FilterOutputStream,
	public DataOutput
{
public:
	///
	explicit DataOutputStream( OutputStream* out );

	/**
	 * Writes boolean to the stream.
	 *
	 * @exception IOException
	 */
	void writeBoolean( bool value );

	/**
	 * Writes 8-bit signed integer to the stream.
	 *
	 * @exception IOException
	 */
	void writeByte( int value );

	/**
	 * Writes character to the stream.
	 *
	 * @exception IOException
	 */
	void writeChar( char value );

	/**
	 * Writes character sequence to the stream.
	 *
	 * @exception IOException
	 */
	void writeChars( const NS(lang,String)& value );

	/**
	 * Writes double to the stream.
	 *
	 * @exception IOException
	 */
	void writeDouble( double value );

	/**
	 * Writes float to the stream.
	 *
	 * @exception IOException
	 */
	void writeFloat( float value );

	/**
	 * Writes 32-bit signed integer to the stream.
	 *
	 * @exception IOException
	 */
	void writeInt( int value );

	/**
	 * Writes 16-bit signed integer to the stream.
	 *
	 * @exception IOException
	 */
	void writeShort( int value );

	/**
	 * Writes string encoded in UTF-8 to the stream.
	 *
	 * @exception IOException
	 */
	void writeUTF( const NS(lang,String)& value );

private:
	DataOutputStream();
	DataOutputStream( const DataOutputStream& );
	DataOutputStream& operator=( const DataOutputStream& );

	void	writeBE( void* data, int size );
};


END_NAMESPACE() // io


#endif // _IO_DATAOUTPUTSTREAM_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
