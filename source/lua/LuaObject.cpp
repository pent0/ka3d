#include <lua/LuaObject.h>
#include <lua/LuaState.h>
#include <lua/LuaStackRestore.h>
#include "lua-5.0.2/include/lua.h"
#include <config.h>


USING_NAMESPACE(lang)


BEGIN_NAMESPACE(lua) 


LuaObject::LuaObject( LuaState* luastate ) :
	LuaTable( luastate )
{
	LuaStackRestore lsr( luastate );
	luastate->pushTable( this );

	luastate->pushLightUserData( this );
	luastate->rawSetI( -2, 0 );

	luastate->pushString( "_G" );
	luastate->push( LuaState::ENV_GLOBALS );
	luastate->setTable( -3 );

	luastate->pushString( "this" );
	luastate->pushTable( this );
	luastate->setTable( -3 );
}

LuaObject::~LuaObject()
{
}

void LuaObject::registerClosure( const char* name,
	const LuaFunctor* lf, LuaState::CFunction dispatcher )
{
	assert( lf->obj == this ); // each object must register it's own methods to Lua

	LuaState* luastate = luaState();
	LuaStackRestore lsr( luastate );

	luastate->pushTable( this );
	luastate->pushString( name );

	// store data as light user data values
	void* const* data = reinterpret_cast<void* const*>(lf); 
	const int datasize = int(sizeof(LuaFunctor)/sizeof(void*));
	for ( int i = 0 ; i < datasize ; ++i )
		luastate->pushLightUserData( data[i] );

	luastate->pushCClosure( dispatcher, datasize );
	luastate->setTable( -3 );
}

void LuaObject::call( const char* name )
{
	LuaState* luastate = luaState();
	LuaStackRestore lsr( luastate );
	pushMember( name );
	luastate->call( 0, 0 );
}


END_NAMESPACE() // lua

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
