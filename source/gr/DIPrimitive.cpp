#include <gr/impl/DIPrimitive.h>
#include <gr/impl/SortBuffer.h>
#include <gr/VertexFormat.h>
#include <lang/Math.h>
#include <lang/Float.h>
#include <lang/Debug.h>
#include <lang/OutOfMemoryException.h>
#include <lang/algorithm/sort.h>
#include <lang/algorithm/unique.h>
#include <math/float4.h>
#include <math/float4x4.h>
#include <string.h>
#include <stdint.h>
#include <config.h>


USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(gr) 


DIPrimitive::DIPrimitive() :
	m_posScaleBias(1,0,0,0),
	m_texcoordScaleBias(1,0,0,0),
	m_boundMin(0,0,0),
	m_boundMax(0,0,0),
	m_boundRadius(0),
	m_sysMemData( 0 ),
	m_usedBone( 0 ),
	m_vertices( 0 ),
	m_indices( 0 ),
	m_vertexRangeBegin( 0 ),
	m_vertexRangeEnd( 0 ),
	m_indexRangeBegin( 0 ),
	m_indexRangeEnd( 0 ),
	m_vf(),
	m_usedBones( 0 )
{
}

DIPrimitive::~DIPrimitive()
{
	reset();
}

void DIPrimitive::reset()
{
	m_usedBones = 0;
	m_vf = VertexFormat();
	m_indices = 0;
	m_vertices = 0;
	m_vertexRangeBegin = 0;
	m_vertexRangeEnd = 0;
	m_indexRangeBegin = 0;
	m_indexRangeEnd = 0;

	if ( 0 != m_usedBone )
	{
		delete[] m_usedBone; 
		m_usedBone = 0;
	}

	deallocate();
}

void DIPrimitive::setVertexRangeBegin( int begin )
{
	assert( begin >= 0 && begin < int(m_vertices) );
	m_vertexRangeBegin = begin;
}

void DIPrimitive::setVertexRangeEnd( int end )
{
	assert( end >= 0 && end <= int(m_vertices) );
	m_vertexRangeEnd = end;
}

void DIPrimitive::setIndexRangeBegin( int begin )
{
	assert( begin >= 0 && begin < int(m_indices) );
	m_indexRangeBegin = (uint16_t)begin;
}

void DIPrimitive::setIndexRangeEnd( int end )
{
	assert( end >= 0 && end <= int(m_indices) );
	m_indexRangeEnd = (uint16_t)end;
}

int	DIPrimitive::vertexRangeBegin() const
{
	return m_vertexRangeBegin;
}

int	DIPrimitive::vertexRangeEnd() const
{
	return m_vertexRangeEnd;
}

int DIPrimitive::indexRangeBegin() const
{
	return m_indexRangeBegin;
}

int DIPrimitive::indexRangeEnd() const
{
	return m_indexRangeEnd;
}

void DIPrimitive::setFormat( const VertexFormat& vf, int vertices, int indices )
{
	assert( vertices > 0 && vertices < 10000000 );
	assert( indices % 3 == 0 && indices < 0x10000 );

	reset();

	m_vf = vf;
	m_vertices = vertices;
	m_indices = (uint16_t)indices;
	m_vertexRangeBegin = 0;
	m_vertexRangeEnd = vertices;
	m_indexRangeBegin = 0;
	m_indexRangeEnd = (uint16_t)indices;

	allocate( vf, vertices, indices );
}

void DIPrimitive::setVertexData( VertexFormat::DataType dt, int index, 
	const void* data, VertexFormat::DataFormat df, int count )
{
	VertexFormat::DataFormat dstdf = vf().getDataFormat(dt);
	assert( VertexFormat::DF_NONE != dstdf );
	if ( VertexFormat::DF_NONE == dstdf )
		return;

	uint8_t* dst;
	int dstpitch;
	getVertexDataPtr( dt, &dst, &dstpitch );
	dst += dstpitch*index;

	VertexFormat::copyData( dst, dstpitch, dstdf, data, VertexFormat::getDataSize(df), df, count );
}

void DIPrimitive::setVertexData( VertexFormat::DataType dt, int index, const float4* data, int count )
{
	setVertexData( dt, index, data, VertexFormat::DF_V4_32, count );
}

