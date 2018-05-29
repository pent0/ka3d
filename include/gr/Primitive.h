#ifndef _GR_PRIMITIVE_H
#define _GR_PRIMITIVE_H


#include <gr/VertexFormat.h>
#include <gr/ContextObject.h>
#include <math/float3.h>
#include <stdint.h>


BEGIN_NAMESPACE(math) 
	class float4;END_NAMESPACE()


BEGIN_NAMESPACE(gr) 


class Shader;


/**
 * Geometry primitive list.
 * @ingroup gr
 */
class Primitive :
	public ContextObject
{
public:
	/**
	 * Geometry primitive type. 
	 */
	enum PrimType
	{
		PRIM_POINT		= 0,
		PRIM_LINE		= 1,
		PRIM_LINESTRIP	= 2,
		PRIM_TRI		= 3,
		PRIM_TRISTRIP	= 4,
		PRIM_TRIFAN		= 5,
		PRIM_SPRITE		= 6,
		PRIM_INVALID	= 7
	};

	enum
	{
		/** Maximum number of bones / primitive. */
		MAX_BONES		= 255,
	};

	Primitive();

	/**
	 * Sets shader to be used while rendering the primitive.
	 * 
	 * Rendering sequence:
	 * <ol>
	 * <li>First call NS(Shader,begin)() for the shader before rendering any geometry
	 * <li>Set shader parameters which need to be updated every frame, like transforms.
	 * <li>Call NS(Shader,beginPass)(0) to start rendering geometry using the first pass of the shader.
	 * <li>Call render() for each primitive to be rendered.
	 * <li>Call NS(Shader,endPass)() to stop rendering using this pass of the shader.
	 * <li>Loop to NS(Shader,beginPass)(i) for other passes, as many times as return value of begin() specified.
	 * <li>End rendering by calling NS(Shader,end)() for the shader.
	 */
	virtual void	setShader( Shader* fx ) = 0;

	/**
	 * Sets perspective correction quality level to be used while rending this primitive.
	 * @param quality Perspective correction quality in range 0-10, 0=off, 10=best. 
	 */
	virtual void	setPerspectiveCorrection( int quality );

	/**
	 * Sets vertex position coordinates.
	 * Primitive needs to be locked for writing before calling this method.
	 * Works only with float valued vertex data. Use getVertexDataPtr
	 * to access compressed data.
	 * @see NS(ContextObject,Lock)
	 * @param index First vertex to set.
	 * @param data Position coordinate data.
	 * @param count Position coordinate count.
	 */
	virtual void	setVertexPositions( int index, const NS(math,float4)* data, int count ) = 0;

	/**
	 * Sets scale & bias for unpacking vertex data.
	 * @param scalebias scale=x, bias=yzw
	 * v(1) = v(0) * scale + bias
	 */
	virtual void	setVertexPositionScaleBias( const NS(math,float4)& scalebias ) = 0;

	/**
	 * Sets transformed vertex position coordinates.
	 * Primitive needs to be locked for writing before calling this method.
	 * Works only with float valued vertex data. Use getVertexDataPtr
	 * to access compressed data.
	 * @see NS(ContextObject,Lock)
	 * @param index First vertex to set.
	 * @param data Position coordinate data.
	 * @param count Position coordinate count.
	 */
	virtual void	setVertexTransformedPositions( int index, const NS(math,float4)* data, int count ) = 0;

	/**
	 * Sets vertex normals.
	 * Primitive needs to be locked for writing before calling this method.
	 * Works only with float valued vertex data. Use getVertexDataPtr
	 * to access compressed data.
	 * @see NS(ContextObject,Lock)
	 * @param index First vertex to set.
	 * @param data Normal coordinate data.
	 * @param count Normal coordinate count.
	 */
	virtual void	setVertexNormals( int index, const NS(math,float4)* data, int count ) = 0;

	/**
	 * Sets vertex diffuse RGBA colors.
	 * Colors are expected in [0,255] range.
	 * Primitive needs to be locked for writing before calling this method.
	 * Works only with float valued vertex data. Use getVertexDataPtr
	 * to access compressed data.
	 * @see NS(ContextObject,Lock)
	 * @param index First vertex to set.
	 * @param data Normal coordinate data.
	 * @param count Normal coordinate count.
	 */
	virtual void	setVertexDiffuseColors( int index, const NS(math,float4)* data, int count ) = 0;

	/**
	 * Sets vertex specular RGBA colors.
	 * Colors are expected in [0,255] range.
	 * Primitive needs to be locked for writing before calling this method.
	 * @see NS(ContextObject,Lock)
	 * @param index First vertex to set.
	 * @param data Normal coordinate data.
	 * @param count Normal coordinate count.
	 */
	virtual void	setVertexSpecularColors( int index, const NS(math,float4)* data, int count ) = 0;

	/**
	 * Sets vertex tangents.
	 * Primitive needs to be locked for writing before calling this method.
	 * Works only with float valued vertex data. Use getVertexDataPtr
	 * to access compressed data.
	 * @see NS(ContextObject,Lock)
	 * @param index First vertex to set.
	 * @param data Vertex data.
	 * @param count Vertex count.
	 */
	virtual void	setVertexTangents( int index, const NS(math,float4)* data, int count ) = 0;

	/**
	 * Sets vertex bone blending weights used in skinning.
	 * Primitive needs to be locked for writing before calling this method.
	 * Works only with float valued vertex data. Use getVertexDataPtr
	 * to access compressed data.
	 * @see NS(ContextObject,Lock)
	 * @param index First vertex to set.
	 * @param data Normal coordinate data.
	 * @param count Normal coordinate count.
	 */
	virtual void	setVertexBoneWeights( int index, const NS(math,float4)* data, int count ) = 0;

	/**
	 * Sets vertex bone blending indices used in skinning.
	 * Primitive needs to be locked for writing before calling this method.
	 * Works only with float valued vertex data. Use getVertexDataPtr
	 * to access compressed data.
	 * @see NS(ContextObject,Lock)
	 * @param index First vertex to set.
	 * @param data Normal coordinate data.
	 * @param count Normal coordinate count.
	 */
	virtual void	setVertexBoneIndices( int index, const NS(math,float4)* data, int count ) = 0;

	/**
	 * Sets vertex texture coordinates.
	 * Primitive needs to be locked for writing before calling this method.
	 * Works only with float valued vertex data. Use getVertexDataPtr
	 * to access compressed data.
	 * @see NS(ContextObject,Lock)
	 * @param index First vertex to set.
	 * @param layer Texture layer index.
	 * @param data Texture coordinate data.
	 * @param count Texture coordinate count.
	 */
	virtual void	setVertexTextureCoordinates( int index, int layer, const NS(math,float4)* data, int count ) = 0;

	/**
	 * Sets scale & bias for unpacking vertex data.
	 * @param scalebias scale=x, bias=yzw
	 * v(1) = v(0) * scale + bias
	 */
	virtual void	setVertexTextureCoordinateScaleBias( const NS(math,float4)& scalebias ) = 0;

	/** 
	 * Sets vertex components. Supports format conversion. 
	 * Primitive needs to be locked for writing before calling this method.
	 * @param dt Vertex data type to set.
	 * @param index First vertex to set.
	 * @param data Pointer to source data.
	 * @param df Format of source data.
	 * @param count Number of vertices to set.
	 */
	virtual void 	setVertexData( VertexFormat::DataType dt, int index, 
						const void* data, VertexFormat::DataFormat df, int count ) = 0;

	/** 
	 * Sets vertex indices. Supports format conversion. 
	 * Primitive needs to be locked for writing before calling this method.
	 * @param v0 First vertex index to set.
	 * @param data Pointer to source data.
	 * @param indexsize Byte size of single index (1,2 or 4).
	 * @param count Number of indices to set.
	 */
	virtual void 	setIndexData( int v0, const void* data, int indexsize, int count ) = 0;

	/**
	 * Sets primitive vertex indices.
	 * Primitive needs to be locked for writing before calling this method.
	 * @see NS(ContextObject,Lock)
	 * @param index Index of the first index to set.
	 * @param data Index data.
	 * @param count Number of indices to set.
	 */
	virtual void	setIndices( int index, const int* data, int count ) = 0;

	/**
	 * Optimizes bone list so that the bones used by this primitive are
	 * numbered from 0 (inclusive) to n (exclusive), where n is total number 
	 * bones used by this primitive. usedBoneArray return list of actual
	 * skin mesh bone indices.
	 * Primitive needs to be locked for writing before calling this method.
	 * @see NS(ContextObject,Lock)
	 * @see usedBoneArray
	 * @see usedBones
	 */
	virtual void	packBones() = 0;

	/**
	 * Sets list of bones used by this primitive. List contains indices
	 * numbered from 0 (inclusive) to n (exclusive), where n is total number 
	 * bones used by this primitive. usedBoneArray return lists of actual
	 * skin mesh bone indices.
	 * Primitive needs to be locked for writing before calling this method.
	 * @see NS(ContextObject,Lock)
	 * @see usedBoneArray
	 * @see usedBones
	 */
	virtual void	setUsedBones( const uint8_t* usedbonearray, int usedbones ) = 0;

	/**
	 * Renders the primitive.
	 */
	virtual void	render() = 0;

	/**
	 * Sets wireframe rendering color of the model as specified RGB888 color.
	 * Wireframe color is used when debug mode NS(Context,setWireframeEnabled) is true.
	 */
	virtual void	setWireframeColor( int color );

	/**
	 * Returns pointer to vertex data of specified component.
	 * Primitive needs to be locked before calling this method.
	 * WARNING: Low level operation, better to use getVertexPositions etc. methods.
	 */
	virtual void	getVertexDataPtr( VertexFormat::DataType dt, uint8_t** data, int* pitch ) = 0;

	/**
	 * Returns pointer to face index data.
	 * Primitive needs to be locked before calling this method.
	 * WARNING: Low level operation, better to use getIndices etc. methods.
	 */
	virtual void	getIndexDataPtr( uint16_t** data, int* indexsize ) = 0;

	/**
	 * Sets range of vertices used in rendering.
	 * Range must be below maximum vertex count specified in object creation.
	 * Range specifies the first vertex used to render the primitive.
	 */
	virtual void	setVertexRangeBegin( int begin ) = 0;

	/**
	 * Sets range of vertices used in rendering.
	 * Range must be below or equal to maximum vertex count specified in object creation.
	 * Range specifies one beyond the last vertex used to render the primitive.
	 */
	virtual void	setVertexRangeEnd( int end ) = 0;

	/**
	 * Sets range of indices used in rendering.
	 * Range must be below maximum index count specified in object creation.
	 * Range specifies the first index used to render the primitive.
	 */
	virtual void	setIndexRangeBegin( int begin ) = 0;

	/**
	 * Sets range of indices used in rendering.
	 * Range must be below or equal to maximum index count specified in object creation.
	 * Range specifies one beyond the last index used to render the primitive.
	 */
	virtual void	setIndexRangeEnd( int end ) = 0;

	/**
	 * Sets primitive bounding volume.
	 */
	virtual void	setBound( const NS(math,float3)& boundmin, const NS(math,float3)& boundmax, float boundradius ) = 0;

	/**
	 * Gets range of vertices used in rendering.
	 */
	virtual int		vertexRangeBegin() const = 0;

	/**
	 * Gets range of vertices used in rendering.
	 */
	virtual int		vertexRangeEnd() const = 0;

	/**
	 * Gets range of indices used in rendering.
	 */
	virtual int		indexRangeBegin() const = 0;

	/**
	 * Gets range of indices used in rendering.
	 */
	virtual int		indexRangeEnd() const = 0;

	/**
	 * Returns array of bones used by this primitive.
	 * Indices returned by this function represent skin mesh bone indices.
	 * Beware that changing skin mesh might invalidate the indices
	 * returned by the primitive, unless primitive is updated accordingly.
	 * List returned by this method is guaranteed to be valid only as long as
	 * any other method of this primitive is not called.
	 */
	virtual const uint8_t*	usedBoneArray() const = 0;

	/**
	 * Returns primitive type.
	 */
	virtual PrimType		type() const = 0;

	/**
	 * Returns number of bones used by this primitive.
	 */
	virtual int		usedBones() const = 0;

	/**
	 * Sets shader to be used while rendering the primitive.
	 */
	virtual Shader*	shader() const = 0;

	/**
	 * Returns number of vertices in the primitive.
	 */
	virtual int		vertices() const = 0;

	/**
	 * Returns number of indices in the primitive.
	 */
	virtual int		indices() const = 0;

	/**
	 * Returns vertex format.
	 */
	virtual const VertexFormat&	vertexFormat() const = 0;

	/**
	 * Gets vertex position coordinates.
	 * Primitive needs to be locked for reading before calling this method.
	 * @see NS(ContextObject,Lock)
	 * @param index First vertex to get.
	 * @param data Vertex data.
	 * @param count Vertex count.
	 */
	virtual void	getVertexPositions( int index, NS(math,float4)* data, int count ) const = 0;

	/**
	 * Gets vertex normals.
	 * Primitive needs to be locked for reading before calling this method.
	 * @see NS(ContextObject,Lock)
	 * @param index First vertex to get.
	 * @param data Vertex data.
	 * @param count Vertex count.
	 */
	virtual void	getVertexNormals( int index, NS(math,float4)* data, int count ) const = 0;

	/**
	 * Gets vertex diffuse RGBA colors.
	 * Colors are returned in [0,255] range.
	 * Primitive needs to be locked for reading before calling this method.
	 * @see NS(ContextObject,Lock)
	 * @param index First vertex to get.
	 * @param data Vertex data.
	 * @param count Vertex count.
	 */
	virtual void	getVertexDiffuseColors( int index, NS(math,float4)* data, int count ) const = 0;

	/**
	 * Gets vertex specular RGBA colors.
	 * Colors are returned in [0,255] range.
	 * Primitive needs to be locked for reading before calling this method.
	 * @see NS(ContextObject,Lock)
	 * @param index First vertex to get.
	 * @param data Vertex data.
	 * @param count Vertex count.
	 */
	virtual void	getVertexSpecularColors( int index, NS(math,float4)* data, int count ) const = 0;

	/**
	 * Gets vertex tangents.
	 * Primitive needs to be locked for reading before calling this method.
	 * @see NS(ContextObject,Lock)
	 * @param index First vertex to get.
	 * @param data Vertex data.
	 * @param count Vertex count.
	 */
	virtual void	getVertexTangents( int index, NS(math,float4)* data, int count ) const = 0;

	/**
	 * Gets vertex bone blending weights used in skinning.
	 * @param index First vertex to get.
	 * @param data Vertex data.
	 * @param count Vertex count.
	 */
	virtual void	getVertexBoneWeights( int index, NS(math,float4)* data, int count ) const = 0;

	/**
	 * Gets vertex bone blending indices used in skinning.
	 * @param index First vertex to get.
	 * @param data Vertex data.
	 * @param count Vertex count.
	 */
	virtual void	getVertexBoneIndices( int index, NS(math,float4)* data, int count ) const = 0;

	/**
	 * Gets vertex texture coordinates.
	 * Primitive needs to be locked for reading before calling this method.
	 * @see NS(ContextObject,Lock)
	 * @param index First vertex to get.
	 * @param layer Texture layer index.
	 * @param data Texture coordinate data.
	 * @param count Texture coordinate count.
	 */
	virtual void	getVertexTextureCoordinates( int index, int layer, NS(math,float4)* data, int count ) const = 0;

	/** 
	 * Gets vertex components. Supports format conversion. 
	 * @param dt Vertex data type to get.
	 * @param index First vertex to get.
	 * @param data Pointer to buffer for data.
	 * @param count Number of vertices to get.
	 */
	virtual void 	getVertexData( VertexFormat::DataType dt, int index, NS(math,float4)* data, int count ) const = 0;

	/**
	 * Gets primitive vertex indices.
	 * Primitive needs to be locked for reading before calling this method.
	 * @see NS(ContextObject,Lock)
	 * @param index Index of the first index to get.
	 * @param data Index data.
	 * @param count Number of indices to get.
	 */
	virtual void	getIndices( int index, int* data, int count ) const = 0;

	/**
	 * Returns vertex texture coordinate scale/bias.
	 */
	virtual const NS(math,float4)&		vertexTextureCoordinateScaleBias() const = 0;

	/**
	 * Returns vertex position scale/bias.
	 */
	virtual const NS(math,float4)&		vertexPositionScaleBias() const = 0;

	/**
	 * Returns bounding box min.
	 */
	virtual const NS(math,float3)&		boundMin() const = 0;

	/**
	 * Returns bounding box max.
	 */
	virtual const NS(math,float3)&		boundMax() const = 0;

	/**
	 * Returns bounding radius.
	 */
	virtual float						boundRadius() const = 0;
};
	
	
END_NAMESPACE() // gr


#endif // _GR_PRIMITIVE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
