#include <io/FilterInputStream.h>
#include <config.h>


USING_NAMESPACE(lang)


BEGIN_NAMESPACE(io) 


FilterInputStream::FilterInputStream( InputStream* source ) : 
	m_source( source ),
	m_bytesRead( 0 )
{
}

int FilterInputStream::read( void* data, int size )											
{
	int bytes = m_source->read( data, size );
	m_bytesRead += bytes;
	return bytes;
}

int FilterInputStream::skip( int n )															
{
	assert( n >= 0 );
	int bytes = m_source->skip(n);
	m_bytesRead += bytes;
	return bytes;
}

int FilterInputStream::available() const														
{
	return m_source->available();
}

String FilterInputStream::toString() const
{
	return m_source->toString();
}

int FilterInputStream::bytesRead() const
{
	return m_bytesRead;
}


END_NAMESPACE() // io

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
