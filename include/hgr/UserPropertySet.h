#ifndef _HGR_USERPROPERTYSET_H
#define _HGR_USERPROPERTYSET_H


#include <lang/String.h>
#include <lang/Hashtable.h>


BEGIN_NAMESPACE(hgr) 


/**
 * Set of node name based user property strings.
 * Individual strings can be accessed by name in constant time.
 * Format of strings is not restricted in any way, but usually user
 * property strings contain set of X=... values separated
 * by newline character.
 *
 * @ingroup hgr
 */
class UserPropertySet :
	public NS(lang,Hashtable)< NS(lang,String), NS(lang,String), NS(lang,Hash)< NS(lang,String) > >
{
public:
	/**
	 * Creates empty user property set.
	 */
	UserPropertySet();

	/**
	 * Creates a set with initial space reserved for n nodes.
	 */
	explicit UserPropertySet( int n );

	/**
	 * Checks that there is no user property keys other than
	 * the ones listed. Pass the keys as lower-case.
	 * @exception Exception
	 */
	void	check( const char** keylist, int count );

private:
	UserPropertySet( const UserPropertySet& );
	UserPropertySet& operator=( const UserPropertySet& );
};


END_NAMESPACE() // hgr


#endif // _HGR_USERPROPERTYSET_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
