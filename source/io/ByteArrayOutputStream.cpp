#include <io/ByteArrayOutputStream.h>
#include <string.h>
#include <config.h>


USING_NAMESPACE(lang)


BEGIN_NAMESPACE(io) 


ByteArrayOutputStream::ByteArrayOutputStream( int size ) :
	m_buffer( new Array<uint8_t>(size) ),
	m_userDefined( false )
{
	m_buffer->resize( 0 );
}

ByteArrayOutputStream::ByteArrayOutputStream( Array<uint8_t>* buffer ) :
	m_buffer( buffer ),
	m_userDefined( true )
{
	assert( 0 != m_buffer );

	m_buffer->resize( 0 );
}

ByteArrayOutputStream::~ByteArrayOutputStream()
{
	if ( !m_userDefined )
	{
		delete m_buffer;
		m_buffer = 0;
	}
}

void ByteArrayOutputStream::reset()
{
	m_buffer->resize( 0 );
}

void ByteArrayOutputStream::write( const void* data, int size )
{
	int oldsize = m_buffer->size();
	m_buffer->resize( oldsize + size );
	for ( int i = 0 ; i < size ; ++i )
		(*m_buffer)[oldsize+i] = reinterpret_cast<const char*>(data)[i];
}

int ByteArrayOutputStream::size() const
{
	return m_buffer->size();
}

String ByteArrayOutputStream::toString() const
{
	return "ByteArrayInputStream";
}


END_NAMESPACE() // io

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
