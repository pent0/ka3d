#include <hgr/UserPropertySet.h>
#include <io/PropertyParser.h>
#include <lang/Exception.h>
#include <string.h>
#include <config.h>


USING_NAMESPACE(io)
USING_NAMESPACE(lang)


BEGIN_NAMESPACE(hgr) 


UserPropertySet::UserPropertySet()
{
}

UserPropertySet::UserPropertySet( int n ) :
	Hashtable< String, String, Hash<String> >( n )
{
}

void UserPropertySet::check( const char** keylist, int count )
{
	PropertyParser parser;
	for ( HashtableIterator<String,String> it = begin() ; it != end() ; ++it )
	{
		parser.reset( it.value(), it.key() );

		for ( PropertyParser::ConstIterator i = parser.begin() ; i != parser.end() ; ++i )
		{
			bool found = false;
			for ( int k = 0 ; k < count ; ++k )
			{
				if ( !strcmp(keylist[k],i.key()) )
				{
					found = true;
					break;
				}
			}

			if ( !found )
				throwError( Exception( Format("\"{0}\" has unsupported type of property: {1}={2}", it.key(), i.key(), i.value()) ) );
		}
	}
}


END_NAMESPACE() // hgr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
