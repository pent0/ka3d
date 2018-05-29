#include <io/FilterOutputStream.h>
#include <config.h>


USING_NAMESPACE(lang)


BEGIN_NAMESPACE(io) 


FilterOutputStream::FilterOutputStream( OutputStream* target ) :
	m_target( target ),
	m_bytesWritten( 0 )
{
}

void FilterOutputStream::write( const void* data, int size )									
{
	m_target->write( data, size );
	m_bytesWritten += size;
}

String FilterOutputStream::toString() const
{
	return m_target->toString();
}

int FilterOutputStream::bytesWritten() const
{
	return m_bytesWritten;
}


END_NAMESPACE() // io

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
