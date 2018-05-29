#include <framework/App.h>
#include <gr/all.h>
#include <hgr/all.h>


class HierarchyApp : 
	public NS(framework,App)
{
public:
	HierarchyApp( NS(framework,OSInterface)* os, NS(gr,Context)* context );

	void		update( float dt, NS(gr,Context)* context );
	void		drawLine( const NS(math,float3)& wp0, const NS(math,float3)& wp1 );

	static HierarchyApp*	get()			{return static_cast<HierarchyApp*>( App::get() );}	

private:
	float				m_time;
	P(NS(hgr,Scene))		m_scene;
	P(NS(hgr,Camera))		m_camera;
	P(hgr::Node)		m_sun;
	P(hgr::Node)		m_earth;
	P(hgr::Node)		m_moon;
	P(NS(hgr,PipeSetup))	m_pipeSetup;
	P(NS(hgr,Pipe))		m_defaultPipe;
	P(NS(hgr,Pipe))		m_glowPipe;
};

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
