#include <gr/dx/DX_common.h>
#include <gr/dx/DX_Primitive.h>
#include <gr/dx/DX_Shader.h>
#include <gr/dx/DX_Context.h>
#include <gr/dx/DX_helpers.h>
#include <gr/GraphicsException.h>
#include <lang/Debug.h>
#include <lang/Profile.h>
#include <math/float2.h>
#include <math/float4.h>
#include <math/float4x4.h>
#include <string.h>
#include <config.h>


USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(gr) 


DX_Primitive::DX_Primitive( DX_Context* context, PrimType prim, const VertexFormat& vf,
	int vertices, int indices, Context::UsageFlags usage ) :
	DX_ContextItem( context, CLASSID_PRIMITIVE ),
	m_vb( 0 ),
	m_ib( 0 ),
	m_lockedVertexData(0),
	m_lockedIndexData(0),
	m_fx( 0 ),
	m_deviceFVF( 0 ),
	m_deviceVertexSize( 0 ),
	m_prim( prim ),
	m_locked( LOCK_NONE ),
	m_sort( Shader::SORT_NONE ),
	m_usage( (uint8_t)usage )
{
	assert( vertices > 0 );
	assert( indices % 3 == 0 );
	assert( PRIM_SPRITE != prim ); // PRIM_SPRITE not supported
	assert( PRIM_POINT != prim || 0 == indices ); // indexed point list unsupported
	
	setFormat( validateDeviceVertexFormat(vf), vertices, indices );
}

DX_Primitive::~DX_Primitive()
{
	deallocate();
}

void DX_Primitive::setShader( Shader* fx )
{
	m_fx = static_cast<DX_Shader*>(fx);
}

Shader* DX_Primitive::shader() const
{
	assert( m_fx != 0 && "Primitive has no shader" );
	return m_fx;
}

void DX_Primitive::lock( LockType lock )
{
	assert( LOCK_NONE == locked() );
	assert( !m_lockedVertexData );

	int vertices = vertexCount();
	UINT lockoffset = 0;
	UINT locklength = m_deviceVertexSize * vertices;
	UINT lockflags = 0;
	if ( m_usage == Context::USAGE_DYNAMIC )
		lockflags |= D3DLOCK_DISCARD;
	HRESULT hr = DX_TRY( m_vb->Lock( lockoffset, locklength, reinterpret_cast<void**>(&m_lockedVertexData), lockflags ) );
	if ( D3D_OK != hr )
		throwError( GraphicsException( Format("Failed to lock DirectX vertex buffer ({0} vertices, format {1})", vertices, vertexFormat().toString()) ) );

	if ( indexCount() > 0 )
	{
		assert( m_ib != 0 );

		lockoffset = 0;
		locklength = indexSize() * indexCount();
		UINT lockflags = 0;
		if ( m_usage == Context::USAGE_DYNAMIC )
			lockflags |= D3DLOCK_DISCARD;
		hr = DX_TRY( m_ib->Lock( lockoffset, locklength, reinterpret_cast<void**>(&m_lockedIndexData), lockflags ) );
		if ( D3D_OK != hr )
		{
			m_vb->Unlock();
			m_lockedVertexData = 0;
			throwError( GraphicsException( Format("Failed to lock DirectX index buffer ({0} indices)", indices()) ) );
		}
	}

	m_locked = lock;
	if ( m_locked != LOCK_READ )
		m_sort = Shader::SORT_NONE;
}

void DX_Primitive::unlock()
{
	assert( 0 != m_vb );

	m_vb->Unlock();
	
	if ( indexCount() > 0 )
	{
		assert( 0 != m_ib );
		m_ib->Unlock();
	}

	m_lockedVertexData = 0;
	m_lockedIndexData = 0;
	m_locked = LOCK_NONE;
}

DX_Primitive::LockType DX_Primitive::locked() const
{
	return m_locked;
}

