#include <hgr/Node.h>
#include <gr/Context.h>
#include <lang/Math.h>
#include <lang/Float.h>
#include <lang/Debug.h>
#include <lang/Exception.h>
#include <math/float4x4.h>
#ifdef PLATFORM_WIN32
#include <typeinfo.h>
#endif
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(hgr) 


Node::Node() :
	m_modeltm( 1.f ),
	m_flags(NODE_DEFAULTS),
	m_tmindex(-1),
	m_parent(0),
	m_child(0),
	m_next(0),
	m_previous(0),
	m_name(),
	m_userData( 0 )
{
}

Node::Node( const Node& other ) :
	m_modeltm( other.m_modeltm ),
	m_flags( other.m_flags ),
	m_tmindex( other.m_tmindex ),
	m_parent(0),
	m_child(0),
	m_next(0),
	m_previous(0),
	m_name( other.m_name ),
	m_userData( 0 ),
	m_id( 0 )
{
	for ( Node* c = other.m_child ; c != 0 ; c = c->m_next )
	{
		P(Node) node = c->clone();
#ifdef PLATFORM_WIN32
		assert( typeid(*node.ptr()) == typeid(*c) ); // missing clone() or invalid copy ctor somewhere?
#endif
		node->linkTo( this );
	}
}

Node::~Node()
{
	unlink();

	while ( m_child != 0 )
		m_child->unlink();
}

Node* Node::clone() const
{
	return new Node( *this );
}

void Node::setTransform( const float3x4& transform )
{
	assert( transform.finite() );

	m_modeltm = transform;
}

void Node::setPosition( const float3& position )									
{
	assert( position.finite() );

	m_modeltm.setTranslation( position );
}

void Node::setRotation( const float3x3& rotation )
{
	assert( rotation.finite() );

	m_modeltm.setRotation( rotation );
}

float3x4 Node::worldTransform() const
{
	float3x4 worldtm = m_modeltm;
	for ( Node* parent = m_parent ; parent != 0 ; parent = parent->m_parent )
		worldtm = parent->m_modeltm * worldtm;
	return worldtm;
}

void Node::lookAt( const NS(math,float3)& target, const NS(math,float3)& up )
{
	assert( Math::abs(up.length()-1.f) < 1e-3f ); // Up direction must be normalized
	
	float3x4 parentworldtminverse( 1.f );
	if ( parent() )
		parentworldtminverse = parent()->worldTransform().inverse();

	float3x4 tm = worldTransform();
	float3 dir = target - tm.translation();
	float len2 = dot(dir,dir);
	if ( len2 < 1e-10f )
		return;
	dir *= 1.f/Math::sqrt(len2);
	float3x3 newrot( 1.f );
	newrot.generateOrthonormalBasisFromZ( dir, up );
	float3x4 newtm( newrot, tm.translation() );
	setTransform( parentworldtminverse * newtm );
}

void Node::lookAt( const Node* target, const float3& up )
{
	lookAt( target->worldTransform().translation(), up );
}

bool Node::hasParent( const Node* other ) const
{
	for ( const Node* parent = this->parent() ; parent != 0 ; parent = parent->parent() )
	{
		if ( other == parent )
			return true;
	}
	return false;
}

void Node::linkTo( Node* parent )
{
	assert( parent );					// parent node must exist
	assert( !parent->hasParent(this) ); // cannot handle cyclic hierarchies
	assert( parent != this );			// immediate cyclic hierarchy

	// keep reference for safety (avoid premature destruction)
	P(Node) thisnode = this;

	unlink();

	m_next = parent->m_child;
	if ( m_next ) 
		m_next->m_previous = this;

	m_parent = parent;
	m_parent->m_child = this;
}

void Node::unlink()
{
	if ( m_parent )
	{
		// keep reference for safety (avoid premature destruction)
		P(Node) thisnode = this;
		
		if ( m_parent->m_child.ptr() == this )
		{
			// this is the first child
			assert( !m_previous );
			m_parent->m_child = m_next;
			if ( m_next )
				m_next->m_previous = 0;
		}
		else
		{
			// this is not the first child
			assert( m_previous );
			if ( m_previous )
				m_previous->m_next = m_next;
			if ( m_next )
				m_next->m_previous = m_previous;
		}

		m_parent = 0;
		m_next = 0;
		m_previous = 0;
	}
}

Node* Node::root() const
{
	Node* root = const_cast<Node*>(this);
	while ( root->parent() )
		root = root->parent();
	return root;
}

Node* Node::next( const Node* root ) const
{
	assert( 0 != root ); // iteration root must be defined. use node->root() if you need to enumerate whole scene.

	if ( m_child )
		return m_child; // found child
	else if ( this == root )
		return 0;
	else if ( m_next ) 
		return m_next; // found sibling
	else 
	{
		// parent done, move to next one (unless it's given root)
		for ( Node* node = m_parent ; node && node != root ; node = node->m_parent )
			if ( node->m_next )
				return node->m_next;
	}

	return 0; // end of hierarchy
}

int Node::depth() const
{
	int d = 0;
	for ( Node* p = m_parent ; 0 != p ; p = p->m_parent )
		++d;
	return d;
}

void Node::setClassId( NodeClassId type )
{
	//assert( NODE_OTHER == classId() && "NodeClassId can be set only once per object" );
	m_flags = short( (m_flags & ~int(NODE_CLASS)) + int(type) );
}

void Node::setID( int id )
{
	m_id = id;
}

Node* Node::find( int id ) const
{
	Node* node = 0;
	Node* root = this->root();
	for ( node = root ; node != 0 ; node = node->next(root) )
		if ( node->m_id == id )
			break;

	if ( node == 0 )
		throwError( Exception( Format("Node with id \"{0}\" not found in scene \"{1}\".", id, root->name()) ) );
	return node;
}

Node* Node::getNodeByName( const String& name ) const
{
	Node* node = 0;
	Node* root = this->root();
	for ( node = root ; node != 0 ; node = node->next(root) )
		if ( node->name() == name )
			break;

	if ( node == 0 )
		throwError( Exception( Format("Node named \"{0}\" not found in scene \"{1}\".", name, root->name()) ) );
	return node;
}

Node* Node::getNodeByNameFromSubtree( const String& name )
{
	Node* node = 0;
	Node* root = this;
	for ( node = root ; node != 0 ; node = node->next(root) )
		if ( node->name() == name )
			break;

	if ( node == 0 )
		throwError( Exception( Format("Node named \"{0}\" not found in scene \"{1}\".", name, root->name()) ) );
	return node;
}

Node* Node::findNodeByName( const String& name ) const
{
	Node* node = 0;
	Node* scene = root();
	for ( node = scene ; node != 0 ; node = node->next(scene) )
		if ( node->name() == name )
			break;
	return node;
}

Node* Node::findNodeByNameFromSubtree( const String& name )
{
	Node* node = 0;
	Node* scene = this;
	for ( node = scene ; node != 0 ; node = node->next(scene) )
		if ( node->name() == name )
			break;
	return node;
}

void Node::setEnabled( bool enabled )
{
	setFlag( NODE_ENABLED, enabled );
}

void Node::setUserData( UserData* userdata )
{
	m_userData = userdata;
}

Node::UserData* Node::userData() const
{
	return m_userData;
}


END_NAMESPACE() // hgr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
