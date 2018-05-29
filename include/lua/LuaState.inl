class LuaFunctor
{
public:
	void*	obj;
	char	func[8];
};

template <typename T> class LuaStateMethodDispatcher
{
public:
	typedef int (T::*LuaStateMethod_t)( LuaState* );

	static int dispatch( lua_State* impl )
	{
		LuaState* luastate;
		LuaFunctor lf;
		LuaState::getDispatchData( impl, &luastate, &lf );
		LuaStateMethod_t func = *reinterpret_cast<LuaStateMethod_t*>(lf.func);
		return (static_cast<T*>(lf.obj)->*func)( luastate );
	}
};

template <class T> void LuaState::pushMethodClosure( T* obj, int (T::*func)(LuaState*) )
{
	LuaFunctor lf;
	assert( sizeof(func) <= sizeof(lf.func) );
	lf.obj = obj;
	for ( int i = 0 ; i < int(sizeof(func)) ; ++i )
		lf.func[i] = reinterpret_cast<const char*>(&func)[i];

	// store data as light user data values
	void* const* data = reinterpret_cast<void* const*>(&lf); 
	const int datasize = int(sizeof(LuaFunctor)/sizeof(void*));
	for ( int i = 0 ; i < datasize ; ++i )
		pushLightUserData( data[i] );
	pushCClosure( LuaStateMethodDispatcher<T>::dispatch, datasize );
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
