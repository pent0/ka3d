#include <io/FileOutputStream.h>
#include <io/FileNotFoundException.h>
#include <lang/TempBuffer.h>
#include <stdio.h>
#include <string.h>
#include <config.h>


USING_NAMESPACE(lang)


BEGIN_NAMESPACE(io) 


FileOutputStream::FileOutputStream( const String& filename ) :
	m_filename( filename )
{
	const int bufsize = 1000;
	//char buf[bufsize];
	TempBuffer<char> tempbuf( bufsize );
	char* buf = tempbuf.buffer();

	String::cpy( buf, bufsize, filename );
	m_fh = fopen( buf, "wb" );
	if ( !m_fh )
		throwError( FileNotFoundException( Format("Failed to open {0}", filename) ) );
}

FileOutputStream::~FileOutputStream()
{
	FILE* fh = reinterpret_cast<FILE*>(m_fh);
	if ( fh )
		fclose( fh );
}

void FileOutputStream::write( const void* data, int size )
{
	FILE* fh = reinterpret_cast<FILE*>(m_fh);
	int bytes = fwrite( data, 1, size, fh );
	if ( bytes < size && ferror(fh) )
		throwError( IOException( Format("Failed to write {1} bytes to {0}", toString(), size) ) );
}

String FileOutputStream::toString() const
{
	return m_filename;
}


END_NAMESPACE() // io

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
