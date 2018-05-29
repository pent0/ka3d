#include <hgr/SceneOutputStream.h>
#include <gr/Context.h>
#include <gr/Primitive.h>
#include <gr/VertexFormat.h>
#include <io/IOException.h>
#include <hgr/Mesh.h>
#include <hgr/Lines.h>
#include <hgr/Light.h>
#include <hgr/Dummy.h>
#include <hgr/Camera.h>
#include <hgr/UserPropertySet.h>
#include <hgr/TransformAnimation.h>
#include <lang/Float.h>
#include <math/float.h>
#include <math/float3x4.h>
#include <math/quaternion.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(io)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(hgr) 


const int SceneOutputStream::VERSION = 193;


SceneOutputStream::SceneOutputStream( OutputStream* out, int dataflags, NS(gr,Context)::PlatformType platformid, int exporterversion ) :
	DataOutputStream( out ),
	m_dataFlags( dataflags )
{
	writeChars( "hgrf" );
	writeShort( VERSION );
	writeInt( exporterversion );
	writeShort( dataflags );
	writeShort( platformid );
}

SceneOutputStream::~SceneOutputStream()
{
}

void SceneOutputStream::writeFloat3( const float3& obj )
{
	writeFloat( obj.x );
	writeFloat( obj.y );
	writeFloat( obj.z );
}

void SceneOutputStream::writeFloat4( const float4& obj )
{
	writeFloat( obj.x );
	writeFloat( obj.y );
	writeFloat( obj.z );
	writeFloat( obj.w );
}

void SceneOutputStream::writeQuaternion( const quaternion& obj )
{
	writeFloat( obj.x );
	writeFloat( obj.y );
	writeFloat( obj.z );
	writeFloat( obj.w );
}

void SceneOutputStream::writeFloat3x4( const float3x4& obj )
{
	for ( int i = 0 ; i < float3x4::ROWS ; ++i )
		for ( int j = 0 ; j < float3x4::COLUMNS ; ++j )
			writeFloat( obj(i,j) );
}

void SceneOutputStream::writeVertexFormat( const VertexFormat& vf )
{
	int count = 0;
	for ( int i = 0 ; i < VertexFormat::DT_SIZE ; ++i )
	{
		VertexFormat::DataType dt = (VertexFormat::DataType)i;
		if ( vf.hasData(dt) )
			++count;
	}
	writeByte( count );

	for ( int i = 0 ; i < VertexFormat::DT_SIZE ; ++i )
	{
		VertexFormat::DataType dt = (VertexFormat::DataType)i;
		VertexFormat::DataFormat df = vf.getDataFormat(dt);
		if ( df != VertexFormat::DF_NONE )
		{
			writeUTF( VertexFormat::toString(dt) );
			writeUTF( VertexFormat::toString(df) );
		}
	}
}

void SceneOutputStream::writePrimitive( Primitive* obj, const Array<Shader*>& globshaderlist )
{
	Primitive::Lock lock( obj, Primitive::LOCK_READ );

	const int verts = obj->vertices();
	writeInt( verts );
	writeInt( obj->indices() );
	
	VertexFormat vf = obj->vertexFormat();
	writeVertexFormat( vf );

	int matindex = globshaderlist.indexOf( obj->shader() );
	if ( matindex < 0 )
		throwError( IOException( Format("Shader which was used in primitive is not listed in global shader list of scene file \"{0}\"", toString()) ) );
	writeShort( matindex );

	writeShort( obj->type() );

	writeFloat4( obj->vertexPositionScaleBias() );
	writeFloat4( obj->vertexTextureCoordinateScaleBias() );

	for ( int i = 0 ; i < VertexFormat::DT_SIZE ; ++i )
	{
		VertexFormat::DataType dt = (VertexFormat::DataType)i;
		VertexFormat::DataFormat df = vf.getDataFormat( dt );

		if ( VertexFormat::DF_NONE != df )
		{
			uint8_t* data = 0;
			int pitch = 0;
			obj->getVertexDataPtr( dt, &data, &pitch );
			int vsize = VertexFormat::getDataSize( df );
	
			// WARNING: Endianess dependent
			for ( int n = 0 ; n < verts ; ++n )
			{
				write( data, vsize );
				data += pitch;
			}
		}
	}

	uint16_t* indexdata = 0;
	int indexsize = 0;
	obj->getIndexDataPtr( &indexdata, &indexsize );
	if ( indexsize != 2 )
		throwError( IOException( Format("Index size not 2 in scene file \"{0}\"", toString()) ) );
	// WARNING: Endianess dependent
	write( indexdata, indexsize*obj->indices() );
	for ( int i = 0 ; i < obj->indices() ; ++i )
	{
		if ( (int)indexdata[i] >= verts )
			throwError( IOException( Format("Wrote out-of-bounds index data in scene file \"{0}\"", toString()) ) );
	}

	writeByte( obj->usedBones() );
	write( obj->usedBoneArray(), obj->usedBones() );
}

