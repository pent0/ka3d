#include <lang/Formattable.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <config.h>


BEGIN_NAMESPACE(lang) 


static void parseNumberPattern( 
	const String& pattern, int pos,
	int& minIntDigits, int& maxIntDigits,
	int& minFracDigits, int& maxFracDigits,
	bool& fraction, bool& grouping, char& hex )
{
	const int absMaxIntDigits = 20;

	minIntDigits		= 0;
	maxIntDigits		= 0;
	minFracDigits		= 0;
	maxFracDigits		= 0;
	fraction			= false;
	grouping			= false;
	hex					= 0;

	int patternEnd = pos;
	for ( ; patternEnd < pattern.length() ; ++patternEnd )
		if ( pattern.charAt(patternEnd) == char('}') )
			break;

	for ( int i = pos ; i < patternEnd ; ++i )
	{
		char ch = pattern.charAt(i);

		if ( ch == 'x' || ch == 'X' )
		{
			hex = (char)ch;
			break;
		}
		else if ( ch == char('.') )
		{
			fraction = true;
		}
		else if ( ch == char(',') )
		{
			grouping = true;
		}
		else if ( ch == char('0') )
		{
			// compulsory digit
			if ( fraction )
				++minFracDigits, ++maxFracDigits;
			else
				++minIntDigits, maxIntDigits=absMaxIntDigits;
		}
		else if ( ch == char('#') )
		{
			// optional digit
			if ( fraction )
				++maxFracDigits;
			else
				maxIntDigits=absMaxIntDigits;
		}
	}
}


Formattable::Formattable()
{
	m_type = VALUE_NONE;
}

Formattable::Formattable( double value )
{
	m_type = VALUE_DOUBLE;
	m_dbl = value;
}

Formattable::Formattable( const String& value )
{
	m_type = VALUE_STRING;
	m_str = value;
}

Formattable::Formattable( const char* value )
{
	m_type = VALUE_STRING;
	m_str = value;
}

Formattable::ValueType Formattable::type() const
{
	return m_type;
}

double Formattable::doubleValue() const
{
	assert( m_type == VALUE_DOUBLE );
	return m_dbl;
}

String Formattable::stringValue() const
{
	assert( m_type == VALUE_STRING );
	return m_str;
}

int	Formattable::format( char* buffer, int size, const String& pattern, int pos ) const
{
	if ( VALUE_DOUBLE == m_type )
	{
		int minIntDigits, maxIntDigits;
		int minFracDigits, maxFracDigits;
		bool fraction, grouping;
		char hex;
		parseNumberPattern( pattern, pos, minIntDigits, maxIntDigits, minFracDigits, maxFracDigits, fraction, grouping, hex );

		bool chopInt = false;
		char fmt[32];
		if ( hex )
			sprintf( fmt, "%%%c", hex ), chopInt = true;
		else if ( 0 == maxFracDigits && 0 == maxIntDigits )
			sprintf( fmt, "%%g" );
		else if ( minIntDigits > 0 )
			sprintf( fmt, "%%0%i.%if", minIntDigits, maxFracDigits );
		else
			sprintf( fmt, "%%.%if", maxFracDigits );

		char buff[32];
		if ( chopInt )
			sprintf( buff, fmt, (int)m_dbl );
		else
			sprintf( buff, fmt, m_dbl );

		int needed = strlen(buff);
		int count = needed;
		if ( count > size ) 
			count = size;
		for ( int i = 0 ; i < count ; ++i )
			buffer[i] = buff[i];
		if ( needed < size )
			buffer[needed] = 0;
		else if ( size > 0 )
			buffer[size-1] = 0;
		return needed;
	}
	else if ( VALUE_STRING == m_type )
	{
		int needed = m_str.length();
		int count = needed;
		if ( count > size ) 
			count = size;
		m_str.getChars( 0, count, buffer );
		if ( needed < size )
			buffer[needed] = 0;
		else if ( size > 0 )
			buffer[size-1] = 0;
		return needed;
	}
	return 0;
}


END_NAMESPACE()

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
