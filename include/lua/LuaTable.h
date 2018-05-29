#ifndef _LUA_LUATABLE_H
#define _LUA_LUATABLE_H


#include <lang/Object.h>
#include <lang/String.h>


struct lua_State;

BEGIN_NAMESPACE(io) 
	class InputStream;
	class OutputStream;END_NAMESPACE()


BEGIN_NAMESPACE(lua) 


class LuaState;


/** 
 * Handle to Lua table.
 * 
 * @ingroup lua
 */
class LuaTable :
	public NS(lang,Object)
{
public:
	/** Skips table initialization. */
	LuaTable();

	/** Creates a table to LuaState. */
	explicit LuaTable( LuaState* state );

	/** Copy by reference. */
	LuaTable( const LuaTable& other );
	
	/** Releases a reference. */
	~LuaTable();

	/** Copy by reference. */
	LuaTable& operator=( const LuaTable& other );

	/**
	 * Writes Lua table to output stream. Note that
	 * functions are not written to the output.
	 * @exception IOException
	 * @exception LuaException
	 */
	void write( NS(io,OutputStream)* out, int margin=0 );

	/**
	 * Reads Lua table from input stream.
	 * @exception IOException
	 * @exception LuaException
	 */
	void read( NS(io,InputStream)* in );

	/** Removes the member from the table. */
	void remove( int index );

	/** Removes the member from the table. */
	void remove( const NS(lang,String)& name );

	/** Sets boolean member value. */
	void setBoolean( int index, bool v );

	/** Sets boolean member value. */
	void setBoolean( const NS(lang,String)& name, bool v );

	/** Sets number member value. */
	void setNumber( int index, float v );

	/** Sets number member value. */
	void setNumber( const NS(lang,String)& name, float v );

	/** Sets string member value. */
	void setString( int index, const NS(lang,String)& v );
	
	/** Sets string member value. */
	void setString( const NS(lang,String)& name, const NS(lang,String)& v );

	/** Sets table member value. */
	void setTable( int index, const LuaTable& v );

	/** Sets table member value. */
	void setTable( int index, const LuaTable* v );

	/** Sets table member value. */
	void setTable( const NS(lang,String)& name, const LuaTable& v );

	/** Sets table member value. */
	void setTable( const NS(lang,String)& name, const LuaTable* v );

	/** Sets member value. */
	void setMember( int index, bool v )									{setBoolean(index,v);}

	/** Sets member value. */
	void setMember( const NS(lang,String)& name, bool v )					{setBoolean(name,v);}

	/** Sets member value. */
	void setMember( int index, float v )								{setNumber(index,v);}

	/** Sets member value. */
	void setMember( const NS(lang,String)& name, float v )					{setNumber(name,v);}

	/** Sets member value. */
	void setMember( int index, const NS(lang,String)& v )					{setString(index,v);}
	
	/** Sets member value. */
	void setMember( const NS(lang,String)& name, const NS(lang,String)& v )	{setString(name,v);}

	/** Sets member value. */
	void setMember( int index, const LuaTable& v )						{setTable(index,v);}

	/** Sets member value. */
	void setMember( int index, const LuaTable* v )						{setTable(index,v);}

	/** Sets member value. */
	void setMember( const NS(lang,String)& name, const LuaTable& v )		{setTable(name,v);}

	/** Sets member value. */
	void setMember( const NS(lang,String)& name, const LuaTable* v )		{setTable(name,v);}

	/** 
	 * Returns boolean member of the table. 
	 * @exception LuaException
	 */
	bool getBoolean( const NS(lang,String)& name ) const;

	/** 
	 * Returns boolean member of the table. 
	 * @exception LuaException
	 */
	bool getBoolean( int index ) const;

	/** 
	 * Returns number member of the table. 
	 * @exception LuaException
	 */
	float getNumber( const NS(lang,String)& name ) const;
	
	/** 
	 * Returns number member of the table. 
	 * @exception LuaException
	 */
	float getNumber( int index ) const;

	/** 
	 * Returns string member of the table. 
	 * @exception LuaException
	 */
	NS(lang,String) getString( const NS(lang,String)& name ) const;

	/** 
	 * Returns string member of the table. 
	 * @exception LuaException
	 */
	NS(lang,String) getString( int index ) const;

	/** 
	 * Returns handle to table member of the table. 
	 * @exception LuaException
	 */
	LuaTable getTable( const NS(lang,String)& name ) const;

	/** 
	 * Returns handle to table member of the table. 
	 * @exception LuaException
	 */
	LuaTable getTable( int index ) const;

	/** Returns true if the member is boolean. */
	bool isBoolean( const NS(lang,String)& name ) const;

	/** Returns true if the member is boolean. */
	bool isBoolean( int index ) const;

	/** Returns true if the member is nil. */
	bool isNil( const NS(lang,String)& name ) const;

	/** Returns true if the member is nil. */
	bool isNil( int index ) const;

	/** Returns true if the member is number. */
	bool isNumber( const NS(lang,String)& name ) const;

	/** Returns true if the member is number. */
	bool isNumber( int index ) const;

	/** Returns true if the member is string. */
	bool isString( const NS(lang,String)& name ) const;

	/** Returns true if the member is string. */
	bool isString( int index ) const;

	/** Returns true if the member is table. */
	bool isTable( const NS(lang,String)& name ) const;

	/** Returns true if the member is table. */
	bool isTable( int index ) const;

	/** Pushes member by name to LuaState stack. */
	void pushMember( int index ) const;

	/** Pushes member by index to LuaState stack. */
	void pushMember( const NS(lang,String)& name ) const;

	/** Returns number of items in the table. */
	int size() const;

	/** Returns pointer to low level Lua state object. */
	LuaState*	luaState() const			{return m_luastate;}

private:
	friend class LuaState;

	LuaState*	m_luastate;
	int			m_ref;

	void	writeChar( NS(io,OutputStream)* out, int ch );
	void	writeMargin( NS(io,OutputStream)* out, int margin );
	void	writeString( NS(io,OutputStream)* out, const NS(lang,String)& str );
	void	writeValue( NS(io,OutputStream)* out, int index, int margin );
};


END_NAMESPACE() // lua


#endif // _LUA_LUATABLE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
