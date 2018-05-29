#ifndef _HGR_GLOBALS_H
#define _HGR_GLOBALS_H


#ifndef HGR_NOPARTICLES
#include <hgr/ParticleSystem.h>
#endif
#include <lang/Array.h>
#include <lang/Hashtable.h>
#include <lang/GlobalStorage.h>


BEGIN_NAMESPACE(hgr) 


class ResourceManager;


/** 
 * Globals in hgr library.
 * @ingroup hgr
 */
class Globals
{
public:
	/**
	 * Default resource manager.
	 */
	P(ResourceManager)		resourceManager;

	/**
	 * Initializes the globals.
	 */
	static void				init();

	/**
	 * Releases the resources allocated to the globals.
	 */
	static void				cleanup();

	/**
	 * Returns the globals.
	 */
	static Globals&			get();

private:
	Globals();
	~Globals();

	Globals( const Globals& );
	Globals& operator=( const Globals& );
};


inline Globals& Globals::get()
{
	NS(lang,GlobalStorage)& gs = NS(lang,GlobalStorage)::get(); 
	if ( !gs.hgrGlobals )
		init();
	return *gs.hgrGlobals;
}


END_NAMESPACE() // hgr


#endif // _HGR_GLOBALS_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
