#ifndef _HGR_CAMERA_H
#define _HGR_CAMERA_H


#include <hgr/Node.h>
#include <hgr/Lines.h>
#include <hgr/LightSorter.h>
#include <hgr/ViewFrustum.h>
#include <lang/Array.h>
#include <math/float4x4.h>


BEGIN_NAMESPACE(gr) 
	class Shader;
	class Context;END_NAMESPACE()


BEGIN_NAMESPACE(hgr) 


class Light;
class Visual;


/**
 * Camera node in scene graph.
 * Camera contains point-of-view, field-of-view, viewport and 
 * other attributes that desribe how the scene is rendered.
 * There can be multiple cameras in the scene.
 * 
 * @ingroup hgr
 */
class Camera : 
	public Node
{
public:
	/**
	 * Scene rendering statistics.
	 */
	struct Statistics
	{
		/** Number of visuals before cull. */
		int visualsBeforeCull;
		/** Number of visuals after cull. */
		int visualsAfterCull;

		/** Sets all statistics counters to 0. */
		void reset();
	} statistics;

	/*
	 * Temporary buffers used in rendering.
	 */
	class TempBuffers
	{
	public:
		NS(lang,Array)<NS(math,float4x4)>		bonematrices;
		NS(lang,Array)<NS(math,float4x4)*>	bonematrixp;
	};

	/** 
	 * Creates default camera. 
	 */
	Camera();

	/** 
	 * Copy by value. New camera is unlinked. 
	 */
	Camera( const Camera& other );

	///
	~Camera();

	/**
	 * Returns clone of this node. Clones child nodes as well.
	 */
	virtual Node*			clone() const;

	/**
	 * Toggles orthographic projection. If this is enabled then
	 * all view coordinates are used directly as screen coordinates.
	 */
	void	setOrthographicProjection( bool enabled );

	/** 
	 * Sets horizontal field of view (in radians). 
	 */
	void	setVerticalFov( float fovy );

	/** 
	 * Sets horizontal field of view (in radians). 
	 */
	void	setHorizontalFov( float fovx );

	/** 
	 * Sets front plane distance. 
	 */
	void	setFront( float front );
	
	/** 
	 * Sets back plane distance. 
	 */
	void	setBack( float back );

	/** 
	 * Renders the scene to the rendering context. 
	 * @param context Rendering context.
	 */
	void	render( NS(gr,Context)* context );

	/** 
	 * Renders the scene to the rendering context.
	 * @param context Rendering context.
	 * @param minpriority Minimum shader priority to render in this pass.
	 * @param maxpriority Maximum shader priority to render in this pass.
	 * @param visual Visuals to be rendered.
	 * @param priorities Priorities to be rendered.
	 * @param lightsorter Lights to be rendered.
	 */
	void	render( NS(gr,Context)* context, int minpriority, int maxpriority,
				const NS(lang,Array)<Visual*>& visuals, const NS(lang,Array)<int>& priorities,
				LightSorter* lightsorter );

	/** 
	 * Caches camera transforms and all world transforms from the nodes.
	 * Used in rendering. After this method, NS(Camera,getCachedTransform)() can be
	 * used to get valid world transform for any node in the array.
	 */
	void	cacheTransforms( NS(gr,Context)* context, const NS(lang,Array)<Node*>& nodes );

	/**
	 * Collects all visible visuals from 'nodes' array to 'visuals' array
	 * and sorts them by ascending distance to camera.
	 * Visuals are culled by bounding box before adding them to the array.
	 */
	void	cullVisuals( const NS(lang,Array)<Node*>& nodes, NS(lang,Array)<Visual*>& visuals );

	/**
	 * Mirrors local transform X-axis. Used for platforms which
	 * can accept rendering only in right-handed coordinate system.
	 */
	void	mirrorXAxis();

	/** 
	 * Returns front plane distance. 
	 */
	float	front() const;
	
	/** 
	 * Returns back plane distance. 
	 */
	float	back() const;

	/** 
	 * Returns horizontal field of view in radians. 
	 */
	float	horizontalFov() const;

	/** 
	 * Returns vertical field of view in radians. 
	 */
	float	verticalFov() const;

	/** 
	 * Returns view (inverse world) transform. 
	 */
	NS(math,float3x4)			viewTransform() const;

	/**
	 * Returns cached world transformation for the node.
	 * This function can be used only during rendering.
	 * (Requires that the node has been in the scene graph
	 * when NS(Camera,cacheTransforms) was called AND that
	 * the scene graph hasn't been modified after since.)
	 */
	const NS(math,float3x4)&	getCachedWorldTransform( Node* node ) const;

	/**
	 * Returns world (=camera to world) transformation.
	 * This function can be used only during rendering.
	 * Updated by calling cacheTransforms().
	 */
	const NS(math,float3x4)&	cachedWorldTransform() const;

	/**
	 * Returns view (=world to camera) transformation.
	 * This function can be used only during rendering.
	 * Updated by calling cacheTransforms().
	 */
	const NS(math,float3x4)&	cachedViewTransform() const;

	/**
	 * Returns view-projection transformation.
	 * This function can be used only during rendering.
	 * Updated by calling cacheTransforms().
	 */
	const NS(math,float4x4)&	cachedViewProjectionTransform() const;

	/**
	 * Gets cached lights sorted by distance to world position.
	 * Number of lights returned is limited to NS(Shader,MAX)_LIGHTS.
	 * This function can be used only during rendering.
	 */
	NS(lang,Array)<Light*>&	getLightsSortedByDistance( const NS(math,float3)& worldpos ) const;

	/**
	 * Returns current view frustum.
	 * Used only if orthographic() is false.
	 */
	const ViewFrustum&		frustum() const					{return m_frustum;}

	/**
	 * Returns true if orthographic projection is enabled. If this is enabled then
	 * all view coordinates are used directly as screen coordinates.
	 */
	bool					orthographicProjection() const	{return m_ortho;}

	/*
	 * Returns camera temporary buffers used in rendering.
	 */
	TempBuffers&			tempBuffers() const				{return m_temp;}

	/*
	 * Returns list of visible visuals in last rendering.
	 * Warning: Objects might not exist anymore.
	 */
	const NS(lang,Array)<Visual*>&	visuals() const			{return m_visuals;}

private:
	/* Helper class used to sort visuals. */
	class VisualSorter
	{
	public:
		Visual*	obj;
		float	depth;

		bool	operator<( const VisualSorter& other ) const	{return depth < other.depth;}
	};

	ViewFrustum				m_frustum;
	NS(math,float3x4)		m_worldtm;
	NS(math,float3x4)		m_viewtm;
	NS(math,float4x4)		m_viewprojtm;
	mutable TempBuffers		m_temp;
	P(NS(hgr,Lines))		m_lines;

	NS(lang,Array)<NS(math,float3x4)>	m_worldTransformCache; // used by cacheTransforms
	NS(lang,Array)<Node*>				m_nodes; // used by simple render
	NS(lang,Array)<Visual*>				m_visuals; // used by simple render
	NS(lang,Array)<NS(gr,Shader)*>		m_shaders; // used by simple render
	NS(lang,Array)<int>					m_priorities; // used by simple render
	LightSorter							m_lightSorter; // used by simple render
	LightSorter*						m_lightSorterPtr; // used by getLightsSortedByDistance
	NS(lang,Array)<VisualSorter>		m_visualSorter; // used by cullVisuals

	bool					m_ortho;

	Camera& operator=( const Camera& other );
};


#include <hgr/Camera.inl>


END_NAMESPACE() // hgr


#endif // _HGR_CAMERA_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
