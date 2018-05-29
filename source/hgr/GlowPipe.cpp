#include <hgr/GlowPipe.h>
#include <gr/Shader.h>
#include <gr/Context.h>
#include <gr/Texture.h>
#include <hgr/Scene.h>
#include <hgr/Camera.h>
#include <hgr/PipeSetup.h>
#include <config.h>


USING_NAMESPACE(gr)


BEGIN_NAMESPACE(hgr) 


GlowPipe::GlowPipe( PipeSetup* setup ) :
	Pipe( setup )
{
	Context* context = setup->context();

	int rttsize = 256;
	m_rtt = context->createTexture( rttsize, rttsize, SurfaceFormat::SURFACE_A8R8G8B8, 0, Context::USAGE_RENDERTARGET );
	m_rttBlurH = context->createTexture( rttsize, rttsize, SurfaceFormat::SURFACE_A8R8G8B8, 0, Context::USAGE_RENDERTARGET );
	m_rttNewBlur = context->createTexture( rttsize, rttsize, SurfaceFormat::SURFACE_A8R8G8B8, 0, Context::USAGE_RENDERTARGET );
	m_rttOldBlur = context->createTexture( rttsize, rttsize, SurfaceFormat::SURFACE_A8R8G8B8, 0, Context::USAGE_RENDERTARGET );
	m_rttBlur = context->createTexture( rttsize, rttsize, SurfaceFormat::SURFACE_A8R8G8B8, 0, Context::USAGE_RENDERTARGET );
	m_add = context->createShader("pp-add");
	m_trail = context->createShader("pp-trail");
	m_mulA = context->createShader("pp-mula");
	m_blurH = context->createShader("pp-blurh");
	m_blurV = context->createShader("pp-blurv");
	m_overlay = createOverlayPrimitive( context, rttsize, rttsize );
}

void GlowPipe::render( Texture* target, Context* context, Scene* /*scene*/, Camera* camera  )
{
	PipeSetup* setup = this->setup();

	// render scene to texture 
	context->setRenderTarget( m_rtt );
	context->clear();
	setup->setTechnique( "GLOW" );
	camera->render( context, -1, 100, setup->visuals, setup->priorities, &setup->lights );

	// horizontal blur (to texture)
	context->setRenderTarget( m_rttBlurH );
	renderOverlay( m_overlay, m_blurH, m_rtt );

	// vertical blur (to texture)
	context->setRenderTarget( m_rttNewBlur );
	renderOverlay( m_overlay, m_blurV, m_rttBlurH );

	// combine faded old blur to the new blur (to texture)
	context->setRenderTarget( m_rttBlur );
	m_trail->setTexture( "BASEMAP1", m_rttNewBlur );
	m_trail->setTexture( "BASEMAP2", m_rttOldBlur );
	renderOverlay( m_overlay, m_trail );

	// add blur to actual target
	context->setRenderTarget( target );
	renderOverlay( m_overlay, m_add, m_rttBlur );

	// swap current blur as next old blur
	P(Texture) tmp = m_rttOldBlur;
	m_rttOldBlur = m_rttBlur;
	m_rttBlur = tmp;
}


END_NAMESPACE() // hgr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
