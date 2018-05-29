// Note: In actual application you should
// include individual headers for optimal
// compilation performance. This example includes
// everything just for convenience.
#include <framework/App.h>
#include <io/all.h>
#include <gr/all.h>
#include <hgr/all.h>
#include <lang/all.h>
#include <math/all.h>
#include <ode/ODEWorld.h>
#include <ode/ODEObject.h>


class PhysicsApp : 
	public NS(framework,App)
{
public:
	PhysicsApp( NS(framework,OSInterface)* os, NS(gr,Context)* context );

	~PhysicsApp();
	
	void	keyDown( KeyType key );

	void	update( float dt, NS(gr,Context)* context );

private:
	float				m_timeToUpdate;
	P(NS(gr,Context))		m_context;
	P(NS(hgr,Scene))		m_scene;
	P(NS(hgr,Camera))		m_camera;
	P(hgr::Console)		m_console;

	P(ode::ODEWorld)				m_world;
	NS(lang,Array)<P(ode::ODEObject)>	m_objects;

	void		simulate( float dt );
	void		render( NS(gr,Context)* context, float fps );
	void		renderDebugInfo( NS(gr,Context)* context, float fps );
	void		swapBackBuffer( NS(gr,Context)* context );

	void		restart();
	void		initSim();
	static void collisionCheckCallback( void* data, dGeomID o1, dGeomID o2 );

	PhysicsApp( const PhysicsApp& );
	PhysicsApp& operator=( const PhysicsApp& );
};

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
