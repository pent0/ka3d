#include <io/DataOutputStream.h>
#include <io/IOException.h>
#include <lang/UTFConverter.h>
#include <stdint.h>
#include <config.h>


USING_NAMESPACE(lang)


BEGIN_NAMESPACE(io) 


DataOutputStream::DataOutputStream( OutputStream* out )	: 
	FilterOutputStream(out) 
{
}

void DataOutputStream::writeBoolean( bool value )
{
	uint8_t v = uint8_t(value ? 1 : 0);
	write( &v, sizeof(v) );
}

void DataOutputStream::writeByte( int value )
{
	int8_t v = (uint8_t)value;
	write( &v, sizeof(v) );
}

void DataOutputStream::writeChar( char value )
{
	write( &value, sizeof(value) );
}

void DataOutputStream::writeChars( const String& value )
{
	int len = value.length();
	for ( int i = 0 ; i < len ; ++i )
		writeChar( value.charAt(i) );
}

void DataOutputStream::writeDouble( double value )
{
	double v = value;
	uint8_t* bytes = reinterpret_cast<uint8_t*>( &v );
	writeBE( bytes, sizeof(v) );
}

void DataOutputStream::writeFloat( float value )
{
	float v = value;
	uint8_t* bytes = reinterpret_cast<uint8_t*>( &v );
	writeBE( bytes, sizeof(v) );
}

void DataOutputStream::writeInt( int value )
{
	int32_t v = value;
	uint8_t* bytes = reinterpret_cast<uint8_t*>( &v );
	writeBE( bytes, sizeof(v) );
}

void DataOutputStream::writeShort( int value )
{
	int16_t v = (int16_t)value;
	uint8_t* bytes = reinterpret_cast<uint8_t*>( &v );
	writeBE( bytes, sizeof(v) );
}

void DataOutputStream::writeUTF( const String& value )
{
	int len = value.length();
	if ( len >= 0xFFFF )
		throwError( IOException( Format("Too long UTF-8 string to write to {0}", toString()) ) );
	writeShort( len );

	char buf[1024];
	for ( int i = 0 ; i < len ; )
	{
		int c = len - i;
		if ( c > (int)sizeof(buf) )
			c = (int)sizeof(buf);
		value.getChars( i, i+c, buf );
		write( buf, c );
		i += c;
	}
}

void DataOutputStream::writeBE( void* data, int size )
{
	int x = 1;
	if ( 0 != *reinterpret_cast<char*>(&x) )
	{
		uint8_t* begin = reinterpret_cast<uint8_t*>(data);
		uint8_t* end = begin + size;
		for ( ; begin != end && begin != --end ; ++begin )
		{
			uint8_t tmp = *begin;
			*begin = *end;
			*end = tmp;
		}
	}

	write( data, size );
}


END_NAMESPACE() // io

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
