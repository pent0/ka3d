#ifndef _LUA_LUASTATE_H
#define _LUA_LUASTATE_H


#include <lua/LuaTable.h>
#include <lang/Object.h>
#include <lang/String.h>
#include <stddef.h>


struct lua_State;

BEGIN_NAMESPACE(io) 
	class InputStream;END_NAMESPACE()

BEGIN_NAMESPACE(lang) 
	class Throwable;END_NAMESPACE()


BEGIN_NAMESPACE(lua) 


class LuaTable;
class LuaFunctor;


/**
 * State of Lua script running environment.
 * 
 * @ingroup lua
 */
class LuaState :
	public NS(lang,Object)
{
public:
	typedef int (*CFunction)( lua_State* );

	/** Type of Lua value. */
	enum Type
	{
		/** Nil. */
		TYPE_NIL,
		/** Boolean. */
		TYPE_BOOLEAN,
		/** Light user data. */
		TYPE_LIGHTUSERDATA,
		/** Number. */
		TYPE_NUMBER,
		/** String (UTF-8). */
		TYPE_STRING,
		/** Table. */
		TYPE_TABLE,
		/** Function. */
		TYPE_FUNCTION,
		/** User data. */
		TYPE_USERDATA,
		/** Thread. */
		TYPE_THREAD,
	};

	/** Pseudo table indices which can be used to refer environments. */
	enum Environment
	{
		/** Index of globals table. */
		ENV_GLOBALS = -10001,
	};

	/** 
	 * Creates Lua state by calling lua_open. 
	 * @exception LuaException
	 */
	LuaState();

	/** Destroys Lua state by calling lua_close. */
	~LuaState();

	/**
	 * Returns table of Lua state globals.
	 */
	LuaTable& globals();

	/** 
	 * Sets top of stack. Accepts any acceptable index,
	 * or 0, and sets the stack top to that index. If the 
	 * new top is larger than the old one, then the new 
	 * elements are filled with nil. If index is 0, then
	 * all stack elements are removed. 
	 */
	void setTop( int index );

	/**
	 * Pops n values from the stack.
	 * Usually do not use this method (unless while iterating a table),
	 * but LuaStackRestore instead.
	 */
	void pop( int n=1 );

	/**
	 * Pushes onto the stack a copy of the element at the given index.
	 */
	void push( int index );

	/**
	 * Removes the element at the given position, shifting down the 
	 * elements above that position to fill the gap
	 */
	void remove( int index );

	/**
	 * Moves the top element into the given position, shifting up 
	 * the elements above that position to open space.
	 */
	void insert( int index );

	/**
	 * Moves the top element into the given position, without 
	 * shifting any element therefore replacing the value 
	 * at the given position.
	 */
	void replace( int index );

	/** Returns type of value at specified stack index. */
	Type type( int index );

	/** Returns true if value at specified stack index is nil. */
	bool isNil( int index );

	/** Returns true if value at specified stack index is boolean. */
	bool isBoolean( int index );

	/** Returns true if value at specified stack index is number. */
	bool isNumber( int index );

	/** Returns true if value at specified stack index is string. */
	bool isString( int index );

	/** Returns true if value at specified stack index is table. */
	bool isTable( int index );

	/** Returns true if value at specified stack index is function. */
	bool isFunction( int index );

	/** Returns true if value at specified stack index is C-function. */
	bool isCFunction( int index );

	/** Returns true if value at specified stack index is user data. */
	bool isUserData( int index );

	/** Returns true if value at specified stack index is light user data. */
	bool isLightUserData( int index );

	/** Compares values at two stack positions. */
	bool equal( int index1, int index2 );

	/** Compares values at two stack positions. */
	bool lessThan( int index1, int index2 );

	/** Compares values at two stack positions without using metamethods. */
	bool rawEqual( int index1, int index2 );

	/**
	 * Returns boolean at specified stack position.
	 * @exception LuaException If types do not match.
	 */
	bool toBoolean( int index );

	/**
	 * Returns number at specified stack position.
	 * @exception LuaException If types do not match.
	 */
	float toNumber( int index );

	/**
	 * Returns zero-terminated string at specified stack position.
	 * @exception LuaException If types do not match.
	 */
	const char* toString( int index );

	/**
	 * Returns C-function at specified stack position.
	 * @exception LuaException If types do not match.
	 */
	CFunction toCFunction( int index );

	/**
	 * Returns light user data at specified stack position.
	 * @exception LuaException If types do not match.
	 */
	void* toLightUserData( int index );

	/**
	 * Returns user data at specified stack position.
	 * @exception LuaException If types do not match.
	 */
	void* toUserData( int index );

	/**
	 * Returns handle to table at specified stack position.
	 * @exception LuaException If types do not match.
	 */
	LuaTable toTable( int index );

	/**
	 * Returns value at specified stack position.
	 * @exception LuaException If types do not match.
	 */
	void toValue( int index, bool& v )				{v=toBoolean(index);}

	/**
	 * Returns value at specified stack position.
	 * @exception LuaException If types do not match.
	 */
	void toValue( int index, float& v )				{v=toNumber(index);}

	/**
	 * Returns value at specified stack position.
	 * @exception LuaException If types do not match.
	 */
	void toValue( int index, NS(lang,String)& v )		{v=toString(index);}

	/** Pushes boolean onto the Lua stack. */
	void pushBoolean( bool v );
	
	/** Pushes number onto the Lua stack. */
	void pushNumber( float v );
	
	/** Pushes zero-terminated string onto the Lua stack. */
	void pushString( const char* v );

	/** Pushes string onto the Lua stack. */
	void pushString( const NS(lang,String)& v );
	
	/** Pushes nil onto the Lua stack. */
	void pushNil();
	
	/** Pushes C-function onto the Lua stack. */
	void pushCFunction( CFunction v );

	/** Pushes light user data onto the Lua stack. */
	void pushLightUserData( void* v );

	/** Pushes value onto the Lua stack. */
	void pushValue( bool v )					{pushBoolean(v);}

	/** Pushes value onto the Lua stack. */
	void pushValue( float v )					{pushNumber(v);}

	/** Pushes value onto the Lua stack. */
	void pushValue( const NS(lang,String)& v )		{pushString(v);}

	/** Pushes value onto the Lua stack. */
	void pushValue( const LuaTable* v )			{pushTable(v);}

	/** Pushes value onto the Lua stack. */
	void pushValue( const LuaTable& v )			{pushTable(v);}

	/** Pushes C function closure onto the Lua stack. */
	void pushCClosure( CFunction v, int n );

	/** 
	 * Pushes method closure onto the Lua stack. 
	 * Method has fixed function calling convention, returns int
	 * and accepts one argument, LuaState*. Arguments passed from
	 * the script to the function are passed on top of Lua stack,
	 * and return value(s) are returned on top of Lua stack.
	 * Function should return number of return values in Lua stack.
	 */
	template <class T> void pushMethodClosure( T* obj, int (T::*func)(LuaState*) );

	/** Returns index of n-th up-value, starting from 1. */
	int upValueIndex( int n );

	/** Pushes table onto the Lua stack. */
	void pushTable( const LuaTable* v )			{pushTable(m_impl,v);}

	/** Pushes table onto the Lua stack. */
	void pushTable( const LuaTable& v )			{pushTable(m_impl,&v);}

	/** 
	 * Creates a new, empty table and pushes it onto the stack. 
	 * @exception LuaException
	 */
	void newTable();

	/**
	 * Pops a key from the stack and returns (on the stack) 
	 * the contents of the table at that key.
	 * The table is left where it was in the stack.
	 * @exception LuaException
	 */
	void getTable( int index );

	/**
	 * Pops a key from the stack and returns (on the stack) 
	 * the contents of the table at that key.
	 * The table is left where it was in the stack.
	 * Ignores metamethods.
	 * @exception LuaException
	 */
	void rawGet( int index );

	/** 
	 * Pops from the stack both the key and the value.
	 * Key must be pushed first, and then value.
	 * The table is left where it was in the stack.
	 * @exception LuaException
	 */
	void setTable( int index );

	/** 
	 * Pops from the stack both the key and the value.
	 * Key must be pushed first, and then value.
	 * The table is left where it was in the stack.
	 * Ignores metamethods.
	 * @exception LuaException
	 */
	void rawSet( int index );

	/**
	 * The function pops a key from the stack, and pushes a key-value pair 
	 * from the table (the "next" pair after the given key). If there are 
	 * no more elements, then next returns false and pushes nothing. 
	 * Use a nil key to signal the start of a traversal.
	 * @exception LuaException
	 */
	bool next( int index );

	/**
	 * Pushes the value of the n-th element of the table at stack position index.
	 * @exception LuaException
	 */
	void rawGetI( int index, int n );

	/**
	 * Sets the value of the n-th element of the table at stack position index 
	 * to the value at the top of the stack, removing this value from the stack.
	 * @exception LuaException
	 */
	void rawSetI( int index, int n );

	/**
	 * Pops a table from the stack and sets it as the new environment 
	 * for the function at index index in the stack.
	 * @exception LuaException
	 */
	void setFEnv( int index );

	/**
	 * Pushes on the stack the environment table of the function 
	 * at index index in the stack.
	 * @exception LuaException
	 */
	void getFEnv( int index );

	/**
	 * Calls Lua function. Functions defined in Lua and C functions registered in 
	 * Lua can be called from the host program. This is done using the following 
	 * protocol: First, the function to be called is pushed onto the stack; 
	 * then, the arguments to the function are pushed in direct order, that is, 
	 * the first argument is pushed first.
	 * @param nargs Number of arguments that you pushed onto the stack.
	 * @param nresults Number of results returned are adjusted to this number.
	 * @exception LuaException
	 */
	void call( int nargs, int nresults );

	/**
	 * Compiles Lua chunk from memory buffer.
	 * @param buffer Lua chunk (ASCII/binary).
	 * @param size Size of Lua chunk in bytes.
	 * @param name Name of Lua chunk.
	 * @param env Function environment if any.
	 * @exception LuaException
	 */
	void compile( const char* buffer, int size, const NS(lang,String)& name, LuaTable* env );

	/**
	 * Compiles Lua chunk from file.
	 * @param in Lua chunk (ASCII/binary) input stream.
	 * @param size Size of Lua chunk in bytes.
	 * @param name Name of Lua chunk.
	 * @param env Function environment if any.
	 * @exception LuaException
	 */
	void compile( NS(io,InputStream)* in, int size, const NS(lang,String)& name, LuaTable* env );

	/** 
	 * Checks type at specified stack index.
	 * @exception LuaException If types don't match.
	 */
	void checkType( int index, Type type );

	/** 
	 * Called after dynamic cast fails to specified type when dispatching calls.
	 * Used by implementation.
	 * @param index Lua stack index where the incorrect type was found.
	 * @exception LuaException Always.
	 */
	void incorrectType( int index );

	/** Returns low level Lua state object. */
	lua_State* impl()		{return m_impl;}

	/**
	 * Returns top of the stack.
	 */
	int top() const;

	/** Returns type name. */
	static const char* toString( Type type );

	/* Concatenates stack trace to limited buffer. Used by implementation. */
	static void appendStackTrace( char* buf, int bufsize, lua_State* impl );

	/* Pushes string to Lua state stack. Used by implementation. */
	static void pushString( lua_State* impl, const NS(lang,String)& v );

	/* Pushes table to Lua state stack. Used by implementation. */
	static void pushTable( lua_State* impl, const LuaTable* v );

	/* 
	 * Returns C++ this pointer from table in stack at specified position.
	 * C++ this pointer is set to index [0] of the table.
	 * If [0] is not this pointer, then the table has no C++ counterpart.
	 * Used by implementation.
	 * @exception LuaException
	 */
	static LuaTable* getThisPtr( lua_State* impl, int index );

	/*
	 * Used by the implementation of Lua call dispatch system.
	 */
	static void getDispatchData( lua_State* impl, LuaState** luastate, LuaFunctor* lf );

private:
	struct LoadBuffer
	{
		const char* buffer;
		int size;
	};

	struct LoadFile
	{
		NS(io,InputStream)* in;
		int size;
		char buf[2048];
	};

	lua_State*			m_impl;
	LuaTable			m_globals;

	void				handleLoadResult( int err, const NS(lang,String)& name, LuaTable* env );
	static const char*	loadBuffer( lua_State* impl, void* data, size_t* size );
	static const char*	loadFile( lua_State* impl, void* data, size_t* size );
	static int			handleError( lua_State* impl );

	LuaState( const LuaState& );
	LuaState& operator=( const LuaState& );
};


#include <lua/LuaState.inl>


END_NAMESPACE() // lua


#endif // _LUA_LUASTATE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
