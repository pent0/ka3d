#include "GameObject.h"
#include "GameLevel.h"
#include <lang/Math.h>
#include <lang/Exception.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(hgr)
using namespace ode;
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


GameObject::GameObject( lua::LuaState* luastate, GameLevel* level ) :
	LuaObject( luastate ),
	m_level( level ),
	m_root( 0 ),
	m_mesh( 0 ),
	m_remove( false )
{
	registerMethod( "setMesh", this, &GameObject::setMesh );
	registerMethod( "setSkinnedMesh", this, &GameObject::setSkinnedMesh );
}

GameObject::~GameObject()
{
}

void GameObject::serialize( io::DataInputStream* in, io::DataOutputStream* out )
{
	if ( in )
	{
		float3x4 tm = transform();
		for ( int i = 0 ; i < 3 ; ++i )
			for ( int j = 0 ; j < 4 ; ++j )
				tm(i,j) = in->readFloat();
		setTransform( tm );
	}
	
	if ( out )
	{
		float3x4 tm = transform();
		for ( int i = 0 ; i < 3 ; ++i )
			for ( int j = 0 ; j < 4 ; ++j )
				out->writeFloat( tm(i,j) );
	}
}

void GameObject::update( float /*dt*/ )
{
}

void GameObject::setMesh( NS(lang,String) filename )
{
	P(Scene) scene = new Scene( context(), filename );
	scene->removeLightsAndCameras();

	// find first Mesh
	for ( Node* node = scene ; node != 0 ; node = node->next(scene) )
	{
		m_mesh = dynamic_cast<Mesh*>( node );
		if ( m_mesh != 0 )
			break;
	}
	if ( !m_mesh )
		throwError( Exception(Format("Scene {0} needs to have at least one triangle mesh", scene->name())) );
	assert( m_mesh );

	m_root = scene.ptr();
}

void GameObject::setSkinnedMesh( NS(lang,String) filename )
{
	// setup character mesh and bone hierarchy
	P(Scene) scene = new Scene( context(), filename );
	scene->removeLightsAndCameras();

	// find first Mesh
	for ( Node* node = scene ; node != 0 ; node = node->next(scene) )
	{
		m_mesh = dynamic_cast<Mesh*>( node );
		if ( m_mesh != 0 && m_mesh->bones() > 0 )
			break;
	}
	if ( !m_mesh )
		throwError( Exception(Format("Scene {0} needs to have at least one skinned triangle mesh", scene->name())) );
	assert( m_mesh );

	m_root = scene.ptr();
}

int GameObject::collided( GameObject* /*obj*/, dContact* /*contacts*/, int numc )
{
	return numc;
}

void GameObject::getObjectsToRender( GameCamera* /*camera*/ )
{
}

void GameObject::addImpulse( const float3& imp, float dt )
{
	m_obj.addImpulse( m_level->world(), imp, dt );
}

void GameObject::addImpulseAt( const float3& point, const float3& imp, float dt )
{
	m_obj.addImpulseAt( m_level->world(), point, imp, dt );
}

float GameObject::mass() const
{
	dMass m;
	dBodyGetMass( body(), &m );
	return m.mass;
}

void GameObject::disable()
{
	dGeomID geom = m_obj.geom();
	if ( geom )
	{
		dGeomDisable( geom );

		dBodyID body = dGeomGetBody( geom );
		if ( body )
			dBodyDisable( body );
	}
}

void GameObject::enable()
{
	dGeomID geom = m_obj.geom();
	if ( geom )
	{
		dGeomEnable( geom );

		dBodyID body = dGeomGetBody( geom );
		if ( body )
			dBodyEnable( body );
	}
}

void GameObject::setGeom( dGeomID geom, int category, int collides, const dMass* m )
{
	assert( geom );
	assert( m );

	dBodyID body = dBodyCreate( m_level->world() );
	dBodySetAutoDisableFlag( body, 0 );
	dBodySetMass( body, m );
	dGeomSetBody( geom, body );

	dGeomSetData( geom, this );
	dGeomSetCategoryBits( geom, category );
	dGeomSetCollideBits( geom, collides );
	m_obj.create( geom );
}

