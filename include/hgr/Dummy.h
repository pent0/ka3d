#ifndef _HGR_DUMMY_H
#define _HGR_DUMMY_H


#include <hgr/Node.h>


BEGIN_NAMESPACE(hgr) 


/**
 * Dummy object for storing non-rendering related properties.
 * 
 * @ingroup hgr
 */
class Dummy : 
	public Node
{
public:
	/** Creates default dummy object. */
	Dummy();

	/** Create a value copy of this object. */
	Dummy( const Dummy& other );

	///
	~Dummy();

	/**
	 * Returns clone of this node. Clones child nodes as well.
	 */
	virtual Node*	clone() const;

	/** Sets box dimensions for this dummy object. Default is (0,0,0), (0,0,0). */
	void	setBox( const NS(math,float3)& boxmin, const NS(math,float3)& boxmax );

	/** Returns box minimum corner for this object. Default is (0,0,0). */
	const NS(math,float3)&	boxMin() const			{return m_boxMin;}

	/** Returns box minimum corner for this object. Default is (0,0,0). */
	const NS(math,float3)&	boxMax() const			{return m_boxMax;}

private:
	NS(math,float3)			m_boxMin;
	NS(math,float3)			m_boxMax;

	Dummy& operator=( const Dummy& other );
};


END_NAMESPACE() // hgr


#endif // _HGR_DUMMY_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
