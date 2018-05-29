#ifndef _LUA_ALL_H
#define _LUA_ALL_H


/**
 * @defgroup lua Scripting support library (Lua C++ wrapper)
 *
 * Purpose of the library is to provide scripting support by providing much higher-level interface to Lua than using plain Lua directly.
 * 
 * Provides support for one-line C++ method registration
 * and one-line Lua function call. For usage example 
 * see source/lua/test.cpp (=unit test of Lua library).
 * For brief introduction to Lua, see docs/lua.pdf document.
 *
 * The library consists of classes LuaState, LuaTable and LuaObject. LuaState represents state of Lua script running environment.  LuaTable is a handle class to Lua's associative array, see Making your game scriptable chapter for more information. LuaObject is base class for C++ objects, which are extended by Lua scripts. Provides functionality for mapping C++ methods to Lua.
 *
 * Simple usage example:
 * 
\verbatim 

In myclass.lua ---------------------------

function f( x )
    print( "x+x=" .. add(x,x) )
end


In MyClass.cpp ---------------------------
  
#include <lua/LuaState.h>
#include <lua/LuaObject.h>

class MyClass : public NS(lua,LuaObject)
{
public:
    float add( float a, float b )
    {
        return a+b;
    }

    MyClass( LuaState* luastate ) :
        LuaObject( luastate )
    {
        // after this call, add(a,b) can be used from Lua script:
        registerMethod( "add", this, &NS(MyClass,add) );
    }
};

int main()
{
    P(LuaState) luastate = new LuaState;
    P(MyClass) myobj = new MyClass( luastate );

    FileInputStream in( "myclass.lua" );
    luastate->compile( &in, in.available(), in.toString(), myobj );

    // executes the function in Lua script:
    myobj->call( "f", 3.f );
}

\endverbatim
 *
 *
 * @{
 */

#include <lua/LuaException.h>
#include <lua/LuaObject.h>
#include <lua/LuaStackRestore.h>
#include <lua/LuaState.h>
#include <lua/LuaTable.h>

/** @} */


#endif // _LUA_ALL_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
