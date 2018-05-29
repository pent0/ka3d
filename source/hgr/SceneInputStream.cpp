#include <hgr/SceneInputStream.h>
#include <hgr/Scene.h>
#include <gr/Context.h>
#include <gr/impl/DIPrimitive.h>
#include <io/IOException.h>
#include <lang/Math.h>
#include <lang/Debug.h>
#include <math/float3x4.h>
#include <math/quaternion.h>
#include <string.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(io)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(hgr) 


const int SceneInputStream::MIN_VERSION = 170;
const int SceneInputStream::MAX_VERSION = 193;


SceneInputStream::SceneInputStream( InputStream* in ) :
	DataInputStream( in ),
	m_ver( 0 ),
	m_dataFlags( 0 ),
	m_platformID( Context::PLATFORM_DX )
{
	char magic[4];
	read( magic, sizeof(magic) );
	if ( memcmp(magic,"hgrf",4) )
		throwError( IOException( Format("Scene file \"{0}\" does not start with 'hgrf' tag", toString()) ) );
	
	m_ver = readShort();
	if ( m_ver < MIN_VERSION || m_ver > MAX_VERSION )
		throwError( IOException( Format("Scene file \"{0}\" is version {1}.{2}, should be {3}.{4}", toString(), m_ver/100, m_ver%100, MAX_VERSION/100, MAX_VERSION%100) ) );

	if ( m_ver > 190 )
		m_exporterVer = readInt();
	else
		m_exporterVer = 0x020903;

	m_dataFlags = readShort();

	if ( m_ver >= 180 )
		m_platformID = (Context::PlatformType)readShort();
}

SceneInputStream::~SceneInputStream()
{
}

float3 SceneInputStream::readFloat3()
{
	float3 obj;
	obj.x = readFloat();
	obj.y = readFloat();
	obj.z = readFloat();
	return obj;
}

float4 SceneInputStream::readFloat4()
{
	float4 obj;
	obj.x = readFloat();
	obj.y = readFloat();
	obj.z = readFloat();
	obj.w = readFloat();
	return obj;
}

quaternion SceneInputStream::readQuaternion()
{
	quaternion obj;
	obj.x = readFloat();
	obj.y = readFloat();
	obj.z = readFloat();
	obj.w = readFloat();
	return obj;
}

float3x4 SceneInputStream::readFloat3x4()
{
	float3x4 obj;
	for ( int i = 0 ; i < float3x4::ROWS ; ++i )
		for ( int j = 0 ; j < float3x4::COLUMNS ; ++j )
			obj(i,j) = readFloat();
	return obj;
}

void SceneInputStream::readNode( Node* node, int* parentindex )
{
	node->setName( readUTF() );
	node->setTransform( readFloat3x4() );
	
	// flags are read from file, but *only* flags which don't affect class id
	// (this is done like this because class id definitions changed recently and
	// old exported files wouldn't be compatible otherwise)
	int flags = readInt();
	flags &= ~Node::NODE_CLASS;
	flags |= node->flags() & Node::NODE_CLASS;
	node->setFlags( flags );

	if ( version() > 190 )
		node->setID( readInt() );
	*parentindex = readInt();
}

P(TransformAnimation) SceneInputStream::readTransformAnimation()
{
	int poskeyrate = readByte();
	int rotkeyrate = readByte();
	int sclkeyrate = readByte();
	TransformAnimation::BehaviourType endbehaviour = (TransformAnimation::BehaviourType)readByte();

	if ( endbehaviour >= TransformAnimation::BEHAVIOUR_COUNT )
		throwError( IOException( Format("Failed to load scene \"{0}\". Invalid transform animation end behaviour ({1}).", toString(), int(endbehaviour)) ) );

	bool isoptimized = false;
	if ( version() >= 192 )
		isoptimized = readBoolean();

	P(TransformAnimation) obj;
	if ( !isoptimized )
	{
		P(KeyframeSequence) pos = readKeyframeSequence();
		P(KeyframeSequence) rot = readKeyframeSequence();
		P(KeyframeSequence) scl = readKeyframeSequence();
		obj = new TransformAnimation( endbehaviour, pos, rot, scl, poskeyrate, rotkeyrate, sclkeyrate );
	}
	else
	{
		P(TransformAnimation::Float3Anim) pos = readFloat3Anim();
		P(KeyframeSequence) rot = readKeyframeSequence();
		P(TransformAnimation::Float3Anim) scl = readFloat3Anim();
		
		float endtime = 0.f;
		if ( version() >= 193 )
		{
			endtime = readFloat();
		}
		else
		{
			endtime = float(rot->keys()) * float(rotkeyrate);
		}

		obj = new TransformAnimation( endbehaviour, pos, rot, scl, poskeyrate, rotkeyrate, sclkeyrate, endtime );
	}
	return obj;
}

