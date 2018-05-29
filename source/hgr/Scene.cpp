#include <hgr/Scene.h>
#include <hgr/Camera.h>
#include <hgr/ViewFrustum.h>
#include <hgr/LightSorter.h>
#include <hgr/ResourceManager.h>
#include <hgr/SceneInputStream.h>
#include <hgr/DefaultResourceManager.h>
#include <gr/Context.h>
#include <gr/Texture.h>
#include <gr/Primitive.h>
#include <gr/CubeTexture.h>
#include <gr/VertexFormat.h>
#include <io/PathName.h>
#include <io/IOException.h>
#include <io/PropertyParser.h>
#include <io/FileInputStream.h>
#include <hgr/Mesh.h>
#include <hgr/Lines.h>
#include <hgr/Light.h>
#include <hgr/Dummy.h>
#include <hgr/Camera.h>
#include <hgr/Globals.h>
#include <hgr/TransformAnimation.h>
#include <lang/Math.h>
#include <lang/Float.h>
#include <lang/Debug.h>
#include <math/toString.h>
#include <math/float3x4.h>
#include <math/quaternion.h>
#include <string.h>
#include <stdio.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(io)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(hgr) 


Scene::Scene() :
	m_fogColor( 1.f, 1.f, 1.f ),
	m_fogStart( 0.f ),
	m_fogEnd( 1000.f ),
	m_fogType( FOG_NONE )
{
	setClassId( NODE_SCENE );
}