void DIPrimitive::setIndexData( int v0, const void* data, int indexsize, int count )
{
	assert( locked() & LOCK_WRITE );
	assert( v0 >= 0 && count > 0 && v0+count <= m_indices );
	assert( indexsize == 1 || indexsize == 2 || indexsize == 4 );

	uint16_t* d = 0;
	int isize = 0;
	getIndexDataPtr( &d, &isize );
	assert( isize == 2 );
	d += v0;

	switch ( indexsize )
	{
	case 1:{
		const uint8_t* s = reinterpret_cast<const uint8_t*>(data);
		for ( int i = 0 ; i < count ; ++i )
			d[i] = s[i];
		break;}

	case 2:{
		const uint16_t* s = reinterpret_cast<const uint16_t*>(data);
		for ( int i = 0 ; i < count ; ++i )
			d[i] = s[i];
		break;}

	case 4:{
		const uint32_t* s = reinterpret_cast<const uint32_t*>(data);
		for ( int i = 0 ; i < count ; ++i )
		{
			d[i] = (uint16_t)s[i];
			assert( d[i] == s[i] );
		}
		break;}
	}
}

void DIPrimitive::getVertexData( VertexFormat::DataType dt, int index, float4* data, int count ) const
{
	assert( locked() & LOCK_READ );
	assert( index >= 0 && count > 0 && index+count <= m_vertices );
	assert( (unsigned)dt < (unsigned)VertexFormat::DT_SIZE );
	
	uint8_t* src = 0;
	int pitch = 0;
	const_cast<DIPrimitive*>(this)->getVertexDataPtr( dt, &src, &pitch );
	src += index * pitch;
	VertexFormat::DataFormat df = m_vf.getDataFormat( dt );
	
	VertexFormat::copyData( data, sizeof(float4), VertexFormat::DF_V4_32, src, pitch, df, count );
}

void DIPrimitive::setVertexPositions( int index, const float4* data, int count )
{
	setVertexData( VertexFormat::DT_POSITION, index, data, count );
}

void DIPrimitive::setVertexTransformedPositions( int index, const float4* data, int count )
{
	setVertexData( VertexFormat::DT_POSITIONT, index, data, count );
}

void DIPrimitive::setVertexNormals( int index, const float4* data, int count )
{
	setVertexData( VertexFormat::DT_NORMAL, index, data, count );
}

void DIPrimitive::setVertexDiffuseColors( int index, const float4* data, int count )
{
	setVertexData( VertexFormat::DT_DIFFUSE, index, data, count );
}

void DIPrimitive::setVertexSpecularColors( int index, const float4* data, int count )
{
	setVertexData( VertexFormat::DT_SPECULAR, index, data, count );
}

void DIPrimitive::setVertexTangents( int index, const float4* data, int count )
{
	setVertexData( VertexFormat::DT_TANGENT, index, data, count );
}

void DIPrimitive::setVertexBoneWeights( int index, const float4* data, int count )
{
	setVertexData( VertexFormat::DT_BONEWEIGHTS, index, data, count );
}

void DIPrimitive::setVertexBoneIndices( int index, const float4* data, int count )
{
	setVertexData( VertexFormat::DT_BONEINDICES, index, data, count );
}

void DIPrimitive::setVertexTextureCoordinates( int index, int layer, const float4* data, int count )
{
	assert( layer >= 0 && layer < m_vf.textureCoordinates() );

	VertexFormat::DataType dt = (VertexFormat::DataType)(VertexFormat::DT_TEX0 + layer);
	setVertexData( dt, index, data, count );
}

void DIPrimitive::setIndices( int index, const int* data, int count )
{
	uint16_t* d = 0;
	int indexsize = 0;
	getIndexDataPtr( &d, &indexsize );
	assert( indexsize == 2 );

	d += index;
	for ( int i = 0 ; i < count ; ++i )
	{
		uint16_t ix = (uint16_t)data[i];
		assert( (int)ix == data[i] );		// ensure output == input
		assert( (int)ix < m_vertices );
		d[i] = ix;
	}
}

int DIPrimitive::vertexSize() const
{
	int totalsize = 0;
	for ( int i = 0 ; i < VertexFormat::DT_SIZE ; ++i )
	{
		VertexFormat::DataFormat df = m_vf.getDataFormat( (VertexFormat::DataType)i );
		int buffersize = VertexFormat::getDataSize( df );
		totalsize += buffersize;
	}

	assert( totalsize <= 255 && "DIPrimitive supports only max 255 byte vertices" );
	return totalsize;
}

int DIPrimitive::vertexDataSize() const
{
	int totalsize = 0;
	for ( int i = 0 ; i < VertexFormat::DT_SIZE ; ++i )
	{
		VertexFormat::DataFormat df = m_vf.getDataFormat( (VertexFormat::DataType)i );
		int buffersize = VertexFormat::getDataSize( df, m_vertices );
		totalsize += buffersize;
	}
	return totalsize;
}

int DIPrimitive::indexSize() const
{
	return 2;
}

int DIPrimitive::memoryUsed() const
{
	return BUFFER_HEADER_SIZE + vertexDataSize() + indexSize()*indices();
}

void DIPrimitive::getVertexPositions( int index, float4* data, int count ) const
{
	getVertexData( VertexFormat::DT_POSITION, index, data, count );
}

