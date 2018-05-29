template <class V> void getValue( LuaState* luastate, int index, V& v )
{
	luastate->toValue( index, v );
}

template <class V> void getValue( LuaState* luastate, int index, V*& v )
{
	LuaTable* tab = LuaState::getThisPtr( luastate->impl(), index );
	v = dynamic_cast<V*>(tab);
	if ( 0 == v )
		luastate->incorrectType( index );
}

template <class V> int pushValue( LuaState* luastate, const V& v )
{
	luastate->pushValue( v );
	return 1;
}

template <class R> class ReturnValue
{
public:
	template <class T> static int
		callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)() )
	{
		return pushValue( luastate, (thisptr->*funcptr)() );
	}

	template <class T, class A1> static int
		callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)(A1) )
	{
		A1 a1; getValue(luastate,1,a1);
		return pushValue( luastate, (thisptr->*funcptr)( a1 ) );
	}

	template <class T, class A1, class A2> static int
		callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)(A1,A2) )
	{
		A1 a1; getValue(luastate,1,a1);
		A2 a2; getValue(luastate,2,a2);
		return pushValue( luastate, (thisptr->*funcptr)( a1, a2 ) );
	}

	template <class T, class A1, class A2, class A3> static int
		callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)(A1,A2,A3) )
	{
		A1 a1; getValue(luastate,1,a1);
		A2 a2; getValue(luastate,2,a2);
		A3 a3; getValue(luastate,3,a3);
		return pushValue( luastate, (thisptr->*funcptr)( a1, a2, a3 ) );
	}

	template <class T, class A1, class A2, class A3, class A4> static int
		callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)(A1,A2,A3,A4) )
	{
		A1 a1; getValue(luastate,1,a1);
		A2 a2; getValue(luastate,2,a2);
		A3 a3; getValue(luastate,3,a3);
		A4 a4; getValue(luastate,4,a4);
		return pushValue( luastate, (thisptr->*funcptr)( a1, a2, a3, a4 ) );
	}

	template <class T, class A1, class A2, class A3, class A4, class A5> static int
		callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)(A1,A2,A3,A4,A5) )
	{
		A1 a1; getValue(luastate,1,a1);
		A2 a2; getValue(luastate,2,a2);
		A3 a3; getValue(luastate,3,a3);
		A4 a4; getValue(luastate,4,a4);
		A5 a5; getValue(luastate,5,a5);
		return pushValue( luastate, (thisptr->*funcptr)( a1, a2, a3, a4, a5 ) );
	}

	template <class T, class A1, class A2, class A3, class A4, class A5, class A6> static int
		callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)(A1,A2,A3,A4,A5,A6) )
	{
		A1 a1; getValue(luastate,1,a1);
		A2 a2; getValue(luastate,2,a2);
		A3 a3; getValue(luastate,3,a3);
		A4 a4; getValue(luastate,4,a4);
		A5 a5; getValue(luastate,5,a5);
		A6 a6; getValue(luastate,6,a6);
		return pushValue( luastate, (thisptr->*funcptr)( a1, a2, a3, a4, a5, a6 ) );
	}

	template <class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7> static int
		callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)(A1,A2,A3,A4,A5,A6,A7) )
	{
		A1 a1; getValue(luastate,1,a1);
		A2 a2; getValue(luastate,2,a2);
		A3 a3; getValue(luastate,3,a3);
		A4 a4; getValue(luastate,4,a4);
		A5 a5; getValue(luastate,5,a5);
		A6 a6; getValue(luastate,6,a6);
		A7 a7; getValue(luastate,7,a7);
		return pushValue( luastate, (thisptr->*funcptr)( a1, a2, a3, a4, a5, a6, a7 ) );
	}

	template <class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> static int
		callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)(A1,A2,A3,A4,A5,A6,A7,A8) )
	{
		A1 a1; getValue(luastate,1,a1);
		A2 a2; getValue(luastate,2,a2);
		A3 a3; getValue(luastate,3,a3);
		A4 a4; getValue(luastate,4,a4);
		A5 a5; getValue(luastate,5,a5);
		A6 a6; getValue(luastate,6,a6);
		A7 a7; getValue(luastate,7,a7);
		A8 a8; getValue(luastate,8,a8);
		return pushValue( luastate, (thisptr->*funcptr)( a1, a2, a3, a4, a5, a6, a7, a8 ) );
	}

	template <class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9> static int
		callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)(A1,A2,A3,A4,A5,A6,A7,A8,A9) )
	{
		A1 a1; getValue(luastate,1,a1);
		A2 a2; getValue(luastate,2,a2);
		A3 a3; getValue(luastate,3,a3);
		A4 a4; getValue(luastate,4,a4);
		A5 a5; getValue(luastate,5,a5);
		A6 a6; getValue(luastate,6,a6);
		A7 a7; getValue(luastate,7,a7);
		A8 a8; getValue(luastate,8,a8);
		A9 a9; getValue(luastate,9,a9);
		return pushValue( luastate, (thisptr->*funcptr)( a1, a2, a3, a4, a5, a6, a7, a8, a9 ) );
	}
};

