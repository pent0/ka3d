#include <ode/ODEWorld.h>
#include <ode/ODEDefaultCollisionChecker.h>
#include <ode/ode.h>
#include <lang/Math.h>
#include <math/float3.h>
#include <config.h>


USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(ode) 


ODEWorld::ODEWorld( float groundlevel ) :
	m_world( 0 ),
	m_space( 0 ),
	m_contacts( 0 ),
	m_ground( 0 )
{
	m_world = dWorldCreate();
	setGravity( float3(0,-9.8f,0) );
	dWorldSetAutoDisableFlag( m_world, 1 );
	dWorldSetAutoDisableLinearThreshold( m_world, 0.01f );
	dWorldSetAutoDisableAngularThreshold( m_world, Math::toRadians(1.f) );
	dWorldSetContactMaxCorrectingVel( m_world, 100.f );
	dWorldSetContactSurfaceLayer( m_world, 0.01f );
	dWorldSetERP( m_world, 0.2f );

	m_space = dHashSpaceCreate(0);
	dSpaceSetCleanup( m_space, 0 );

	m_contacts = dJointGroupCreate(0);

	// create bottom plane to stop objects falling infinitely
	m_ground = dCreatePlane( m_space, 0,1,0, groundlevel );
}

ODEWorld::~ODEWorld()
{
	if ( m_ground )
	{
		dGeomDestroy( m_ground );
		m_ground = 0;
	}

	if ( m_contacts )
	{
		dJointGroupDestroy( m_contacts );
		m_contacts = 0;
	}

	if ( m_space )
	{
		dSpaceDestroy( m_space );
		m_space = 0;
	}

	if ( m_world )
	{
		dWorldDestroy( m_world );
		m_world = 0;
	}
}

void ODEWorld::setGravity( const float3& f )
{
	dWorldSetGravity( m_world, f.x, f.y, f.z );
}

void ODEWorld::step( float dt, ODECollisionInterface* checker )
{
	// use default collision checker if needed
	ODEDefaultCollisionChecker defaultcollisionchecker;
	if ( !checker )
		checker = &defaultcollisionchecker;

	// collect contacts
	dJointGroupEmpty( m_contacts );
	CollisionCallbackProxyData data;
	data.world = this;
	data.checker = checker;
	dSpaceCollide( m_space, &data, collisionCallbackProxy );

	// simulate step
	dWorldQuickStep( m_world, dt );
	//dWorldStep( m_world, dt );
}

dSpaceID ODEWorld::space() const
{
	return m_space;
}

dWorldID ODEWorld::world() const
{
	return m_world;
}

dJointGroupID ODEWorld::contacts() const		
{
	return m_contacts;
}

int ODEWorld::enabledBodies() const
{
	int bodies = 0;
	int geoms = dSpaceGetNumGeoms( m_space );
	for ( int i = 0 ; i < geoms ; ++i )
	{
		dGeomID geom = dSpaceGetGeom( m_space, i );
		dBodyID body = dGeomGetBody( geom );
		if ( body != 0 && dBodyIsEnabled(body) )
			++bodies;
	}
	return bodies;
}

void ODEWorld::collisionCallbackProxy( void* data, dGeomID o1, dGeomID o2 )
{
	// exit without doing anything if the two bodies are connected by a joint
	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);
	if ( b1 && b2 && dAreConnectedExcluding(b1,b2,dJointTypeContact) )
		return;

	// exit if both bodies are disabled
	if ( (!b1 || !dBodyIsEnabled(b1)) && (!b2 || !dBodyIsEnabled(b2)) )
		return;

	// ask contacts from the callback
	CollisionCallbackProxyData* proxydata = reinterpret_cast<CollisionCallbackProxyData*>( data );
	ODEWorld* world = proxydata->world;
	int numc = proxydata->checker->checkCollisions( o1, o2, proxydata->contacts, MAX_CONTACTS );

	// add contacts to contact joint group
	for ( int i = 0 ; i < numc ; ++i )
	{
		dJointID c = dJointCreateContact( world->m_world, world->m_contacts, &proxydata->contacts[i] );
		dJointAttach( c, b1, b2 );
	}
}


END_NAMESPACE() // ode

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