void DX_Primitive::render()
{
	int firstvertex = vertexBegin();
	int vertices = vertexEnd() - vertexBegin();
	int firstindex = indexBegin();
	int indices = indexEnd() - indexBegin();
	assert( firstvertex >= 0 && firstvertex+vertices <= vertexCount() );
	assert( firstindex >= 0 && firstindex+indices <= indexCount() );
	if ( vertices <= 0 )
		return;

	// sorting mode different than current state of primitive?
	DX_Context* context = m_context;
	Shader::SortType sort = context->sort();
	if ( m_sort != sort )
	{
		int tricount = triangleListSize();
		if ( tricount > 0 ) // don't sort lines, strips or fans
		{
			Lock lk( this, LOCK_READWRITE );

			switch ( sort )
			{
			case Shader::SORT_OUTSIDEIN:
				sortOutsideIn( context->sortbuffer );
				break;

			case Shader::SORT_INSIDEOUT:
				sortInsideOut( context->sortbuffer );
				break;

			case Shader::SORT_FRONTTOBACK:
				sortFrontToBack( context->camerapos.xyz(), context->worldtm, context->boneworldtm.begin(), context->boneworldtm.size(), context->sortbuffer );
				break;

			case Shader::SORT_BACKTOFRONT:
				sortBackToFront( context->camerapos.xyz(), context->worldtm, context->boneworldtm.begin(), context->boneworldtm.size(), context->sortbuffer );
				break;
			}
		}

		// reset every time rendered unless INSIDEOUT which is static
		if ( sort == Shader::SORT_INSIDEOUT )
			m_sort = Shader::SORT_INSIDEOUT;
		else
			m_sort = Shader::SORT_NONE;
		context->setSort( m_sort );
	}

	assert( m_vb && "Primitive vertices were not locked for writing before rendering!" );
	assert( m_deviceVertexSize > 0 );
	IDirect3DDevice9* dev = context->device();
	DX_TRY( dev->SetFVF( m_deviceFVF ) );
	DX_TRY( dev->SetStreamSource( 0, m_vb, 0, m_deviceVertexSize ) );

	if ( indices > 0 )
	{
		assert( PRIM_TRI == m_prim );
		assert( m_ib );
		DX_TRY( dev->SetIndices( m_ib ) );

		switch ( m_prim )
		{
		case PRIM_POINT:
			break;
		case PRIM_LINE:{
			int lines = indices>>1;
			context->statistics.renderedLines += lines;
			DX_TRY( dev->DrawIndexedPrimitive( D3DPT_LINELIST, firstvertex, 0, vertices, firstindex, lines ) );
			break;}
		case PRIM_LINESTRIP:{
			int lines = indices - 1;
			context->statistics.renderedLines += lines;
			DX_TRY( dev->DrawIndexedPrimitive( D3DPT_LINESTRIP, firstvertex, 0, vertices, firstindex, lines ) );
			break;}
		case PRIM_TRI:{
			int triangles = indices/3;
			context->statistics.renderedTriangles += triangles;
			DX_TRY( dev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, firstvertex, 0, vertices, firstindex, triangles ) );
			break;}
		case PRIM_TRISTRIP:{
			int triangles = indices-2;
			context->statistics.renderedTriangles += triangles;
			DX_TRY( dev->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, firstvertex, 0, vertices, firstindex, triangles ) );
			break;}
		case PRIM_TRIFAN:{
			int triangles = indices-2;
			context->statistics.renderedTriangles += triangles;
			DX_TRY( dev->DrawIndexedPrimitive( D3DPT_TRIANGLEFAN, firstvertex, 0, vertices, firstindex, triangles ) );
			break;}
		case PRIM_SPRITE:
			break;
		}
	}
	else if ( vertices > 0 )
	{
		switch ( m_prim )
		{
		case PRIM_POINT:
			context->statistics.renderedPoints++;
			DX_TRY( dev->DrawPrimitive( D3DPT_POINTLIST, firstvertex, vertices ) );
			break;
		case PRIM_LINE:{
			int lines = vertices>>1;
			context->statistics.renderedLines += lines;
			DX_TRY( dev->DrawPrimitive( D3DPT_LINELIST, firstvertex, lines ) );
			break;}
		case PRIM_LINESTRIP:{
			int lines = indices - 1;
			context->statistics.renderedLines += lines;
			DX_TRY( dev->DrawPrimitive( D3DPT_LINESTRIP, firstvertex, lines ) );
			break;}
		case PRIM_TRI:{
			int triangles = vertices/3;
			context->statistics.renderedTriangles += triangles;
			DX_TRY( dev->DrawPrimitive( D3DPT_TRIANGLELIST, firstvertex, triangles ) );
			break;}
		case PRIM_TRISTRIP:{
			int triangles = vertices-2;
			context->statistics.renderedTriangles += triangles;
			DX_TRY( dev->DrawPrimitive( D3DPT_TRIANGLESTRIP, firstvertex, triangles ) );
			break;}
		case PRIM_TRIFAN:{
			int triangles = vertices-2;
			context->statistics.renderedTriangles += triangles;
			DX_TRY( dev->DrawPrimitive( D3DPT_TRIANGLEFAN, firstvertex, triangles ) );
			break;}
		case PRIM_SPRITE:
			break;
		}
	}

	m_context->statistics.renderedPrimitives += 1;
}

