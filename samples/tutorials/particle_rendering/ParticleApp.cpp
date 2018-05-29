//
// Tutorial: Particle System Rendering
//
// Tested on:
// - WinXP/DX9/Athlon XP 2700+/ATI Radeon 9600
//
#include "ParticleApp.h"
#include <lang/all.h>
#include <math/all.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(io)
USING_NAMESPACE(hgr)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)
USING_NAMESPACE(framework)


ParticleApp::ParticleApp( NS(framework,OSInterface)* os, Context* context ) :
	App( os )
{
	// scene setup
	m_scene = new Scene;

	// particle system setup
	m_particleSystem = new ParticleSystem( context, "data/fire.prs" );
	m_particleSystem->linkTo( m_scene );

	// camera setup
	m_camera = new Camera;
	m_camera->setFront( 1.f );
	m_camera->setBack( 10000.f );
	m_camera->linkTo( m_scene );
	m_camera->setPosition( float3(0,30,-200) );
	m_camera->lookAt( m_particleSystem );
}

void ParticleApp::update( float dt, Context* context )
{
	// update particles
	m_particleSystem->update( dt );

	// render frame
	{
		Context::RenderScene rs( context );
		m_camera->render( context );
	}

	// flip back buffer
	context->present();
}

void framework::configure( App::Configuration& config )
{
	static const char* USED_PARTICLES[] = { "fire.prs", 0 };
	//config.usedParticles = USED_PARTICLES;
	config.name = "Tutorial: Particle System Rendering";
}

NS(framework,App)* framework::init( NS(framework,OSInterface)* os, Context* context )
{
	return new ParticleApp( os, context );
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