Scene::Scene( Context* context, const String& filename, ResourceManager* res,
	const String& texturepath, const String& shaderpath, const String& particlepath ) :
	m_fogColor( 1.f, 1.f, 1.f ),
	m_fogStart( 0.f ),
	m_fogEnd( 1000.f ),
	m_fogType( FOG_NONE )
{
	setClassId( NODE_SCENE );
	setName( filename );

	// make sure we have some resource manager
	if ( !res )
		res = DefaultResourceManager::get( context );

	FileInputStream fin( filename );
	SceneInputStream in( &fin );

	if ( in.platform() != context->platform() )
		throwError( IOException( Format("Cannot load scene file {0}, invalid platform (file:{1}, current:{2})", filename, Context::getString(in.platform()), context->platformString()) ) );

	// directories to read data from
	// (char arrays used as memory usage optimization)
	PathName parentpath = PathName(filename).parent();

	char shaderpathsz[PathName::MAXLEN];
	if ( shaderpath != "" )
		shaderpath.get( shaderpathsz, sizeof(shaderpathsz) );
	else
		String::cpy( shaderpathsz, sizeof(shaderpathsz), parentpath.toString() );
	
	char texturepathsz[PathName::MAXLEN];
	if ( texturepath != "" )
		texturepath.get( texturepathsz, sizeof(texturepathsz) );
	else
		String::cpy( texturepathsz, sizeof(texturepathsz), parentpath.toString() );

	char particlepathsz[PathName::MAXLEN];
	if ( particlepath != "" )
		particlepath.get( particlepathsz, sizeof(particlepathsz) );
	else
		String::cpy( particlepathsz, sizeof(particlepathsz), parentpath.toString() );

	// read fog
	FogType type = (FogType)in.readByte();
	float start = in.readFloat();
	float end = in.readFloat();
	float3 color = in.readFloat3();
	setFog( type, start, end, color );

	// read textures
	int checkid = 0x12345600;
	readId( in, checkid );

	int n = in.readInt();
	Array<P(BaseTexture)> textures;
	Array<char> buf;
	textures.resize( n );
	for ( int i = 0 ; i < n ; ++i )
	{
		in.readUTF( buf );
		String texfname = PathName(texturepathsz,buf.begin()).toString();

		int type = 0;
		if ( in.version() >= 160 )
			type = in.readInt();

		switch ( type )
		{
		case 0:
			textures[i] = res->getTexture( texfname );
			break;
		case 1:
			textures[i] = res->getCubeTexture( texfname );
			break;
		default:
			throwError( IOException( Format("Failed to load scene \"{0}\". Invalid texture type (\"{1}\" type was {2})", filename, texfname, type) ) );
		}
	}

	// read materials
	n = in.readInt();
	Array<P(Shader)> materials;
	materials.resize( n );
	for ( int i = 0 ; i < n ; ++i )
	{
		String name = in.readUTF();
		in.readUTF( buf );
		int flags = in.readInt();
		P(Shader) fx = res->getShader( PathName(shaderpathsz,buf.begin()).toString(), flags );
		fx->setName( name );

		int texparams = in.readByte();
		for ( int k = 0 ; k < texparams ; ++k )
		{
			in.readUTF( buf );
			int ix = in.readShort();
			if ( ix < 0 || ix > textures.size() )
				throwError( IOException( Format("Failed to load scene \"{0}\". Invalid texture index ({1}) in material \"{2}\"", filename, ix, name) ) );
			fx->setTexture( &buf[0], textures[ix] );
		}

		int vec4params = in.readByte();
		for ( int k = 0 ; k < vec4params ; ++k )
		{
			in.readUTF( buf );
			float4 v = in.readFloat4();
			fx->setVector( &buf[0], v );
		}

		int floatparams = in.readByte();
		for ( int k = 0 ; k < floatparams ; ++k )
		{
			in.readUTF( buf );
			float v = in.readFloat();
			fx->setFloat( &buf[0], v );
		}

		materials[i] = fx;
	}

	// read primitives
	readId( in, checkid );
	n = in.readInt();
	Array<P(Primitive)> primitives;
	primitives.resize( n );
	for ( int i = 0 ; i < n ; ++i )
	{
		int matix=-1;
		P(Primitive) prim = in.readPrimitive( context, &matix );
		if ( matix < 0 || matix >= materials.size() )
			throwError( IOException( Format("Failed to load scene \"{0}\". Invalid material index ({1}), maximum is {2}.", filename, matix, materials.size()) ) );

		P(Shader) shader = materials[matix];
		prim->setShader( shader );
		primitives[i] = prim;
	}

	// read meshes
	readId( in, checkid );
	Array<P(Node)> nodes;
	Array<String> nodenames;
	Array<int> nodeparents;
	n = in.readInt();
	Array<MeshBoneCount> meshbonecounts;
	Array<MeshBone> meshbones;
	meshbonecounts.resize( n );
	for ( int i = 0 ; i < n ; ++i )
	{
		P(Mesh) obj = new Mesh;
		readNode( in, obj.ptr(), nodeparents, nodenames, nodes );

		// add primitives
		int count = in.readInt();
		for ( int k = 0 ; k < count ; ++k )
		{
			int ix = in.readInt();
			if ( ix < 0 || ix >= primitives.size() )
				throwError( IOException( Format("Failed to load scene \"{0}\". Invalid primitive index ({1}) in primitive {2}.", filename, ix, i) ) );
			obj->addPrimitive( primitives[ix] );
		}

		// add bones
		MeshBoneCount& mbc = meshbonecounts[i];
		mbc.mesh = obj;
		mbc.bonecount = in.readInt();
		for ( int k = 0 ; k < mbc.bonecount ; ++k )
		{
			MeshBone mb;
			mb.boneindex = in.readInt();
			mb.invresttm = in.readFloat3x4();
			meshbones.add( mb );
		}

		obj->computeBound();
	}

	// read cameras
	readId( in, checkid );
	n = in.readInt();
	for ( int i = 0 ; i < n ; ++i )
	{
		P(Camera) obj = new Camera;
		readNode( in, obj.ptr(), nodeparents, nodenames, nodes );

		float front = in.readFloat();
		float back = in.readFloat();
		float fov = in.readFloat();

		if ( front < 1e-4f || back < front || back > 1e9f )
			throwError( IOException( Format("Failed to load scene \"{0}\". Invalid front/back plane ({1}/{2}) in \"{3}\".", filename, front, back, obj->name()) ) );
		if ( fov < Math::toRadians(1.5f) || fov > Math::toRadians(179.f) )
			throwError( IOException( Format("Failed to load scene \"{0}\". Invalid horizontal field-of-view ({1}) in \"{2}\".", filename, fov, obj->name()) ) );

		obj->setFront( front );
		obj->setBack( back );
		obj->setVerticalFov( fov );
	}

	// read lights
	readId( in, checkid );
	n = in.readInt();
	Array<Light*> lights;
	lights.resize( n );
	for ( int i = 0 ; i < n ; ++i )
	{
		P(Light) obj = new Light;
		readNode( in, obj.ptr(), nodeparents, nodenames, nodes );

		float3 v3 = in.readFloat3();
		obj->setColor( v3 );

		in.readFloat();
		in.readFloat();

		float v = in.readFloat();
		obj->setFarAttenStart( v );
		v = in.readFloat();
		obj->setFarAttenEnd( v );

		v = in.readFloat();
		if ( v < Math::toRadians(0.f) || v > Math::toRadians(180.f) )
			throwError( IOException( Format("Failed to load scene \"{0}\". Light inner cone ({1}) invalid in object \"{2}\".", filename, v, obj->name()) ) );
		obj->setInnerCone( v );

		v = in.readFloat();
		if ( v < Math::toRadians(0.f) || v > Math::toRadians(180.f) )
			throwError( IOException( Format("Failed to load scene \"{0}\". Light outer cone ({1}) invalid in object \"{2}\".", filename, v, obj->name()) ) );
		obj->setOuterCone( v );

		Light::Type type = (Light::Type)in.readByte();
		if ( type <= Light::TYPE_UNKNOWN || type >= Light::TYPE_COUNT )
			throwError( IOException( Format("Failed to load scene \"{0}\". Light type ({1}) invalid in object \"{2}\".", filename, int(type), obj->name()) ) );
		obj->setType( type );

		lights[i] = obj;
	}

	// read dummies
	readId( in, checkid );
	n = in.readInt();
	for ( int i = 0 ; i < n ; ++i )
	{
		P(Dummy) obj = new Dummy;
		readNode( in, obj.ptr(), nodeparents, nodenames, nodes );

		float3 boxmin = in.readFloat3();
		float3 boxmax = in.readFloat3();
		obj->setBox( boxmin, boxmax );
	}

	// read line shapes
	readId( in, checkid );

	n = in.readInt();
	for ( int i = 0 ; i < n ; ++i )
	{
		P(Lines) obj = new Lines( context );
		readNode( in, obj.ptr(), nodeparents, nodenames, nodes );

		int lines = in.readInt();
		int paths = in.readInt();
		if ( lines > 100000 || paths > 100000 || paths > lines )
			throwError( IOException( Format("Failed to load scene \"{0}\". Invalid line/path count ({1}/{2})", filename, lines, paths) ) );
		obj->reserve( lines, paths );

		for ( int k = 0 ; k < lines ; ++k )
		{
			float3 start = in.readFloat3();
			float3 end = in.readFloat3();
			obj->addLine( start, end, float4(1,1,1,1) );
		}

		for ( int k = 0 ; k < paths ; ++k )
		{
			int begin = in.readInt();
			int end = in.readInt();
			obj->addPath( begin, end );
		}

		obj->computeBound();
	}

	// read other nodes
	readId( in, checkid );
	n = in.readInt();
	for ( int i = 0 ; i < n ; ++i )
	{
		P(Node) obj = new Node;
		readNode( in, obj.ptr(), nodeparents, nodenames, nodes );
	}

	// read transform animations
	readId( in, checkid );
	n = in.readInt();
	m_transformAnims = new TransformAnimationSet( n );
	for ( int i = 0 ; i < n ; ++i )
	{
		String name = in.readUTF();

		// re-use existing string to save memory
		if ( nodenames.size() > 0 )
		{
			if ( n != nodenames.size() )
				throwError( IOException( Format("Failed to load scene \"{0}\". Transform animation count ({1}) does not match node count ({2}).", filename, n, nodenames.size()) ) );
			if ( name == nodenames[i] )
				name = nodenames[i];
			else
				throwError( IOException( Format("Failed to load scene \"{0}\". Transform animation ({1}) does not match node name ({2}).", filename, name, nodenames[i]) ) );
		}

		m_transformAnims->put( name, in.readTransformAnimation() );
	}

	// read user properties
	readId( in, checkid );
	m_userProperties = in.readUserPropertySet( this );

	// connect parents
	assert( nodes.size() == nodeparents.size() );
	for ( int i = 0 ; i < nodes.size() ; ++i )
	{
		int ix = nodeparents[i];
		if ( ix >= nodes.size() )
			throwError( IOException( Format("Failed to load scene \"{0}\". Invalid node parent index ({1}) in \"{2}\".", filename, ix, nodenames[i]) ) );
		if ( ix >= 0 )
			nodes[i]->linkTo( nodes[ix] );
		else
			nodes[i]->linkTo( this );
	}

	// connect bones
	n = 0;
	for ( int i = 0 ; i < meshbonecounts.size() ; ++i )
	{
		const MeshBoneCount& mbc = meshbonecounts[i];
		int bonecount = mbc.bonecount;
		for ( int k = 0 ; k < bonecount ; ++k )
		{
			const MeshBone& mb = meshbones[n++];
			int ix = mb.boneindex;
			if ( ix < 0 || ix >= nodes.size() )
				throwError( IOException( Format("Failed to load scene \"{0}\". Invalid bone index ({1}) in \"{2}\".", filename, ix, mbc.mesh->name()) ) );

			mbc.mesh->addBone( nodes[ix], mb.invresttm );
		}
	}
	assert( meshbones.size() == n );

	// connect lights to meshes
	LightSorter lightsorter;
	lightsorter.collectLights( this );
	if ( lightsorter.lights() > 0 )
	{
		for ( int i = 0 ; i < meshbonecounts.size() ; ++i )
		{
			Mesh* mesh = meshbonecounts[i].mesh;

			float3 meshpos = mesh->worldTransform().transform( (mesh->boundBoxMax()+mesh->boundBoxMin())*.5f );
			Array<Light*>& lights = lightsorter.getLightsByDistance( meshpos );

			if ( lights.size() > 0 )
				mesh->addLight( lights[0] );
		}
	}

	// create particle systems based on user properties Particle=<name>
	if ( m_userProperties != 0 )
	{
		PropertyParser parser;
		for ( HashtableIterator<String,String> it = m_userProperties->begin() ; it != m_userProperties->end() ; ++it )
		{
			parser.reset( it.value(), it.key() );

			float time = 0.f;
			for ( PropertyParser::ConstIterator i = parser.begin() ; i != parser.end() ; ++i )
			{
				if ( !strcmp(i.key(),"perspectivecorrection") )
				{
					float perspf = 5.f;
					if ( sscanf(i.value(),"%g",&perspf) != 1 )
						throwError( IOException( Format("Failed parse PerspectiveCorrection=<level 0-10> User Property field from object \"{0}\" in scene \"{1}\"", it.key(), name()) ) );
					int persp = (int)perspf;
					if ( persp < 0 || persp > 10 )
						throwError( IOException( Format("Failed parse PerspectiveCorrection=<level 0-10> User Property field from object \"{0}\" in scene \"{1}\"", it.key(), name()) ) );
					
					Node* node = getNodeByName( it.key() );
					if ( node->classId() == Node::NODE_MESH )
					{
						Mesh* mesh = static_cast<Mesh*>( node );
						for ( int i = 0 ; i < mesh->primitives() ; ++i )
							mesh->getPrimitive(i)->setPerspectiveCorrection( persp );
					}
				}
				else if ( !strcmp(i.key(),"time") )
				{
					if ( sscanf(i.value(),"%g",&time) != 1 )
						throwError( IOException( Format("Failed parse Time=<seconds> User Property field from object \"{0}\" in scene \"{1}\"", it.key(), name()) ) );
				}
#ifndef HGR_NOPARTICLES
				else if ( !strcmp(i.key(),"particle") )
				{
					PathName particlepathname( particlepathsz, i.value() );

					// append .prs extension
					buf.resize( strlen(particlepathname.toString()) + 1 );
					strcpy( buf.begin(), particlepathname.toString() );
					buf.resize( buf.size()-1 );
					buf.add( '.' );
					buf.add( 'p' );
					buf.add( 'r' );
					buf.add( 's' );
					buf.add( 0 );

					// create particle system from file <particlename>
					P(ParticleSystem) particle = res->getParticleSystem( buf.begin(), texturepath, shaderpath );

					// set particle instance specific properties
					particle->setDelay( time );

					// link particle to node
					Node* node = getNodeByName( it.key() );
					particle->linkTo( node );
				}
#endif // HGR_NOPARTICLES
			}
		}
	}
}

