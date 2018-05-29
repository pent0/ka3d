#include <framework/App.h>
#include <gr/all.h>
#include <hgr/all.h>


class SceneAnimApp : 
	public NS(framework,App)
{
public:
	SceneAnimApp( NS(framework,OSInterface)* os, NS(gr,Context)* context );

	void		update( float dt, NS(gr,Context)* context );
	void		drawLine( const NS(math,float3)& wp0, const NS(math,float3)& wp1 );

	static SceneAnimApp*	get()			{return static_cast<SceneAnimApp*>( App::get() );}	

private:
	P(NS(hgr,Scene))		m_scene;
	P(NS(hgr,Camera))		m_camera;
	float				m_time;
};

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
