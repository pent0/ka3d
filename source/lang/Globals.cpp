#include <lang/Globals.h>
#include <lang/pp.h>
#include <config.h>


#ifdef PLATFORM_WIN32
	#define DEFAULT_STRING_MEMORY	100000
	#define TEMPBUFFER_MEMORY		0x10000
#else
	#define DEFAULT_STRING_MEMORY	10000
	#define TEMPBUFFER_MEMORY		0x10000
#endif


BEGIN_NAMESPACE(lang) 


lang_Globals::lang_Globals( int stringmem, int tempmem ) :
	stringPool(stringmem,0,"String"),
	cstrBufferIndex(0)
{
	tempBufferMem = new char[tempmem];
	tempBufferMemUsed = 0;
	tempBufferMemTotal = tempmem;
	tempBufferCount = 0;
}

lang_Globals::~lang_Globals()
{
	assert( tempBufferMemUsed == 0 );
	delete[] tempBufferMem;
}

void lang_Globals::init()
{
	cleanup();
	GlobalStorage::get().langGlobals = new lang_Globals( DEFAULT_STRING_MEMORY, TEMPBUFFER_MEMORY );
}

void lang_Globals::init( int stringmem, int tempmem )
{
	cleanup();
	GlobalStorage::get().langGlobals = new lang_Globals( stringmem, tempmem );
}

void lang_Globals::cleanup()
{
	delete GlobalStorage::get().langGlobals;
	GlobalStorage::get().langGlobals = 0;
}


END_NAMESPACE() // lang

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
