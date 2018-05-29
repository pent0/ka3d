#include <lang/Debug.h>
#include <lang/pp.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#ifdef PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef PLATFORM_BREW
#include <AEEStdLib.h>
#endif

#include <config.h>


// Maximum length of a single debug message.
#define MAX_MSG_LEN 1000

// Do we want to print timestamps
//#define PRINT_TIMESTAMPS


BEGIN_NAMESPACE(lang) 


void Debug::printf( const char* fmt, ... )
{
	// format variable arguments
	const unsigned MAX_MSG = 2000;
	char msg[MAX_MSG+4];
	char* userMsgStart = msg;

#ifdef PRINT_TIMESTAMPS
	static bool lastPrintEndedWithNewLine = true;
	if(lastPrintEndedWithNewLine) {
		time_t t = time( 0 );							// Get current time in seconds from Epoc	
		struct tm* timeptr = localtime( &t );			// Fill tm struct
		char* asc_time = asctime( timeptr );			// Convert to string

		int timeStrLenth = strlen( asc_time );
		memcpy(msg, asc_time, timeStrLenth - 1);		// Copy timestring without ending "\n\0"
		msg[timeStrLenth - 1] = ':';					// Add ':' for formatting
		msg[timeStrLenth] = ' ';						// Add ' ' for formatting
		userMsgStart = msg + timeStrLenth + 1;			// Calculate pointer for print message
	}
#endif

	va_list marker;
	va_start( marker, fmt );
	vsprintf( userMsgStart, fmt, marker );
	va_end( marker );
	
	assert( strlen(msg) < MAX_MSG ); // too long debug message

#ifdef PRINT_TIMESTAMPS
	lastPrintEndedWithNewLine = msg[strlen(msg) - 1] == '\n';
#endif

#if defined(PLATFORM_NGI)

	FILE* fh = fopen( "C:\\NGI_log.txt", "at" );
	if ( fh != 0 )
	{
		fprintf( fh, "%s", msg );
		fclose( fh );
		//::printf( "%s", msg );	
	}


#elif defined(PLATFORM_S60_1X_2X) || defined(PLATFORM_S60_3X)
	
	FILE* fh = fopen( "E:\\log.txt", "at" );
	if ( fh != 0 )
	{
		fprintf( fh, "%s", msg );
		fclose( fh );
	}

#elif defined(PLATFORM_WIN32)

	OutputDebugStringA( msg );

#elif defined(PLATFORM_WINCE)
	
	FILE* fh = fopen( "log.txt", "at" );
	if ( fh != 0 )
	{
		fprintf( fh, "%s", msg );
		fclose( fh );
	}
	::printf( msg );

#elif defined(PLATFORM_BREW)

	int msglen = strlen(msg);
	if ( msglen > 2 && msg[msglen-1] == 10 ) // DBGPRINTF adds newline
		msg[msglen-1] = 0;
	DBGPRINTF( msg );

#else

	::printf( msg );

#endif
}

const char*	Debug::stripSourceFilename( const char* fname )
{
	const char* sz = strrchr( fname, '/' );
	if ( sz != 0 )
	{
		while ( sz > fname )
		{
			--sz;
			if ( *sz == '/' )
			{
				++sz;
				break;
			}
		}
		return sz;
	}

	return fname;
}


END_NAMESPACE() // lang

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
