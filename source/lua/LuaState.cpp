#include <lua/LuaState.h>
#include <lua/LuaTable.h>
#include <lua/LuaException.h>
#include <lua/LuaStackRestore.h>
#include <io/InputStream.h>
#include <lang/Array.h>
#include "lua-5.0.2/include/lua.h"
#include "lua-5.0.2/include/lauxlib.h"
#include "lua-5.0.2/include/lualib.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <config.h>


USING_NAMESPACE(io)
USING_NAMESPACE(lang)


BEGIN_NAMESPACE(lua) 


LuaState::LuaState()	
{
	assert( LUA_TNIL == TYPE_NIL );
	assert( LUA_TBOOLEAN == TYPE_BOOLEAN );
	assert( LUA_TLIGHTUSERDATA == TYPE_LIGHTUSERDATA );
	assert( LUA_TNUMBER == TYPE_NUMBER );
	assert( LUA_TSTRING == TYPE_STRING );
	assert( LUA_TTABLE == TYPE_TABLE );
	assert( LUA_TFUNCTION == TYPE_FUNCTION );
	assert( LUA_TUSERDATA == TYPE_USERDATA );
	assert( LUA_TTHREAD == TYPE_THREAD );

	m_impl = lua_open();
	if ( !m_impl )
		throwError( LuaException( Format("Failed to initialized Lua interpreter") ) );

	lua_atpanic( m_impl, handleError );
	luaopen_base( m_impl );
	luaopen_math( m_impl );
	luaopen_string( m_impl );
	luaopen_table( m_impl );

	// set error() handler
	lua_pushstring( m_impl, "error" );
	lua_pushlightuserdata( m_impl, this );
	lua_pushcclosure( m_impl, handleError, 1 );
	lua_settable( m_impl, LUA_GLOBALSINDEX );

	// set this ptr
	lua_pushvalue( m_impl, LUA_GLOBALSINDEX );
	lua_pushlightuserdata( m_impl, this );
	lua_rawseti( m_impl, -2, 0 );
	lua_pop( m_impl, 1 );

	// get globals
	m_globals = toTable(LuaState::ENV_GLOBALS);
}

LuaState::~LuaState()
{
	m_globals = LuaTable();

	lua_close( m_impl );
}

void LuaState::setTop( int index )
{
	lua_settop( m_impl, index );
}

void LuaState::pop( int n )
{
	lua_pop( m_impl, n );
}

void LuaState::push( int index )
{
	lua_pushvalue( m_impl, index );
}

void LuaState::remove( int index )
{
	lua_remove( m_impl, index );
}

void LuaState::insert( int index )
{
	lua_insert( m_impl, index );
}

void LuaState::replace( int index )
{
	lua_replace( m_impl, index );
}

LuaState::Type LuaState::type( int index ) 
{
	return (Type)lua_type( m_impl, index );
}

bool LuaState::isNil( int index ) 
{
	return 0 != lua_isnil( m_impl, index );
}

bool LuaState::isBoolean( int index ) 
{
	return 0 != lua_isboolean( m_impl, index );
}

bool LuaState::isNumber( int index ) 
{
	return 0 != lua_isnumber( m_impl, index );
}

bool LuaState::isString( int index ) 
{
	return 0 != lua_isstring( m_impl, index );
}

bool LuaState::isTable( int index ) 
{
	return 0 != lua_istable( m_impl, index );
}

bool LuaState::isFunction( int index ) 
{
	return 0 != lua_isfunction( m_impl, index );
}

bool LuaState::isCFunction( int index ) 
{
	return 0 != lua_iscfunction( m_impl, index );
}

bool LuaState::isUserData( int index ) 
{
	return 0 != lua_isuserdata( m_impl, index );
}

bool LuaState::isLightUserData( int index ) 
{
	return 0 != lua_islightuserdata( m_impl, index );
}

bool LuaState::equal( int index1, int index2 ) 
{
	return 0 != lua_equal( m_impl, index1, index2 );
}

bool LuaState::lessThan( int index1, int index2 ) 
{
	return 0 != lua_lessthan( m_impl, index1, index2 );
}

bool LuaState::rawEqual( int index1, int index2 ) 
{
	return 0 != lua_rawequal( m_impl, index1, index2 );
}

bool LuaState::toBoolean( int index ) 
{
	checkType( index, TYPE_BOOLEAN );
	return 0 != lua_toboolean( m_impl, index );
}

float LuaState::toNumber( int index ) 
{
	checkType( index, TYPE_NUMBER );
	return lua_tonumber( m_impl, index );
}

const char* LuaState::toString( int index ) 
{
	checkType( index, TYPE_STRING );
	return lua_tostring( m_impl, index );
}

LuaState::CFunction LuaState::toCFunction( int index )
{
	if ( !isCFunction(index) )
		throwError( LuaException( Format("Excepted type C-function at stack position {0} but found {1}", index, toString(type(index))) ) );
	return lua_tocfunction( m_impl, index );
}

void* LuaState::toLightUserData( int index ) 
{
	checkType( index, TYPE_LIGHTUSERDATA );
	return lua_touserdata( m_impl, index );
}

