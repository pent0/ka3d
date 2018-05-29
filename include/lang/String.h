#ifndef _LANG_STRING_H
#define _LANG_STRING_H


#include <lang/pp.h>
#include <lang/assert.h>


BEGIN_NAMESPACE(lang) 

	
class Converter;
	

/** 
 * Immutable Unicode character string. Used encoding is UTF-8.
 * String uses internal memory pool, which allows
 * low-overhead allocation and compact operation to avoid
 * run-time memory fragmentation. Not thread-safe.
 * 
 * @ingroup lang
 */
class String
{
public:
	/** Creates an zero length string. */
	String();

	/** 
	 * Creates a string from the 0-terminated UTF-8 code unit sequence.
	 * @exception OutOfMemoryException
	 */
	String( const char* str );

	/** 
	 * Creates a string from the encoded character sequence.
	 * Ignores invalid character sequences.
	 * @exception OutOfMemoryException
	 */
	String( const void* data, int size, const Converter& decoder );

	/** 
	 * Copy by reference. 
	 */
	String( const String& other );

	///
	~String();

	/** 
	 * Copy by reference. 
 	 */
	String&		operator=( const String& other );

	/** 
	 * Returns number of UTF-8 code units in the string. 
	 */
	int			length() const;

	/**
	 * Returns UTF-8 code unit at specified index. 
	 */
	char		charAt( int index ) const;
	
	/**
	 * Copies characters from this string into the destination character array.
	 * NOTE: Does not terminate output with zero, see get() for getting full 
	 * string in zero-terminated form.
	 *
	 * @param begin Index to the beginning (inclusive) of the substring.
	 * @param end Index to the end (exclusive) of the substring.
	 */
	void		getChars( int begin, int end, char* dest ) const;

	/**
	 * Copies characters from this string into the destination character array
	 * and terminates the output with zero.
	 * Asserts if buffer not large enough, but terminates it still with zero.
	 * @param buf [out] Receives the string.
	 * @param bufsize Size of the buffer.
	 */
	void		get( char* buf, int bufsize ) const;

	/**
	 * Copies characters from this string into the destination buffer
	 * using UTF-16 encoding. Terminates always the buffer with zero.
	 * @param buf [out] Receives the string.
	 * @param bufsize Size of the buffer in UTF-16 units.
	 * @return Length of the string in UTF-16 units.
	 */
	int			getUTF16( short* buf, int buflen ) const;

	/**
	 * Copies characters from this string into the destination buffer
	 * using specified encoding. DOES NOT terminate the buffer with zero.
	 * @param buf [out] Receives the string.
	 * @param bufsize Size of the buffer in bytes.
	 * @return Number of bytes encoded.
	 */
	int			getBytes( void* buf, int bufsize, Converter& encoder ) const;

	/**
	 * Returns 0-terminated UTF-8 data.
	 *
	 * WARNING: This is convenience method, which uses static buffer
	 * to store string characters which it returns. So calling the function
	 * too many times in the same statement or with too long string
	 * might result in loss of information in the function return value.
	 * Always use get or cpy in 'mission critical' situations and this function
	 * only for example in debug output.
	 */
	const char*	c_str() const;

	/**
	 * Returns true if the string ends with specified substring.
	 */
	bool		endsWith( const String& suffix ) const;

	/**
	 * Returns true if the string starts with specified substring.
	 */
	bool		startsWith( const String& prefix ) const;

	/**
	 * Returns hash code for this string.
	 */
	int			hashCode() const;

	/**
	 * Returns the first index within this string of the specified character.
	 *
	 * @param ch Character to find.
	 * @return Index of the found position or -1 if the character was not found from the string.
	 */
	int			indexOf( char ch ) const											{return indexOf(ch,0);}

	/**
	 * Returns the first index within this string of the specified character.
	 * Starts the search from the specified position.
	 *
	 * @param ch Character to find.
	 * @param index The first position to search.
	 * @return Index of the found position or -1 if the character was not found from the string.
	 */
	int			indexOf( char ch, int index ) const;

	/**
	 * Returns the first index within this string of the specified substring.
	 *
	 * @param str Substring to find.
	 * @return Index of the found position or -1 if the substring was not found from the string.
	 */
	int			indexOf( const String& str ) const									{return indexOf(str,0);}

	/**
	 * Returns the first index within this string of the specified substring.
	 * Starts the search from the specified position.
	 *
	 * @param str Substring to find.
	 * @param index The first position to search.
	 * @return Index of the found position or -1 if the substring was not found from the string.
	 */
	int			indexOf( const String& str, int index ) const;

	/**
	 * Returns the last index within this string of the specified character.
	 *
	 * @param ch Character to find.
	 * @return Index of the found position or -1 if the character was not found from the string.
	 */
	int			lastIndexOf( char ch ) const;

