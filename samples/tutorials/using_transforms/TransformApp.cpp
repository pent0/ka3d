//
// Tutorial: Using transformations and low level rendering
//
// Tested on:
// - WinXP/DX9/Athlon XP 2700+/ATI Radeon 9600
// - Playstation 2 Tool
//
#include "TransformApp.h"
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)
USING_NAMESPACE(framework)


TransformApp::TransformApp( NS(framework,OSInterface)* os, Context* context ) :
	App( os ),
	m_time( 0 )
{
	// texture setup
	P(Texture) tex = context->createTexture( "data/rgb.bmp" );

	// shader setup
	P(Shader) fx = context->createShader( "diff-tex" );
	fx->setTexture( "BASEMAP", tex );
	fx->setVector( "AMBIENTC", float4(0.1f,0.1f,0.1f,0.1f) );
	fx->setVector( "DIFFUSEC", float4(1.f,1.f,1.f,1.f) );

	// geometry primitive data
	const int VERTS = 4;
	const int INDICES = 6;

	float4 vertpos[VERTS] = {
		float4(-100,100,0,1),
		float4(100,100,0,1),
		float4(100,-100,0,1),
		float4(-100,-100,0,1) };

	float4 vertnorm[VERTS] = {
		float4(0,0,-1,0),
		float4(0,0,-1,0),
		float4(0,0,-1,0),
		float4(0,0,-1,0) };

	float4 vertuv[VERTS] = {
		float4(0,0,0,0),
		float4(1,0,0,0),
		float4(1,1,0,0),
		float4(0,1,0,0) };

	int indices[INDICES] = {
		0,1,2,
		0,2,3 };

	// create primitive with our vertex format
	VertexFormat vf;
	vf.addPosition();
	vf.addNormal();
	vf.addTextureCoordinate( VertexFormat::DF_V2_32 );
	m_prim = context->createPrimitive( Primitive::PRIM_TRI, vf, VERTS, INDICES );

	// lock primitive & set data
	Primitive::Lock lock( m_prim, Primitive::LOCK_WRITE );
	m_prim->setVertexPositions( 0, vertpos, VERTS );
	m_prim->setVertexNormals( 0, vertnorm, VERTS );
	m_prim->setVertexTextureCoordinates( 0, 0, vertuv, VERTS );
	m_prim->setIndices( 0, indices, INDICES );
	m_prim->setShader( fx );
}

void TransformApp::update( float dt, Context* context )
{
	// setup animated angle
	m_time += dt;
	float angle = m_time * Math::PI;

	// setup translation in world space
	float3x4 worldtm( 1.f );
	float movedistance = 100.f;
	float x = Math::sin(angle) * movedistance;
	worldtm.setTranslation( float3(x,0,0) );

	// setup rotation in world space
	worldtm.setRotation( float3x3( float3(0,0,1), angle ) );

	// setup camera world space transform
	float3 camerapos = float3(100,200,-400);
	float3x3 camerarot( float3(1,0,0), Math::toRadians(30.f) );
	float3x4 cameraworldtm( camerarot, camerapos );

	// setup view transform, which transforms
	// world space to camera space
	float3x4 viewtm = cameraworldtm.inverse();

	// calculate transformation from model space to camera space
	float3x4 modelview = viewtm * worldtm;

	// setup view->projection transform
	float horzfov = Math::PI/180.f * 90.f;
	float farz = 10000.f;
	float nearz = 0.10f;
	float4x4 projtm;
	context->setPerspectiveProjection( horzfov, nearz, farz, context->aspect() );

	// setup total (model->screen) transform
	// (note: 'screen space' in this context is platform dependent)
	float4x4 totaltm = context->projectionTransform() * modelview;

	// render frame
	{
		Context::RenderScene rs( context );
		Shader* fx = m_prim->shader();
		Shader::Begin begin( fx );

		// setup dynamic shader parameters
		fx->setMatrix( Shader::PARAM_TOTALTM, totaltm );
		fx->setMatrix( Shader::PARAM_WORLDTM, worldtm );
		fx->setMatrix( Shader::PARAM_VIEWTM, viewtm );
		fx->setVector( Shader::PARAM_CAMERAP, float4(camerapos,1.f) );
		fx->setVector( Shader::PARAM_LIGHTP0, float4(camerapos,1.f) );
		fx->setVector( Shader::PARAM_LIGHTC0, float4(.7f,1,.7f,1) );

		for ( int i = 0 ; i < begin.passes() ; ++i )
		{
			Shader::Pass pass( fx, i );
			m_prim->render();
		}
	}

	// flip back buffer
	context->present();
}

void framework::configure( App::Configuration& config )
{
	config.name = "Tutorial: Using transformations";
}

App* framework::init( NS(framework,OSInterface)* os, NS(gr,Context)* context )
{
	return new TransformApp( os, context );
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