VertexFormat DX_Primitive::validateDeviceVertexFormat( const VertexFormat& origvf ) const
{
	VertexFormat vf = origvf;

	if ( vf.hasData(VertexFormat::DT_POSITION) )
		vf.setDataFormat( VertexFormat::DT_POSITION, VertexFormat::DF_V3_32 );

	if ( vf.hasData(VertexFormat::DT_POSITIONT) )
		vf.setDataFormat( VertexFormat::DT_POSITIONT, VertexFormat::DF_V4_32 );

	if ( vf.hasData(VertexFormat::DT_NORMAL) )
		vf.setDataFormat( VertexFormat::DT_NORMAL, VertexFormat::DF_V3_32 );

	if ( vf.hasData(VertexFormat::DT_DIFFUSE) )
		vf.setDataFormat( VertexFormat::DT_DIFFUSE, VertexFormat::DF_V4_8 );

	if ( vf.hasData(VertexFormat::DT_SPECULAR) )
		vf.setDataFormat( VertexFormat::DT_SPECULAR, VertexFormat::DF_V4_8 );

	if ( vf.hasData(VertexFormat::DT_BONEWEIGHTS) )
	{
		vf.setDataFormat( VertexFormat::DT_BONEWEIGHTS, VertexFormat::DF_V2_32 );
		vf.setDataFormat( VertexFormat::DT_BONEINDICES, VertexFormat::DF_V3_32 );
	}

	for ( int i = 0 ; i < vf.textureCoordinates() ; ++i )
	{
		VertexFormat::DataType dt = (VertexFormat::DataType)(VertexFormat::DT_TEX0+i);
		VertexFormat::DataFormat df = vf.getTextureCoordinateFormat(i);
		switch ( VertexFormat::getDataDim(df) )
		{
		case 1:	vf.setDataFormat( dt, VertexFormat::DF_S_32 ); break;
		case 2: vf.setDataFormat( dt, VertexFormat::DF_V2_32 ); break;
		case 3: vf.setDataFormat( dt, VertexFormat::DF_V3_32 ); break;
		case 4: vf.setDataFormat( dt, VertexFormat::DF_V4_32 ); break;
		}
	}

	return vf;
}

void DX_Primitive::getVertexDataPtr( VertexFormat::DataType dt, uint8_t** data, int* pitch )
{
	assert( LOCK_NONE != m_locked );
	assert( 0 != m_lockedVertexData );
	assert( vf().hasData(dt) );
	assert( m_deviceVertexSize > 0 );
	assert( m_deviceVertexDataOffset[dt] < m_deviceVertexSize );

	*data = m_lockedVertexData + m_deviceVertexDataOffset[dt];
	*pitch = m_deviceVertexSize;
}

void DX_Primitive::getIndexDataPtr( uint16_t** data, int* indexsize )
{
	assert( LOCK_NONE != m_locked );
	assert( 0 != m_lockedVertexData );
	
	*data = m_lockedIndexData;
	*indexsize = 2;
}

void DX_Primitive::allocate( const VertexFormat& vf, int vertices, int indices )
{
	if ( vertexCount() != vertices )
		throwError( GraphicsException( Format("Failed to create DirectX primitive, too many vertices ({0})", vertices) ) );
	if ( indexCount() != indices )
		throwError( GraphicsException( Format("Failed to create DirectX primitive, too many vertices ({0})", vertices) ) );

	setupDeviceVertexFormat();

	if ( !createVertexBuffer() )
	{
		deallocate();
		throwError( GraphicsException( Format("Failed to create DirectX vertex buffer ({0} vertices, format {1})", vertices, vf.toString()) ) );
	}

	if ( indices > 0 && !createIndexBuffer() )
	{
		deallocate();
		throwError( GraphicsException( Format("Failed to create DirectX index buffer ({0} indices)", indices) ) );
	}
}

