#include <io/FileInputStream.h>
#include <io/IOException.h>
#include <lang/TempBuffer.h>
#include <stdio.h>
#include <config.h>


USING_NAMESPACE(lang)


BEGIN_NAMESPACE(io) 

	
FileInputStream::FileInputStream( const String& filename ) :
	m_filename( filename )
{
	const int bufsize = 1000;
	//char buf[bufsize];
	TempBuffer<char> tempbuf( bufsize );
	char* buf = tempbuf.buffer();

	String::cpy( buf, bufsize, filename );
	m_fh = fopen( buf, "rb" );
	if ( !m_fh )
	{
		if ( filename.length() > 128 )
		{
			int offset = filename.length() - 124;
			throwError( IOException( Format("Failed to open {0}", String("... ")+filename.substring(offset)) ) );
		}
		else
		{
			throwError( IOException( Format("Failed to open {0}", filename) ) );
		}
	}
}

FileInputStream::~FileInputStream()
{
	FILE* fh = reinterpret_cast<FILE*>(m_fh);
	if ( fh )
		fclose( fh );
}

int FileInputStream::read( void* data, int size )
{
	FILE* fh = reinterpret_cast<FILE*>(m_fh);
	int bytes = fread( data, 1, size, fh );
	if ( bytes < size && ferror(fh) )
		throwError( IOException( Format("Failed to read {1} bytes from {0}", toString(), size) ) );
	return bytes;
}

int FileInputStream::available() const
{
	FILE* fh = reinterpret_cast<FILE*>(m_fh);
	if ( !fh )
		return 0;
	int cur = ftell( fh );
	fseek( fh, 0, SEEK_END );
	int end = ftell( fh );
	fseek( fh, cur, SEEK_SET );
	if ( ferror(fh) )
		throwError( IOException( Format("Failed to seek {0}", toString()) ) );
	return end-cur;
}

String FileInputStream::toString() const
{
	return m_filename;
}


END_NAMESPACE() // io

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
