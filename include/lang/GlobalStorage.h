#ifndef _LANG_GLOBALSTORAGE_H
#define _LANG_GLOBALSTORAGE_H


#include <lang/pp.h>


BEGIN_NAMESPACE(hgr) class Globals;END_NAMESPACE()
BEGIN_NAMESPACE(lang) class lang_Globals;END_NAMESPACE()
BEGIN_NAMESPACE(framework) class App;END_NAMESPACE()


BEGIN_NAMESPACE(lang) 


/** 
 * Storage for global variables. 
 * Dependency to users in the name only.
 * NS(GlobalStorage,release) should be called after globals are no longer used (i.e. last call)
 * Library globals (e.g. NS(lang,lang_Globals)) need to be cleaned up before NS(GlobalStorage,release).
 * NS(GlobalStorage,release) cannot call library cleanup functions since then GlobalStorage
 * (and lang lib) would become dependent on each of the lang_Globals declared here.
 *
 * @ingroup lang
 */
class GlobalStorage
{
public:
	NS(hgr,Globals)*			hgrGlobals;
	NS(lang,lang_Globals)*		langGlobals;
	void*									userGlobals;
	NS(framework,App)*			app;

	GlobalStorage();

	/** Returns global storage singleton. */
	static GlobalStorage&	get();

	/** Releases global storage singleton. */
	static void				release();

private:
#if defined(PLATFORM_BREW)
	static NS(lang,GlobalStorage)* sm_globalStorage;
#elif defined(PLATFORM_PS2) || defined(PLATFORM_WIN32) || defined(PLATFORM_WINCE) || defined(PLATFORM_NGI) || defined(PLATFORM_S60_3X)
	static GlobalStorage sm_instance;
#elif defined(PLATFORM_S60_1X_2X)
	// no static data on this platform
#else
	#error GlobalStorage not defined on this platform
#endif
};

#if defined(PLATFORM_BREW)
	// get in GlobalStorage.cpp
#elif defined(PLATFORM_PS2) || defined(PLATFORM_WIN32) || defined(PLATFORM_WINCE) || defined(PLATFORM_NGI) || defined(PLATFORM_S60_3X)
	inline GlobalStorage& GlobalStorage::get() 	{return sm_instance;}
#elif defined(PLATFORM_S60_1X_2X)
	// normal function in cpp on this platform
#else
	#error NS(GlobalStorage,get)() not defined on this platform
#endif


END_NAMESPACE() // lang


#endif // _LANG_GLOBALSTORAGE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
