#include <lang/String.h>
#include <lang/Globals.h>
#include <lang/MemoryPool.h>
#include <lang/UTFConverter.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <config.h>


BEGIN_NAMESPACE(lang) 


static int allocateString( int len )
{
	int handle = -1;
	if ( len > 0 )
	{
		handle = lang_Globals::get().stringPool.allocate( len+1 );
		char* s = lang_Globals::get().stringPool.get(handle);
		s[0] = s[len] = 0;
	}
	return handle;
}

static int allocateString( const void* data, int bytes, const Converter& decoder )
{
	// find out UTF-8 length
	const uint8_t* databytes = reinterpret_cast<const uint8_t*>(data);
	int len = 0;
	UTFConverter encoder( UTFConverter::ENCODING_UTF8 );
	char buf[32];
	for ( int i = 0 ; i < bytes ; )
	{
		int decodedbytes = 1;
		int cp;
		if ( decoder.decode(databytes+i, databytes+bytes, &decodedbytes, &cp) )
		{
			i += decodedbytes;
			int encodedbytes = 0;
			encoder.encode( buf, buf+sizeof(buf), &encodedbytes, cp );
			len += encodedbytes;
		}
		else
		{
			++i;
		}
	}

	// alloc and set UTF-8 string
	int strh = allocateString( len );
	if ( len > 0 )
	{
		char* s = lang_Globals::get().stringPool.get(strh);
		int d = 0;
		for ( int i = 0 ; i < bytes ; )
		{
			int decodedbytes = 1;
			int cp;
			if ( decoder.decode(databytes+i, databytes+bytes, &decodedbytes, &cp) )
			{
				i += decodedbytes;
				int encodedbytes = 0;
				encoder.encode( s+d, s+len, &encodedbytes, cp );
				d += encodedbytes;
			}
			else
			{
				++i;
			}
		}
	}
	
	return strh;
}


String::String() :
	m_h(-1)
{
}

String::String( const char* str ) :
	m_h(-1)
{
	if ( str )
	{
		// DEBUG: print String ctor
		//printf( "String('%s')\n", str );
		int len = strlen( str );
		if ( len > 0 )
		{
			m_h = allocateString( len );
			memcpy( lang_Globals::get().stringPool.get(m_h), str, len );
		}
	}
}

String::String( const void* data, int size, const Converter& decoder ) :
	m_h( allocateString(data, size, decoder) )
{
}

String::String( const String& other ) :
	m_h( other.m_h )
{
	if ( other.m_h != -1 )
		lang_Globals::get().stringPool.ref( other.m_h );
}

String::~String()
{
	if ( m_h != -1 )
		lang_Globals::get().stringPool.unref( m_h );
}

int String::getBytes( void* buf, int bufsize, Converter& encoder ) const
{
	if ( m_h == -1 )
		return 0;

	UTFConverter decoder( UTFConverter::ENCODING_UTF8 );
	char* s = lang_Globals::get().stringPool.get( m_h );
	int len = strlen( s );

	int bytesread = 0;
	int byteswritten = 0;

	while ( bytesread < len )
	{
		int decodedbytes = 1;
		int cp;
		if ( decoder.decode(s+bytesread, s+len, &decodedbytes, &cp) )
		{
			bytesread += decodedbytes;

			char temp[8];
			int encodedbytes = 0;
			if ( encoder.encode( temp, temp+8, &encodedbytes, cp ) )
			{
				for ( int i = 0 ; i < encodedbytes && byteswritten+i < bufsize ; ++i )
					((uint8_t*)buf)[byteswritten+i] = temp[i];

				byteswritten += encodedbytes;
			}
		}
		else
		{
			// skip invalid UTF-8 byte
			++bytesread;
		}
	}
	return byteswritten;
}

int String::getUTF16( short* buf, int buflen ) const
{
	UTFConverter conv( UTFConverter::ENCODING_UTF16 );
	int len = getBytes( buf, (buflen<<1), conv ) >> 1;
	if ( len >= buflen )
		len = buflen-1;
	buf[len] = 0;
	return len;
}

