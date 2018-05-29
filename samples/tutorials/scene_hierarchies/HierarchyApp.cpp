//
// Tutorial: Transformation Hierarchies And Parenting
//
// Tested on:
// - WinXP/DX9/Athlon XP 2700+/ATI Radeon 9600
//
#include "HierarchyApp.h"
#include <io/PathName.h>
#include <lang/all.h>
#include <math/all.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(io)
USING_NAMESPACE(hgr)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)
USING_NAMESPACE(framework)


HierarchyApp::HierarchyApp( NS(framework,OSInterface)* os, Context* context ) :
	App( os ),
	m_time( 0 )
{
	// get resources
	m_scene = new Scene( context, "data/space.hgr" );
	m_camera = m_scene->camera();
	m_sun = m_scene->getNodeByName( "Sun" );
	m_earth = m_scene->getNodeByName( "Earth" );
	m_moon = m_scene->getNodeByName( "Moon" );

	// set up some hierarchies
	// (note: this hierarchy could also
	// be made already in 3dsmax, but we
	// do it here just to show how its done)
	m_sun->linkTo( m_scene );
	m_earth->linkTo( m_sun );
	m_moon->linkTo( m_earth );

	// parent simple point light to sun
	P(Light) lt = new Light;
	lt->setColor( float3(1,1,1) );
	lt->linkTo( m_sun );

	// put camera looking at sun
	// (don't try this at home)
	m_camera->lookAt( m_sun );

	// setup extra rendering pipeline for glow
	m_pipeSetup = new PipeSetup( context );
	m_defaultPipe = new DefaultPipe( m_pipeSetup );
	m_glowPipe = new GlowPipe( m_pipeSetup );
}

void HierarchyApp::update( float dt, Context* context )
{
	// update time
	m_time += dt;

	// put sun to center and rotate it about Y-axis
	float sunangle = m_time * Math::toRadians(45.f);
	m_sun->setPosition( float3(0,0,0) );
	m_sun->setRotation( float3x3(float3(0,1,0), sunangle) );

	// put earth 25 units away from sun and rotate it twice as fast
	float earthangle = m_time * Math::toRadians(90.f);
	m_earth->setPosition( float3(25,0,0) );
	m_earth->setRotation( float3x3(float3(0,1,0), earthangle) );

	// put moon 10 units away from earth and rotate it twice as fast
	float moonangle = m_time * Math::toRadians(180.f);
	m_earth->setPosition( float3(10,0,0) );
	m_earth->setRotation( float3x3(float3(0,1,0), moonangle) );

	// render frame
	{
		Context::RenderScene rs( context );
		m_pipeSetup->setup( m_camera );
		m_defaultPipe->render( 0, context, m_scene, m_camera );
		m_glowPipe->render( 0, context, m_scene, m_camera );
	}

	// flip back buffer
	//context->capture( "C:/Documents and Settings/jani/My Documents/tmp/shot%04d.png" );
	context->present();
}

void framework::configure( App::Configuration& config )
{
	//config.width = 1280;
	//config.height = 960;
	config.name = "Tutorial: Transformation Hierarchies And Parenting";
}

App* framework::init( NS(framework,OSInterface)* os, Context* context )
{
	return new HierarchyApp( os, context );
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