template <> class ReturnValue<void>
{
public:
	template <class T> static int
		callMethod( LuaState* luastate, T* thisptr, void (T::*funcptr)() )
	{
		(thisptr->*funcptr)(); return 0;
	}

	template <class T, class A1> static int
		callMethod( LuaState* luastate, T* thisptr, void (T::*funcptr)(A1) )
	{
		A1 a1; getValue(luastate,1,a1);
		(thisptr->*funcptr)( a1 ); return 0;
	}

	template <class T, class A1, class A2> static int
		callMethod( LuaState* luastate, T* thisptr, void (T::*funcptr)(A1,A2) )
	{
		A1 a1; getValue(luastate,1,a1);
		A2 a2; getValue(luastate,2,a2);
		(thisptr->*funcptr)( a1, a2 ); return 0;
	}

	template <class T, class A1, class A2, class A3> static int
		callMethod( LuaState* luastate, T* thisptr, void (T::*funcptr)(A1,A2,A3) )
	{
		A1 a1; getValue(luastate,1,a1);
		A2 a2; getValue(luastate,2,a2);
		A3 a3; getValue(luastate,3,a3);
		(thisptr->*funcptr)( a1, a2, a3 ); return 0;
	}

	template <class T, class A1, class A2, class A3, class A4> static int
		callMethod( LuaState* luastate, T* thisptr, void (T::*funcptr)(A1,A2,A3,A4) )
	{
		A1 a1; getValue(luastate,1,a1);
		A2 a2; getValue(luastate,2,a2);
		A3 a3; getValue(luastate,3,a3);
		A4 a4; getValue(luastate,4,a4);
		(thisptr->*funcptr)( a1, a2, a3, a4 ); return 0;
	}

	template <class T, class A1, class A2, class A3, class A4, class A5> static int
		callMethod( LuaState* luastate, T* thisptr, void (T::*funcptr)(A1,A2,A3,A4,A5) )
	{
		A1 a1; getValue(luastate,1,a1);
		A2 a2; getValue(luastate,2,a2);
		A3 a3; getValue(luastate,3,a3);
		A4 a4; getValue(luastate,4,a4);
		A5 a5; getValue(luastate,5,a5);
		(thisptr->*funcptr)( a1, a2, a3, a4, a5 ); return 0;
	}

	template <class T, class A1, class A2, class A3, class A4, class A5, class A6> static int
		callMethod( LuaState* luastate, T* thisptr, void (T::*funcptr)(A1,A2,A3,A4,A5,A6) )
	{
		A1 a1; getValue(luastate,1,a1);
		A2 a2; getValue(luastate,2,a2);
		A3 a3; getValue(luastate,3,a3);
		A4 a4; getValue(luastate,4,a4);
		A5 a5; getValue(luastate,5,a5);
		A6 a6; getValue(luastate,6,a6);
		(thisptr->*funcptr)( a1, a2, a3, a4, a5, a6 ); return 0;
	}

	template <class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7> static int
		callMethod( LuaState* luastate, T* thisptr, void (T::*funcptr)(A1,A2,A3,A4,A5,A6,A7) )
	{
		A1 a1; getValue(luastate,1,a1);
		A2 a2; getValue(luastate,2,a2);
		A3 a3; getValue(luastate,3,a3);
		A4 a4; getValue(luastate,4,a4);
		A5 a5; getValue(luastate,5,a5);
		A6 a6; getValue(luastate,6,a6);
		A7 a7; getValue(luastate,7,a7);
		(thisptr->*funcptr)( a1, a2, a3, a4, a5, a6, a7 ); return 0;
	}

	template <class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> static int
		callMethod( LuaState* luastate, T* thisptr, void (T::*funcptr)(A1,A2,A3,A4,A5,A6,A7,A8) )
	{
		A1 a1; getValue(luastate,1,a1);
		A2 a2; getValue(luastate,2,a2);
		A3 a3; getValue(luastate,3,a3);
		A4 a4; getValue(luastate,4,a4);
		A5 a5; getValue(luastate,5,a5);
		A6 a6; getValue(luastate,6,a6);
		A7 a7; getValue(luastate,7,a7);
		A8 a8; getValue(luastate,8,a8);
		(thisptr->*funcptr)( a1, a2, a3, a4, a5, a6, a7, a8 ); return 0;
	}

	template <class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9> static int
		callMethod( LuaState* luastate, T* thisptr, void (T::*funcptr)(A1,A2,A3,A4,A5,A6,A7,A8,A9) )
	{
		A1 a1; getValue(luastate,1,a1);
		A2 a2; getValue(luastate,2,a2);
		A3 a3; getValue(luastate,3,a3);
		A4 a4; getValue(luastate,4,a4);
		A5 a5; getValue(luastate,5,a5);
		A6 a6; getValue(luastate,6,a6);
		A7 a7; getValue(luastate,7,a7);
		A8 a8; getValue(luastate,8,a8);
		A9 a9; getValue(luastate,9,a9);
		(thisptr->*funcptr)( a1, a2, a3, a4, a5, a6, a7, a8, a9 ); return 0;
	}
};

