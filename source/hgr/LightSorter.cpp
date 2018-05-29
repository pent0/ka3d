#include <hgr/LightSorter.h>
#include <lang/algorithm/sort.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(hgr) 


LightSorter::LightSorter()
{
}

Array<Light*>& LightSorter::getLightsByDistance( const float3& worldpos, int maxlights )
{
	// compute light distance (squared) to object
	int lights = m_lightData.size();
	m_lightSorter.resize( lights );
	for ( int i = 0 ; i < lights ; ++i )
	{
		m_lightData[i].v = (m_lightData[i].wpos - worldpos).lengthSquared();
		m_lightSorter[i].data = &m_lightData[i];
	}
	LANG_SORT( m_lightSorter.begin(), m_lightSorter.end() );

	// return lights
	if ( lights > maxlights )
		lights = maxlights;
	m_lights.resize( lights );
	for ( int i = 0 ; i < lights ; ++i )
		m_lights[i] = m_lightSorter[i].data->obj;
	return m_lights;
}

void LightSorter::addLight( Light* obj )
{
	LightData objdata;
	objdata.v = 0.f;
	objdata.obj = obj;
	objdata.wpos = obj->worldTransform().translation();
	m_lightData.add( objdata );
}

void LightSorter::removeLights()
{
	m_lightData.clear();
	m_lightSorter.clear();
	m_lights.clear();
}

void LightSorter::collectLights( Node* root )
{
	assert( !root->parent() );

	m_lightData.clear();
	for ( Node* node = root ; node != 0 ; node = node->next(root) )
	{
		if ( Node::NODE_LIGHT == node->classId() )
		{
			Light* obj = static_cast<Light*>( node );
			if ( obj->enabled() )
				addLight( obj );
		}
	}
}


END_NAMESPACE() // hgr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