void SceneInputStream::readFloat4Array16( float4* out, int count )
{
	if ( count > 2 )
	{
		float4 minv = readFloat4();
		float4 maxv = readFloat4();

		float4 delta;
		for ( int k = 0 ; k < 4 ; ++k )
			delta[k] = maxv[k] - minv[k];

		for ( int i = 0 ; i < count ; ++i )
		{
			for ( int k = 0 ; k < 4 ; ++k )
			{
				int xi = (int)(uint16_t)readShort();
				float x = float(xi) * (1.f/65535.f);
				x *= delta[k];
				x += minv[k];
				out[i][k] = x;
			}
		}
	}
	else
	{
		for ( int i = 0 ; i < count ; ++i )
			out[i] = readFloat4();
	}
}

void SceneInputStream::readFloat3Array16( float3* out, int count )
{
	if ( count > 2 )
	{
		float3 minv = readFloat3();
		float3 maxv = readFloat3();

		float3 delta;
		for ( int k = 0 ; k < 3 ; ++k )
			delta[k] = maxv[k] - minv[k];

		for ( int i = 0 ; i < count ; ++i )
		{
			for ( int k = 0 ; k < 3 ; ++k )
			{
				int xi = (int)(uint16_t)readShort();
				float x = float(xi) * (1.f/65535.f);
				x *= delta[k];
				x += minv[k];
				out[i][k] = x;
			}
		}
	}
	else
	{
		for ( int i = 0 ; i < count ; ++i )
			out[i] = readFloat3();
	}
}

P(TransformAnimation::Float3Anim) SceneInputStream::readFloat3Anim()
{
	P(TransformAnimation::Float3Anim) anim = new TransformAnimation::Float3Anim;
	int keycount = readInt();
	anim->keys.resize( keycount );
	readFloat4Array16( &anim->keys[0], keycount );
	return anim;
}

P(KeyframeSequence) SceneInputStream::readKeyframeSequence()
{
	P(KeyframeSequence) obj = 0;
	int keys = readInt();

	if ( version() < 192 )
	{
		char buf[256];
		readUTF( buf, sizeof(buf) );
		VertexFormat::DataFormat format = VertexFormat::toDataFormat( buf );

		float scale = readFloat();
		float4 bias = readFloat4();

		if ( 0 ==keys )
			return 0;

		obj = new KeyframeSequence( keys, format );
		obj->setScale( scale );
		obj->setBias( bias );
		readFully( obj->data(), VertexFormat::getDataSize(format,keys) );
	}
	else
	{
		int dim = readInt();
		if ( dim != 4 && dim != 3 )
			throwError( IOException( Format("Keyframe sequence in {0} dimension invalid ({1})", toString(), dim) ) );

		if ( dim == 4 )
		{
			obj = new KeyframeSequence( keys, VertexFormat::DF_V4_32 );
			readFloat4Array16( (float4*)obj->data(), keys );
		}
		else
		{
			obj = new KeyframeSequence( keys, VertexFormat::DF_V3_32 );
			readFloat3Array16( (float3*)obj->data(), keys );
		}
	}
	return obj;
}

void SceneInputStream::readVertexFormat( VertexFormat* vf )
{
	char buf[256];
	int count = readByte();
	for ( int i = 0 ; i < count ; ++i )
	{
		readUTF( buf, sizeof(buf) );
		VertexFormat::DataType dt = VertexFormat::toDataType( buf );
		if ( dt == VertexFormat::DT_SIZE )
			throwError( IOException( Format("Invalid vertex data type in \"{0}\": {1}", toString(), buf) ) );

		readUTF( buf, sizeof(buf) );
		VertexFormat::DataFormat df = VertexFormat::toDataFormat( buf );
		if ( df == VertexFormat::DF_SIZE )
			throwError( IOException( Format("Invalid vertex data format in \"{0}\": {1}", toString(), buf) ) );

		vf->setDataFormat( dt, df );
	}
}

