#include <lang/Format.h>
#include <lang/Array.h>
#include <lang/Character.h>
#include <lang/FormatException.h>
#include <config.h>


BEGIN_NAMESPACE(lang) 


Format::Format()
{
	m_args = 0;
}

Format::Format( const String& pattern )
{
	m_pattern = pattern;
	m_args = 0;
}

Format::Format( const String& pattern, const Formattable& arg0 )
{
	m_pattern = pattern;
	m_args	= 1;
	m_argv[0] = arg0;
}

Format::Format( const String& pattern, const Formattable& arg0, const Formattable& arg1 )
{
	m_pattern = pattern;
	m_args	= 2;
	m_argv[0] = arg0;
	m_argv[1] = arg1;
}

Format::Format( const String& pattern, const Formattable& arg0, const Formattable& arg1, const Formattable& arg2 )
{
	m_pattern = pattern;
	m_args	= 3;
	m_argv[0] = arg0;
	m_argv[1] = arg1;
	m_argv[2] = arg2;
}

Format::Format( const String& pattern, const Formattable& arg0, const Formattable& arg1, const Formattable& arg2, const Formattable& arg3 )
{
	m_pattern = pattern;
	m_args	= 4;
	m_argv[0] = arg0;
	m_argv[1] = arg1;
	m_argv[2] = arg2;
	m_argv[3] = arg3;
}

Format::Format( const String& pattern, const Formattable& arg0, const Formattable& arg1, const Formattable& arg2, const Formattable& arg3, const Formattable& arg4 )
{
	m_pattern = pattern;
	m_args	= 5;
	m_argv[0] = arg0;
	m_argv[1] = arg1;
	m_argv[2] = arg2;
	m_argv[3] = arg3;
	m_argv[4] = arg4;
}

Format::Format( const String& pattern, const Formattable& arg0, const Formattable& arg1, const Formattable& arg2, const Formattable& arg3, const Formattable& arg4, const Formattable& arg5 )
{
	m_pattern = pattern;
	m_args	= 6;
	m_argv[0] = arg0;
	m_argv[1] = arg1;
	m_argv[2] = arg2;
	m_argv[3] = arg3;
	m_argv[4] = arg4;
	m_argv[5] = arg5;
}

Format::Format( const String& pattern, const Formattable& arg0, const Formattable& arg1, const Formattable& arg2, const Formattable& arg3, const Formattable& arg4, const Formattable& arg5, const Formattable& arg6 )
{
	m_pattern = pattern;
	m_args	= 7;
	m_argv[0] = arg0;
	m_argv[1] = arg1;
	m_argv[2] = arg2;
	m_argv[3] = arg3;
	m_argv[4] = arg4;
	m_argv[5] = arg5;
	m_argv[6] = arg6;
}

Format::Format( const String& pattern, const Formattable& arg0, const Formattable& arg1, const Formattable& arg2, const Formattable& arg3, const Formattable& arg4, const Formattable& arg5, const Formattable& arg6, const Formattable& arg7 )
{
	m_pattern = pattern;
	m_args	= 8;
	m_argv[0] = arg0;
	m_argv[1] = arg1;
	m_argv[2] = arg2;
	m_argv[3] = arg3;
	m_argv[4] = arg4;
	m_argv[5] = arg5;
	m_argv[6] = arg6;
	m_argv[7] = arg7;
}

Format::Format( const String& pattern, const Formattable& arg0, const Formattable& arg1, const Formattable& arg2, const Formattable& arg3, const Formattable& arg4, const Formattable& arg5, const Formattable& arg6, const Formattable& arg7, const Formattable& arg8 )
{
	m_pattern = pattern;
	m_args	= 9;
	m_argv[0] = arg0;
	m_argv[1] = arg1;
	m_argv[2] = arg2;
	m_argv[3] = arg3;
	m_argv[4] = arg4;
	m_argv[5] = arg5;
	m_argv[6] = arg6;
	m_argv[7] = arg7;
	m_argv[8] = arg8;
}

Format::Format( const String& pattern, const Formattable& arg0, const Formattable& arg1, const Formattable& arg2, const Formattable& arg3, const Formattable& arg4, const Formattable& arg5, const Formattable& arg6, const Formattable& arg7, const Formattable& arg8, const Formattable& arg9 )
{
	m_pattern = pattern;
	m_args	= 10;
	m_argv[0] = arg0;
	m_argv[1] = arg1;
	m_argv[2] = arg2;
	m_argv[3] = arg3;
	m_argv[4] = arg4;
	m_argv[5] = arg5;
	m_argv[6] = arg6;
	m_argv[7] = arg7;
	m_argv[8] = arg8;
	m_argv[9] = arg9;
}

Format::Format( const String& pattern, int argc, Formattable* argv )
{
	assert( argc <= 10 );

	if ( argc < 10 )
		argc = 10;

	m_pattern = pattern;
	m_args	= argc;
	for ( int i = 0 ; i < argc ; ++i )
		m_argv[i] = argv[i];
}

void Format::setPattern( const String& str )
{
	m_pattern = str;
}

const String& Format::pattern() const
{
	return m_pattern;
}

int Format::arguments() const
{
	return m_args;
}

const Formattable& Format::getArgument( int i ) const
{
	assert( i >= 0 && i < arguments() );
	return m_argv[i];
}

int Format::format( char* buffer, int size ) const
{
	int		d		= 0;
	int		s		= 0;
	bool	skip	= false;

	while ( d < size && s < (int)m_pattern.length() )
	{
		char ch = m_pattern.charAt( s++ );

		if ( ch == '{' && !skip && s < m_pattern.length() )
		{
			// <index><,opt>}
			int end = m_pattern.indexOf( '}', s );
			if ( -1 == end )
				throwError( FormatException( Format("Failed to format \"{0}\", '}' not found after {1}", m_pattern, s) ) );
			if ( end-s == 0 )
				throwError( FormatException( Format("Failed to format \"{0}\", argument index not found at {1}", m_pattern, s) ) );

			// argument index
			char digit = m_pattern.charAt( s++ );
			if ( !Character::isDigit(digit) )
				throwError( FormatException( Format("Failed to format \"{0}\", digit expected at {1}", m_pattern, s-1) ) );
			int arg = digit - '0';
			if ( !(arg >= 0 && arg < m_args) )
				throwError( FormatException( Format("Failed to format \"{0}\", invalid argument index at {1}", m_pattern, s-1) ) );
				
			// <,opt>}
			int left = size - d;
			if ( left < 0 )
				left = 0;
			if ( s < m_pattern.length() && m_pattern.charAt(s) == ',' )
				++s;
				
			d += m_argv[arg].format( buffer+d, left, m_pattern, s );
			s = end+1;
		}
		else if ( ch == '\\' && !skip )
		{
			skip = true;
		}
		else
		{
			if ( d < size )
				buffer[d] = ch;
			++d;
			skip = false;
		}
	}

	int end = d;
	if ( end >= size )
		end = size-1;
	if ( end >= 0 )
		buffer[end] = 0;
	return d+1;
}

String Format::format() const
{
	char buf[512];
	int len = format( buf, sizeof(buf) );
	if ( len >= (int)sizeof(buf) )
	{
		Array<char> buf2( len+2 );
		format( &buf2[0], buf2.size() );
		return String( &buf2[0] );
	}
	return buf;
}


END_NAMESPACE()

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
