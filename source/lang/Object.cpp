#include <lang/Object.h>
#include <config.h>
#include <lang/assert.h>

BEGIN_NAMESPACE(lang) 
	

Object::Object() :
	m_refs(0)
{
}

Object::Object( const Object& ) :
	m_refs(0)
{
}

Object::~Object()
{
	assert( 0 == m_refs ); // ref count of a stack/auto object incremented?
}

	
END_NAMESPACE() // lang

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
