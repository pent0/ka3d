inline NS(gr,Primitive)* Mesh::getPrimitive( int index ) const
{
	assert( index >= 0 && index < m_primitives.size() );
	return m_primitives[index];
}

inline int Mesh::primitives() const
{
	return m_primitives.size();
}

inline Light* Mesh::getLight( int index ) const
{
	assert( index >= 0 && index < m_lights.size() );
	return m_lights[index];
}

inline int Mesh::lights() const
{
	return m_lights.size();
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
