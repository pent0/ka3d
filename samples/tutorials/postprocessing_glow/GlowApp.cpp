//
// Tutorial: Multipass rendering and glow post-processing effect
// (and recording video as image sequence)
//
// Tested on:
// - WinXP/DX9/Athlon XP 2700+/ATI Radeon 9600
//
#include "GlowApp.h"
#include <io/PathName.h>
#include <io/FileOutputStream.h>
#include <lang/all.h>
#include <math/all.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(io)
USING_NAMESPACE(hgr)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)
USING_NAMESPACE(framework)


GlowApp::GlowApp( NS(framework,OSInterface)* os, Context* context ) :
	App( os ),
	m_time( 0 ),
	m_capture( false )
{
	// init rendering pipes (starting with shared pipeline setup)
	m_pipeSetup = new PipeSetup( context );
	m_glowPipe = new GlowPipe( m_pipeSetup );
	m_defaultPipe = new DefaultPipe( m_pipeSetup );

	// init scene, load textures from same directory as the scene
	m_scene = new Scene( context, "data/zax.hgr" );
	m_camera = m_scene->camera();

	// load camera from different scene
	P(Scene) camerascene = new Scene( context, "data/camera.hgr" );
	m_camera = camerascene->camera();
	m_scene->merge( camerascene );
}

void GlowApp::update( float dt, Context* context )
{
	if ( m_capture )
		dt = 1.f/30.f;

	// update animations
	m_time += dt;
	m_scene->applyAnimations( m_time, dt );

	// render frame
	{
		Context::RenderScene rs( context );

		// prepare scene for rendering
		m_pipeSetup->setup( m_camera );

		// render using the pipes, first normal, then add glow
		m_defaultPipe->render( 0, context, m_scene, m_camera );
		m_glowPipe->render( 0, context, m_scene, m_camera );
	}

	// store animation if requested
	if ( m_capture )
		context->capture( "/zax%04d.jpg" );

	// flip back buffer
	context->present();
}

void GlowApp::keyDown( KeyType key )
{
	if ( KEY_F12 == key )
		m_capture = !m_capture;
}

void framework::configure( App::Configuration& config )
{
	// accept default initialization parameters
	config.name = "Tutorial: Multipass rendering and glow post-processing effect";
}

NS(framework,App)* framework::init( NS(framework,OSInterface)* os, Context* context )
{
	return new GlowApp( os, context );
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
