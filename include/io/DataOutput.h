#ifndef _IO_DATAOUTPUT_H
#define _IO_DATAOUTPUT_H


#include <lang/String.h>


BEGIN_NAMESPACE(io) 


/**
 * Interface for writing primitive types to the output in portable way.
 * 
 * @ingroup io
 */
class DataOutput
{
public:
	/**
	 * Writes boolean to the stream.
	 *
	 * @exception IOException
	 */
	virtual void writeBoolean( bool value ) = 0;

	/**
	 * Writes 8-bit signed integer to the stream.
	 *
	 * @exception IOException
	 */
	virtual void writeByte( int value ) = 0;

	/**
	 * Writes character to the stream.
	 *
	 * @exception IOException
	 */
	virtual void writeChar( char value ) = 0;

	/**
	 * Writes character sequence to the stream.
	 *
	 * @exception IOException
	 */
	virtual void writeChars( const NS(lang,String)& value ) = 0;

	/**
	 * Writes double to the stream.
	 *
	 * @exception IOException
	 */
	virtual void writeDouble( double value ) = 0;

	/**
	 * Writes float to the stream.
	 *
	 * @exception IOException
	 */
	virtual void writeFloat( float value ) = 0;

	/**
	 * Writes 32-bit signed integer to the stream.
	 *
	 * @exception IOException
	 */
	virtual void writeInt( int value ) = 0;

	/**
	 * Writes 16-bit signed integer to the stream.
	 *
	 * @exception IOException
	 */
	virtual void writeShort( int value ) = 0;

	/**
	 * Writes string encoded in UTF-8 to the stream.
	 *
	 * @exception IOException
	 */
	virtual void writeUTF( const NS(lang,String)& value ) = 0;
};


END_NAMESPACE() // io


#endif // _IO_DATAOUTPUT_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
