#ifndef _HGR_LIGHT_H
#define _HGR_LIGHT_H


#include <hgr/Node.h>


BEGIN_NAMESPACE(hgr) 


/**
 * Describes dynamic light source properties in the scene.
 * Note that the light properties are only 'hints' to the
 * shader which is used to render the geometry.
 * Actual shader might or might not use some specific property
 * and the usage of light properties is shader dependent.
 * 
 * @ingroup hgr
 */
class Light : 
	public Node
{
public:
	/** 
	 * Type of light.
	 */
	enum Type
	{
		/** Unknown light type. */
		TYPE_UNKNOWN,
		/** Distant light source (like sun). */
		TYPE_DIRECTIONAL,
		/** Omnidirectional point light source (like a candle). Default. */
		TYPE_OMNI,
		/** Spot light source (like a lamp). */
		TYPE_SPOT,
		/** Number of different types. */
		TYPE_COUNT
	};

	/** 
	 * Maximum light range. 
	 */
	static const float	MAX_RANGE;

	/** 
	 * Maximum (spotlight) cone angle. 
	 */
	static const float	MAX_CONE_ANGLE;

	/** 
	 * Creates default white light. 
	 */
	Light();

	/** 
	 * Create a value copy of this light. 
	 */
	Light( const Light& other );

	///
	~Light();

	/**
	 * Returns clone of this node. Clones child nodes as well.
	 */
	virtual Node*	clone() const;

	/** 
	 * Sets type of the light. 
	 */
	void	setType( Type type );

	/** 
	 * Sets color of the light. 
	 */
	void	setColor( const NS(math,float3)& color );

	/** 
	 * Sets angle, in radians, of a fully illuminated spotlight cone. 
	 * @see setOuterCone
	 */
	void	setInnerCone( float angle );
	
	/** 
	 * Sets angle, in radians, defining the outer edge of the spotlight's outer cone. 
	 * Points outside this cone are not lit by the spotlight. 
	 * @see setInnerCone
	 */
	void	setOuterCone( float angle );

	/** 
	 * Sets distance in world space when light starts to fade away. 
	 * Maximum allowable value is maxRange(). 
	 * @param farstart Distance when light starts to fade away.
	 */
	void	setFarAttenStart( float farstart );

	/** 
	 * Sets distance in world space beyond which the light has no effect. 
	 * Maximum allowable value is maxRange(). 
	 * Updates node bounding volume.
	 * @param farend Distance when light has faded away.
	 */
	void	setFarAttenEnd( float farend );

	/**
	 * Illuminates a point on surface by this light.
	 */
	void	illuminate( const NS(math,float3)& point, const NS(math,float3)& normal,
				NS(math,float3)* diffuse ) const;

	/**
	 * Returns light distance attenuation [0,1] by distance.
	 * @param dist Distance from point to light.
	 */
	float	applyDistanceAtten( float dist ) const;

	/**
	 * Returns light shape attenuation [0,1] by distance.
	 * @param cosa Cosine of angle between light direction and vector from light to point.
	 */
	float	applyShapeAtten( float cosa ) const;

	/** 
	 * Returns type of the light. 
	 */
	Type	type() const							{return Type( (flags() & NODE_LIGHTTYPE) >> NODE_LIGHTTYPE_SHIFT );}

	/** 
	 * Returns angle, in radians, of a fully illuminated spotlight cone. 
	 * @see setOuterCone
	 */
	float	innerCone() const						{return m_inner;}
	
	/** 
	 * Returns angle, in radians, defining the outer edge of the spotlight's outer cone.
	 * Points outside this cone are not lit by the spotlight. 
	 * @see innerCone
	 */
	float	outerCone() const						{return m_outer;}

	/** 
	 * Returns distance in world space when light starts to fade away. 
	 * Maximum allowable value is maxRange(). 
	 * @param farstart Distance when light starts to fade away.
	 */
	float	farAttenStart() const					{return m_farAttenStart;}

	/** 
	 * Returns distance in world space beyond which the light has no effect. 
	 * Maximum allowable value is maxRange(). 
	 * Updates node bounding volume.
	 * @param farend Distance when light has faded away.
	 */
	float	farAttenEnd() const						{return m_farAttenEnd;}

	/** 
	 * Returns color of the light. 
	 */	
	const NS(math,float3)&	color() const				{return m_color;}

private:
	NS(math,float3)			m_color;
	float					m_farAttenStart;
	float					m_farAttenEnd;
	float					m_inner;
	float					m_outer;
	float					m_innerCos;
	float					m_outerCos;

	Light& operator=( const Light& other );
};


END_NAMESPACE() // hgr


#endif // _HGR_LIGHT_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
