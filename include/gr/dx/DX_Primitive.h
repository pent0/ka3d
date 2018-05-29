#ifndef _GR_DX_PRIMITIVE_H
#define _GR_DX_PRIMITIVE_H


#include <gr/Shader.h>
#include <gr/Context.h>
#include <gr/VertexFormat.h>
#include <gr/impl/DIPrimitive.h>
#include <math/float4.h>
#include "DX_common.h"
#include "DX_ContextItem.h"


BEGIN_NAMESPACE(math) 
	class float4;END_NAMESPACE()

BEGIN_NAMESPACE(ps2util) 
	class DMAChain;END_NAMESPACE()
	

BEGIN_NAMESPACE(gr) 
	

class Shader;
class DX_Shader;
class DX_Context;
	
	
/**
 * DirectX geometry primitive.
 */
class DX_Primitive :
	public DIPrimitive,
	public DX_ContextItem
{
public:
	DX_Primitive( DX_Context* context, PrimType prim, const VertexFormat& vf, int vertices, int indices, NS(gr,Context)::UsageFlags usage );
	~DX_Primitive();

	void	setShader( Shader* fx );

	/**
	 * Renders the primitive.
	 */
	void	render();

	/**
	 * Returns pointer to vertex data of specified component.
	 * Primitive needs to be locked before calling this method.
	 * WARNING: Low level operation, better to use getVertexPositions etc. methods.
	 */
	void	getVertexDataPtr( VertexFormat::DataType dt, uint8_t** data, int* pitch );

	/**
	 * Returns pointer to face index data.
	 * Primitive needs to be locked before calling this method.
	 * WARNING: Low level operation, better to use getIndices etc. methods.
	 */
	void	getIndexDataPtr( uint16_t** data, int* indexsize );

	/**
	 * Sets shader to be used while rendering the primitive.
	 */
	Shader*	shader() const;

	/**
	 * Returns number of triangles the primitive has in triangle list.
	 * NOTE: Only indexed and non-indexed triangle lists are counted,
	 * i.e. triangle strips, triangle fans and lines return 0 as triangle list size.
	 */
	int		triangleListSize() const	{return m_prim!=PRIM_TRI ? 0 : (indexCount()>0 ? indexCount()/3 : vertexCount()/3);}

	/**
	 * Returns primitive type.
	 */
	PrimType		type() const;

	/**
	 * Returns object string description.
	 */
	NS(lang,String)	toString() const;

private:
	IDirect3DVertexBuffer9*	m_vb;
	IDirect3DIndexBuffer9*	m_ib;
	uint8_t*				m_lockedVertexData;
	uint16_t*				m_lockedIndexData;
	P(DX_Shader)			m_fx;
	uint32_t				m_deviceFVF;
	uint8_t					m_deviceVertexSize;
	uint8_t					m_deviceVertexDataOffset[VertexFormat::DT_SIZE];
	PrimType 				m_prim;
	LockType				m_locked;
	Shader::SortType		m_sort;
	uint8_t					m_usage;

	void		deviceLost();
	void		deviceReset();
	void		allocate( const VertexFormat& vf, int vertices, int indices );
	void		deallocate();
	void		lock( LockType lock );
	void		unlock();
	bool		createVertexBuffer();
	bool		createIndexBuffer();
	int			setupDeviceVertexDataOffset( int offset, VertexFormat::DataType dt );
	void		setupDeviceVertexFormat();
	LockType	locked() const;
	VertexFormat validateDeviceVertexFormat( const VertexFormat& origvf ) const;

	DX_Primitive( const DX_Primitive& );
	DX_Primitive& operator=( const DX_Primitive& );
};
	
	
END_NAMESPACE() // gr


#endif // _GR_DX_PRIMITIVE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
