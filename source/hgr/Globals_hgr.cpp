#include <hgr/Globals.h>
#include <hgr/DefaultResourceManager.h>
#include <lang/GlobalStorage.h>
#include <config.h>


USING_NAMESPACE(lang)


BEGIN_NAMESPACE(hgr) 


Globals::Globals()
{
}

Globals::~Globals()
{
}

void Globals::init()
{
	cleanup();
	GlobalStorage::get().hgrGlobals = new Globals;
}

void Globals::cleanup()
{
	delete GlobalStorage::get().hgrGlobals;
	GlobalStorage::get().hgrGlobals = 0;
}


END_NAMESPACE() // hgr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