void SceneOutputStream::writeNode( Node* obj, const Array<Node*>& globnodelist )
{
	writeUTF( obj->name() );

	writeFloat3x4( obj->transform() );

	writeInt( obj->flags() );

	writeInt( obj->id() );

	int parentix = -1;
	if ( obj->parent() != 0 )
	{
		parentix = globnodelist.indexOf( obj->parent() );
		if ( parentix < 0 )
			throwError( IOException( Format("Parent node of {0} not in global node list in scene file \"{1}\"", obj->name(), toString()) ) );
	}
	writeInt( parentix );
}

void SceneOutputStream::writeMesh( Mesh* obj,
	const Array<Primitive*>& globprimitivelist,
	const Array<Node*>& globnodelist )
{
	writeNode( obj, globnodelist );

	writeInt( obj->primitives() );
	for ( int i = 0 ; i < obj->primitives() ; ++i )
	{
		int primix = globprimitivelist.indexOf( obj->getPrimitive(i) );
		if ( primix < 0 )
			throwError( IOException( Format("Primitive of {0} not in global primitive list in scene file \"{1}\"", obj->name(), toString()) ) );

		writeInt( primix );
	}

	writeInt( obj->bones() );
	for ( int i = 0 ; i < obj->bones() ; ++i )
	{
		int boneix = globnodelist.indexOf( obj->getBone(i) );
		if ( boneix < 0 )
			throwError( IOException( Format("Bone of {0} not in global node list in scene file \"{1}\"", obj->name(), toString()) ) );

		writeInt( boneix );
		writeFloat3x4( obj->getBoneInverseRestTransform(i) );
	}
}

void SceneOutputStream::writeCamera( Camera* obj, const Array<Node*>& globnodelist )
{
	writeNode( obj, globnodelist );

	writeFloat( obj->front() );
	writeFloat( obj->back() );
	writeFloat( obj->horizontalFov() );
}

void SceneOutputStream::writeLight( Light* obj, const Array<Node*>& globnodelist )
{
	writeNode( obj, globnodelist );

	writeFloat3( obj->color() );
	writeFloat( 0.f );
	writeFloat( 0.f );
	writeFloat( obj->farAttenStart() );
	writeFloat( obj->farAttenEnd() );
	writeFloat( obj->innerCone() );
	writeFloat( obj->outerCone() );
	writeByte( obj->type() );
}

void SceneOutputStream::writeDummy( Dummy* obj, const Array<Node*>& globnodelist )
{
	writeNode( obj, globnodelist );

	writeFloat3( obj->boxMin() );
	writeFloat3( obj->boxMax() );
}

void SceneOutputStream::writeLines( Lines* obj, const Array<Node*>& globnodelist )
{
	writeNode( obj, globnodelist );

	writeInt( obj->lines() );
	writeInt( obj->paths() );

	for ( int i = 0 ; i < obj->lines() ; ++i )
	{
		writeFloat3( obj->getLineStart(i) );
		writeFloat3( obj->getLineEnd(i) );
	}

	for ( int i = 0 ; i < obj->paths() ; ++i )
	{
		writeInt( obj->getPathBegin(i) );
		writeInt( obj->getPathEnd(i) );
	}
}

void SceneOutputStream::writeTransformAnimation( TransformAnimation* obj )
{
	assert( obj->isOptimized() );

	writeByte( obj->positionKeyRate() );
	writeByte( obj->rotationKeyRate() );
	writeByte( obj->scaleKeyRate() );
	writeByte( obj->endBehaviour() );
	writeBoolean( obj->isOptimized() );

	if ( obj->isOptimized() )
	{
		writeFloat3Anim( obj->positionAnimation() );
		writeKeyframeSequence( obj->rotationKeyframeSequence() );
		writeFloat3Anim( obj->scaleAnimation() );
		writeFloat( obj->endTime() );
	}
	else
	{
		writeKeyframeSequence( obj->positionKeyframeSequence() );
		writeKeyframeSequence( obj->rotationKeyframeSequence() );
		writeKeyframeSequence( obj->scaleKeyframeSequence() );
	}
}

