#include <io/all.h> 
#include <lang/all.h>
#include <string.h>
#include <config.h>


USING_NAMESPACE(lang)


BEGIN_NAMESPACE(io) 


static void run()
{
	// test PathName
	{
		assert( String("C:") == PathName("C:/mydocs/test.doc").drive() );
		assert( String("test") == PathName("C:/mydocs/test.doc").basename() );
		assert( String(".doc") == PathName("C:/mydocs/test.doc").suffix() );
		assert( String("C:/mydocs") == PathName("C:/mydocs/test.doc").parent().toString() );
		assert( String("C:/mydocs/test.doc") == PathName("C:/mydocs","test.doc").toString() );
	}

	// test PropertyParser (1)
	{
		PropertyParser parser( "x=12\ny=23\n -- this is comment\nz=34\nw = 123 123 \n -- second comment", "test" );
		Array<char> v;
		parser.get( "x", v );
		assert( !strcmp("12",v.begin()) );
		parser.get( "y", v );
		assert( !strcmp("23",v.begin()) );
		parser.get( "z", v );
		assert( !strcmp("34",v.begin()) );
		parser.get( "w", v );
		assert( !strcmp("123 123",v.begin()) );
	}

	// test PropertyParser (2)
	{
		PropertyParser parser( "\n  \n\n\n  \n\n\n\n  ", "test" );
		PropertyParser::ConstIterator it = parser.begin();
		assert( it == parser.end() );
	}
}

void test()
{
	String libname = "io";

	Debug::printf( "\n-------------------------------------------------------------------------\n" );
	Debug::printf( "%s library test begin\n", libname.c_str() );
	Debug::printf( "-------------------------------------------------------------------------\n" );
	run();
	Debug::printf( "%s library test ok\n", libname.c_str() );
}


END_NAMESPACE() // io

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
