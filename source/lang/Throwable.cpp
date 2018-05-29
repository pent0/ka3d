#include <lang/Throwable.h>
#include <lang/Debug.h>
#include <lang/assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOSERVICE
#define NOMCX
#define NOIME
#define NOSOUND
#define NOCOMM
#define NOKANJI
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#include <windows.h>
#endif
#include <config.h>


#ifdef WIN32
#pragma warning( disable : 4702 ) // unreachable code, ok for debug
#endif

#define PRINTF_OUTPUT
#ifdef WIN32
#define WIN32_MESSAGEBOX_OUTPUT
#endif
#if defined(PLATFORM_BREW) || defined(PLATFORM_NGI)
#define FPRINTF_OUTPUT_AND_TERMINATE
#endif


BEGIN_NAMESPACE(lang) 


Throwable::Throwable()
{
}

Throwable::Throwable( const Format& msg ) :
	m_msg( msg )
{
	char buf[256];
	msg.format( buf, sizeof(buf) );
	Debug::printf( "Created Throwable( \"%s\" )\n", buf );
}

const Format& Throwable::getMessage() const
{
	return m_msg;
}

void Throwable::showExceptionDialog()
{
	String msg = m_msg.format();

#ifdef PRINTF_OUTPUT
	printf( "ERROR: %s\n", msg.c_str() );
#endif

#ifdef WIN32_MESSAGEBOX_OUTPUT
	int ret = MessageBoxA( 0, msg.c_str(), "C++ Exception Generated!", MB_ABORTRETRYIGNORE );
	if ( IDABORT == ret )
		exit(-1);
	return;
#endif

#ifdef FPRINTF_OUTPUT_AND_TERMINATE
	FILE* fh = fopen( "C:\\error.txt", "wt" );
	fprintf( fh, "ERROR: %s\n", msg.c_str() );
	printf( "ERROR: %s\n", msg.c_str() );
	fclose( fh );	
	assert(false && "Exception generated! - Read more information: 'C:\\error.txt'");
	for (;;) {}
#endif

	assert( msg == "Exception generated! (now stuck in Throwable::showExceptionDialog)" );
	for (;;) {}
}


END_NAMESPACE() // lang
