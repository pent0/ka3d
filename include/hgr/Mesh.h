#ifndef _HGR_MESH_H
#define _HGR_MESH_H


#include <gr/Primitive.h>
#include <hgr/Light.h>
#include <hgr/Visual.h>
#include <lang/Array.h>


BEGIN_NAMESPACE(math) 
	class float4x4;END_NAMESPACE()


BEGIN_NAMESPACE(hgr) 


class Light;
class Camera;


/**
 * Container for visual primitives.
 * 
 * @ingroup hgr
 */
class Mesh : 
	public Visual
{
public:
	///
	Mesh();

	/** 
	 * Creates a copy of this object. 
	 */
	Mesh( const Mesh& other );

	~Mesh();

	/**
	 * Returns clone of this node. Clones child nodes as well.
	 */
	virtual Node*	clone() const;

	/** 
	 * Renders this mesh to the active device.
	 * @param context Rendering context.
	 * @param camera Active camera.
	 * @param priority Shader priority level which is to be rendered.
	 */
	void			render( NS(gr,Context)* context, Camera* camera, int priority );

	/**
	 * Returns array of shaders used by this visual.
	 * NOTE: Implementation should not clear the list of returned shaders
	 * before adding new ones, since shadesr from multiple Visuals might be 
	 * collected at the same time.
	 */
	void			getShaders( NS(lang,Array)<NS(gr,Shader)*>& shaders );

	/**
	 * Sets rendering technique of the shaders used by this mesh.
	 */
	void			setTechnique( const char* name );

	/**
	 * Computes bounding volume of the visual in model space.
	 */
	void			computeBound();

	/** 
	 * Adds a visual primitive to the mesh. 
	 */
	void			addPrimitive( NS(gr,Primitive)* primitive );

	/** 
	 * Removes a visual primitive from the mesh. 
	 */
	void			removePrimitive( int index );

	/** 
	 * Removes all visual primitives from the mesh. 
	 */
	void			removePrimitives();

	/** 
	 * Adds a bone to the mesh.
	 * @param bone Bone node.
	 * @param invresttm Transform from skin to bone space in non-deforming pose.
	 */
	void			addBone( Node* bone, const NS(math,float3x4)& invresttm );

	/** 
	 * Removes a bone from the mesh. 
	 */
	void			removeBone( int index );

	/**
	 * Adds a local light to this mesh.
	 * Local light affects only this mesh.
	 * If the mesh has no local lights, then closest
	 * lights are retrieved from the scene in run-time.
	 * As this causes some performance penalty and is
	 * less predictable in terms of visual output, it
	 * is usually best always to use local lights if possible.
	 */
	void			addLight( Light* lt );

	/**
	 * Removes ith local light from this mesh.
	 * @see addLight
	 */
	void			removeLight( int i );

	/**
	 * Removes all local lights from this mesh.
	 * @see addLight
	 */
	void			removeLights();

	/**
	 * Returns ith local light from this mesh.
	 * @see addLight
	 */
	Light*			getLight( int i ) const;

	/** 
	 * Returns specified visual primitive of the mesh. 
	 */
	NS(gr,Primitive)*	getPrimitive( int index ) const;

	/**
	 * Returns number of local lights in this mesh.
	 * @see addLight
	 */
	int				lights() const;

	/** 
	 * Returns number of visual primitives in the mesh. 
	 */
	int				primitives() const;

	/** 
	 * Returns a bone from the mesh. 
	 */
	Node*			getBone( int index ) const;

	/** 
	 * Returns transform from skin space to bone space. 
	 */
	const NS(math,float3x4)&	getBoneInverseRestTransform( int index ) const;

	/** 
	 * Returns model->world transforms for n bones.
	 * Matrices are returned in modified 4x3 format, i.e.
	 * translation at row 3, X-axis at column 0.
	 * Camera transform cache must contain valid world transforms of the node hierarchy.
	 * This can be ensured by calling NS(Camera,cacheTransforms)()
	 * before this method.
	 */
	void			getBoneMatrix4x3Array( Camera* camera, NS(math,float4x4)* tm, int count ) const;

	/** 
	 * Returns number of bones in the mesh. 
	 */
	int				bones() const;

private:
	class Bone
	{
	public:
		NS(math,float3x4)	invresttm; // skin->bone tm in non-deforming pose
		Node*			node;
	};

	NS(lang,Array)<P(NS(gr,Primitive))>	m_primitives;
	NS(lang,Array)<NS(hgr,Light)*>		m_lights;
	NS(lang,Array)<Bone>				m_bones;

	Mesh& operator=( const Mesh& other );
};


#include <hgr/Mesh.inl>


END_NAMESPACE() // hgr


#endif // _HGR_MESH_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