void DX_Primitive::deallocate()
{
	DX_release( m_ib );
	DX_release( m_vb );
}

int DX_Primitive::setupDeviceVertexDataOffset( int offset, VertexFormat::DataType dt )
{
	assert( offset < 255 );
	assert( m_deviceVertexDataOffset[dt] == 255 );

	int bytes = 0;
	if ( vf().hasData(dt) )
	{
		m_deviceVertexDataOffset[dt]= (uint8_t)offset;
		bytes = VertexFormat::getDataSize( vf().getDataFormat(dt) );
	}
	return bytes;
}

void DX_Primitive::setupDeviceVertexFormat()
{
	// find device vertex format

	const VertexFormat& vf = this->vf();

	DWORD fvf = 0;
	if ( vf.hasData(VertexFormat::DT_NORMAL) )
		fvf |= D3DFVF_NORMAL;
	if ( vf.hasData(VertexFormat::DT_DIFFUSE) )
		fvf |= D3DFVF_DIFFUSE;
	if ( vf.hasData(VertexFormat::DT_SPECULAR) )
		fvf |= D3DFVF_SPECULAR;

	int texcoords = vf.textureCoordinates();
	for ( int i = 0 ; i < texcoords ; ++i )
	{
		VertexFormat::DataFormat df = vf.getTextureCoordinateFormat( i );
		switch ( VertexFormat::getDataDim(df) )
		{
		case 1:		fvf |= D3DFVF_TEXCOORDSIZE1(i); break;
		case 2:		fvf |= D3DFVF_TEXCOORDSIZE2(i); break;
		case 3:		fvf |= D3DFVF_TEXCOORDSIZE3(i); break;
		case 4:		fvf |= D3DFVF_TEXCOORDSIZE4(i); break;
		}
	}

	const int weights = vf.getDataDim( vf.getDataFormat(VertexFormat::DT_BONEWEIGHTS) );
	if ( 0 == weights )
	{
		if ( vf.hasData(VertexFormat::DT_POSITIONT) )
			fvf |= D3DFVF_XYZRHW;
		else
			fvf |= D3DFVF_XYZ;
	}
	else
	{
		if ( m_context->shaderHardware() )
		{
			// pass weights to shader as texcoords
			fvf |= D3DFVF_XYZ;

			// add weights & indices
			fvf |= D3DFVF_TEXCOORDSIZE2(texcoords);
			++texcoords;
			fvf |= D3DFVF_TEXCOORDSIZE3(texcoords);
			++texcoords;
		}
		else
		{
			// last weight == UBYTE4 indices
			switch ( weights )
			{
			case 1:		fvf |= D3DFVF_XYZB2; break;
			case 2:		fvf |= D3DFVF_XYZB3; break;
			default:	fvf |= D3DFVF_XYZB4; break;
			}
			fvf |= D3DFVF_LASTBETA_UBYTE4;
		}
	}

	if ( m_context->shaderHardware() && vf.hasData(VertexFormat::DT_TANGENT) )
	{
		// pass tangents as texcoords
		fvf |= D3DFVF_TEXCOORDSIZE3(texcoords);
		texcoords += 1;
	}

	switch ( texcoords )
	{
	case 0:		fvf |= D3DFVF_TEX0; break;
	case 1:		fvf |= D3DFVF_TEX1; break;
	case 2:		fvf |= D3DFVF_TEX2; break;
	case 3:		fvf |= D3DFVF_TEX3; break;
	case 4:		fvf |= D3DFVF_TEX4; break;
	case 5:		fvf |= D3DFVF_TEX5; break;
	case 6:		fvf |= D3DFVF_TEX6; break;
	case 7:		fvf |= D3DFVF_TEX7; break;
	case 8:		fvf |= D3DFVF_TEX8; break;
	}

#ifdef _DEBUG
	int d3dvsize = D3DXGetFVFVertexSize(fvf);
	int thisvsize = vertexSize();
	assert( d3dvsize == thisvsize );
#endif

	m_deviceFVF = fvf;

	// compute device vertex size and FVF vertex offsets
	m_deviceVertexSize = (uint8_t)vertexSize();

	for ( int i = 0 ; i < VertexFormat::DT_SIZE ; ++i )
		m_deviceVertexDataOffset[i] = 255;

	const bool shaderhw = m_context->shaderHardware();
	int offset = 0;
	offset += setupDeviceVertexDataOffset( offset, VertexFormat::DT_POSITION );
	offset += setupDeviceVertexDataOffset( offset, VertexFormat::DT_POSITIONT );

	if ( !shaderhw )
	{
		// would require 32->8 conversion
		if ( vf.hasData(VertexFormat::DT_BONEINDICES) )
			throwError( GraphicsException( Format("Fixed pipeline palette skinning unsupported for now") ) );
	}

	offset += setupDeviceVertexDataOffset( offset, VertexFormat::DT_NORMAL );
	offset += setupDeviceVertexDataOffset( offset, VertexFormat::DT_DIFFUSE );
	offset += setupDeviceVertexDataOffset( offset, VertexFormat::DT_SPECULAR );

	for ( int i = VertexFormat::DT_TEX0 ; i <= VertexFormat::DT_TEX3 ; ++i )
		offset += setupDeviceVertexDataOffset( offset, (VertexFormat::DataType)i );

	if ( shaderhw )
	{
		offset += setupDeviceVertexDataOffset( offset, VertexFormat::DT_BONEWEIGHTS );
		offset += setupDeviceVertexDataOffset( offset, VertexFormat::DT_BONEINDICES );
		offset += setupDeviceVertexDataOffset( offset, VertexFormat::DT_TANGENT );
	}

	assert( offset == m_deviceVertexSize && "Vertex size and end offset do not match!" );
}