Scene::Scene( const Scene& other ) :
	Node( other ),
	m_transformAnims( other.m_transformAnims ),
	m_userProperties( other.m_userProperties ),
	m_fogColor( other.m_fogColor ),
	m_fogStart( other.m_fogStart ),
	m_fogEnd( other.m_fogEnd ),
	m_fogType( other.m_fogType )
{
}

Scene::~Scene()
{
}

Node* Scene::clone() const
{
	return new Scene( *this );
}

Camera* Scene::camera()
{
	for ( Node* node = this ; node != 0 ; node = node->next(this) )
	{
		if ( NODE_CAMERA == node->classId() )
			return static_cast<Camera*>(node);
	}

	P(Camera) camera = new Camera;
	camera->linkTo( this );
	return camera;
}

void Scene::readId( SceneInputStream& in, int& id )
{
	int was = in.readInt();
	if ( id != was )
		throwError( IOException(Format("Failed to load scene file \"{0}\", check id was {1,x} but should be {2,x}", in.toString(), was, id)) );
	++id;
}

void Scene::readNode( SceneInputStream& in, Node* node, Array<int>& nodeparents,
	Array<String>& nodenames, Array<P(Node)>& nodes )
{
	int parentindex;
	in.readNode( node, &parentindex );

	nodenames.add( node->name() );
	nodeparents.add( parentindex );
	nodes.add( node );
}

