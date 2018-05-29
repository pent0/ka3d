#include <gr/Context.h>
#include <gr/Texture.h>
#include <lang/String.h>
#include <string.h>
#include <config.h>


BEGIN_NAMESPACE(gr) 


const char* const PLATFORM_NAME[] = 
{
	"dx",
	"egl",
	"psp",
	"sw",
	"n3d",
};

const char* const PLATFORM_DESC[] =
{
	"DirectX",
	"OpenGL ES",
	"Playstation Portable",
	"Software Renderer",
	"Nokia N3D",
};


Context::RenderScene::RenderScene( Context* context ) :
	m_context( context )
{
	m_context->beginScene();
}
	
Context::RenderScene::~RenderScene()
{
	m_context->endScene();
}

void Context::Statistics::reset()
{
	memset( this, 0, sizeof(Statistics) );
}

Context::Context()
{
	statistics.reset();
}

Context::~Context()
{
}

float Context::aspect() const
{
	return (float)width() / (float)height();
}

const char*	Context::getString( PlatformType platform )
{
	assert( (int)platform >= 0 && (int)platform < int(sizeof(PLATFORM_NAME)/sizeof(PLATFORM_NAME[0])) );
	return PLATFORM_NAME[platform];
}

const char*	Context::getDescription( PlatformType platform )
{
	assert( (int)platform >= 0 && (int)platform < int(sizeof(PLATFORM_DESC)/sizeof(PLATFORM_DESC[0])) );
	return PLATFORM_DESC[platform];
}

const char* Context::platformString() const
{
	assert( (int)platform() >= 0 && (int)platform() < int(sizeof(PLATFORM_NAME)/sizeof(PLATFORM_NAME[0])) );
	return PLATFORM_NAME[platform()];
}

void Context::setWireframeEnabled( bool /*enabled*/ )
{
}


END_NAMESPACE() // gr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
