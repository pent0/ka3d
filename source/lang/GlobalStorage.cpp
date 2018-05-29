#include <lang/GlobalStorage.h>
#if defined(PLATFORM_S60_1X_2X) || defined(PLATFORM_S60_3X)
#include <e32std.h>
#endif
#include <string.h>
#include <config.h>


BEGIN_NAMESPACE(lang) 

#if defined(PLATFORM_BREW)
	GlobalStorage* GlobalStorage::sm_globalStorage;
#elif defined(PLATFORM_PS2) || defined(PLATFORM_WIN32) || defined(PLATFORM_WINCE) || defined(PLATFORM_NGI) || defined(PLATFORM_S60_3X)
	GlobalStorage GlobalStorage::sm_instance;
#elif defined(PLATFORM_S60_1X_2X)
	// no static data
#else
	#error GlobalStorage not defined on this platform
#endif


GlobalStorage::GlobalStorage()
{
	// a bit ugly but this class contains only regular pointers
	// and we don't want to end up forgetting to initialize some pointer
	memset( this, 0, sizeof(GlobalStorage) );
}

#if defined(PLATFORM_S60_1X_2X)

GlobalStorage& GlobalStorage::get()
{
	GlobalStorage* gs = reinterpret_cast<GlobalStorage*>( Dll::Tls() );
	if ( !gs )
	{
		gs = new GlobalStorage;
		Dll::SetTls( gs );
	}
	return *gs;
}

void GlobalStorage::release()
{
	GlobalStorage* gs = reinterpret_cast<GlobalStorage*>( Dll::Tls() );
	if ( gs != 0 )
	{
		delete gs;
		Dll::SetTls( 0 );
	}
}

#elif defined(PLATFORM_BREW)

	GlobalStorage & GlobalStorage::get()
	{
		if (sm_globalStorage == NULL)
			sm_globalStorage = new GlobalStorage();

		return *sm_globalStorage;
	}
	 void GlobalStorage::release()
	 {
	 	if (sm_globalStorage != NULL)
	 	{
	 		delete sm_globalStorage;
	 		sm_globalStorage = NULL;
	 	}
	 }

#elif defined(PLATFORM_PS2) || defined(PLATFORM_WIN32) || defined(PLATFORM_WINCE) || defined(PLATFORM_NGI) || defined(PLATFORM_S60_3X)

void GlobalStorage::release()
{
}

#else

#error GlobalStorage::release not defined on this platform

#endif

END_NAMESPACE() // lang

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
