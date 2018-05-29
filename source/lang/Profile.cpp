#ifndef __SYMBIAN32__
#ifndef _WIN32_WCE
#include <lang/Profile.h>
#include <lang/Exception.h>
#include <string.h>
#include <config.h>


BEGIN_NAMESPACE(lang) 


int		Profile::sm_id = 0;
bool	Profile::sm_init = false;
rdtsc_t	Profile::sm_time = 0;
double	Profile::sm_scale = 0;
rdtsc_t	Profile::sm_times[MAX_BLOCKS];
int		Profile::sm_counts[MAX_BLOCKS];
char	Profile::sm_names[MAX_BLOCKS][MAX_NAME];


void Profile::setName( int id, const char* sz )
{
	assert( id >= 0 && id < int(sizeof(sm_names)/sizeof(sm_names[0])) );

	if ( !sm_init )
		init();

	strncpy( sm_names[id], sz, MAX_NAME-1 );
	sm_names[id][MAX_NAME-1] = 0;
}

void Profile::beginFrame()
{
	if ( !sm_init )
		init();

	sm_time = rdtsc();
}

void Profile::endFrame()
{
	if ( !sm_init )
		init();

	rdtsc_t dt = rdtsc() - sm_time;
	sm_scale = 100.0 / double(dt);
}

int	Profile::getCount( int i )
{
	return sm_counts[indexToId(i)];
}

const char*	Profile::getName( int i )
{
	return sm_names[indexToId(i)];
}

float Profile::getPercent( int i )
{
	return (float)( double(sm_times[ indexToId(i) ]) * sm_scale );
}

int Profile::blocks()
{
	int count = 0;
	for ( int k = 0 ; k < MAX_BLOCKS ; ++k )
		if ( sm_names[k][0] != 0 )
			++count;
	return count;
}

void Profile::reset()
{
	memset( sm_times, 0, sizeof(sm_times) );
	memset( sm_counts, 0, sizeof(sm_counts) );
}

int Profile::id()
{
	assert( sm_id >= 0 && sm_id < MAX_BLOCKS );
	return sm_id++;
}

void Profile::init()
{
	try
	{
		rdtsc();
		sm_init = true;
	}
	catch ( ... )
	{
		throwError( Exception(Format("High resolution timer not available. Re-compile application with PROFILE_ENABLED #undefined.")) );
	}
}

int Profile::indexToId( int i )
{
	assert( i >= 0 && i < MAX_BLOCKS );

	int count = 0;
	for ( int k = 0 ; k < MAX_BLOCKS ; ++k )
	{
		if ( sm_names[k][0] != 0 )
		{
			if ( count == i )
				return k;
			++count;
		}
	}

	assert( i == MAX_BLOCKS );
	return 0;
}


END_NAMESPACE() // lang

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
#endif // _WIN32_WCE
#endif // __SYMBIAN32__
