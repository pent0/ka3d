#ifndef _HGR_SCENEINPUTSTREAM_H
#define _HGR_SCENEINPUTSTREAM_H


#include <gr/Context.h>
#include <gr/Primitive.h>
#include <io/DataInputStream.h>
#include <hgr/UserPropertySet.h>
#include <hgr/TransformAnimation.h>
#include <lang/Array.h>
#include <math/float4.h>


BEGIN_NAMESPACE(gr) 
	class Context;END_NAMESPACE()


BEGIN_NAMESPACE(hgr) 


class Node;
class Mesh;
class Scene;
class Dummy;
class Light;
class Camera;


/* 
 * Helper class for reading hgr scene files. 
 * See maxexport and NS(Scene,Scene) for usage examples
 * and hgr-fileformat.txt for documentation.
 * @ingroup hgr
 */
class SceneInputStream :
	public NS(io,DataInputStream)
{
public:
	/** Data content flags. */
	enum DataFlags
	{
		/** Scene stream contains materials. */
		DATA_MATERIALS		= 1,
		/** Scene stream contains primitives. */
		DATA_PRIMITIVES		= 2,
		/** Scene stream contains nodes and hierarchy. */
		DATA_NODES			= 4,
		/** Scene stream contains animations. */
		DATA_ANIMATIONS		= 8,
		/** Scene stream contains user properties. */
		DATA_USERPROPERTIES	= 16,
	};

	/** Minimum version number (major*100+minor) the scene loader supports. */
	static const int MIN_VERSION;

	/** Maximum version number (major*100+minor) the scene loader supports. */
	static const int MAX_VERSION;

	/**
	 * Prepares to read scene from the input stream.
	 * @exception IOException
	 */
	explicit SceneInputStream( NS(io,InputStream)* in );

	///
	~SceneInputStream();

	/**
	 * Reads float3 data from the input stream.
	 * @exception IOException
	 */
	NS(math,float3)			readFloat3();

	/**
	 * Reads float4 data from the input stream.
	 * @exception IOException
	 */
	NS(math,float4)			readFloat4();

	/**
	 * Reads quaternion data from the input stream as 4 floats.
	 * @exception IOException
	 */
	NS(math,quaternion)		readQuaternion();

	/**
	 * Reads float3x4 data from the input stream.
	 * @exception IOException
	 */
	NS(math,float3x4)			readFloat3x4();

	/**
	 * Reads transform animation data from the input stream.
	 * @exception IOException
	 */
	P(TransformAnimation)	readTransformAnimation();

	/**
	 * Reads keyframe sequence data from the input stream.
	 * @exception IOException
	 */
	P(KeyframeSequence)		readKeyframeSequence();

	/**
	 * Reads animation data from the input stream.
	 * @exception IOException
	 */
	P(TransformAnimation::Float3Anim) readFloat3Anim();

	/**
	 * Reads array of float3s from the input stream.
	 * @exception IOException
	 */
	void					readFloat3Array16( NS(math,float3)* out, int count );

	/**
	 * Reads array of float4s from the input stream.
	 * @exception IOException
	 */
	void					readFloat4Array16( NS(math,float4)* out, int count );

	/**
	 * Reads set of node user property strings.
	 * @param anims Optional parameter, which can be used to enable sharing of node name strings.
	 * @exception IOException
	 */
	P(UserPropertySet)		readUserPropertySet( Scene* scene=0 );
		
	/**
	 * Reads vertex format from the input stream.
	 * @param vf [out] Receives read vertex format.
	 * @exception IOException
	 */
	void					readVertexFormat( NS(gr,VertexFormat)* vf );

	/**
	 * Reads geometry primitive data from the input stream.
	 * @param materialindex [out] Receives material index of the primitive.
	 * @exception IOException
	 */
	P(NS(gr,Primitive))		readPrimitive( NS(gr,Context)* context, int* materialindex );

	/**
	 * Reads Node data from the input stream.
	 * @param node [out] Receives read node data.
	 * @param parentindex [out] Receives read node parent index.
	 * @exception IOException
	 */
	void					readNode( Node* node, int* parentindex );

	/**
	 * Returns version number (major*100+minor) of this scene file.
	 */
	int						version() const				{return m_ver;}

	/**
	 * Returns exporter version number.
	 * Note that this is different from file format version number.
	 */
	int						exporterVersion() const		{return m_exporterVer;}

	/**
	 * Returns true if the scene file has specified data.
	 * @see DataFlags
	 */
	bool					hasData( int flags ) const;

	/**
	 * Returns platform id of the file.
	 */
	NS(gr,Context)::PlatformType	platform() const;

private:
	int							m_ver;
	int							m_exporterVer;
	int							m_dataFlags;
	NS(gr,Context)::PlatformType	m_platformID;

	SceneInputStream( const SceneInputStream& );
	SceneInputStream& operator=( const SceneInputStream& );
};


END_NAMESPACE() // hgr


#endif // _HGR_SCENEINPUTSTREAM_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