template <class R, class T> int
	callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)() )
{
	return ReturnValue<R>::callMethod( luastate, thisptr, funcptr );
}

template <class R, class T, class A1> int
	callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)(A1) )
{
	return ReturnValue<R>::callMethod( luastate, thisptr, funcptr );
}

template <class R, class T, class A1, class A2> int
	callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)(A1,A2) )
{
	return ReturnValue<R>::callMethod( luastate, thisptr, funcptr );
}

template <class R, class T, class A1, class A2, class A3> int
	callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)(A1,A2,A3) )
{
	return ReturnValue<R>::callMethod( luastate, thisptr, funcptr );
}

template <class R, class T, class A1, class A2, class A3, class A4> int
	callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)(A1,A2,A3,A4) )
{
	return ReturnValue<R>::callMethod( luastate, thisptr, funcptr );
}

template <class R, class T, class A1, class A2, class A3, class A4, class A5> int
	callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)(A1,A2,A3,A4,A5) )
{
	return ReturnValue<R>::callMethod( luastate, thisptr, funcptr );
}

template <class R, class T, class A1, class A2, class A3, class A4, class A5, class A6> int
	callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)(A1,A2,A3,A4,A5,A6) )
{
	return ReturnValue<R>::callMethod( luastate, thisptr, funcptr );
}

template <class R, class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7> int
	callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)(A1,A2,A3,A4,A5,A6,A7) )
{
	return ReturnValue<R>::callMethod( luastate, thisptr, funcptr );
}

template <class R, class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> int
	callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)(A1,A2,A3,A4,A5,A6,A7,A8) )
{
	return ReturnValue<R>::callMethod( luastate, thisptr, funcptr );
}

template <class R, class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9> int
	callMethod( LuaState* luastate, T* thisptr, R (T::*funcptr)(A1,A2,A3,A4,A5,A6,A7,A8,A9) )
{
	return ReturnValue<R>::callMethod( luastate, thisptr, funcptr );
}

template <typename T, typename F> class LuaRawMethodDispatcher
{
public:
	static int dispatch( lua_State* impl )
	{
		LuaState* luastate;
		LuaFunctor lf;
		LuaState::getDispatchData( impl, &luastate, &lf );
		return callMethod( luastate, static_cast<T*>(lf.obj), *reinterpret_cast<F*>(lf.func) );
	}
};

template <typename T, typename F> void
	LuaObject::registerMethod( const char* name, T* obj, F func )
{
	LuaFunctor lf;
	assert( sizeof(F) <= sizeof(lf.func) );
	lf.obj = obj;
	for ( int i = 0 ; i < int(sizeof(F)) ; ++i )
		lf.func[i] = reinterpret_cast<const char*>(&func)[i];
	registerClosure( name, &lf, LuaRawMethodDispatcher<T,F>::dispatch );
}

