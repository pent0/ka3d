#ifndef _HGR_LIGHTSORTER_H
#define _HGR_LIGHTSORTER_H


#include <hgr/Light.h>
#include <lang/Array.h>


BEGIN_NAMESPACE(hgr) 


class Light;


/**
 * Helper class for sorting lights by distance.
 * Used to select the most important lights in object rendering.
 *
 * @ingroup hgr
 */
class LightSorter
{
public:
	/* Private implementation class. */
	class LightData
	{
	public:
		float v;
		P(Light) obj;
		NS(math,float3) wpos;
	};

	/* Private implementation class. */
	class LightSortValue
	{
	public:
		LightData* data;

		bool operator<( const LightSortValue& other ) const		{return data->v < other.data->v;}
	};

	/**
	 * Constructs empty light sorter.
	 */
	LightSorter();

	/**
	 * Returns array of lights sorted by distance to the world space position. 
	 * Closest lights become first.
	 */
	NS(lang,Array)<Light*>&	getLightsByDistance( const NS(math,float3)& worldpos, int maxlights=8 );

	/**
	 * Adds a new collected light.
	 */
	void		addLight( Light* obj );

	/**
	 * Removes all collected lights.
	 */
	void		removeLights();

	/**
	 * Collects lights from scene to sm_lightdata and sm_lightsorter.
	 * Used to get quickly the lights which are near the object.
	 * Ignores disabled lights.
	 */
	void		collectLights( Node* root );

	/**
	 * Returns ith light.
	 */
	NS(hgr,Light)*	getLight( int i )					{return m_lightData[i].obj;}

	/**
	 * Returns number of lights added.
	 */
	int			lights() const						{return m_lightData.size();}

private:
	NS(lang,Array)<Light*>			m_lights;
	NS(lang,Array)<LightData>		m_lightData;
	NS(lang,Array)<LightSortValue>	m_lightSorter;
};


END_NAMESPACE() // hgr


#endif // _HGR_LIGHTSORTER_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
