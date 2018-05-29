#ifndef _LANG_FORMATTABLE_H
#define _LANG_FORMATTABLE_H


#include <lang/String.h>


BEGIN_NAMESPACE(lang) 


/** 
 * Container for an argument that can be formatted to text. 
 * 
 * @ingroup lang
 */
class Formattable
{
public:
	/** Type of value. */
	enum ValueType 
	{ 
		/** There is no value. */
		VALUE_NONE, 
		/** Value is numeric. */
		VALUE_DOUBLE, 
		/** Value is string. */
		VALUE_STRING 
	};

	/** Stores empty value. */
	Formattable();

	/** Stores numeric value. */
	Formattable( double value );

	/** Stores string value. */
	Formattable( const String& value );

	/** Stores string value. */
	Formattable( const char* value );

	/** Returns type of value. */
	ValueType	type() const;

	/** Returns numeric value. */
	double		doubleValue() const;

	/** Returns string value. */
	String		stringValue() const;

	/**
	 * Formats the value to the buffer using English/US locale and UTF-8 encoding.
	 * @param buffer Receives formatted value.
	 * @param size Maximum number of characters in the formatted value.
	 * @param pattern Pattern for formatting the value.
	 * @param pos Pattern position for the formatting options. (-1 if none)
	 * @return Number of characters needed to format the value.
	 */
	int			format( char* buffer, int size, const String& pattern, int pos ) const;

private:
	double		m_dbl;
	String		m_str;
	ValueType	m_type;
};


END_NAMESPACE() // lang


#endif // _LANG_FORMATTABLE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
