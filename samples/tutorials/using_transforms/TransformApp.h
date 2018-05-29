// Note: In actual application you should
// include individual headers for optimal
// compilation performance. This example includes
// everything just for convenience.
#include <framework/App.h>
#include <gr/all.h>
#include <lang/all.h>
#include <math/all.h>


class TransformApp : 
	public NS(framework,App)
{
public:
	TransformApp( NS(framework,OSInterface)* os, NS(gr,Context)* context );
	
	void update( float dt, NS(gr,Context)* context );

private:
	P(NS(gr,Primitive))				m_prim;
	float							m_time;
};

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
