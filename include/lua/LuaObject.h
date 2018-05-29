#ifndef _LUA_LUAOBJECT_H
#define _LUA_LUAOBJECT_H


#include <lua/LuaState.h>
#include <lua/LuaStackRestore.h>


BEGIN_NAMESPACE(io) 
	class InputStream;END_NAMESPACE()


BEGIN_NAMESPACE(lua) 


class LuaState;
class LuaFunctor;


/** 
 * Base class for C++ objects which are extended by Lua scripts.
 * Provides functionality for mapping C++ methods to Lua.
 * 
 * @ingroup lua
 */
class LuaObject :
	public LuaTable
{
public:
	/**
	 * Creates a Lua table for this C++ object.
	 * @param luastate Lua state.
	 */
	explicit LuaObject( LuaState* luastate );

	~LuaObject();

	/** 
	 * Calls method defined in Lua script.
	 * @param name Name of the Lua method to be called.
	 */
	void call( const char* name );

	/** 
	 * Calls method defined in Lua script with 1 argument.
	 * @param name Name of the Lua method to be called.
	 */
	template <class A1> void call( const char* name, A1 a1 );
	
	/** 
	 * Calls method defined in Lua script with 2 arguments.
	 * @param name Name of the Lua method to be called.
	 */
	template <class A1, class A2> void call( const char* name, A1 a1, A2 a2 );

	/** 
	 * Calls method defined in Lua script with 3 arguments.
	 * @param name Name of the Lua method to be called.
	 */
	template <class A1, class A2, class A3> void call( const char* name, A1 a1, A2 a2, A3 a3 );
		
	/** 
	 * Calls method defined in Lua script with 4 arguments.
	 * @param name Name of the Lua method to be called.
	 */
	template <class A1, class A2, class A3, class A4> void call( const char* name, A1 a1, A2 a2, A3 a3, A4 a4 );
		
	/** 
	 * Calls method defined in Lua script with 5 arguments.
	 * @param name Name of the Lua method to be called.
	 */
	template <class A1, class A2, class A3, class A4, class A5> void call( const char* name, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 );
	
	/** 
	 * Calls method defined in Lua script with 6 arguments.
	 * @param name Name of the Lua method to be called.
	 */
	template <class A1, class A2, class A3, class A4, class A5, class A6> void call( const char* name, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6 );
		
	/** 
	 * Calls method defined in Lua script with 7 arguments.
	 * @param name Name of the Lua method to be called.
	 */
	template <class A1, class A2, class A3, class A4, class A5, class A6, class A7> void call( const char* name, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7 );
		
	/** 
	 * Calls method defined in Lua script with 8 arguments.
	 * @param name Name of the Lua method to be called.
	 */
	template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> void call( const char* name, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8 );
		
	/** 
	 * Calls method defined in Lua script with 9 arguments.
	 * @param name Name of the Lua method to be called.
	 */
	template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9> void call( const char* name, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9 );

	/** 
	 * Registers normal C++ method which can be used from Lua scripts. 
	 * @param T Class which has methods of type F.
	 * @param F Method of class T.
	 * @param name Name of the method to be added in the Lua table.
	 * @param obj Object to be used as 'this' when calling the method.
	 * @param func Method to be called.
	 */
	template <typename T, typename F> void
		registerMethod( const char* name, T* obj, F func );

	/** 
	 * Registers C++ method which uses LuaState directly.
	 * Method has fixed function calling convention, returns int
	 * and accepts one argument, LuaState*. Arguments passed from
	 * the script to the function are passed on top of Lua stack,
	 * and return value(s) are returned on top of Lua stack.
	 * Function should return number of return values in Lua stack.
	 * @param T Class which has methods of type F.
	 * @param name Name of the method to be added in the Lua table.
	 * @param obj Object to be used as 'this' when calling the method.
	 * @param func Method to be called.
	 */
	template <class T> void	
		registerLuaMethod( const char* name, T* obj, int (T::*func)(LuaState*) );

private:
	void	registerClosure( const char* name, const LuaFunctor* lf, NS(LuaState,CFunction) dispatcher );

	LuaObject( const LuaObject& );
	LuaObject& operator=( const LuaObject& );
};


#include "LuaObject.inl"


END_NAMESPACE() // lua


#endif // _LUA_LUAOBJECT_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
