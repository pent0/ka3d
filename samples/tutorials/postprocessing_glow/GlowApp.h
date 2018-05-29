#include <framework/App.h>
#include <gr/all.h>
#include <hgr/all.h>


class GlowApp : 
	public NS(framework,App)
{
public:
	GlowApp( NS(framework,OSInterface)* os, NS(gr,Context)* context );
	
	void update( float dt, NS(gr,Context)* context );

	void keyDown( KeyType key );

private:
	P(NS(hgr,PipeSetup))	m_pipeSetup;
	P(NS(hgr,Pipe))		m_glowPipe;
	P(NS(hgr,Pipe))		m_defaultPipe;
	float				m_time;
	P(NS(hgr,Scene))		m_scene;
	P(NS(hgr,Camera))		m_camera;
	bool				m_capture;
};

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
