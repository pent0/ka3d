#ifndef _LANG_GLOBALS_H
#define _LANG_GLOBALS_H


#include <lang/pp.h>
#include <lang/MemoryPool.h>
#include <lang/TempBuffer.h>
#include <lang/GlobalStorage.h>


BEGIN_NAMESPACE(lang) 


/** 
 * lang_Globals in lang library.
 * @ingroup lang
 */
class lang_Globals
{
public:
	/** IMPLEMENTATION ONLY. "Extended stack" memory to be used by TempBuffers. This is not adjusted dynamically. */
	char*				tempBufferMem;

	/** IMPLEMENTATION ONLY. Amount of memory used from tempBufferMem. */
	int					tempBufferMemUsed;

	/** IMPLEMENTATION ONLY. Amount of memory used from tempBufferMem. */
	int					tempBufferMemTotal;

	/** IMPLEMENTATION ONLY. Allocated TempBuffers. */
	ByteTempBuffer		tempBuffers[ ByteTempBuffer::MAX_TEMP_BUFFERS ];

	/** IMPLEMENTATION ONLY. Number of allocated TempBuffers. */
	int					tempBufferCount;

	/** IMPLEMENTATION ONLY. String pool used by the String class implementation. */
	MemoryPool			stringPool;

	/** IMPLEMENTATION ONLY. Temporary buffer used by the NS(String,c)_str() implementation. */
	char				cstrBuffer[2000];

	/** IMPLEMENTATION ONLY. Temporary buffer index used by the NS(String,c)_str() implementation. */
	int					cstrBufferIndex;

	/**
	 * Initializes the globals with default string memory pool size.
	 * Note: This is a separate function from init(stringmem) to save space since
	 * get() is inlined.
	 */
	static void			init();

	/**
	 * Initializes the globals.
	 * @param stringmem Initial string memory pool size.
	 * @param tempmem Memory, in bytes, to allocate for TempBuffers. This is not adjusted dynamically! (on purpose)
	 */
	static void			init( int stringmem, int tempmem );

	/**
	 * Releases the resources allocated to the globals.
	 */
	static void			cleanup();

	/**
	 * Returns the globals.
	 */
	static lang_Globals&		get();

private:
	lang_Globals( int stringmem, int tempmem );
	~lang_Globals();
};


inline lang_Globals& lang_Globals::get()
{
	NS(lang,GlobalStorage)& gs = NS(lang,GlobalStorage)::get(); 
	if ( !gs.langGlobals )
		init();
	return *gs.langGlobals;
}


END_NAMESPACE() // lang


#endif // _LANG_GLOBALS_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
