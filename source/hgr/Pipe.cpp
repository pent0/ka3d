#include <hgr/Pipe.h>
#include <gr/Shader.h>
#include <gr/Context.h>
#include <gr/Texture.h>
#include <hgr/PipeSetup.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(hgr) 


Pipe::Pipe( PipeSetup* setup ) :
	m_setup( setup )
{
}

Pipe::~Pipe()
{
}

PipeSetup* Pipe::setup() const
{
	assert( m_setup->valid() ); // PipeSetup::setup() not called in this frame
	return m_setup;
}

P(Primitive) Pipe::createOverlayPrimitive( Context* context, int width, int height )
{
	assert( width > 0 );
	assert( height > 0 );

	// primitive setup
	VertexFormat vf;
	vf.addPosition( VertexFormat::DF_V3_32 );
	vf.addTextureCoordinate( VertexFormat::DF_V2_32 );
	const int VERTS = 4;
	P(Primitive) prim = context->createPrimitive( Primitive::PRIM_TRISTRIP, vf, VERTS, 0 );
	
	const float smallz = 0.001f;
	float4 vertpos[VERTS] = {
		float4(-1.f,1.f,		smallz,1),
		float4(1.f,1.f,		smallz,1),
		float4(-1.f,-1.f,		smallz,1),
		float4(1.f,-1.f,		smallz,1) };

	float4 offset( .5f/float(width), .5f/float(height), 0, 0 );
	float4 vertuv[VERTS] = {
		float4(0,0,0,0)+offset,
		float4(1,0,0,0)+offset,
		float4(0,1,0,0)+offset,
		float4(1,1,0,0)+offset };

	{
		Primitive::Lock lock( prim, Primitive::LOCK_WRITE );
		prim->setVertexPositions( 0, vertpos, VERTS );
		prim->setVertexTextureCoordinates( 0, 0, vertuv, VERTS );
	}

	return prim;
}

void Pipe::renderOverlay( Primitive* overlay, Shader* shader, Texture* basetex )
{
	if ( basetex != 0 )
		shader->setTexture( "BASEMAP", basetex );

	Shader::Begin use( shader );
	for ( int i = 0 ; i < use.passes() ; ++i )
	{
		Shader::Pass pass( shader, i );
		overlay->render();
	}
}


END_NAMESPACE() // hgr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
