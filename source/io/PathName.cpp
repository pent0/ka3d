#include <io/PathName.h>
#include <io/IOException.h>
#include <lang/Debug.h>
#include <lang/String.h>
#include <string.h>
#include <config.h>
#include <lang/pp.h>

USING_NAMESPACE(lang)

BEGIN_NAMESPACE(io) 


PathName::PathName() :
	m_alloc( 0 )
{
	m_path = m_drv = m_dir = m_basename = m_suffix = allocate(1);
}

PathName::PathName( const char* pathname )
{
	parse( pathname, 0 );
}

PathName::PathName( const String& pathname )
{
	char buf[MAXLEN];
	if ( !String::cpy(buf, sizeof(buf), pathname) )
		throwError( IOException( Format("Too long path name: \"{0}\"", pathname) ) );
	parse( buf, 0 );
}

PathName::PathName( const char* first, const char* second )
{
	parse( first, second );
}

PathName::PathName( const String& first, const String& second )
{
	char buf[MAXLEN+4];
	if ( !String::cpy(buf, MAXLEN, first) )
		throwError( IOException( Format("Too long path name: \"{0}\" and \"{1}\"", first, second) ) );

	int len1 = strlen( buf );
	buf[len1] = 0;
	char* buf2 = buf+len1+1;

	if ( !String::cpy(buf2, buf+sizeof(buf)-buf2, second) )
		throwError( IOException( Format("Too long path name: \"{0}\" and \"{1}\"", first, second) ) );

	parse( buf, buf2 );
}

PathName::PathName( const PathName& other )
{
	*this = other;
}

PathName& PathName::operator=( const PathName& other )
{
	m_path		= m_buf + (other.m_path - other.m_buf);
	m_drv		= m_buf + (other.m_drv - other.m_buf);
	m_dir		= m_buf + (other.m_dir - other.m_buf);
	m_basename	= m_buf + (other.m_basename - other.m_buf);
	m_suffix	= m_buf + (other.m_suffix - other.m_buf);
	m_alloc		= other.m_alloc;
	memcpy( m_buf, other.m_buf, other.m_alloc );

	assert( !strcmp(m_drv,other.m_drv) );
	assert( !strcmp(m_dir,other.m_dir) );
	assert( !strcmp(m_basename,other.m_basename) );
	assert( !strcmp(m_suffix,other.m_suffix) );
	return *this;
}

PathName PathName::parent() const
{
	char buf[MAXLEN];
	strcpy( buf, m_path );
	int len = strlen(buf);
	if ( len > 0 && buf[len-1] == '/' )
		buf[len-1] = 0;
	char* sep = strrchr( buf, '/' );
	if ( sep != 0 )
		*sep = 0;
	else
		buf[0] = 0;
	return PathName( buf );
}

char* PathName::allocate( int chars )
{
	assert( m_alloc+chars <= (int)sizeof(m_buf) );
	if ( m_alloc+chars > (int)sizeof(m_buf) )
		throwError( IOException( Format("Path name buffer overflow, {0} bytes. Initial path name was \"{1}\".", m_alloc+chars, m_path) ) );

	char* p = m_buf + m_alloc;
	m_alloc += chars;
	memset( p, 0, chars );
	return p;
}

void PathName::parse( const char* path, const char* second )
{
	assert( path != 0 );

	// skip preceding / from second path
	if ( second != 0 && (*second == '/' || *second == '\\') )
		++second;

	// reset
	m_alloc = 0;
	m_path = m_drv = m_dir = m_basename = m_suffix = allocate(1);

	// get whole path
	int len1 = strlen(path);
	int len2 = 0;
	if ( second != 0 )
		len2 = strlen(second);
	int len = len1 + len2;
	m_path = allocate( len+2 );
	strcpy( m_path, path );
	if ( second != 0 )
	{
		if ( len1 > 0 && m_path[len1-1] != '/' && m_path[len1-1] != '\\' )		
			m_path[len1++] = '/';
		memcpy( m_path+len1, second, len2 );
		len = len1+len2;
		m_path[len] = 0;
	}

	// Fix backslashes
	for ( char* s = m_path ; *s != 0 ; ++s )
	{

		if ( *s == '\\' )
			*s = '/';		
	}


	path = m_path;

	// parse drive letter: C:/mydocs/myfile.doc -> C:
	const char* sep = strchr( path, ':' );
	if ( 0 != sep )
	{
		int n = sep - path + 1;
		m_drv = allocate( n+1 );
		memcpy( m_drv, path, n );
		m_drv[n] = 0;
		path = sep + 1;
	}

	// parse directory: /mydocs/myfile.doc -> /mydocs/
	sep = strrchr( path, '/' );
	if ( 0 != sep )
	{
		int n = sep - path + 1;
		m_dir = allocate( n+1 );
		memcpy( m_dir, path, n );
		m_dir[n] = 0;
		path += n;
	}

	// parse basename and suffix
	sep = strrchr( path, '.' );
	if ( 0 != sep )
	{
		int n = sep - path;
		m_basename = allocate( n+1 );
		memcpy( m_basename, path, n );
		m_basename[n] = 0;
		m_suffix = allocate( strlen(sep)+1 );
		strcpy( m_suffix, sep );
	}
	else if ( strlen(path) > 0 )
	{
		m_basename = allocate( strlen(path)+1 );
		strcpy( m_basename, path );
	}
}

String PathName::filename() const
{
	char buf[MAXLEN];
	strcpy( buf, basename() );
	strcat( buf, suffix() );
	return buf;
}


END_NAMESPACE() // io

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