void Scene::printHierarchy() const
{
	Debug::printf( "---------------------------------------------------------\n" );
	for ( const Node* node = this ; node ; node = node->next(this) )
	{
		int depth = 0;
		for ( Node* parent = node->parent() ; parent ; parent = parent->parent() )
			++depth;

		char buf[256];
		const int tab = 4;
		assert( tab*depth < int(sizeof(buf)) );
		int n = 0;
		for ( ; n < tab*depth ; ++n )
			buf[n] = ' ';
		buf[n] = 0;

		Debug::printf( "%s%s\n", buf, node->name().c_str() );
	}

	if ( m_userProperties != 0 )
	{
		Debug::printf( "---------------------------------------------------------\n" );
		for ( const Node* node = this ; node ; node = node->next(this) )
		{
			Debug::printf( "%s:\n", node->name().c_str() );
			Debug::printf( "  %s\n", m_userProperties->get(node->name()).c_str() );
		}
	}
}

void Scene::applyAnimations( float time, float dt )
{
	// update key frame animations
	if ( m_transformAnims != 0 )
	{
		float3x4 tm;
		for ( Node* node = this ; node != 0 ; node = node->next(this) )
		{
			String name = node->name();
			TransformAnimation* tmanim = m_transformAnims->get( name );
			if ( tmanim != 0 )
			{
				tmanim->eval( time, &tm );
				node->setTransform( tm );
			}
		}
	}

	// update particles
#ifndef HGR_NOPARTICLES
	for ( Node* node = this ; node != 0 ; node = node->next(this) )
	{
		if ( node->classId() == NODE_PARTICLESYSTEM )
		{
			ParticleSystem* ps = static_cast<ParticleSystem*>( node );
			if ( ps )
				ps->update( dt );
		}
	}
#endif // HGR_NOPARTICLES
}