void SceneOutputStream::writeFloat4Array16( const float4* a, int count )
{
	if ( count > 2 )
	{
		float4 minv( Float::MAX_VALUE, Float::MAX_VALUE, Float::MAX_VALUE, Float::MAX_VALUE );
		float4 maxv( -Float::MAX_VALUE, -Float::MAX_VALUE, -Float::MAX_VALUE, -Float::MAX_VALUE );

		for ( int i = 0 ; i < count ; ++i )
		{
			for ( int k = 0 ; k < 4 ; ++k )
			{
				float x = a[i][k];
				if ( x < minv[k] )
					minv[k] = x;
				if ( x > maxv[k] )
					maxv[k] = x;
			}
		}
		for ( int k = 0 ; k < 4 ; ++k )
		{
			if ( maxv[k]-minv[k] < 1e-10f )
				maxv[k] = minv[k] + 1e-10f;
		}

		writeFloat4( minv );
		writeFloat4( maxv );

		for ( int i = 0 ; i < count ; ++i )
		{
			for ( int k = 0 ; k < 4 ; ++k )
			{
				float x = a[i][k];
				x -= minv[k];
				x /= maxv[k] - minv[k];
				int xi = (int)(x*65535.f);
				if ( xi > 65535 )
					xi = 65535;
				else if ( xi < 0 )
					xi = 0;
				writeShort( xi );
			}
		}
	}
	else
	{
		for ( int i = 0 ; i < count ; ++i )
			writeFloat4( a[i] );
	}
}

void SceneOutputStream::writeFloat3Array16( const float3* a, int count )
{
	if ( count > 2 )
	{
		float3 minv( Float::MAX_VALUE, Float::MAX_VALUE, Float::MAX_VALUE );
		float3 maxv( -Float::MAX_VALUE, -Float::MAX_VALUE, -Float::MAX_VALUE );

		for ( int i = 0 ; i < count ; ++i )
		{
			for ( int k = 0 ; k < 3 ; ++k )
			{
				float x = a[i][k];
				if ( x < minv[k] )
					minv[k] = x;
				if ( x > maxv[k] )
					maxv[k] = x;
			}
		}
		for ( int k = 0 ; k < 4 ; ++k )
		{
			if ( maxv[k]-minv[k] < 1e-10f )
				maxv[k] = minv[k] + 1e-10f;
		}

		writeFloat3( minv );
		writeFloat3( maxv );

		for ( int i = 0 ; i < count ; ++i )
		{
			for ( int k = 0 ; k < 3 ; ++k )
			{
				float x = a[i][k];
				x -= minv[k];
				x /= maxv[k] - minv[k];
				int xi = (int)(x*65535.f);
				if ( xi > 65535 )
					xi = 65535;
				else if ( xi < 0 )
					xi = 0;
				writeShort( xi );
			}
		}
	}
	else
	{
		for ( int i = 0 ; i < count ; ++i )
			writeFloat3( a[i] );
	}
}

void SceneOutputStream::writeFloat3Anim( TransformAnimation::Float3Anim* obj )
{
	writeInt( obj->keys.size() );
	writeFloat4Array16( &obj->keys[0], obj->keys.size() );
}

void SceneOutputStream::writeKeyframeSequence( KeyframeSequence* obj )
{
	assert( obj->format() == VertexFormat::DF_V4_32 || obj->format() == VertexFormat::DF_V3_32 );

	if ( obj->format() != VertexFormat::DF_V4_32 && obj->format() != VertexFormat::DF_V3_32 )
		throwError( IOException(Format("writeKeyframeSequence supports only V3_32 and V4_32 (tried to write {0})", VertexFormat::toString(obj->format()))) );

	int dim = obj->format() == VertexFormat::DF_V4_32 ? 4 : 3;
	void* data = obj->data();
	int keys = obj->keys();

	writeInt( keys );
	writeInt( dim );
	if ( dim == 4 )
		writeFloat4Array16( (float4*)data, keys );
	if ( dim == 3 )
		writeFloat3Array16( (float3*)data, keys );
	assert( dim == 3 || dim == 4 );
}

void SceneOutputStream::writeUserPropertySet( UserPropertySet* obj )
{
	writeInt( obj->size() );
	for ( HashtableIterator<String,String> it = obj->begin() ; it != obj->end() ; ++it )
	{
		writeUTF( it.key() );
		writeUTF( it.value() );
	}
}

bool SceneOutputStream::hasData( int flags ) const
{
	return 0 != (m_dataFlags & flags);
}


END_NAMESPACE() // hgr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