void DIPrimitive::getVertexNormals( int index, float4* data, int count ) const
{
	getVertexData( VertexFormat::DT_NORMAL, index, data, count );
}

void DIPrimitive::getVertexDiffuseColors( int index, float4* data, int count ) const
{
	getVertexData( VertexFormat::DT_DIFFUSE, index, data, count );
}

void DIPrimitive::getVertexSpecularColors( int index, float4* data, int count ) const
{
	getVertexData( VertexFormat::DT_SPECULAR, index, data, count );
}

void DIPrimitive::getVertexTangents( int index, float4* data, int count ) const
{
	getVertexData( VertexFormat::DT_TANGENT, index, data, count );
}

void DIPrimitive::getVertexBoneWeights( int index, float4* data, int count ) const
{
	getVertexData( VertexFormat::DT_BONEWEIGHTS, index, data, count );
}

void DIPrimitive::getVertexBoneIndices( int index, float4* data, int count ) const
{
	getVertexData( VertexFormat::DT_BONEINDICES, index, data, count );
}

void DIPrimitive::getVertexTextureCoordinates( int index, int layer, float4* data, int count ) const
{
	assert( layer >= 0 && layer < m_vf.textureCoordinates() );

	VertexFormat::DataType dt = (VertexFormat::DataType)(VertexFormat::DT_TEX0 + layer);
	getVertexData( dt, index, data, count );
}

void DIPrimitive::getIndices( int index, int* data, int count ) const
{
	uint16_t* s = 0;
	int indexsize = 0;
	const_cast<DIPrimitive*>(this)->getIndexDataPtr( &s, &indexsize );
	assert( indexsize == 2 );

	s += index;
	for ( int i = 0 ; i < count ; ++i )
		data[i] = s[i];
}

void DIPrimitive::packBones()
{
	assert( locked() & LOCK_READ );
	assert( 0 == m_usedBone ); // packBones is destructive and it can be done only once

	if ( m_vf.hasData(VertexFormat::DT_BONEINDICES) )
	{
		// count number of used bones
		bool boneused[MAX_BONES];
		for ( int i = 0 ; i < MAX_BONES ; ++i )
			boneused[i] = false;
		int vertices = this->m_vertices;
		m_usedBones = 0;
		for ( int i = 0 ; i < vertices ; ++i )
		{
			float4 bw, bi;
			getVertexBoneWeights( i, &bw, 1 );
			getVertexBoneIndices( i, &bi, 1 );
			for ( int k = 0 ; k < 4 ; ++k )
			{
				int ix = (int)bi[k];
				assert( ix >= 0 && ix < MAX_BONES );
				if ( bw[k] > 0.f && !boneused[ix] )
				{
					assert( m_usedBones < MAX_BONES );
					boneused[ix] = true;
					++m_usedBones;
				}
			}
		}
		
		// build used bone list
		delete[] m_usedBone; m_usedBone=0;
		m_usedBone = new uint8_t[m_usedBones];
		if ( 0 == m_usedBone )
			throw_OutOfMemoryException();
		int k = 0;
		for ( int i = 0 ; i < MAX_BONES ; ++i )
		{
			if ( boneused[i] )
			{
				assert( k < int(m_usedBones) );
				m_usedBone[k++] = (uint8_t)i;
				assert( m_usedBone[k-1] == i ); // wrapped
			}
		}
		assert( m_usedBones == k );

		// remap vertex bone indices to new bone list
		int bonemap[MAX_BONES];
		for ( int i = 0 ; i < MAX_BONES ; ++i )
			bonemap[i] = 0;
		for ( int i = 0 ; i < (int)m_usedBones ; ++i )
			bonemap[ m_usedBone[i] ] = i;
		for ( int i = 0 ; i < vertices ; ++i )
		{
			float4 bi;
			getVertexBoneIndices( i, &bi, 1 );
			for ( int k = 0 ; k < 4 ; ++k )
			{
				int ix = (int)bi[k];
				assert( ix >= 0 && ix < MAX_BONES );
				bi[k] = (float)bonemap[ix];
			}
			setVertexBoneIndices( i, &bi, 1 );
		}
	}
}

void DIPrimitive::setUsedBones( const uint8_t* usedbonearray, int usedbones )
{
	assert( locked() & LOCK_WRITE );
	assert( usedbones >= 0 && usedbones < 0x100 );

	if ( m_usedBone != 0 )
	{
		delete[] m_usedBone;
		m_usedBone = 0;
	}
	
	m_usedBones = (uint8_t)usedbones;

	if ( usedbones > 0 )
	{
		m_usedBone = new uint8_t[usedbones];
		memcpy( m_usedBone, usedbonearray, usedbones );
	}
}

const uint8_t* DIPrimitive::usedBoneArray() const
{
	return m_usedBone;
}

