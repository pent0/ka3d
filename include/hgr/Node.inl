inline Node* Node::parent() const
{
	return m_parent;
}
	
inline Node* Node::firstChild() const
{
	return m_child;
}
	
inline Node* Node::getNextChild( const Node* child ) const
{
	assert( child->m_parent == this );
	return child->m_next;
}

inline const NS(math,float3x4)& Node::transform() const
{
	return m_modeltm;
}

inline void Node::setName( const NS(lang,String)& name )
{
	m_name = name;
}

inline const NS(lang,String)& Node::name() const
{
	return m_name;
}

inline NS(math,float3x3) Node::rotation() const
{
	return m_modeltm.rotation();
}

inline NS(math,float3) Node::position() const
{
	return m_modeltm.translation();
}

inline Node::NodeClassId Node::classId() const		
{
	return NodeClassId(m_flags & int(NODE_CLASS));
}

inline void Node::setFlags( int flags )			
{
	m_flags = (short)flags;
}

inline void Node::setFlag( NodeFlags flag, bool enabled )
{
	if ( enabled )
		m_flags = (short)( int(m_flags) | flag );
	else
		m_flags = (short)( int(m_flags) & ~flag );
}

inline bool Node::getFlag( NodeFlags flag ) const
{
	return 0 != (flag & int(m_flags));
}

inline int Node::flags() const					
{
	return m_flags;
}

inline bool Node::enabled() const
{
	return getFlag(NODE_ENABLED);
}

inline bool Node::isVisualNode() const	
{
	NodeClassId id = classId(); 
	return id >= NODE_FIRST_VISUAL && id <= NODE_LAST_VISUAL;
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
