inline void GameObject::setAngularVel( const NS(math,float3)& vel )
{
	m_obj.setAngularVel( vel );
}

inline void GameObject::setLinearVel( const NS(math,float3)& vel )
{
	m_obj.setLinearVel( vel );
}

inline NS(math,float3) GameObject::linearVel() const
{
	NS(math,float3) v;
	m_obj.getLinearVel( &v );
	return v;
}

inline NS(math,float3) GameObject::angularVel() const
{
	NS(math,float3) v;
	m_obj.getAngularVel( &v );
	return v;
}

inline void GameObject::setTransform( const NS(math,float3x4)& tm )
{
	m_obj.setTransform( tm );
}

inline void GameObject::setRotation( const NS(math,float3x3)& rot )	
{
	m_obj.setRotation( rot );
}

inline void GameObject::setPosition( const NS(math,float3)& pos )			
{
	m_obj.setPosition( pos );
}

inline NS(math,float3x3) GameObject::rotation() const
{
	return m_obj.rotation();
}

inline NS(math,float3) GameObject::position() const					
{
	return m_obj.position();
}

inline NS(math,float3x4) GameObject::transform() const
{
	return m_obj.transform();
}

inline dGeomID GameObject::geom() const		
{
	return m_obj.geom();
}

inline dBodyID GameObject::body() const		
{
	return m_obj.body();
}

inline GameObject::Type GameObject::type() const
{
	return (Type)dGeomGetCategoryBits( m_obj.geom() );
}

inline bool GameObject::destroyed() const	
{
	return m_remove;
}

inline bool	GameObject::isType( Type x ) const
{
	return ( dGeomGetCategoryBits(m_obj.geom()) & int(x) ) != 0;
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