int DIPrimitive::usedBones() const
{
	return m_usedBones;
}

int DIPrimitive::vertices() const
{
	return m_vertices;
}

int	DIPrimitive::indices() const
{
	return m_indices;
}

const VertexFormat&	DIPrimitive::vertexFormat() const
{
	return m_vf;
}

void DIPrimitive::allocate( const VertexFormat& vf, int vertices, int indices )
{
	assert( m_sysMemData == 0 );
	assert( m_vf == vf );

	// allocate memory for buffers
	int indexdatasize = indexSize() * indices;
	int memsize = BUFFER_HEADER_SIZE + vertexDataSize() + indexdatasize;
	uint8_t* mem = new uint8_t[ memsize ];
	if ( 0 == mem )
		throw_OutOfMemoryException();
	memset( mem, 0, memsize );
	m_sysMemData = reinterpret_cast<uint8_t**>(mem);
	
	// setup vbuffer table
	uint8_t* p = mem + BUFFER_HEADER_SIZE;
	for ( int i = 0 ; i < VertexFormat::DT_SIZE ; ++i )
	{
		VertexFormat::DataType dt = (VertexFormat::DataType)i;
		VertexFormat::DataFormat df = vf.getDataFormat( dt );
		
		m_sysMemData[i] = 0;
		if ( df != VertexFormat::DF_NONE )
		{
			m_sysMemData[i] = p;
			p += VertexFormat::getDataSize(df,vertices);
		}
	}

	// setup index table
	m_sysMemData[VertexFormat::DT_SIZE] = (m_indices>0 ? p : 0);
	p += indexdatasize;
}

void DIPrimitive::deallocate()
{
	if ( 0 != m_sysMemData )
	{
		delete[] reinterpret_cast<uint8_t*>(m_sysMemData);	
		m_sysMemData = 0;
	}
}

void DIPrimitive::getVertexDataPtr( VertexFormat::DataType dt, uint8_t** data, int* pitch )
{
	//Debug::printf( "getVertexDataPtr(%s)\n", VertexFormat::toString(dt) );
	assert( m_vf.hasData(dt) );
	assert( locked() != LOCK_NONE );

	*data = m_sysMemData[dt];
	*pitch = VertexFormat::getDataSize( m_vf.getDataFormat(dt) );
}

void DIPrimitive::getIndexDataPtr( uint16_t** data, int* indexsize )
{
	assert( m_indices > 0 );
	assert( locked() != LOCK_NONE );
	assert( m_sysMemData[VertexFormat::DT_SIZE] != 0 && "No index data" );

	*data = reinterpret_cast<uint16_t*>( m_sysMemData[VertexFormat::DT_SIZE] );
	*indexsize = 2;
}

void DIPrimitive::sortFrontToBack( const float3& refpos, const float4x4& worldtm,
	const float4x4* boneworldtm, int boneworldtmcount, SortBuffer& tmp )
{
	assert( indexCount() > 0 && "Only indexed primitives can be sorted" );

	int tricount = indexCount()/3;
	tmp.reset( tricount+indexCount(), tricount );
	uint16_t* intbuffer = tmp.intBuffer();
	float* floatbuffer = tmp.floatBuffer();

	getTriangleDistances( refpos, worldtm, boneworldtm, boneworldtmcount, intbuffer, floatbuffer, tricount );
	LANG_SORT( intbuffer, intbuffer+tricount, SortLess(floatbuffer) );
	assert( floatbuffer[intbuffer[tricount-1]] >= floatbuffer[intbuffer[0]] );
	reorderTriangles( intbuffer, intbuffer+tricount );
}

void DIPrimitive::sortBackToFront( const float3& refpos, const float4x4& worldtm,
	const float4x4* boneworldtm, int boneworldtmcount, SortBuffer& tmp )
{
	assert( indexCount() > 0 && "Only indexed primitives can be sorted" );

	int tricount = indexCount()/3;
	tmp.reset( tricount+indexCount(), tricount );
	uint16_t* intbuffer = tmp.intBuffer();
	float* floatbuffer = tmp.floatBuffer();

	getTriangleDistances( refpos, worldtm, boneworldtm, boneworldtmcount, intbuffer, floatbuffer, tricount );
	LANG_SORT( intbuffer, intbuffer+tricount, SortGreater(floatbuffer) );
	assert( floatbuffer[intbuffer[tricount-1]] <= floatbuffer[intbuffer[0]] );
	reorderTriangles( intbuffer, intbuffer+tricount );
}