template <class T> void	
	LuaObject::registerLuaMethod( const char* name, T* obj, int (T::*func)(LuaState*) )
{
	assert( func != 0 && obj != 0 );
	LuaState* luastate = luaState();
	LuaStackRestore lsr( luastate );
	luastate->pushTable( this );
	luastate->pushString( name );
	luastate->pushMethodClosure( obj, func );
	assert( !luastate->isNil(-1) );
	luastate->setTable( -3 );
}

template <class A1> void LuaObject::call( const char* name, A1 a1 )
{
	LuaState* luastate = luaState();
	LuaStackRestore lsr( luastate );
	pushMember( name );
	pushValue( luastate, a1 );
	luastate->call( 1, 0 );
}

template <class A1, class A2> void LuaObject::call( const char* name, A1 a1, A2 a2 )
{
	LuaState* luastate = luaState();
	LuaStackRestore lsr( luastate );
	pushMember( name );
	pushValue( luastate, a1 );
	pushValue( luastate, a2 );
	luastate->call( 2, 0 );
}

template <class A1, class A2, class A3> void LuaObject::call( const char* name, A1 a1, A2 a2, A3 a3 )
{
	LuaState* luastate = luaState();
	LuaStackRestore lsr( luastate );
	pushMember( name );
	pushValue( luastate, a1 );
	pushValue( luastate, a2 );
	pushValue( luastate, a3 );
	luastate->call( 3, 0 );
}

template <class A1, class A2, class A3, class A4> void LuaObject::call( const char* name, A1 a1, A2 a2, A3 a3, A4 a4 )
{
	LuaState* luastate = luaState();
	LuaStackRestore lsr( luastate );
	pushMember( name );
	pushValue( luastate, a1 );
	pushValue( luastate, a2 );
	pushValue( luastate, a3 );
	pushValue( luastate, a4 );
	luastate->call( 4, 0 );
}

template <class A1, class A2, class A3, class A4, class A5> void LuaObject::call( const char* name, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 )
{
	LuaState* luastate = luaState();
	LuaStackRestore lsr( luastate );
	pushMember( name );
	pushValue( luastate, a1 );
	pushValue( luastate, a2 );
	pushValue( luastate, a3 );
	pushValue( luastate, a4 );
	pushValue( luastate, a5 );
	luastate->call( 5, 0 );
}

template <class A1, class A2, class A3, class A4, class A5, class A6> void LuaObject::call( const char* name, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6 )
{
	LuaState* luastate = luaState();
	LuaStackRestore lsr( luastate );
	pushMember( name );
	pushValue( luastate, a1 );
	pushValue( luastate, a2 );
	pushValue( luastate, a3 );
	pushValue( luastate, a4 );
	pushValue( luastate, a5 );
	pushValue( luastate, a6 );
	luastate->call( 6, 0 );
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7> void LuaObject::call( const char* name, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7 )
{
	LuaState* luastate = luaState();
	LuaStackRestore lsr( luastate );
	pushMember( name );
	pushValue( luastate, a1 );
	pushValue( luastate, a2 );
	pushValue( luastate, a3 );
	pushValue( luastate, a4 );
	pushValue( luastate, a5 );
	pushValue( luastate, a6 );
	pushValue( luastate, a7 );
	luastate->call( 7, 0 );
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> void LuaObject::call( const char* name, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8 )
{
	LuaState* luastate = luaState();
	LuaStackRestore lsr( luastate );
	pushMember( name );
	pushValue( luastate, a1 );
	pushValue( luastate, a2 );
	pushValue( luastate, a3 );
	pushValue( luastate, a4 );
	pushValue( luastate, a5 );
	pushValue( luastate, a6 );
	pushValue( luastate, a7 );
	pushValue( luastate, a8 );
	luastate->call( 8, 0 );
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9> void LuaObject::call( const char* name, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9 )
{
	LuaState* luastate = luaState();
	LuaStackRestore lsr( luastate );
	pushMember( name );
	pushValue( luastate, a1 );
	pushValue( luastate, a2 );
	pushValue( luastate, a3 );
	pushValue( luastate, a4 );
	pushValue( luastate, a5 );
	pushValue( luastate, a6 );
	pushValue( luastate, a7 );
	pushValue( luastate, a8 );
	pushValue( luastate, a9 );
	luastate->call( 9, 0 );
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
