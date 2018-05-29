#include <framework/App.h>
#include <gr/all.h>
#include <hgr/all.h>


class ParticleApp : 
	public NS(framework,App)
{
public:
	ParticleApp( NS(framework,OSInterface)* os, NS(gr,Context)* context );
	
	void update( float dt, NS(gr,Context)* context );

private:
	P(NS(gr,Primitive))	createSimplePrimitive( NS(gr,Context)* context );

	P(NS(hgr,Scene))				m_scene;
	P(NS(hgr,ParticleSystem))		m_particleSystem;
	P(NS(hgr,Camera))				m_camera;
};

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
