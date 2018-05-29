#include <hgr/Dummy.h>
#include <config.h>


USING_NAMESPACE(math)


BEGIN_NAMESPACE(hgr) 


Dummy::Dummy() :
	m_boxMin(0,0,0),
	m_boxMax(0,0,0)
{
	setClassId( NODE_DUMMY );
}

Dummy::~Dummy()
{
}

Dummy::Dummy( const Dummy& other ) :
	Node(other),
	m_boxMin( other.m_boxMin ),
	m_boxMax( other.m_boxMax )
{
	setClassId( NODE_DUMMY );
}

Node* Dummy::clone() const
{
	return new Dummy( *this );
}

void Dummy::setBox( const float3& boxmin, const float3& boxmax )
{
	m_boxMin = boxmin;
	m_boxMax = boxmax;
}


END_NAMESPACE() // hgr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