	/**
	 * Returns the last index within this string of the specified character.
	 * Starts the search from the specified position.
	 *
	 * @param ch Character to find.
	 * @param index The last position to search.
	 * @return Index of the found position or -1 if the character was not found from the string.
	 */
	int			lastIndexOf( char ch, int index ) const;

	/**
	 * Returns the last index within this string of the specified substring.
	 *
	 * @param str Substring to find.
	 * @return Index of the found position or -1 if the substring was not found from the string.
	 */
	int			lastIndexOf( const String& str ) const								{return lastIndexOf(str,length()-1);}

	/**
	 * Returns the last index within this string of the specified substring.
	 * Starts the search from the specified position.
	 *
	 * @param str Substring to find.
	 * @param index The last position to search.
	 * @return Index of the found position or -1 if the substring was not found from the string.
	 */
	int			lastIndexOf( const String& str, int index ) const;

	/**
	 * Tests if two string regions are equal.
	 *
	 * @param thisoffset Offset to this string.
	 * @param other The other string to compare.
	 * @param otheroffset Offset to other string.
	 * @param length Length of the sequences to compare.
	 */
	bool		regionMatches( int thisoffset, const String& other, int otheroffset, int length ) const;

	/**
	 * Returns a new string which has oldchar characters replaced with newchar.
	 * @exception OutOfMemoryException
	 */
	String		replace( char oldchar, char newchar ) const;

	/**
	 * Returns a new string that is a substring of this string.
	 *
	 * @param begin Index to the beginning (inclusive) of the substring.
	 * @param end Index to the end (exclusive) of the substring.
	 * @exception OutOfMemoryException
	 */
	String		substring( int begin, int end ) const;

	/**
	 * Returns a new string that is a substring of this string.
	 *
	 * @param begin Index to the beginning (inclusive) of the substring.
	 * @exception OutOfMemoryException
	 */
	String		substring( int begin ) const;

	/**
	 * Returns a new string that has all characters of this string converted to lowercase.
	 * Doesn't handle locale dependent special casing.
	 * @exception OutOfMemoryException
	 */
	String		toLowerCase() const;

	/**
	 * Returns a new string that has all characters of this string converted to uppercase.
	 * Doesn't handle locale dependent special casing.
	 * @exception OutOfMemoryException
	 */
	String		toUpperCase() const;

	/**
	 * Returns a new string that is otherwise identical to this string 
	 * but has whitespace removed from both ends of the string.
	 * @exception OutOfMemoryException
	 */
	String		trim() const;

	/**
	 * Bitwise lecigographical compare between this string and other string. 
	 * @return If this string is lexicographically before other then the return value is <0 and if this string is after other string then >0. If the strings are equal then the return value is 0.
	 */
	int			compareTo( const String& other ) const;

	/**
	 * Bitwise lecigographical compare between this string and other string. 
	 * @return If this string is lexicographically before other then the return value is <0 and if this string is after other string then >0. If the strings are equal then the return value is 0.
	 */
	int			compareTo( const char* other ) const;

	/** Concatenate 0-terminated UTF-8 char sequence and String. */
	String		
		operator+( const char* other ) const;

	/** Concatenate two Strings. */
	String
		operator+( const String& other ) const;

	/** Compare if two strings are equal. */
	bool
		operator==( const char* other ) const						{return 0==compareTo(other);}

	/** Compare if two strings are equal. */
	bool
		operator==( const String& other ) const						{return 0==compareTo(other);}

	/** Compare if two strings are inequal. */
	bool
		operator!=( const char* other ) const						{return 0!=compareTo(other);}

	/** Compare if two strings are inequal. */
	bool
		operator!=( const String& other ) const						{return 0!=compareTo(other);}

	/** Lexicographical compare. */
	bool
		operator<( const char* other ) const						{return compareTo(other)<0;}

	/** Lexicographical compare. */
	bool
		operator<( const String& other ) const						{return compareTo(other)<0;}

	/** 
	 * Safe zero-terminated character string copy to limited buffer. 
	 * @return true if string fit to the buffer.
	 */
	static bool cpy( char* buf, int bufsize, const char* sz );

	/** 
	 * Safe string copy to limited buffer. 
	 * @return true if string fit to the buffer.
	 */
	static bool cpy( char* buf, int bufsize, const String& str );

	/** 
	 * Safe zero-terminated character string concatenation to limited buffer. 
	 * @return true if string fit to the buffer.
	 */
	static bool cat( char* buf, int bufsize, const char* sz );

	/** 
	 * Safe string concatenation to limited buffer. 
	 * @return true if string fit to the buffer.
	 */
	static bool cat( char* buf, int bufsize, const String& str );

private:
	int m_h;
};


END_NAMESPACE() // lang


/** 
 * Concatenate 0-terminated UTF-8 char sequence and String. 
 * @exception OutOfMemoryException
 */
inline NS(lang,String) 
	operator+( const char* first, const NS(lang,String)& second )		{return NS(lang,String)(first)+second;}


#endif // _LANG_STRING_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
