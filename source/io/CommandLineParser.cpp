#include <io/CommandLineParser.h>
#include <lang/String.h>
#include <stdio.h>
#include <string.h>
#include <config.h>


USING_NAMESPACE(lang)


BEGIN_NAMESPACE(io) 


CommandLineParser::CommandLineParser( int argc, char** argv ) :
	m_argc(argc),
	m_argv(argv)
{
}

bool CommandLineParser::getInt( const char* name, int* value ) const
{
	for ( int i = 1 ; i < m_argc-1 ; ++i )
	{
		if ( !strcmp(m_argv[i],name) )
			return 1 == sscanf( m_argv[i+1], "%d", value );
	}
	return false;
}

bool CommandLineParser::getString( const char* name, String* str ) const
{
	for ( int i = 1 ; i < m_argc-1 ; ++i )
	{
		if ( !strcmp(m_argv[i],name) )
		{
			*str = m_argv[i+1];
			return true;
		}
	}
	return false;
}

bool CommandLineParser::hasOption( const char* name ) const
{
	for ( int i = 1 ; i < m_argc ; ++i )
	{
		if ( !strcmp(m_argv[i],name) )
			return true;
	}
	return false;
}

int	CommandLineParser::argc() const
{
	return m_argc;
}

const char* CommandLineParser::getArgv( int i ) const
{
	assert( i >= 0 && i < m_argc );
	return m_argv[i];
}


END_NAMESPACE() // io

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
