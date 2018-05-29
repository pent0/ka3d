#include <lang/all.h> 
#include <stdio.h>
#include <config.h>


BEGIN_NAMESPACE(lang) 


class TestItem : public SingleLinkedListItem<TestItem> 
{
public: 
	TestItem() {}

private:
	TestItem( const TestItem& );
	TestItem& operator=( const TestItem& );
};


static void run()
{
	// Array test
	{
		Array<int> x1;
		for ( int i = 0 ; i < 3 ; ++i )
			x1.add( i*2 );
		// x1 = {0,2,4}

		Array<int> x2;
		x2.add( 4 );
		// x2 = {4}
		assert( x2.size() == 1 );
		assert( x2[0] == 4 );
		
		x2 = x1;
		x2.add( 1, 99 );
		// x2 = {0,99,2,4}
		assert( x2[0] == 0 );
		assert( x2[1] == 99 );
		assert( x2[2] == 2 );
		assert( x2[3] == 4 );

		assert( x2.size() == 4 );
		x2.remove( 1 );
		assert( x2.size() == 3 );
		assert( x2[0] == 0 );
		assert( x2[1] == 2 );
		assert( x2[2] == 4 );

		assert( x2.size() == 3 );
		x2.remove( 0 );
		assert( x2.size() == 2 );
		assert( x2[0] == 2 );
		assert( x2[1] == 4 );

		assert( x2.size() == 2 );
		x2.remove( 1 );
		assert( x2.size() == 1 );
		assert( x2[0] == 2 );
	}
	
	// String test
	{
		String a = "hello";
		String b = "world";
		assert( b.length() == 5 );
		String c = a + ", " + b;
		String d = "to be overriden";
		d = c;
		assert( d == "hello, world" );
		assert( d.charAt(5) == ',' );
		
		const char* xa = "Hello, world!";
		const char* xa2 = " \n Hello, world! \n "; xa2=xa2;
		String x = xa; x=x;
		String xs = x.substring(7,12); xs=xs;
		assert( xs.length() == 12-7 );
		assert( xs == "world" );
		assert( x.endsWith("world!") );
		assert( !x.endsWith("world") );
		assert( x.startsWith("Hello") );
		assert( !x.startsWith("ello") );
		assert( x.hashCode() != 0 );
		assert( 7 == x.indexOf('w') );
		assert( 8 == x.indexOf("orld") );
		assert( 8 == x.lastIndexOf('o') );
		assert( 8 == x.lastIndexOf("o") );
		assert( x.replace('l','m') == "Hemmo, wormd!" );
		assert( String(xa2).trim() == xa );
		assert( String("greenblob #f").indexOf("#f") == 10 );
		String fmtstr = Format("1,2,{0}",3).format();
		int len = fmtstr.length(); len=len;
		assert( len == 5 );
		assert( fmtstr == "1,2,3" );

		String str0( "../../data/" );
		String str1( "rgb_text-4b.bmp" );
		String str2( str0 + String("images/") + str1 );
		assert( str2 == "../../data/images/rgb_text-4b.bmp" );
	}
	
	// Format test
	{
		char buff[512];
			
		Format fmt( "Hello, world." );
		int len = fmt.format( buff, 512 );
		assert( String(buff) == "Hello, world." );
		
		fmt = Format( "1={1}, 1.23={0,#.00}, 0023={2,0000}, xyz={3}", 1.23, 1, 23, "xyz" );
		len = fmt.format( buff, 512 );
		assert( String(buff) == "1=1, 1.23=1.23, 0023=0023, xyz=xyz" );
		
		fmt = Format( "Hello, world." );
		len = fmt.format( buff, 13 );
		assert( String(buff) == "Hello, world" );
		
		fmt = Format( "1={1}, 1.23={0,#.00}, 0023={2,0000}", 1.23, 1, 23 );
		len = fmt.format( buff, 13 );
		assert( String(buff) == "1=1, 1.23=1." );
	}

	// SingleLinkedList test
	{
		SingleLinkedList<TestItem> list;
		TestItem a, b, c;

		list.insert( &a );
		assert( list.first() == &a );
		list.insert( &b );
		assert( list.first() == &b );
		list.insert( &c );
		assert( list.first() == &c );
		assert( list.first()->previous() == 0 );
		assert( list.first()->next() == &b );
		assert( list.first()->next()->next() == &a );
		list.remove( &a );
		list.remove( &b );
		list.remove( &c );
		assert( !list.first() );

		list.insert( &c );
		list.insert( &a );
		list.insert( &b );
		while ( list.last() )
			list.remove( list.last() );
		assert( !list.first() );
	}

	// Hashtable test
	{
		
	}
}

void test()
{
	String libname = "lang";

	Debug::printf( "\n-------------------------------------------------------------------------\n" );
	Debug::printf( "%s library test begin\n", libname.c_str() );
	Debug::printf( "-------------------------------------------------------------------------\n" );
	run();
	Debug::printf( "%s library test ok\n", libname.c_str() );
}


END_NAMESPACE() // lang

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