String& String::operator=( const String& other )
{
	if ( other.m_h != -1 )
		lang_Globals::get().stringPool.ref( other.m_h );
	if ( m_h != -1 )
		lang_Globals::get().stringPool.unref( m_h );
	m_h = other.m_h;
	return *this;
}

int String::length() const
{
	return m_h != -1 ? strlen(lang_Globals::get().stringPool.get(m_h)) : 0;
}

char String::charAt( int index ) const											
{
	assert( index >= 0 && index < length() );
	char* s = lang_Globals::get().stringPool.get(m_h);
	return s[index];
}

void String::getChars( int begin, int end, char* dest ) const
{
	assert( begin >= 0 && begin <= length() );
	assert( begin >= 0 && begin <= end );
	assert( end <= length() );

	if ( end > begin )
	{
		char* s = lang_Globals::get().stringPool.get(m_h);
		memcpy( dest, s+begin, end-begin );
	}
}

void String::get( char* buf, int bufsize ) const
{
	assert( buf != 0 );
	assert( bufsize > length() );

	int len = 0;
	if ( -1 != m_h )
	{
		char* s = lang_Globals::get().stringPool.get(m_h);
		len = strlen(s);
		if ( len >= bufsize )
			len = bufsize-1;
		memcpy( buf, s, len );
	}
	if ( bufsize > 0 )
		buf[len] = 0;
}

const char* String::c_str() const
{
	lang_Globals& g = lang_Globals::get();
	const int BUFSIZE = sizeof(g.cstrBuffer)/sizeof(g.cstrBuffer[0]);
	const char* sz = (m_h != -1 ? g.stringPool.get(m_h) : "");
	int len = strlen(sz);
	if ( len >= BUFSIZE )
		len = BUFSIZE-1;

	char* buf = g.cstrBuffer;
	int& bufp = lang_Globals::get().cstrBufferIndex;
	if ( bufp+len >= BUFSIZE )
		bufp = 0;
	
	assert( bufp+len < BUFSIZE );
	const int bufp0 = bufp;
	bufp += len+1;

	assert( bufp0 >= 0 && bufp0 < BUFSIZE );
	assert( bufp0+len >= 0 && bufp0+len < BUFSIZE );
	if ( len > 0 )
		memcpy( buf+bufp0, sz, len );

	buf[bufp0+len] = 0;
	return buf+bufp0;
}

bool String::endsWith( const String& suffix ) const
{
	assert( suffix.length() > 0 );
	
	int		thislen		= length();
	int		otherlen	= suffix.length();

	if ( otherlen <= thislen )
		return 0 == strncmp( lang_Globals::get().stringPool.get(m_h)+thislen-otherlen, lang_Globals::get().stringPool.get(suffix.m_h), otherlen );
	else
		return false;
}

bool String::startsWith( const String& prefix ) const
{
	assert( prefix.length() > 0 );
	
	int		thislen		= length();
	int		otherlen	= prefix.length();

	if ( otherlen <= thislen )
		return 0 == strncmp( lang_Globals::get().stringPool.get(m_h), lang_Globals::get().stringPool.get(prefix.m_h), otherlen );
	else
		return false;
}

int String::hashCode() const
{
	int code = 0;
	int thislen = length();

	if ( thislen > 0 )
	{
		char* s = lang_Globals::get().stringPool.get(m_h);
		for ( int i = 0 ; i < thislen ; ++i )
		{
			code *= 31;
			code += s[i];
		}
	}
	return code;
}

int String::indexOf( char ch, int index ) const
{
	assert( index >= 0 && index < length() );

	int thislen = length();
	char* s = lang_Globals::get().stringPool.get(m_h);
	for ( ; index < thislen ; ++index )
	{
		if ( s[index] == ch )
			return index;
	}
	return -1;
}