void DIPrimitive::sortInsideOut( SortBuffer& tmp )
{
	assert( indexCount() > 0 && "Only indexed primitives can be sorted" );

	int tricount = indexCount()/3;
	tmp.reset( tricount+indexCount(), tricount );
	uint16_t* intbuffer = tmp.intBuffer();
	float* floatbuffer = tmp.floatBuffer();

	const float4x4 id( 1.f );
	getTriangleDistances( center(), id, 0, 0, intbuffer, floatbuffer, tricount );
	LANG_SORT( intbuffer, intbuffer+tricount, SortLess(floatbuffer) );
	assert( floatbuffer[intbuffer[tricount-1]] >= floatbuffer[intbuffer[0]] );
	reorderTriangles( intbuffer, intbuffer+tricount );
}

void DIPrimitive::sortOutsideIn( SortBuffer& tmp )
{
	assert( indexCount() > 0 && "Only indexed primitives can be sorted" );

	int tricount = indexCount()/3;
	tmp.reset( tricount+indexCount(), tricount );
	uint16_t* intbuffer = tmp.intBuffer();
	float* floatbuffer = tmp.floatBuffer();

	const float4x4 id( 1.f );
	getTriangleDistances( center(), id, 0, 0, intbuffer, floatbuffer, tricount );
	LANG_SORT( intbuffer, intbuffer+tricount, SortGreater(floatbuffer) );
	assert( floatbuffer[intbuffer[tricount-1]] <= floatbuffer[intbuffer[0]] );
	reorderTriangles( intbuffer, intbuffer+tricount );
}

float3 DIPrimitive::center() const
{
	assert( locked() != LOCK_NONE );
	
	float3 boxmax( -Float::MAX_VALUE, -Float::MAX_VALUE, -Float::MAX_VALUE );
	float3 boxmin( Float::MAX_VALUE, Float::MAX_VALUE, Float::MAX_VALUE );
	
	uint8_t* vposdata = 0;
	int vpospitch = 0;
	int vertices = m_vertices;
	const_cast<DIPrimitive*>(this)->getVertexDataPtr( VertexFormat::DT_POSITION, &vposdata, &vpospitch );
	VertexFormat::DataFormat vposdatafmt = m_vf.getDataFormat( VertexFormat::DT_POSITION );

	for ( int i = 0 ; i < vertices ; ++i )
	{
		float4 v;
		VertexFormat::getData( vposdatafmt, vposdata, &v );

		for ( int k = 0 ; k < 3 ; ++k )
		{
			boxmax[k] = Math::max( boxmax[k], v[k] );
			boxmin[k] = Math::min( boxmin[k], v[k] );
		}

		vposdata += vpospitch;
	}

	return (boxmax + boxmin) * .5f;
}

