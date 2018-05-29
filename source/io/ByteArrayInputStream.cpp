#include <io/ByteArrayInputStream.h>
#include <stdint.h>
#include <string.h>
#include <config.h>


USING_NAMESPACE(lang)


BEGIN_NAMESPACE(io) 


ByteArrayInputStream::ByteArrayInputStream( const void* data, int size )
{
	reset( data, size );
}

ByteArrayInputStream::~ByteArrayInputStream()
{
}

void ByteArrayInputStream::reset( const void* data, int size )
{
	m_data.resize( size );
	if ( size > 0 )
		memcpy( m_data.begin(), data, size );
	m_index = 0;
}

int ByteArrayInputStream::read( void* data, int size )
{
	assert( m_index+size <= m_data.size() );
	
	int left = available();
	int count = size;
	if ( left < count )
		count = left;

	const uint8_t*	src		= reinterpret_cast<const uint8_t*>( m_data.begin() ) + m_index;
	uint8_t*		dest	= reinterpret_cast<uint8_t*>( data );

	for ( int i = 0 ; i < count ; ++i )
		dest[i] = src[i];

	m_index += count;
	return count;
}

int ByteArrayInputStream::available() const
{
	return m_data.size() - m_index;
}

String ByteArrayInputStream::toString() const
{
	return "ByteArrayInputStream";
}


END_NAMESPACE()

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
