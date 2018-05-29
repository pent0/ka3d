#ifndef _LANG_FORMAT_H
#define _LANG_FORMAT_H


#include <lang/Formattable.h>


BEGIN_NAMESPACE(lang) 


/**
 * Format is a class for constructing messages that keep parameters 
 * separately from the formatting string. This is useful 
 * to be used for example in translation.
 *
 * Syntax of format string (=first parameter of every constructor):
 * <ul>
 * <li>{0} = replaced with 1st argument formatted to string.
 * <li>{1} = replaced with 2nd argument formatted to string.
 * <li>{n} = replaced with nth argument formatted to string.
 * <li>{0,x} = replaced with 1st argument formatted to hex number.
 * <li>{0,#.##} = replaced with 1st argument formatted to number using max 2 digits, e.g. 1.2=1.2 but 1.211=1.21.
 * <li>{0,#.00} = replaced with 1st argument formatted to number using exactly 2 digits, e.g. 1.2=1.20 and 1.211=1.21.
 * <li>{0,000} = replaced with 1st argument formatted to number using at least 3 numbers, padded with zeros, e.g. 3=003.
 * </ul>
 * 
 * After constructing a Format object, it can be either formatted 
 * directly to text by using format() method, or the format string 
 * can be accessed or changed separately for translation.
 * 
 * @ingroup lang
 */
class Format
{
public:
	/** Creates an empty message. */
	Format();

	/** Creates a message with no arguments. */
	Format( const String& str );

	/** Creates a message with 1 argument. */
	Format( const String& pattern, const Formattable& arg0 );

	/** Creates a message with 2 arguments. */
	Format( const String& pattern, const Formattable& arg0, const Formattable& arg1 );

	/** Creates a message with 3 arguments. */
	Format( const String& pattern, const Formattable& arg0, const Formattable& arg1, const Formattable& arg2 );

	/** Creates a message with 4 arguments. */
	Format( const String& pattern, const Formattable& arg0, const Formattable& arg1, const Formattable& arg2, const Formattable& arg3 );

	/** Creates a message with 5 arguments. */
	Format( const String& pattern, const Formattable& arg0, const Formattable& arg1, const Formattable& arg2, const Formattable& arg3, const Formattable& arg4 );

	/** Creates a message with 6 arguments. */
	Format( const String& pattern, const Formattable& arg0, const Formattable& arg1, const Formattable& arg2, const Formattable& arg3, const Formattable& arg4, const Formattable& arg5 );

	/** Creates a message with 7 arguments. */
	Format( const String& pattern, const Formattable& arg0, const Formattable& arg1, const Formattable& arg2, const Formattable& arg3, const Formattable& arg4, const Formattable& arg5, const Formattable& arg6 );

	/** Creates a message with 8 arguments. */
	Format( const String& pattern, const Formattable& arg0, const Formattable& arg1, const Formattable& arg2, const Formattable& arg3, const Formattable& arg4, const Formattable& arg5, const Formattable& arg6, const Formattable& arg7 );

	/** Creates a message with 9 arguments. */
	Format( const String& pattern, const Formattable& arg0, const Formattable& arg1, const Formattable& arg2, const Formattable& arg3, const Formattable& arg4, const Formattable& arg5, const Formattable& arg6, const Formattable& arg7, const Formattable& arg8 );

	/** Creates a message with 10 arguments. */
	Format( const String& pattern, const Formattable& arg0, const Formattable& arg1, const Formattable& arg2, const Formattable& arg3, const Formattable& arg4, const Formattable& arg5, const Formattable& arg6, const Formattable& arg7, const Formattable& arg8, const Formattable& arg9 );

	/** Creates a message with n arguments. */
	Format( const String& pattern, int argc, Formattable* argv );

	/** 
	 * Sets format string but keeps the format parameters. 
	 * Can be used to translate text.
	 */
	void				setPattern( const String& str );

	/** Returns format pattern. */
	const String&		pattern() const;

	/** Returns number of arguments. */
	int					arguments() const;

	/** Returns ith argument. */
	const Formattable&	getArgument( int i ) const;

	/**
	 * Returns message formatted using English/US locale. 
	 * @exception FormatException
	 */
	String				format() const;

	/** 
	 * Formats the message to the buffer using English/US locale and UTF-8 encoding.
	 * The buffer is always 0-terminated even if the whole message
	 * cannot be stored.
	 * @exception FormatException
	 * @return Number of characters needed to store the whole message.
	 */
	int					format( char* buffer, int size ) const;

private:
	String				m_pattern;
	int					m_args;
	Formattable			m_argv[10];
};


END_NAMESPACE() // lang


#endif // _LANG_FORMAT_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
