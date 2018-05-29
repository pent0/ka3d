#include <hgr/PipeSetup.h>
#include <gr/Shader.h>
#include <gr/Context.h>
#include <hgr/Mesh.h>
#include <hgr/Visual.h>
#include <hgr/Camera.h>
#include <lang/algorithm/sort.h>
#include <lang/algorithm/unique.h>
#include <lang/algorithm/greater.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(math)
USING_NAMESPACE(lang)


BEGIN_NAMESPACE(hgr) 


PipeSetup::PipeSetup( Context* context ) :
	m_frameCounter( -1 ),
	m_context( context )
{
}

PipeSetup::~PipeSetup()
{
}

void PipeSetup::setup( Camera* camera )
{
	getNodes( camera, nodes );
	getLights( nodes, lights );
	camera->cacheTransforms( m_context, nodes );
	camera->cullVisuals( nodes, visuals );
	getShaders( visuals, shaders );
	getPriorities( shaders, priorities );

	m_frameCounter = m_context->statistics.renderedFrames;
}

void PipeSetup::setTechnique( const char* name )
{
	assert( valid() ); // setup() not called in the same frame

	for ( int i = 0 ; i < shaders.size() ; ++i )
		shaders[i]->setTechnique( name );
}

Context* PipeSetup::context() const
{
	return m_context;
}

bool PipeSetup::valid() const
{
	return m_frameCounter == m_context->statistics.renderedFrames;
}

void PipeSetup::getNodes( Node* scene, Array<Node*>& nodes )
{
	nodes.clear();
	scene = scene->root();
	for ( Node* node = scene ; node != 0 ; node = node->next(scene) )
	{
#if defined(_DEBUG) && defined(PLATFORM_WIN32)
		if ( node->classId() == Node::NODE_MESH )
		{
			assert( dynamic_cast<Mesh*>(node) != 0 );
		}
#endif

		nodes.add( node );
	}
}

void PipeSetup::getLights( const Array<Node*>& nodes, LightSorter& lights )
{
	lights.removeLights();
	for ( int i = 0 ; i < nodes.size() ; ++i )
	{
		Node* node = nodes[i];
		if ( Node::NODE_LIGHT == node->classId() )
		{
			Light* lt = static_cast<Light*>(node);
			if ( lt->enabled() )
				lights.addLight( lt );
		}
	}
}

void PipeSetup::getShaders( const Array<Visual*>& visuals, Array<Shader*>& shaders )
{
	shaders.clear();
	for ( int i = 0 ; i < visuals.size() ; ++i )
	{
		Visual* vis = visuals[i];
		vis->getShaders( shaders );
	}

	LANG_SORT( shaders.begin(), shaders.end() );
	shaders.resize( unique( shaders.begin(), shaders.end() ) - shaders.begin() );
}

void PipeSetup::getPriorities( const Array<Shader*>& shaders, Array<int>& priorities )
{
	priorities.clear();
	int lastpriority = 0x7FFFFFFF;
	for ( int i = 0 ; i < shaders.size() ; ++i )
	{
		int priority = shaders[i]->priority();
		if ( lastpriority != priority )
		{
			priorities.add( priority );
			lastpriority = priority;
		}
	}
	
	LANG_SORT( priorities.begin(), priorities.end(), greater<int>() );
	priorities.resize( unique( priorities.begin(), priorities.end() ) - priorities.begin() );
}


END_NAMESPACE() // hgr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