void Scene::merge( Scene* other )
{
	// merge hierarchy
	P(Node) nextnode;
	for ( P(Node) node = other->firstChild() ; node != 0 ; node = nextnode )
	{
		nextnode = other->getNextChild( node );
		node->unlink();
		node->linkTo( this );
	}

	// merge transform animations
	if ( m_transformAnims != 0 && other->transformAnimations() )
		m_transformAnims->merge( *other->transformAnimations() );
	else if ( other->transformAnimations() )
		m_transformAnims = other->transformAnimations();
}

void Scene::removeLightsAndCameras()
{
	for ( Node* node = this ; node != 0 ; node = node->next(this) )
	{
		NodeClassId cid = node->classId();
		if ( cid == NODE_LIGHT || cid == NODE_CAMERA )
		{
			Node* next = node->next(this);
			
			if ( !node->firstChild() )
				node->unlink();

			node = next;
		}
	}
}

TransformAnimationSet* Scene::transformAnimations() const		
{
	return m_transformAnims;
}

UserPropertySet* Scene::userProperties() const			
{
	return m_userProperties;
}

void Scene::setFog( FogType type, float start, float end, const float3& color )
{
	m_fogType = type;
	m_fogStart = start;
	m_fogEnd = end;
	m_fogColor = color;
}

