#ifndef _IO_COMMANDLINEPARSER_H
#define _IO_COMMANDLINEPARSER_H


#include <lang/pp.h>


BEGIN_NAMESPACE(lang) 
	class String;END_NAMESPACE()


BEGIN_NAMESPACE(io) 


/**
 * Parses main() command line arguments.
 * Speculative interface which allows 'try to read option'
 * style usage so you can try to get some command line
 * option and if the query fails you can ask it with prompt.
 * @ingroup io
 */
class CommandLineParser
{
public:
	/** 
	 * Parsest the command line if any.
	 */
	CommandLineParser( int argc, char** argv );

	/**
	 * Parses integer option. For example when command line has
	 * "-maxcount 3" you can say int x; getInt("-maxcount",&x)
	 * and x will contain number 3.
	 * @return true if query succeeds.
	 */
	bool		getInt( const char* name, int* value ) const;

	/**
	 * Parses integer option. For example when command line has
	 * "-maxcount mystr" you can say String x; getInt("-maxcount",&x)
	 * and x will contain "mystr".
	 * @return true if query succeeds.
	 */
	bool		getString( const char* name, NS(lang,String)* str ) const;

	/**
	 * Returns true if specified option exists.
	 */
	bool		hasOption( const char* name ) const;

	/**
	 * Returns number of arguments in command line.
	 */
	int			argc() const;

	/**
	 * Returns ith argument.
	 */
	const char*	getArgv( int i ) const;

private:
	int		m_argc;
	char**	m_argv;
};


END_NAMESPACE() // io


#endif // _IO_COMMANDLINEPARSER_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