void GameObject::setRigidBodyFromRay( Type type, int collides, float mass )
{
	dGeomID geom = dCreateRay( m_level->space(), .1f );
	dMass m;
	dMassSetSphereTotal( &m, mass, 0.1f );
	setGeom( geom, type, collides, &m );
}

void GameObject::setRigidBodyFromCCylinderY( float r, float height, float mass, 
	Type type, int collides )
{
	const float len = height - r*2.f;

	dGeomID cyl = dCreateCCylinder( 0, r, len );
	float3x4 trans( 1.f );
	trans.setTranslation( float3(0,0,height*.5f) );
	float3x4 rot( 1.f );
	rot.setRotation( float3(1,0,0), -Math::PI*.5f );
	dGeomSetTransform( cyl, rot*trans );

	dGeomID geom = dCreateGeomTransform( m_level->space() );
	dGeomTransformSetGeom( geom, cyl );
	dGeomTransformSetCleanup( geom, 1 );

	dMass m;
	dMassSetZero( &m );
	dMassSetCappedCylinderTotal( &m, mass, 1, r, len ); // 1=Y-axis
	dMassTranslate( &m, 0,height*.5f,0 );
	
	setGeom( geom, type, collides, &m );
}

void GameObject::setRigidBodyFromBox( const float3& size, float mass, Type type, int collides )
{
	dGeomID geom = dCreateBox( m_level->space(), size.x,size.y,size.z );

	dMass m;
	dMassSetBoxTotal( &m, mass, size.x,size.y,size.z );

	setGeom( geom, type, collides, &m );
}

void GameObject::setRigidBodyFromSphere( float r, float mass, Type type, int collides )
{
	dMass m;
	dMassSetSphereTotal( &m, mass, r );

	dGeomID geom = dCreateSphere( m_level->space(), r );
	setGeom( geom, type, collides, &m );
}

void GameObject::setRigidBodyFromMesh( Mesh* mesh, ODEObject::GeomType geomtype, 
	ODEObject::MassType masstype, float mass, Type type, int collides )
{
	m_obj.create( m_level->world(), m_level->space(), mesh, geomtype, masstype, mass );

	dGeomSetData( geom(), this );
	dGeomSetCategoryBits( geom(), type );
	dGeomSetCollideBits( geom(), collides );
}

void GameObject::disableGravity()
{
	dBodySetGravityMode( body(), 0 );
}

void GameObject::setAutoDisable( bool autodisabled )
{
	dBodySetAutoDisableFlag( body(), autodisabled ? 1 : 0 );
}

bool GameObject::isOnGround() const
{
	assert( m_level );

	static const float GROUND_CONTACT_LIMIT_ANGLE = Math::sin( Math::toRadians(60.f) );

	enum { MAX_CONTACTS=16 };
	dContactGeom contacts[MAX_CONTACTS];
	memset( contacts, 0, sizeof(contacts) );

	int numc = dCollide( m_obj.geom(), m_level->geom(), MAX_CONTACTS, &contacts[0], sizeof(contacts[0]) );

	for ( int i = 0 ; i < numc ; ++i )
	{
		const dContactGeom& cg = contacts[i];
		if ( cg.normal[1] > GROUND_CONTACT_LIMIT_ANGLE )
			return true;
	}

	return false;
}

Context* GameObject::context() const
{
	assert( m_level != 0 );
	return m_level->context();
}

ResourceManager* GameObject::resourceManager() const
{
	assert( m_level != 0 );
	return m_level->resourceManager();
}

void GameObject::destroy()		
{
	m_remove = true;
}

void GameObject::setMeshTransformFromRigidBody()
{
	m_obj.updateVisualTransform();
}

void GameObject::setShaderVector( const char* param, const float4& value )
{
	for ( Node* node = m_root ; node != 0 ; node = node->next(m_root) )
	{
		Mesh* mesh = dynamic_cast<Mesh*>( node );
		if ( mesh != 0 )
		{
			for ( int i = 0 ; i < mesh->primitives() ; ++i )
				mesh->getPrimitive(i)->shader()->setVector( param, value );
		}
	}
}

void GameObject::disableRigidBody()
{
	m_obj.disableRigidBody();
}