void Scene::getBoundBox( NS(math,float3)* boxmin, NS(math,float3)* boxmax ) const
{
	float3x4 itm = worldTransform().inverse();
	*boxmin = float3( Float::MAX_VALUE, Float::MAX_VALUE, Float::MAX_VALUE );
	*boxmax = float3( -Float::MAX_VALUE, -Float::MAX_VALUE, -Float::MAX_VALUE );

	for ( const Node* node = this ; node != 0 ; node = node->next(this) )
	{
		if ( node->isVisualNode() )
		{
			const Visual* vis = static_cast<const Visual*>( node );
			if ( !vis->isBoundWorld() && !vis->isBoundInfinity() )
			{
				float3 box[2] = { vis->boundBoxMin(), vis->boundBoxMax() };
				float3x4 tm = itm * vis->worldTransform();
				const int corners[8][3] = 
				{{0,0,0}, {0,0,1}, {0,1,0}, {0,1,1}, {1,0,0}, {1,0,1}, {1,1,0}, {1,1,1}};

				for ( int k = 0 ; k < 8 ; ++k )
				{
					float3 point = tm.translation() +
						tm.getColumn(0) * box[ corners[k][0] ][0] +
						tm.getColumn(1) * box[ corners[k][1] ][1] + 
						tm.getColumn(2) * box[ corners[k][2] ][2];
					
					for ( int j = 0 ; j < 3 ; ++j )
					{
						if ( point[j] < (*boxmin)[j] )
							(*boxmin)[j] = point[j];
						if ( point[j] > (*boxmax)[j] )
							(*boxmax)[j] = point[j];
					}
				}
			}
		}
	}

	if ( boxmax->x < boxmin->x )
		*boxmin = *boxmax = float3(0,0,0);
}


END_NAMESPACE() // hgr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