int String::indexOf( const String& str, int index ) const
{
	assert( index >= 0 );
	assert( str.length() > 0 );

	int		thislen		= length();
	int		slen		= str.length();
	int		lastIndex	= thislen - slen;
	char*	s1			= lang_Globals::get().stringPool.get(str.m_h);
	char*	s2			= lang_Globals::get().stringPool.get(m_h);

	for ( ; index <= lastIndex ; ++index )
	{
		int j = 0;
		for ( ; j < slen ; ++j )
		{
			if ( s1[j] != s2[index+j] )
				break;
		}
		if ( slen == j )
			return index;
	}
	return -1;
}

int String::lastIndexOf( char ch ) const										
{
	int len = length();
	if ( 0 == len )
		return -1;
	else
		return lastIndexOf( ch, len-1 );
}

int String::lastIndexOf( char ch, int index ) const
{
	assert( index >= 0 && index < length() );

	char* s = lang_Globals::get().stringPool.get(m_h);
	for ( ; index >= 0 ; --index )
	{
		if ( s[index] == ch )
			return index;
	}
	return -1;
}

int String::lastIndexOf( const String& str, int index ) const
{
	assert( index >= 0 && index < length() );
	assert( str.length() > 0 );

	int		thislen		= length();
	int		slen		= str.length();
	char*	s1			= lang_Globals::get().stringPool.get(str.m_h);
	char*	s2			= lang_Globals::get().stringPool.get(m_h);

	if ( index+slen > thislen )
		index = thislen - slen;

	for ( ; index >= 0 ; --index )
	{
		int j = 0;
		for ( ; j < slen ; ++j )
		{
			if ( s1[j] != s2[index+j] )
				break;
		}
		if ( slen == j )
			return index;
	}
	return -1;
}

bool String::regionMatches( int thisoffset, const String& other, int otheroffset, int length ) const
{
	assert( other.length() > 0 );
	
	int		thislen		= this->length();
	int		otherlen	= other.length();
	char*	s1			= lang_Globals::get().stringPool.get(m_h);
	char*	s2			= lang_Globals::get().stringPool.get(other.m_h);

	if ( thisoffset >= 0 && 
		otheroffset >= 0 &&
		thisoffset+length <= thislen &&
		otheroffset+length <= otherlen )
	{
		for ( int c = 0 ; c < length ; ++c )
		{
			if ( s1[thisoffset+c] != s2[otheroffset+c] )
				return false;
		}
		return true;
	}
	return false;
}

String String::replace( char oldchar, char newchar ) const
{
	int thislen = length();
	String str;
	if ( thislen > 0 )
	{
		str.m_h = allocateString( thislen );
		char* s = lang_Globals::get().stringPool.get(str.m_h);
		char* s2 = lang_Globals::get().stringPool.get(m_h);
		
		for ( int i = 0 ; i < thislen ; ++i )
		{
			char c = s2[i];
			if ( c == oldchar )
				c = newchar;
			s[i] = c;
		}
	}

	return str;
}

String String::substring( int begin, int end ) const
{
	assert( begin >= 0 && end <= length() && end >= begin );

	String str;
	if ( end > begin )
	{
		int len = end - begin;
		str.m_h = allocateString( len );
		memcpy( lang_Globals::get().stringPool.get(str.m_h), lang_Globals::get().stringPool.get(m_h)+begin, len*sizeof(char) );
	}
	
	return str;
}

String String::substring( int begin ) const
{
	assert( begin >= 0 && begin <= length() );
	return substring( begin, length() );
}

String String::toLowerCase() const
{
	int thislen = length();
	String str;
	if ( thislen > 0 )
	{
		str.m_h = allocateString( thislen );
		char* s = lang_Globals::get().stringPool.get(str.m_h);
		char* s0 = lang_Globals::get().stringPool.get(m_h);
		
		for ( int i = 0 ; i < thislen ; ++i )
		{
			char ch = s0[i];
			if ( (unsigned char)ch < (unsigned char)0x80 )
				ch = (char)tolower( ch );
			s[i] = ch;
		}
	}

	return str;
}