P(Primitive) SceneInputStream::readPrimitive( Context* context, int* materialindex )
{
    int verts = 0;
	int inds = 0;
	if ( m_ver < 190 )
	{
		verts = (uint16_t)readShort();
		if ( verts < 0 || verts > 0x10000 )
			throwError( IOException( Format("Failed to load scene \"{0}\". Invalid vertex count ({1}).", toString(), verts) ) );
		inds = (uint16_t)readShort();
		if ( inds < 0 || inds > 0xFFFF )
			throwError( IOException( Format("Failed to load scene \"{0}\". Invalid face index count ({1}).", toString(), inds) ) );
	}
	else // m_ver >= 190
	{
		assert( m_ver >= 190 );
		verts = (uint32_t)readInt();
		if ( verts < 0 || verts >= 0x10000 )
			throwError( IOException( Format("Failed to load scene \"{0}\". Invalid vertex count ({1}).", toString(), verts) ) );
		inds = (uint32_t)readInt();
		if ( inds < 0 )
			throwError( IOException( Format("Failed to load scene \"{0}\". Invalid face index count ({1}).", toString(), inds) ) );
	}

	VertexFormat vf;
	readVertexFormat( &vf );

	*materialindex = readShort();

	Primitive::PrimType type = Primitive::PRIM_TRI;
	type = (Primitive::PrimType)readShort();
	if ( int(type) < 0 || int(type) >= Primitive::PRIM_INVALID )
		throwError( IOException( Format("Failed to load scene \"{0}\". Primitive type ({1}) invalid", toString(), int(type)) ) );

	P(Primitive) prim = context->createPrimitive( type, vf, verts, inds, Context::USAGE_STATIC );
	if ( vf != prim->vertexFormat() )
		throwError( IOException( Format("Failed to load scene \"{0}\". Primitive vertex format ({1}) should be ({2})", toString(), vf.toString(), prim->vertexFormat().toString()) ) );

	Primitive::Lock lk( prim, Primitive::LOCK_WRITE );

	float4 posscalebias(1,0,0,0);
	float4 uvscalebias(1,0,0,0);
	if ( m_ver >= 190 )
	{
		posscalebias = readFloat4();
		uvscalebias = readFloat4();
		prim->setVertexPositionScaleBias( posscalebias );
		prim->setVertexTextureCoordinateScaleBias( uvscalebias );
	}

	Array<uint8_t> buf;
	for ( int k = 0 ; k < (int)VertexFormat::DT_SIZE ; ++k )
	{
		VertexFormat::DataType dt = (VertexFormat::DataType)k;
		VertexFormat::DataFormat df = vf.getDataFormat( dt );

		if ( VertexFormat::DF_NONE != df )
		{
			if ( m_ver < 190 )
			{
				// dummy scale + bias4
				readFloat();
				readFloat4();
			}

			buf.resize( verts*VertexFormat::getDataSize(df) );
			readFully( &buf[0], buf.size() );

			// negative tiling test
			/*if ( dt == VertexFormat::DT_TEX0 )
			{
				int16_t* p = (int16_t*)&buf[0];
				for ( int i = 0 ; i < verts*2 ; ++i )
					p[i] = -p[i];
			}*/

			prim->setVertexData( dt, 0, &buf[0], df, verts );

			if ( dt == VertexFormat::DT_POSITION )
			{
				float4 boundmin,boundmax;
				float boundradius;
				VertexFormat::getBound( &buf[0], df, verts, posscalebias, &boundmin, &boundmax, &boundradius );

				/*float vmax = 0;
				for ( int k = 0 ; k < 3 ; ++k )
				{
					if ( Math::abs(boundmin[k]) > vmax )
						vmax = Math::abs(boundmin[k]);
					if ( Math::abs(boundmax[k]) > vmax )
						vmax = Math::abs(boundmax[k]);
				}
				boundmin = float4(-vmax,-vmax,-vmax,-vmax);
				boundmax = float4(vmax,vmax,vmax,vmax);
				boundradius = vmax;*/

				prim->setBound( boundmin.xyz(), boundmax.xyz(), boundradius );
			}
		}
	}

	if ( inds > 0 )
	{
		uint16_t* indexdata = 0;
		int indexsize = 0;
		prim->getIndexDataPtr( &indexdata, &indexsize );
		if ( indexsize != 2 )
			throwError( IOException( Format("Failed to load scene \"{0}\". Invalid face index size ({1}).", toString(), indexsize) ) );

		// WARNING: Endianess dependent read
		readFully( indexdata, inds*indexsize );

		// check validity of indices
		for ( int k = 0 ; k < inds ; ++k )
		{
			if ( indexdata[k] >= verts )
				throwError( IOException( Format("Failed to load scene \"{0}\". Invalid face vertex index (face {1}).", toString(), k) ) );
		}
	}

	uint8_t usedbonearray[Primitive::MAX_BONES];
	int usedbones = readByte();
	if ( usedbones > Primitive::MAX_BONES )
		throwError( IOException( Format("Failed to load scene \"{0}\". Too many bones ({1}).", toString(), usedbones) ) );
	readFully( usedbonearray, usedbones );
	prim->setUsedBones( usedbonearray, usedbones );
	return prim;
}

P(UserPropertySet) SceneInputStream::readUserPropertySet( Scene* scene )
{
	// read number of user property strings
	int count = readInt();
	if ( count < 0 || count > 1000000 )
		throwError( IOException( Format("Failed to load scene \"{0}\". Invalid user property set size ({1}).", toString(), count) ) );
	if ( 0 == count )
		return 0;
		
	// get access to existing node name strings
	TransformAnimationSet* tmanims = 0;
	if ( scene != 0 )
		tmanims = scene->transformAnimations();

	P(UserPropertySet) ups = new UserPropertySet( count );
	for ( int i = 0 ; i < count ; ++i )
	{
		String key = readUTF();

		// replace key with shared key string to save memory
		// (=same value but shared object)
		if ( tmanims && tmanims->containsKey(key) )
		{
			key = tmanims->getIterator(key).key();
		}
		else if ( scene != 0 )
		{
			for ( Node* node = scene ; node != 0 ; node = node->next(scene) )
			{
				if ( node->name() == key )
				{
					key = node->name();
					break;
				}
			}
		}

		String value = readUTF();
		ups->put( key, value );
	}

	return ups;
}

bool SceneInputStream::hasData( int flags ) const
{
	return 0 != (m_dataFlags & flags);
}

NS(gr,Context)::PlatformType SceneInputStream::platform() const
{
	return m_platformID;
}


END_NAMESPACE() // hgr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
