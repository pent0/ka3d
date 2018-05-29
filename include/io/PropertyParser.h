#ifndef _IO_PROPERTYPARSER_H
#define _IO_PROPERTYPARSER_H


#include <lang/Array.h>
#include <lang/String.h>
#include <lang/Object.h>


BEGIN_NAMESPACE(io) 


/**
 * Parses key=value style property pairs from character string.
 * Pairs are separated by newline.
 * Keys are parsed to lower-case to provide case-insensitive comparison.
 * Trailing and preceding whitespace of values is ignored.
 * Comments can be embedded with line starting with '--'.
 * 
 * For any complex configuration files Lua scripts should be used,
 * but this class is suitable for conditions where for example
 * heap memory allocations need to be avoided.
 *
 * @ingroup io
 */
class PropertyParser :
	public NS(lang,Object)
{
public:
	/** 
	 * Iterates constant (key,value) property pairs. 
	 */
	class ConstIterator
	{
	public:
		ConstIterator( const PropertyParser* parser, int pos, int line );

		/**
		 * Iterates to next (key,value) pair.
		 */
		ConstIterator&	operator++();

		/**
		 * Returns true if the iterators point to the same (key,value) pair.
		 */
		bool			operator==( const ConstIterator& other ) const		{return !this->operator!=(other);}

		/**
		 * Returns true if the iterators do not point to the same (key,value) pair.
		 */
		bool			operator!=( const ConstIterator& other ) const;

		/**
		 * Returns key of current pair.
		 * Valid until next call to parser (direct or indirect).
		 */
		const char*		key() const;

		/**
		 * Returns value of current pair.
		 * Valid until next call to parser (direct or indirect).
		 */
		const char*		value() const;

	private:
		const PropertyParser*	m_parser;
		int						m_pos;
		int						m_line;
	};

	/**
	 * Creates uninitialized property parser. Use
	 * reset() to start parsing.
	 */
	PropertyParser();

	/**
	 * Starts parsing (key=value) pairs from character string.
	 * String can be identified by separate name.
	 */
	explicit PropertyParser( const NS(lang,String)& buf, const NS(lang,String)& name );

	/**
	 * Re-starts parsing from specified string.
	 * String can be identified by separate name.
	 */
	void		reset( const NS(lang,String)& buf, const NS(lang,String)& name );

	/**
	 * Parses key=x user property and returns true if key exist.
	 * @param key Name of the property to find.
	 * @return true if property found, false otherwise.
	 * @exception IOException
	 */
	bool		hasKey( const char* key ) const;

	/**
	 * Parses key=x user property and returns x as 0-terminated character string.
	 * @param key Name of the property to find.
	 * @param x [out] Receives parsed property string.
	 * @return false if property not found, true otherwise.
	 * @exception IOException
	 */
	bool		get( const char* key, NS(lang,Array)<char>& x ) const;

	/**
	 * Parses key=x user property and returns x as 0-terminated string.
	 * String is valid until next call to PropertyParser.
	 * @param key Name of the property to find.
	 * @return 0-terminated key value.
	 * @exception IOException
	 */
	const char*	getString( const char* key ) const;

	/**
	 * Parses key=x user property and returns x as boolean.
	 * Following values are interpreted as 'false':
	 * <ul>
	 * <li>false
	 * <li>disabled
	 * <li>0
	 * </ul>
	 * Following values are interpreted as 'true':
	 * <ul>
	 * <li>true
	 * <li>enabled
	 * <li>1
	 * </ul>
	 * @param key Name of the property to find.
	 * @return Boolean value.
	 * @exception IOException
	 */
	bool		getBoolean( const char* key ) const;

	/**
	 * Parses key=x user property and returns x as integer.
	 * @param key Name of the property to find.
	 * @return Key's value as integer.
	 * @exception IOException
	 */
	int			getInt( const char* key ) const;

	/**
	 * Parses key=x user property and returns x as floating point value.
	 * @param key Name of the property to find.
	 * @return Key's value as floating point value.
	 * @exception IOException
	 */
	float		getFloat( const char* key ) const;

	/**
	 * Returns iterator to the first (key,value) property pair.
	 */
	ConstIterator	begin() const;

	/**
	 * Returns iterator to one beyond the last (key,value) property pair.
	 */
	ConstIterator	end() const;

	/**
	 * Returns name of the property parser (for debugging/error messages).
	 */
	const NS(lang,String)&	name() const	{return m_name;}

private:
	friend class ConstIterator;

	class Pair
	{
	public:
		NS(lang,String)	key;
		NS(lang,String)	value;

		bool operator<( const Pair& other ) const	{return key < other.key;}
	};

	NS(lang,String)					m_buf;
	int								m_len;
	NS(lang,String)					m_name;
	mutable NS(lang,Array)<char>	m_temp;
	NS(lang,Array)<Pair>			m_pairs;

	void		parsePair( int& pos, int& line, NS(lang,Array)<char>& pair ) const;
	void		parseKey( int& pos, int& line, NS(lang,Array)<char>& value ) const;
	void		parseValue( int& pos, int& line, NS(lang,Array)<char>& value ) const;
	bool		skipLine( int& pos, int& line ) const;
	bool		skipSpace( int& pos, int& line ) const;
	bool		skipComments( int& pos, int& line ) const;
	char		get( int pos ) const;
	bool		isComment( int pos ) const;

	static void	toLowerCase( NS(lang,Array)<char>& temp );
};


END_NAMESPACE() // io


#endif // _IO_PROPERTYPARSER_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