String String::toUpperCase() const
{
	int thislen = length();
	String str;
	if ( thislen > 0 )
	{
		str.m_h = allocateString( thislen );
		char* s0 = lang_Globals::get().stringPool.get(m_h);
		char* s = lang_Globals::get().stringPool.get(str.m_h);
		
		for ( int i = 0 ; i < thislen ; ++i )
		{
			char ch = s0[i];
			if ( (unsigned char)ch < (unsigned char)0x80 )
				ch = (char)toupper( ch );
			s[i] = ch;
		}
	}

	return str;
}

String String::trim() const
{
	int thislen = length();
	int begin	= 0;
	int end		= thislen;
	char* s0 	= lang_Globals::get().stringPool.get(m_h);

	for ( ; begin < thislen ; ++begin )
	{
		char ch = s0[begin];
		if ( (unsigned char)ch >= (unsigned char)0x80 ||
			!isspace( ch ) )
			break;
	}
	for ( ; end > 0 ; --end )
	{
		char ch = s0[end-1];
		if ( (unsigned char)ch >= (unsigned char)0x80 ||
			!isspace( ch ) )
			break;
	}
	return substring( begin, end );
}

int String::compareTo( const String& other ) const 
{
	return strcmp(
		m_h != -1 ? lang_Globals::get().stringPool.get(m_h) : "",
		other.m_h != -1 ? lang_Globals::get().stringPool.get(other.m_h) : "" );
}

int String::compareTo( const char* other ) const 
{
	return strcmp( m_h != -1 ? lang_Globals::get().stringPool.get(m_h) : "", other );
}

String String::operator+( const char* other ) const
{
	assert( other );
	
	int len1 = length();
	int len2 = strlen(other);
	int len = len1 + len2;
	
	String s;
	if ( len > 0 )
	{
		s.m_h = allocateString( len );
		char* sz = lang_Globals::get().stringPool.get(s.m_h);
		if ( m_h != -1 )
			strcpy( sz, lang_Globals::get().stringPool.get(m_h) );
		strcat( sz, other );
	}

	return s;
}

String String::operator+( const String& other ) const
{
	int len1 = length();
	int len2 = other.length();
	int len = len1 + len2;
	
	String s;
	if ( len > 0 )
	{
		s.m_h = allocateString( len );
		char* sz = lang_Globals::get().stringPool.get(s.m_h);
		if ( m_h != -1 )
			memcpy( sz, lang_Globals::get().stringPool.get(m_h), len1 );
		if ( other.m_h != -1 )
			memcpy( sz+len1, lang_Globals::get().stringPool.get(other.m_h), len2 );
	}

	return s;
}

bool String::cpy( char* buf, int bufsize, const char* sz )
{
	assert( bufsize > 0 );
	int bufp = 0;
	while ( 0 != *sz && bufp+1 < bufsize )
		buf[bufp++] = *sz++;
	assert( bufp < bufsize );
	buf[bufp] = 0;
	return *sz == 0;
}

bool String::cpy( char* buf, int bufsize, const String& str )
{
	assert( bufsize > 0 );
	if ( str.m_h != -1 )
		return String::cpy( buf, bufsize, lang_Globals::get().stringPool.get(str.m_h) );
	else
        buf[0] = 0;
	return true;
}

bool String::cat( char* buf, int bufsize, const char* sz )
{
	assert( bufsize > 0 );
	int bufp = strlen(buf);
	assert( bufp < bufsize );
	while ( 0 != *sz && bufp+1 < bufsize )
		buf[bufp++] = *sz++;
	assert( bufp < bufsize );
	buf[bufp] = 0;
	return *sz == 0;
}

bool String::cat( char* buf, int bufsize, const String& str )
{
	assert( bufsize > 0 );
	if ( str.m_h != -1 )
		return String::cat( buf, bufsize, lang_Globals::get().stringPool.get(str.m_h) );
	return true;
}


END_NAMESPACE() // lang

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