void* LuaState::toUserData( int index ) 
{
	checkType( index, TYPE_USERDATA );
	return lua_touserdata( m_impl, index );
}

LuaTable& LuaState::globals()
{
	return m_globals;
}

LuaTable LuaState::toTable( int index )
{
	checkType( index, TYPE_TABLE );

	lua_pushvalue( m_impl, index );

	LuaTable tab;
	tab.m_luastate = this;
	tab.m_ref = lua_ref( m_impl, true );
	return tab;
}

void LuaState::pushBoolean( bool v ) 
{
	lua_pushboolean( m_impl, v?1:0 );
}

void LuaState::pushNumber( float v ) 
{
	lua_pushnumber( m_impl, v );
}

void LuaState::pushString( const char* v ) 
{
	lua_pushstring( m_impl, v );
}

void LuaState::pushString( const String& v ) 
{
	pushString( m_impl, v );
}

void LuaState::pushString( lua_State* impl, const NS(lang,String)& v )
{
	char buf0[1000];
	Array<char> buf1;
	char* buf = 0;
	if ( String::cpy(buf0, sizeof(buf0), v) )
	{
		buf = buf0;
	}
	else
	{
		buf1.resize( v.length()+1 );
		String::cpy( buf1.begin(), buf1.size()-1, v );
		buf1.last() = 0;
		buf = buf1.begin();
	}
	
	lua_pushstring( impl, buf );
}

void LuaState::pushNil() 
{
	lua_pushnil( m_impl );
}

void LuaState::pushCFunction( CFunction v ) 
{
	lua_pushcfunction( m_impl, v );
}

void LuaState::pushLightUserData( void* v ) 
{
	lua_pushlightuserdata( m_impl, v );
}

void LuaState::pushCClosure( CFunction v, int n )
{
	lua_pushcclosure( m_impl, v, n );
}

int LuaState::upValueIndex( int n )
{
	return lua_upvalueindex( n );
}

void LuaState::pushTable( lua_State* impl, const LuaTable* v )
{
	if ( v && v->m_ref >= 0 )
		lua_getref( impl, v->m_ref );
	else
		lua_pushnil( impl );
}

void LuaState::newTable() 
{
	lua_newtable( m_impl );
}

void LuaState::getTable( int index ) 
{
	checkType( index, TYPE_TABLE );
	lua_gettable( m_impl, index );
}

void LuaState::rawGet( int index ) 
{
	checkType( index, TYPE_TABLE );
	lua_rawget( m_impl, index );
}

void LuaState::setTable( int index ) 
{
	checkType( index, TYPE_TABLE );
	lua_settable( m_impl, index );
}

void LuaState::rawSet( int index ) 
{
	checkType( index, TYPE_TABLE );
	lua_rawset( m_impl, index );
}

bool LuaState::next( int index ) 
{
	checkType( index, TYPE_TABLE );
	return 0 != lua_next( m_impl, index );
}

void LuaState::rawGetI( int index, int n ) 
{
	checkType( index, TYPE_TABLE );
	lua_rawgeti( m_impl, index, n );
}

void LuaState::rawSetI( int index, int n ) 
{
	checkType( index, TYPE_TABLE );
	lua_rawseti( m_impl, index, n );
}

void LuaState::setFEnv( int index )
{
	checkType( index, TYPE_FUNCTION );
	checkType( -1, TYPE_TABLE );
	lua_setfenv( m_impl, index );
}

void LuaState::getFEnv( int index )
{
	checkType( index, TYPE_FUNCTION );
	lua_getfenv( m_impl, index );
}

void LuaState::call( int nargs, int nresults ) 
{
	lua_call( m_impl, nargs, nresults );
}

void LuaState::compile( const char* buffer, int size, const String& name, LuaTable* env )
{
	LuaStackRestore lsr( m_impl );

	char namebuf[512];
	String::cpy( namebuf, sizeof(namebuf), name );

	LoadBuffer ls;
	ls.buffer = buffer;
	ls.size = size;

	int err = lua_load( m_impl, loadBuffer, &ls, namebuf );
	handleLoadResult( err, name, env );
}

void LuaState::compile( InputStream* in, int size, const String& name, LuaTable* env )
{
	LuaStackRestore lsr( m_impl );

	char namebuf[512];
	String::cpy( namebuf, sizeof(namebuf), name );

	LoadFile ls;
	ls.in = in;
	ls.size = size;

	int err = lua_load( m_impl, loadFile, &ls, namebuf );
	handleLoadResult( err, name, env );
}

void LuaState::handleLoadResult( int err, const String& /*name*/, LuaTable* env )
{
	if ( 0 == err )
	{
		if ( env )
		{
			pushTable( env );
			setFEnv( -2 );
		}

		call( 0, 0 );
	}
	else
	{
		String errsz = toString( -1 );
		throwError( LuaException( Format("Error while compiling Lua: {0}", errsz) ) );
	}
}

