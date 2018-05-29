#ifndef _GR_VERTEXFORMAT_H 
#define _GR_VERTEXFORMAT_H


#include <lang/String.h>
#include <stdint.h>


BEGIN_NAMESPACE(math) 
	class float4;END_NAMESPACE()


BEGIN_NAMESPACE(gr) 


/**
 * Describes vertex bit layout.
 * @ingroup gr
 */
class VertexFormat
{
public:
	enum
	{
		/** Maximum number of texture coordinate layers. */
		MAX_TEXCOORDS = 4 
	};

	/**
	 * Format of a data component.
	 */
	enum DataFormat
	{
		/** Null vertex data format. */
		DF_NONE,
		/** Single 32-bit float. */
		DF_S_32,
		/** Single 16-bit integer. */
		DF_S_16,
		/** Single 8-bit integer. */
		DF_S_8,
		/** 32-bit float 2-vector. */
		DF_V2_32,
		/** 16-bit integer 2-vector. */
		DF_V2_16,
		/** 8-bit integer 2-vector. */
		DF_V2_8,
		/** 32-bit float 3-vector. */
		DF_V3_32,
		/** 16-bit integer 3-vector. */
		DF_V3_16,
		/** 8-bit integer 3-vector. */
		DF_V3_8,
		/** 32-bit float 4-vector. */
		DF_V4_32,
		/** 16-bit integer 4-vector. */
		DF_V4_16,
		/** 8-bit integer 4-vector. */
		DF_V4_8,
		/** 5-bit integer 4-vector. */
		DF_V4_5,
		/** Number of different vertex data formats. */
		DF_SIZE
	};

	/**
	 * Semantic type of a data component.
	 */
	enum DataType
	{
		/** Vertex has model space position data. */
		DT_POSITION,
		/** Vertex has screen space position data. */
		DT_POSITIONT,
		/** Vertex has bone weights used in skinning. */
		DT_BONEWEIGHTS,
		/** Vertex has bone indices used in skinning. */
		DT_BONEINDICES,
		/** Vertex has normal pointing away from surface. */
		DT_NORMAL,
		/** Vertex has diffuse color data. */
		DT_DIFFUSE,
		/** Vertex has specular color data. */
		DT_SPECULAR,
		/** Vertex has texture layer 0. */
		DT_TEX0,
		/** Vertex has texture layer 1. */
		DT_TEX1,
		/** Vertex has texture layer 2. */
		DT_TEX2,
		/** Vertex has texture layer 3. */
		DT_TEX3,
		/** Vertex has tangent data. */
		DT_TANGENT,
		/** Number of different vertex component types. */
		DT_SIZE
	};

	/**
	 * Empty vertex format description.
	 */	
	VertexFormat();

	/**
	 * Adds vertex position information to the vertex format.
	 */
	VertexFormat&	addPosition( DataFormat df=DF_V3_32 );

	/**
	 * Adds transformed vertex position information to the vertex format.
	 */
	VertexFormat&	addTransformedPosition( DataFormat df=DF_V4_32 );

	/**
	 * Adds vertex normal information to the vertex format. 
	 */
	VertexFormat&	addNormal( DataFormat df=DF_V3_32 );
	
	/** 
	 * Adds diffuse color information to the vertex format. 
	 */
	VertexFormat&	addDiffuse( DataFormat df=DF_V4_8 );
	
	/** 
	 * Adds specular color information to the vertex format. 
	 */
	VertexFormat&	addSpecular( DataFormat df=DF_V4_8 );

	/** 
	 * Adds tangent information to the vertex format. 
	 */
	VertexFormat&	addTangent( DataFormat df=DF_V3_32 );

	/** 
	 * Sets vertex weight information to the model vertex format.
	 * Number of vertex weights is one less than the number of 
	 * affecting bones per vertex. (the last vertex weights 
	 * is implicitly defined by the other weights)
	 * @param weightdf Data format of weights per vertex.
	 * @param indexdf Data format of bone indices per vertex.
	 */
	VertexFormat&	addWeights( DataFormat weightdf, DataFormat indexdf );
	
