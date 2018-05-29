#include <hgr/DefaultPipe.h>
#include <gr/Shader.h>
#include <gr/Context.h>
#include <gr/Texture.h>
#include <hgr/Scene.h>
#include <hgr/Camera.h>
#include <hgr/PipeSetup.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(hgr) 


DefaultPipe::DefaultPipe( PipeSetup* setup, 
	const NS(lang,String)& name, int minpriority, int maxpriority ) :
	Pipe( setup ),
	m_minPriority( minpriority ),
	m_maxPriority( maxpriority )
{
	assert( name.length() < (int)sizeof(m_name) );
	name.get( m_name, sizeof(m_name) );
}

void DefaultPipe::render( Texture* target, Context* context, Scene* scene, Camera* camera  )
{
	PipeSetup* setup = this->setup();

	// set fog params
	float4 fogrange( 0,0,0,0 );
	float4 fogcolor( 0,0,0,0 );
	if ( Scene::FOG_LINEAR == scene->fogType() )
	{
		float start = scene->fogStart();
		float end = scene->fogEnd();
		fogrange = float4( start, end, 1.f/(end-start), 0 );
		fogcolor = float4( scene->fogColor(), 1.f );
	}
	Array<Shader*>& shaders = setup->shaders;
	for ( int i = 0 ; i < shaders.size() ; ++i )
	{
		Shader* shader = shaders[i];
		shader->setVector( "FOG_RANGE", fogrange );
		shader->setVector( "FOG_COLOR", fogcolor );
	}

	context->setRenderTarget( target );
	setup->setTechnique( m_name );
	camera->render( context, m_minPriority, m_maxPriority, setup->visuals, setup->priorities, &setup->lights );
}


END_NAMESPACE() // hgr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
