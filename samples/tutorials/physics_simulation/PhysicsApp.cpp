//
// Tutorial: Simulating physics (on scene created in 3dsmax)
// (+rendering text and profiling performance)
//
#include "PhysicsApp.h"
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(io)
using namespace ode;
USING_NAMESPACE(hgr)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)
USING_NAMESPACE(framework)


PhysicsApp::PhysicsApp( NS(framework,OSInterface)* os, Context* context ) :
	App( os ),
	m_timeToUpdate( 0 ),
	m_context( context ),
	m_world( 0 )
{
	// create text output console (using Comic Sans MS font)
	m_console = new Console( context, 
		context->createTexture("data/comic_sans_ms_20x23.dds"), 20, 23 );

	restart();
}

PhysicsApp::~PhysicsApp()
{
}

void PhysicsApp::update( float dt, Context* context )
{
	Profile::beginFrame();

	// update time
	float fps = 1.f / dt;
	float fixdt = 1.f / 100.f; // simulate physics at 100Hz
	if ( isKeyDown(KEY_F4) )
	{
		dt *= .2f;
		fixdt *= .2f;
	}
	m_timeToUpdate += dt;

	m_scene->applyAnimations( m_timeToUpdate, dt );
	simulate( fixdt );
	render( context, fps );
	swapBackBuffer( context );

	Profile::endFrame();
}

void PhysicsApp::simulate( float dt )
{
	PROFILE(simulation);

	// update simulation at fixed interval
	for ( ; m_timeToUpdate > dt ; m_timeToUpdate -= dt )
		m_world->step( dt, 0 );

	// get visual object positions from the rigid bodies
	for ( int i = 0 ; i < m_objects.size() ; ++i )
		m_objects[i]->updateVisualTransform();
}

void PhysicsApp::render( Context* context, float fps )
{
	PROFILE(render);

	Context::RenderScene rs( context );
	m_camera->render( context );
	renderDebugInfo( context, fps );
}

void PhysicsApp::renderDebugInfo( Context* context, float fps )
{
	// set text console origin to top-left corner
	m_console->setPosition( float3(0,0,0) );

	// show UI
	m_console->printf( "F5 resets\n" );

	// show number of active bodies in simulation
	m_console->printf( "active bodies: %d\n", m_world->enabledBodies() );

	// render info about profiled blocks
	for ( int i = 0 ; i < Profile::blocks() ; ++i )
	{
		m_console->printf( "%-16s = %5.1f %% (x%d)\n", Profile::getName(i), 
			Profile::getPercent(i), Profile::getCount(i) );
	}
	Profile::reset();

	// render buffered text to screen
	m_console->render( context );

	// clear debug text buffers
	m_console->clear();
}

void PhysicsApp::swapBackBuffer( Context* context )
{
	PROFILE(swapBackBuffer);
	context->present();
}

void PhysicsApp::keyDown( KeyType key )
{
	if ( KEY_F5 == key )
		restart();
}

void PhysicsApp::restart()
{ 
	// (re)start time
	m_timeToUpdate = 0.f;

	// (re)load scene to be simulated
	m_scene = new Scene( m_context, "data/scene.hgr" );
	m_camera = m_scene->camera();

	// (re)start simulation
	initSim();
}

void PhysicsApp::initSim()
{
	m_objects.clear();
	m_world = new ODEWorld;

	// create simulation objects from meshes
	for ( Node* node = m_scene ; node != 0 ; node = node->next(m_scene) )
	{
		Mesh* mesh = dynamic_cast<Mesh*>( node );
		if ( mesh )
		{
			// parse properties (from 'User Defined Properties' text field)
			String props = m_scene->userProperties()->get( mesh->name() );
			ODEObject::GeomType geomtype;
			ODEObject::MassType masstype;
			float mass;
			ODEObject::parseProperties( mesh, props, 
				&geomtype, &masstype, &mass );

			// remove from animation set
			m_scene->transformAnimations()->remove( mesh->name() );

			// create rigid body with these properties
			P(ODEObject) obj = new ODEObject( m_world->world(), 
				m_world->space(), mesh, geomtype, masstype, mass );
			m_objects.add( obj );
		}
	}
}

void framework::configure( App::Configuration& config )
{
	config.name = "Tutorial: Physics Simulation";
}

App* framework::init( NS(framework,OSInterface)* os, NS(gr,Context)* context )
{
	return new PhysicsApp( os, context );
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