void DIPrimitive::getTriangleDistances( const NS(math,float3)& worldpos, const NS(math,float4x4)& worldtm, const NS(math,float4x4)* boneworldtm, int boneworldtmcount, uint16_t* trix, float* tridist, int tricount ) const
{
	assert( tricount == (indexCount() > 0 ? indexCount()/3 : vertexCount()/3) && "Invalid triangle count" );
	assert( locked() != LOCK_NONE );
	assert( boneworldtmcount >= 0 && boneworldtmcount < 256 ); boneworldtmcount=boneworldtmcount;

	uint8_t* vposdata = 0;
	int vpospitch = 0;
	VertexFormat::DataFormat vposdatafmt = m_vf.getDataFormat( VertexFormat::DT_POSITION );
	const_cast<DIPrimitive*>(this)->getVertexDataPtr( VertexFormat::DT_POSITION, &vposdata, &vpospitch );

	uint8_t* vboneindexdata = 0;
	uint8_t* vboneweightdata = 0;
	int vboneindexpitch = 0;
	int vboneweightpitch = 0;
	VertexFormat::DataFormat vboneindexdatafmt = m_vf.getDataFormat( VertexFormat::DT_BONEINDICES );
	VertexFormat::DataFormat vboneweightdatafmt = m_vf.getDataFormat( VertexFormat::DT_BONEWEIGHTS );
	if ( m_vf.hasData(VertexFormat::DT_BONEWEIGHTS) )
	{
		const_cast<DIPrimitive*>(this)->getVertexDataPtr( VertexFormat::DT_BONEINDICES, &vboneindexdata, &vboneindexpitch );
		const_cast<DIPrimitive*>(this)->getVertexDataPtr( VertexFormat::DT_BONEWEIGHTS, &vboneweightdata, &vboneweightpitch );
	}

	float4 v0,v1,v2,v;
	float4 worldpos4(worldpos,1.f);
	float4 v0bi,v1bi,v2bi;
	float4 v0bw,v1bw,v2bw;

	if ( indexCount() > 0 )
	{
		uint16_t* indexdata;
		int indexsize;
		const_cast<DIPrimitive*>(this)->getIndexDataPtr( &indexdata, &indexsize );

		if ( m_vf.hasData(VertexFormat::DT_BONEWEIGHTS) && boneworldtm != 0 )
		{
			// indexed and skinned
			const int WEIGHTS_USED = 2;
			int i = 0;
			for ( int tri = 0 ; tri < tricount ; ++tri )
			{
				VertexFormat::getData( vposdatafmt, vposdata+vpospitch*indexdata[i], &v0 );
				VertexFormat::getData( vposdatafmt, vposdata+vpospitch*indexdata[i+1], &v1 );
				VertexFormat::getData( vposdatafmt, vposdata+vpospitch*indexdata[i+2], &v2 );
				VertexFormat::getData( vboneindexdatafmt, vboneindexdata+vboneindexpitch*indexdata[i], &v0bi );
				VertexFormat::getData( vboneindexdatafmt, vboneindexdata+vboneindexpitch*indexdata[i+1], &v1bi );
				VertexFormat::getData( vboneindexdatafmt, vboneindexdata+vboneindexpitch*indexdata[i+2], &v2bi );
				VertexFormat::getData( vboneweightdatafmt, vboneweightdata+vboneweightpitch*indexdata[i], &v0bw );
				VertexFormat::getData( vboneweightdatafmt, vboneweightdata+vboneweightpitch*indexdata[i+1], &v1bw );
				VertexFormat::getData( vboneweightdatafmt, vboneweightdata+vboneweightpitch*indexdata[i+2], &v2bw );
				i += 3;

				v0.w = v1.w = v2.w = 1.f;
				v.x = v.y = v.z = 0.f;
				float4 sumw(0,0,0,0);
				for ( int k = 0 ; k < WEIGHTS_USED ; ++k )
				{
					int ix0 = (int)v0bi[k];
					int ix1 = (int)v1bi[k];
					int ix2 = (int)v2bi[k];
					assert( ix0 >= 0 && ix0 < boneworldtmcount );
					assert( ix1 >= 0 && ix1 < boneworldtmcount );
					assert( ix2 >= 0 && ix2 < boneworldtmcount );
					if ( k == (WEIGHTS_USED-1) )
					{
						v0bw[k] = 1.f - sumw[0];
						v1bw[k] = 1.f - sumw[1];
						v2bw[k] = 1.f - sumw[2];
					}
					else
					{
						sumw[0] += v0bw[k];
						sumw[1] += v1bw[k];
						sumw[2] += v2bw[k];
					}

					{
						const float4x4& m = boneworldtm[ix0];
						float4 r0( m(0,0), m(0,1), m(0,2), 0.f );
						float4 r1( m(1,0), m(1,1), m(1,2), 0.f );
						float4 r2( m(2,0), m(2,1), m(2,2), 0.f );
						float4 r3( m(3,0), m(3,1), m(3,2), 0.f );
						v += float4( dot(r0,v0)+r3.x, dot(r1,v0)+r3.y, dot(r2,v0)+r3.z, 0.f ) * v0bw[k] * (1.f/3.f);
					}

					{
						const float4x4& m = boneworldtm[ix1];
						float4 r0( m(0,0), m(0,1), m(0,2), 0.f );
						float4 r1( m(1,0), m(1,1), m(1,2), 0.f );
						float4 r2( m(2,0), m(2,1), m(2,2), 0.f );
						float4 r3( m(3,0), m(3,1), m(3,2), 0.f );
						v += float4( dot(r0,v1)+r3.x, dot(r1,v1)+r3.y, dot(r2,v1)+r3.z, 0.f ) * v1bw[k] * (1.f/3.f);
					}

					{
						const float4x4& m = boneworldtm[ix2];
						float4 r0( m(0,0), m(0,1), m(0,2), 0.f );
						float4 r1( m(1,0), m(1,1), m(1,2), 0.f );
						float4 r2( m(2,0), m(2,1), m(2,2), 0.f );
						float4 r3( m(3,0), m(3,1), m(3,2), 0.f );
						v += float4( dot(r0,v2)+r3.x, dot(r1,v2)+r3.y, dot(r2,v2)+r3.z, 0.f ) * v2bw[k] * (1.f/3.f);
					}
				}
				v.w = 1.f;

				trix[tri] = (uint16_t)tri;
				tridist[tri] = (v-worldpos4).lengthSquared();
			}
		}
		else
		{
			// indexed and non-skinned
			int i = 0;
			for ( int tri = 0 ; tri < tricount ; ++tri )
			{
				VertexFormat::getData( vposdatafmt, vposdata+vpospitch*indexdata[i], &v0 );
				VertexFormat::getData( vposdatafmt, vposdata+vpospitch*indexdata[i+1], &v1 );
				VertexFormat::getData( vposdatafmt, vposdata+vpospitch*indexdata[i+2], &v2 );
				i += 3;

				v = (v0+v1+v2)*(1.f/3.f);
				v.w = 1.f;
				v = worldtm.transform( v );
				trix[tri] = (uint16_t)tri;
				tridist[tri] = (v-worldpos4).lengthSquared();
			}
		}
	}
	else
	{
		const int vpospitch2 = vpospitch+vpospitch;
		const int vpospitch3 = vpospitch+vpospitch2;
		const int vboneindexpitch2 = vboneindexpitch+vboneindexpitch;
		const int vboneweightpitch2 = vboneweightpitch+vboneweightpitch;

		if ( m_vf.hasData(VertexFormat::DT_BONEWEIGHTS) )
		{
			// non-indexed and skinned
			const int WEIGHTS_USED = 2;
			for ( int tri = 0 ; tri < tricount ; ++tri )
			{
				VertexFormat::getData( vposdatafmt, vposdata+vpospitch, &v0 );
				VertexFormat::getData( vposdatafmt, vposdata+vpospitch2, &v1 );
				VertexFormat::getData( vposdatafmt, vposdata+vpospitch3, &v2 );
				VertexFormat::getData( vboneindexdatafmt, vboneindexdata, &v0bi );
				VertexFormat::getData( vboneindexdatafmt, vboneindexdata+vboneindexpitch, &v1bi );
				VertexFormat::getData( vboneindexdatafmt, vboneindexdata+vboneindexpitch2, &v2bi );
				VertexFormat::getData( vboneweightdatafmt, vboneweightdata, &v0bw );
				VertexFormat::getData( vboneweightdatafmt, vboneweightdata+vboneweightpitch, &v1bw );
				VertexFormat::getData( vboneweightdatafmt, vboneweightdata+vboneweightpitch2, &v2bw );

				v0.w = v1.w = v2.w = 1.f;
				v.x = v.y = v.z = 0.f;
				float4 sumw(0,0,0,0);
				for ( int k = 0 ; k < WEIGHTS_USED ; ++k )
				{
					int ix0 = (int)v0bi[k];
					int ix1 = (int)v1bi[k];
					int ix2 = (int)v2bi[k];
					assert( ix0 >= 0 && ix0 < boneworldtmcount );
					assert( ix1 >= 0 && ix1 < boneworldtmcount );
					assert( ix2 >= 0 && ix2 < boneworldtmcount );
					if ( k == (WEIGHTS_USED-1) )
					{
						v0bw[k] = 1.f - sumw[0];
						v1bw[k] = 1.f - sumw[1];
						v2bw[k] = 1.f - sumw[2];
					}
					else
					{
						sumw[0] += v0bw[k];
						sumw[1] += v1bw[k];
						sumw[2] += v2bw[k];
					}

					{
						const float4x4& m = boneworldtm[ix0];
						float4 r0( m(0,0), m(0,1), m(0,2), 0.f );
						float4 r1( m(1,0), m(1,1), m(1,2), 0.f );
						float4 r2( m(2,0), m(2,1), m(2,2), 0.f );
						float4 r3( m(3,0), m(3,1), m(3,2), 0.f );
						v += float4( dot(r0,v0)+r3.x, dot(r1,v0)+r3.y, dot(r2,v0)+r3.z, 0.f ) * v0bw[k] * (1.f/3.f);
					}

					{
						const float4x4& m = boneworldtm[ix1];
						float4 r0( m(0,0), m(0,1), m(0,2), 0.f );
						float4 r1( m(1,0), m(1,1), m(1,2), 0.f );
						float4 r2( m(2,0), m(2,1), m(2,2), 0.f );
						float4 r3( m(3,0), m(3,1), m(3,2), 0.f );
						v += float4( dot(r0,v1)+r3.x, dot(r1,v1)+r3.y, dot(r2,v1)+r3.z, 0.f ) * v1bw[k] * (1.f/3.f);
					}

					{
						const float4x4& m = boneworldtm[ix2];
						float4 r0( m(0,0), m(0,1), m(0,2), 0.f );
						float4 r1( m(1,0), m(1,1), m(1,2), 0.f );
						float4 r2( m(2,0), m(2,1), m(2,2), 0.f );
						float4 r3( m(3,0), m(3,1), m(3,2), 0.f );
						v += float4( dot(r0,v2)+r3.x, dot(r1,v2)+r3.y, dot(r2,v2)+r3.z, 0.f ) * v2bw[k] * (1.f/3.f);
					}
				}
				v.w = 1.f;

				trix[tri] = (uint16_t)tri;
				tridist[tri] = (v-worldpos4).lengthSquared();
			}
		}
		else
		{
			// non-indexed and non-skinned
			for ( int tri = 0 ; tri < tricount ; ++tri )
			{
				VertexFormat::getData( vposdatafmt, vposdata, &v0 );
				VertexFormat::getData( vposdatafmt, vposdata+vpospitch, &v1 );
				VertexFormat::getData( vposdatafmt, vposdata+vpospitch2, &v2 );
				v = (v0+v1+v2)*(1.f/3.f);
				v.w = 1.f;
				v = worldtm.transform( v );
				trix[tri] = (uint16_t)tri;
				tridist[tri] = (v-worldpos4).lengthSquared();
				vposdata += vpospitch3;
			}
		}
	}
}

