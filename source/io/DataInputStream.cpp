#include <io/DataInputStream.h>
#include <io/IOException.h>
#include <lang/UTFConverter.h>
#include <stdint.h>
#include <config.h>


USING_NAMESPACE(lang)


BEGIN_NAMESPACE(io) 

DataInputStream::DataInputStream( InputStream* in ) :
	FilterInputStream(in)
{
}

DataInputStream::~DataInputStream()
{
}

int DataInputStream::skip( int n )
{
	int skipped = FilterInputStream::skip( n );
	return skipped;
}

int DataInputStream::read( void* data, int size )
{
	int bytesread = FilterInputStream::read( data, size );
	return bytesread;
}

void DataInputStream::readFully( void* data, int size )
{
	int bytesread = FilterInputStream::read( data, size );
	if ( bytesread != size )
		throwError( IOException( Format("Unexpected end of file in {0}.",toString()) ) );
}

bool DataInputStream::readBoolean()
{
	uint8_t v;
	readFully( &v, sizeof(v) );
	return v != 0;
}

uint8_t DataInputStream::readByte()
{
	int8_t v;
	readFully( &v, sizeof(v) );
	return v;
}

char DataInputStream::readChar()
{
	uint8_t v;
	readFully( &v, sizeof(v) );
	return (char)v;
}

String DataInputStream::readChars( int n )
{
	const int		buffersize = 256;
	char			buffer[buffersize];
	char*			buff = buffer;
	String			str;

	for ( int i = 0 ; i < n ; ++i )
	{
		*buff++ = readChar();
		
		if ( i+1 >= n || buff-buffer > buffersize-8 )
		{
			*buff = char(0);
			str = str + buffer;
		}
	}

	return str;
}

double DataInputStream::readDouble()
{
	uint8_t bytes[ sizeof(double) ];
	readBE( bytes, sizeof(bytes) );

	double v = *reinterpret_cast<double*>(bytes);
	return v;
}

float DataInputStream::readFloat()
{
	uint8_t bytes[ sizeof(float) ];
	readBE( bytes, sizeof(bytes) );
	float v = *reinterpret_cast<float*>(bytes);
	return v;
}

int DataInputStream::readInt()
{
	uint8_t bytes[ sizeof(int32_t) ];
	readBE( bytes, sizeof(bytes) );
	int32_t v = *reinterpret_cast<int32_t*>(bytes);
	return (int)v;
}

int DataInputStream::readShort()
{
	uint8_t bytes[ sizeof(int16_t) ];
	readBE( bytes, sizeof(bytes) );
	int16_t v = *reinterpret_cast<int16_t*>(bytes);
	return (int)v;
}

String DataInputStream::readUTF()
{
	int encodedbytes = readShort();
	if ( encodedbytes < 0 )
		throwError( IOException( Format("Invalid UTF-8 data in {0}.",toString()) ) );

	String str;
	if ( encodedbytes > 0 )
	{
		m_buf.resize( encodedbytes+1 );
		readFully( m_buf.begin(), encodedbytes );
		str = String( m_buf.begin(), encodedbytes, UTFConverter(UTFConverter::ENCODING_UTF8) );
	}
	return str;
}

void DataInputStream::readUTF( char* buf, int bufsize )
{
	int encodedbytes = readShort();
	if ( encodedbytes < 0 )
		throwError( IOException( Format("Invalid UTF-8 data in {0}.",toString()) ) );
	if ( encodedbytes >= bufsize )
		throwError( IOException( Format("Too small buffer ({0}) for UTF-8 data in {1}.",bufsize,toString()) ) );
	
	readFully( buf, encodedbytes );
	buf[encodedbytes] = 0;
}

void DataInputStream::readUTF( Array<char>& buf )
{
	int encodedbytes = readShort();
	if ( encodedbytes < 0 )
		throwError( IOException( Format("Invalid UTF-8 data in {0}.",toString()) ) );

	buf.resize( encodedbytes+1 );
	readFully( buf.begin(), encodedbytes );
	buf[encodedbytes] = 0;
}

void DataInputStream::readBE( void* data, int size )
{
	readFully( data, size );

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
}


END_NAMESPACE() // io

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