const char* LuaState::loadBuffer( lua_State* /*impl*/, void* data, size_t* size )
{
	LoadBuffer* ls = reinterpret_cast<LoadBuffer*>( data );
	if ( ls->size == 0 )
		return 0;
	*size = ls->size;
	ls->size = 0;
	return ls->buffer;
}

const char* LuaState::loadFile( lua_State* /*impl*/, void* data, size_t* size )
{
	LoadFile* ls = reinterpret_cast<LoadFile*>( data );
	if ( ls->size == 0 )
		return 0;
	int bytesread = ls->in->read( ls->buf, sizeof(ls->buf) );
	*size = bytesread;
	ls->size -= bytesread;
	return ls->buf;
}

void LuaState::checkType( int index, Type type )
{
	Type actualtype = (Type)lua_type(m_impl,index);
	if ( actualtype != type )
	{
		char stacktrace[1000] = {0};
		appendStackTrace( stacktrace, sizeof(stacktrace), m_impl );
		throwError( LuaException( Format("Excepted type {0} at stack position {1} but found {2}{3}", toString(type), index, toString(actualtype), stacktrace) ) );
	}
}

void LuaState::incorrectType( int index )
{
	char stacktrace[1000] = {0};
	appendStackTrace( stacktrace, sizeof(stacktrace), m_impl );
	throwError( LuaException( Format("Incorrect table type at stack position {0} {1}", index, stacktrace) ) );
}

int LuaState::top() const
{
	return lua_gettop( m_impl );
}

LuaTable* LuaState::getThisPtr( lua_State* impl, int index )
{
	LuaTable* thisptr = 0;
	LuaStackRestore lsr( impl );

	if ( !lua_istable(impl,index) )
	{
		char stacktrace[1000] = {0};
		appendStackTrace( stacktrace, sizeof(stacktrace), impl );
		throwError( LuaException( Format("Failed to get this pointer because value at stack index {0} is {1} {2}", index, LuaState::toString((Type)lua_type(impl,index)), stacktrace) ) );
	}

	lua_rawgeti( impl, index, 0 );

	if ( !lua_islightuserdata(impl,-1) )
	{
		char stacktrace[1000] = {0};
		appendStackTrace( stacktrace, sizeof(stacktrace), impl );
		throwError( LuaException( Format("Failed to get this pointer because table at stack index {0} has no user data zero element {1}", index, stacktrace) ) );
	}

	thisptr = static_cast<LuaTable*>( lua_touserdata(impl,-1) );
	return thisptr;
}

const char* LuaState::toString( Type type )
{
	switch ( type )
	{
	case TYPE_NIL: return "NIL";
	case TYPE_NUMBER: return "NUMBER";
	case TYPE_BOOLEAN: return "BOOLEAN";
	case TYPE_STRING: return "STRING";
	case TYPE_TABLE: return "TABLE";
	case TYPE_FUNCTION: return "FUNCTION";
	case TYPE_USERDATA: return "USERDATA";
	case TYPE_THREAD: return "THREAD";
	case TYPE_LIGHTUSERDATA: return "LIGHTUSERDATA";
	default: return "INVALID";
	}
}

void LuaState::appendStackTrace( char* buf, int bufsize, lua_State* impl )
{
	char fmtbuf[512];

	bool callstack = false;
	for ( int level = 0 ; level < 10 ; ++level )
	{
		lua_Debug st;
		memset( &st, 0, sizeof(st) );
		if ( !lua_getstack( impl, 1+level, &st ) )
			break;
		if ( !lua_getinfo( impl, "Snl", &st ) )
			break;

		if ( !callstack )
		{
			String::cat( buf, bufsize, "\nCall stack:\n" );
			callstack = true;
		}

		sprintf( fmtbuf, "%s(%i)\n", st.source, st.currentline );
		String::cat( buf, bufsize, fmtbuf );
	}

	if ( callstack )
		String::cat( buf, bufsize, "(end)\n" );
	else
		String::cat( buf, bufsize, " (call stack not available)" );
}

int LuaState::handleError( lua_State* impl )
{
	assert( lua_gettop(impl) > 0 );
	assert( lua_isstring(impl,-1) );

	char err[2000] = {0};
	const char* errsz = lua_tostring( impl, -1 );
	String::cpy( err, sizeof(err), errsz );
	appendStackTrace( err, sizeof(err), impl );

	throwError( LuaException( Format(err) ) );
	return 0;
}

void LuaState::getDispatchData( lua_State* impl, LuaState** luastate, LuaFunctor* lf )
{
	lua_pushvalue( impl, LUA_GLOBALSINDEX );
	lua_rawgeti( impl, -1, 0 );

	*luastate = static_cast<LuaState*>( lua_touserdata(impl,-1) );
	lua_pop( impl, 2 );

	void** data = reinterpret_cast<void**>(lf);
	const int datasize = int(sizeof(LuaFunctor)/sizeof(void*));
	for ( int i = 0 ; i < datasize ; ++i )
		data[i] = lua_touserdata( impl, lua_upvalueindex(i+1) );

	assert( lf->obj );
}


END_NAMESPACE() // lua

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