bool DX_Primitive::createVertexBuffer()
{
	assert( vertexCount() > 0 );

	UINT length = m_deviceVertexSize * vertexCount();
	DWORD usage = D3DUSAGE_WRITEONLY;
	D3DPOOL pool = D3DPOOL_MANAGED;
	
	if ( m_usage == Context::USAGE_DYNAMIC )
	{
		usage |= D3DUSAGE_DYNAMIC;
		pool = D3DPOOL_DEFAULT;
	}

	IDirect3DDevice9* dev = m_context->device();
	HRESULT hr = DX_TRY( dev->CreateVertexBuffer( length, usage, m_deviceFVF, pool, &m_vb, 0 ) );
	return hr == D3D_OK;
}

bool DX_Primitive::createIndexBuffer()
{
	assert( indexCount() > 0 );

	const int indexsize = indexSize();
	UINT length = indexsize * indexCount();
	DWORD usage = D3DUSAGE_WRITEONLY;
	assert( indexsize == 2 || indexsize == 4 );
	D3DFORMAT fmt = indexsize == 4 ? D3DFMT_INDEX32 : D3DFMT_INDEX16;
	D3DPOOL pool = D3DPOOL_MANAGED;

	if ( m_usage == Context::USAGE_DYNAMIC )
	{
		usage |= D3DUSAGE_DYNAMIC;
		pool = D3DPOOL_DEFAULT;
	}

	IDirect3DDevice9* dev = m_context->device();
	HRESULT hr = DX_TRY( dev->CreateIndexBuffer( length, usage, fmt, pool, &m_ib, 0 ) );

	return D3D_OK == hr;
}

DX_Primitive::PrimType DX_Primitive::type() const
{
	return m_prim;
}

String DX_Primitive::toString() const
{
	return Format( "Primitive( v={0}, i={1}, vf={2} )", vertices(), indices(), vertexFormat().toString() ).format();
}

void DX_Primitive::deviceLost()
{
	if ( Context::USAGE_DYNAMIC == m_usage )
	{
		DX_release( m_vb );
		DX_release( m_ib );
	}
}

void DX_Primitive::deviceReset()
{
	if ( !m_vb )
		setFormat( validateDeviceVertexFormat(vf()), vertexCount(), indexCount() );
}


END_NAMESPACE() // gr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