void DIPrimitive::reorderTriangles( const uint16_t* order, uint16_t* buffer )
{
	assert( indexCount() > 0 && "Only indexed primitives can be re-ordered!" );

	if ( indexCount() > 0 )
	{
		int tricount = indexCount()/3;

		uint16_t* indexdata;
		int indexsize;
		getIndexDataPtr( &indexdata, &indexsize );

		int ind = 0;
		for ( int i = 0 ; i < tricount ; ++i )
		{
			int newtri = order[i];
			assert( newtri >= 0 && newtri < tricount );
			int newind = unsigned(newtri) + unsigned(newtri)*2U;

			buffer[ind] = indexdata[newind];
			buffer[ind+1] = indexdata[newind+1];
			buffer[ind+2] = indexdata[newind+2];
			ind += 3;
		}

		ind = 0;
		for ( int i = 0 ; i < tricount ; ++i )
		{
			indexdata[ind] = buffer[ind];
			indexdata[ind+1] = buffer[ind+1];
			indexdata[ind+2] = buffer[ind+2];
			ind += 3;
		}
	}
}

void DIPrimitive::findSplit( int maxbones,
	Array<int>&	faceset1, Array<int>& faceset2 ) const
{
	assert( usedBones() > 0 );

	Lock lk( const_cast<DIPrimitive*>(this), LOCK_READ );

	Array<bool> boneset;
	int			bonesetsize = 0;
	Array<int>	facebones;
	Array<int>	newfacebones;

	// prepare bone set
	boneset.resize( usedBones(), false );
	faceset1.clear();
	faceset2.clear();

	// prepare source list of faceset1
	const int triangles = indices()/3;
	for ( int i = 0 ; i < triangles ; ++i )
		faceset2.add( i );

	// while room for more bones
	while ( faceset2.size() > 0 )
	{
		// select first face which expands bone set least
		int newface = -1;
		newfacebones.resize( usedBones() );
		for ( int i = 0 ; i < faceset2.size() ; ++i )
		{
			int face = faceset2[i];
			int ind[3];
			getIndices( face*3, ind, 3 );
			facebones.clear();
			for ( int k = 0 ; k < 3 ; ++k )
			{
				float4 boneind, bonew;
				getVertexBoneIndices( ind[k], &boneind, 1 );
				getVertexBoneWeights( ind[k], &bonew, 1 );
				for ( int n = 0 ; n < 4 ; ++n )
				{
					if ( bonew[n] > 0.f )
					{
						int boneix = (int)boneind[n];
						if ( !boneset[boneix] )
							facebones.add( boneix );
					}
				}
			}

			LANG_SORT( facebones.begin(), facebones.end() );
			facebones.resize( unique(facebones.begin(),facebones.end())-facebones.begin() );

			if ( facebones.size() < newfacebones.size() )
			{
				newface = i;
				newfacebones = facebones;
			}
		}

		// check if max bone count would be exceeded
		if ( bonesetsize+newfacebones.size() > maxbones )
			break;

		// expand bone set
		for ( int i = 0 ; i < newfacebones.size() ; ++i )
		{
			assert( !boneset[ newfacebones[i] ] && "Bone already used?" );
			boneset[ newfacebones[i] ] = true;
			++bonesetsize;
		}

		// resize face sets
		int face = faceset2[newface];
		faceset2.remove( newface );
		faceset1.add( face );
	}
}

void DIPrimitive::setVertexPositionScaleBias( const NS(math,float4)& scalebias )
{
	m_posScaleBias = scalebias;
}

void DIPrimitive::setVertexTextureCoordinateScaleBias( const NS(math,float4)& scalebias )
{
	m_texcoordScaleBias = scalebias;
}

const float4& DIPrimitive::vertexTextureCoordinateScaleBias() const
{
	return m_texcoordScaleBias;
}

const float4& DIPrimitive::vertexPositionScaleBias() const
{
	return m_posScaleBias;
}

const float3& DIPrimitive::boundMin() const
{
	return m_boundMin;
}

const float3& DIPrimitive::boundMax() const
{
	return m_boundMax;
}

float DIPrimitive::boundRadius() const
{
	return m_boundRadius;
}

void DIPrimitive::setBound( const float3& boundmin, const float3& boundmax, float boundradius )
{
	m_boundMin = boundmin;
	m_boundMax = boundmax;
	m_boundRadius = boundradius;
}


END_NAMESPACE() // gr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