	/**
	 * Adds a texture coordinate layer. 
	 */
	VertexFormat&	addTextureCoordinate( DataFormat df );

	/**
	 * Override specific data type format which has been previously added.
	 */
	void			setDataFormat( DataType dt, DataFormat x );

	/**
	 * Sets alignment (in bytes) to be used when returning vertex size.
	 */
	void			setAlign( int alignment );

	/**
	 * Returns number of texture coordinates in the vertex format information. 
	 */
	int				textureCoordinates() const;

	/**
	 * Returns texture coordinate layer format.
	 */
	DataFormat		getTextureCoordinateFormat( int layer ) const;

	/**
	 * Returns format of specified semantic type.
	 * @return DF_NONE if format of specified type is not set.
	 */
	DataFormat		getDataFormat( DataType dt ) const;

	/**
	 * Returns true if vertex format has specified component.
	 */
	bool			hasData( DataType dt ) const				{return getDataFormat(dt) != DF_NONE;}

	/**
	 * Returns string representation of this format.
	 * Format is returned as "(TYPE1=FORMAT1, TYPE2=FORMAT2, ..., TYPEN=FORMATN)".
	 */
	NS(lang,String)	toString() const;

	/**
	 * Returns size of single vertex in bytes, ignoring padding
	 * and platform specific considerations.
	 */
	int				vertexSize() const;

	/**
	 * Returns true if vertex formats match.
	 */
	bool			operator==( const VertexFormat& other ) const;

	/**
	 * Returns true if vertex formats do not match.
	 */
	bool			operator!=( const VertexFormat& other ) const		{return !this->operator==(other);}

	/**
	 * Returns data dimensions (in number of components) of specified data format.
	 */
	static int		getDataDim( DataFormat df );

	/**
	 * Returns data size (in bytes) of specified data format.
	 */
	static int		getDataSize( DataFormat df );

	/**
	 * Returns data size (in bytes) of specified data format and n vertices.
	 * Data size is aligned to 32-bit boundary.
	 */
	static int		getDataSize( DataFormat df, int n );

	/**
	 * Returns data from specified format.
	 * If the data format does not have specified components,
	 * 0 is returned in that 4-vector component instead.
	 */
	static void		getData( DataFormat df, const void* data, NS(math,float4)* out );

	/**
	 * Sets data to specified format from float4.
	 */
	static void		setData( DataFormat df, void* data, const NS(math,float4)& v );

	/**
	 * Copies data from one vertex format to another.
	 */
	static void		copyData(
						void* ddata, int dpitch, DataFormat df,
						const void* sdata, int spitch, DataFormat sf, 
						int count );

	/**
	 * Copies data from one vertex format to another, scales and offsets the source.
	 */
	static void		copyData(
						void* ddata, int dpitch, DataFormat df,
						const void* sdata, int spitch, DataFormat sf, 
						const NS(math,float4)& sscale, const NS(math,float4)& sbias,
						int count );

	/**
	 * Computes bounding box and sphere of vertex data.
	 * @param scalebias X=scale, YZW=bias
	 */
	static void		getBound( const void* data, DataFormat df, int vertices,
						const NS(math,float4)& scalebias,
						NS(math,float4)* boundmin, NS(math,float4)* boundmax,
						float* boundradius );

	/**
	 * Converts data format to string.
	 */
	static const char*	toString( DataFormat df );

	/**
	 * Converts data type to string.
	 */
	static const char*	toString( DataType dt );

	/**
	 * Converts vertex data format string to type.
	 * @return DF_SIZE if not match.
	 */
	static DataFormat	toDataFormat( const char* str );

	/**
	 * Converts vertex data type string to type.
	 * @return DT_SIZE if not match.
	 */
	static DataType		toDataType( const char* str );

private:
	uint8_t m_data[7];
	uint8_t	m_align;
};


END_NAMESPACE() // gr


#endif // _GR_VERTEXFORMAT_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
