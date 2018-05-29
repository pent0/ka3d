#include <io/all.h>
#include <lua/LuaState.h>
#include <lua/LuaObject.h>
#include <lang/all.h>
#include <stdio.h>
#include <string.h>
#include <config.h>


USING_NAMESPACE(io)
USING_NAMESPACE(lua)
USING_NAMESPACE(lang)


BEGIN_NAMESPACE(lua) 


class OtherClass :
	public LuaObject
{
public:
	OtherClass( LuaState* luastate ) :
		LuaObject( luastate )
	{
		registerMethod( "value", this, &OtherClass::value );
	}

	float value()
	{
		return 4.f;
	}
};

class MyClass :
	public LuaObject
{
public:
	// just to test that virtual functions work too
	virtual float add1( float a, float b ) {return a+b;}

	float add2( float a, OtherClass* b ) {return a+b->value()*2.f;}

	void modify( float v )
	{
		m_x = v;
	}

	int sum( LuaState* lua )
	{
		// number of arguments to sum
		int count = lua->top();
		// get numbers from top of Lua stack
		float sum = 0.f; 
		for ( int i = 1 ; i <= count ; ++i )
			sum += lua->toNumber(i);
		// push result to the top
		lua->pushNumber( sum );
		// inform Lua to expect one return value
		return 1;
	}

	MyClass( LuaState* luastate ) :
		LuaObject( luastate )
	{
		registerMethod( "add1", this, &MyClass::add1 );
		registerMethod( "add2", this, &MyClass::add2 );
		registerMethod( "other", this, &MyClass::other );
		registerMethod( "modify", this, &MyClass::modify );
		registerLuaMethod( "sum", this, &MyClass::sum );

		m_other = new OtherClass( luastate );
	}

	OtherClass* other()
	{
		return m_other;
	}

private:
	P(OtherClass) m_other;
	float m_x;
};

static void run( const String& datapath )
{
	P(LuaState) luastate = new LuaState;
	P(MyClass) myobj = new MyClass( luastate );

	FileInputStream in( datapath + "myclass.lua" );
	luastate->compile( &in, in.available(), in.toString(), myobj );

	assert( myobj->getString("testvar1") == "hello" );
	assert( myobj->getString("testvar2") == "world" );
	assert( myobj->getNumber("testvar3") == 1 );
	assert( myobj->getNumber("testvar4") == 23 );
	assert( myobj->getBoolean("testvar5") == true );
	assert( myobj->getBoolean("testvar6") == false );

	FileOutputStream out( datapath + "myclass_written.lua" );
	myobj->write( &out );

	myobj->call( "f", 3.f );
}

void test( const String& datapath )
{
	String libname = "lua";

	Debug::printf( "\n-------------------------------------------------------------------------\n" );
	Debug::printf( "%s library test begin\n", libname.c_str() );
	Debug::printf( "-------------------------------------------------------------------------\n" );
	run( datapath );
	Debug::printf( "%s library test ok\n", libname.c_str() );
}


END_NAMESPACE() // lua

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
