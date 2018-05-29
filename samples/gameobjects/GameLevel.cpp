#include "GameLevel.h"
#include "GameDynamicObject.h"
#include <gr/Context.h>
#include <io/IOException.h>
#include <io/PropertyParser.h>
#include <io/FileInputStream.h>
#include <gr/Texture.h>
#include <hgr/Mesh.h>
#include <hgr/Scene.h>
#include <hgr/Lines.h>
#include <hgr/ResourceManager.h>
#include <hgr/TransformAnimationSet.h>
#include <lua/LuaException.h>
#include <lang/Math.h>
#include <lang/Float.h>
#include <lang/Debug.h>
#include <lang/Profile.h>
#include <math/float2.h>
#include <math/toString.h>
#include "Opcode.h"
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(io)
USING_NAMESPACE(hgr)
USING_NAMESPACE(lua)
using namespace ode;
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


GameLevel::GameLevel( Context* context, LuaState* lua, ResourceManager* res ) :
	LuaObject( lua ),
	m_context( context ),
	m_resourceManager( res ),
	m_collisionFaceBuffer( new Opcode::CollisionFaces ),
	m_raycollider( new Opcode::RayCollider ),
	m_lightmapCollisionModel( new Opcode::Model ),
	m_lightmapCollisionMeshI( new Opcode::MeshInterface )
{
	try
	{
		registerMethod( "loadLevel", this, &GameLevel::loadLevel );
		registerMethod( "playAnim", this, &GameLevel::playAnim );

		//loadLevel( "data/level/level.hgr" );
		{FileInputStream in( "data/level0.lua" );
		lua->compile( &in, in.available(), in.toString(), this );}
		call( "init" );

		if ( m_scene == 0 )
			throwError( Exception(Format("Startup level script needs to call loadLevel")) );
	}
	catch ( ... )
	{
		delete m_lightmapCollisionMeshI;
		delete m_lightmapCollisionModel;
		delete m_raycollider;
		delete m_collisionFaceBuffer;
		throw;
	}
}

GameLevel::~GameLevel()
{
	m_dynObjs.clear();
	m_animLib.clear();

	delete m_lightmapCollisionMeshI; m_lightmapCollisionMeshI = 0;
	delete m_lightmapCollisionModel; m_lightmapCollisionModel = 0;
	delete m_raycollider; m_raycollider = 0;
	delete m_collisionFaceBuffer; m_collisionFaceBuffer = 0;

	while ( m_objects.size() > 0 )
		removeObject( m_objects.last() );
}

void GameLevel::serialize( io::DataInputStream* /*in*/, io::DataOutputStream* /*out*/ )
{
}

void GameLevel::update( float dt )
{
	m_lines->removeLines();

	// update game objects
	for ( int i = 0 ; i < m_objects.size() ; ++i )
		m_objects[i]->update( dt );

	// remove destroyed objects
	for ( int i = 0 ; i < m_objects.size() ; ++i )
	{
		if ( m_objects[i]->destroyed() )
		{
			m_objects[i]->disable();
			m_objects.remove( i-- );
		}
	}

	// simulate step
	m_world.step( dt, this );
}

void GameLevel::removeObject( GameObject* obj )
{
	int ix = m_objects.indexOf( obj );
	if ( ix != -1 )
		m_objects.remove( ix );

	assert( m_objects.indexOf(obj) == -1 );
}

void GameLevel::addObject( GameObject* obj )
{
	m_objects.add( obj );

	assert( m_objects.indexOf(obj) == m_objects.size()-1 );
}

float3x4 GameLevel::getStartpoint( const String& name ) const
{
	Node* node = m_scene->findNodeByName(name);
	if ( !node )
		throwError( Exception( Format("Level {0} has no startpoint named \"{1}\"", this->name(), name) ) );
	return node->transform();
}

String GameLevel::name() const
{
	String name;
	if ( m_scene == 0 || m_scene->name() == "" )
		name = "(unnamed)";
	else
		name = m_scene->name();
	return name;
}

GameObject* GameLevel::getObject( int i )
{
	return m_objects[i];
}

LightSorter* GameLevel::lightSorter()			
{
	return &m_lightSorter;
}

Scene* GameLevel::getSceneToRender( GameCamera* /*camera*/, float time, float dt )
{
	m_scene->applyAnimations( time, dt );
	return m_scene;
}

void GameLevel::drawSimulationObjects()
{
	for ( int i = 0 ; i < m_objects.size() ; ++i )
	{
		GameObject* o = m_objects[i];
		dGeomID geom = o->geom();

		if ( geom ) 
		{
			int geomclass = dGeomGetClass( geom );
			float3x4 tm;// = o->transform();
			dGeomGetTransform( geom, &tm );

			if ( geomclass == dBoxClass )
			{
				dVector3 size;
				dGeomBoxGetLengths( geom, size );
				float3 sizev( size[0], size[1], size[2] );
				m_lines->addBox( tm, -sizev*.5f, sizev*.5f, float4(0,1,0,1) );
			}
			else if ( geomclass == dSphereClass )
			{
				float r = dGeomSphereGetRadius( geom );
				m_lines->addSphere( tm, r+.10f, float4(0,1,0,1), 32 );
			}
		}
	}
}

int	GameLevel::objects() const			
{
	return m_objects.size();
}

dWorldID GameLevel::world() const			
{
	return m_world.world();
}

dGeomID GameLevel::geom() const			
{
	return m_levelObj.geom();
}

dSpaceID GameLevel::space() const			
{
	return m_world.space();
}

int	GameLevel::enabledBodies() const
{
	return m_world.enabledBodies();
}

void GameLevel::loadLevel( String filename )
{
	m_dynObjs = Hashtable<String,P(GameDynamicObject)>();

	// load level mesh
	Context* context = m_context;
	m_scene = new Scene( context, filename, 0, "", "data/shaders", "data/particles" );

	// init debug lines
	m_lines = new Lines( context );
	m_lines->linkTo( m_scene );

	// check for valid object properties
	const char* keylist[] = 
	{
		"physics", 
		"mass",
		"density",
		"particle", 
		"time",
		"collision",
		"sound",
	};
	P(UserPropertySet) propset = m_scene->userProperties();
	if ( !propset )
		propset = new UserPropertySet;
	propset->check( keylist, sizeof(keylist)/sizeof(keylist[0]) );

	// extract collision and visual objects
	Array<P(Node)> removelist;
	Array<P(Mesh)> colliders;
	Array<P(Mesh)> visuals;
	PropertyParser parser;
	int collisionpolys = 0;
	int visualpolys = 0;
	int polystotal = 0;
	for ( Node* node = m_scene ; node != 0 ; node = node->next(m_scene) )
	{
		Mesh* mesh = dynamic_cast<Mesh*>( node );
		if ( mesh != 0 )
		{
			parser.reset( propset->get(mesh->name()), mesh->name() );

			int meshtri = 0;
			for ( int i = 0 ; i < mesh->primitives() ; ++i )
				meshtri += mesh->getPrimitive(i)->indices()/3;
			polystotal += meshtri;

			if ( parser.hasKey("collision") )
			{
				String collide = parser.getString( "collision" );
				if ( collide == "only" )
				{
					// only collision mesh
					colliders.add( mesh );

					if ( mesh->firstChild() )
						throwError( IOException( Format("Invalid Collide=none tag in \"{0}\": Level collision meshes can't have children", mesh->name()) ) );
					removelist.add( mesh );

					collisionpolys += meshtri;
				}
				else if ( collide == "none" )
				{
					// only visual mesh
					visuals.add( mesh );
					visualpolys += meshtri;
				}
				else
				{
					throwError( IOException( Format("Invalid Collide=<x> tag in \"{0}\": {1}", mesh->name(), collide) ) );
				}
			}
			else
			{
				// default: both collisions and visuals
				colliders.add( mesh );
				visuals.add( mesh );
				visualpolys += meshtri;
				collisionpolys += meshtri;
			}
		}
	}

	// collect visual collision geometry (for lightmap raytracing)
	for ( int i = 0 ; i < visuals.size() ; ++i )
	{
		Mesh* mesh = visuals[i];

		ODEObject::GeomType geomtype;
		ODEObject::MassType masstype;
		float mass;
		ODEObject::parseProperties( mesh, propset->get(mesh->name()), &geomtype, &masstype, &mass );

		if ( ODEObject::GEOM_DEFAULT == geomtype )
		{
			// append level visual triangles
			float3x4 bodyworldtm, bodytm, bodyvertextm;
			mesh->getBodyTransform( &bodyworldtm, &bodytm, &bodyvertextm );
			float3x4 tm = bodyworldtm * bodyvertextm;

			for ( int i = 0 ; i < mesh->primitives() ; ++i )
			{
				const int oldverts = m_visualVertices.size();
				Primitive* prim = mesh->getPrimitive(i);
				Primitive::Lock lk( prim, Primitive::LOCK_READ );

				for ( int k = 0 ; k < prim->vertices() ; ++k )
				{
					float4 v;
					prim->getVertexPositions( k, &v, 1 );
					m_visualVertices.add( tm.transform( float3(v.x,v.y,v.z) ) );
				}

				for ( int k = 0 ; k < prim->indices() ; k += 3 )
				{
					VisualTriangle tri;
					
					prim->getIndices( k, tri.indices, 3 );
					tri.indices[0] += oldverts;
					tri.indices[1] += oldverts;
					tri.indices[2] += oldverts;

					tri.prim = prim;
					tri.triangle = k;

					m_visualTriangles.add( tri );
				}
			}
		}
	}

	// create visual collision model (for lightmap raytracing)
	Opcode::MeshInterface& mesh = *m_lightmapCollisionMeshI;
	mesh.SetNbTriangles( m_visualTriangles.size() );
	mesh.SetNbVertices( m_visualVertices.size() );
	mesh.SetPointers( (Opcode::IndexedTriangle*)&m_visualTriangles[0], (Opcode::Point*)&m_visualVertices[0] );
	mesh.SetStrides( sizeof(VisualTriangle), sizeof(float3) );
	mesh.Single = true;
	
	Opcode::BuildSettings settings;
	settings.mRules = Opcode::SPLIT_BEST_AXIS;

	Opcode::OPCODECREATE treebuilder;
	treebuilder.mIMesh = &mesh;
	treebuilder.mSettings = settings;
	treebuilder.mNoLeaf = true;
	treebuilder.mQuantized = false;
	treebuilder.mKeepOriginal = false;
	treebuilder.mCanRemap = false;
	Opcode::Model& model = *m_lightmapCollisionModel;
	model.Build( treebuilder );

	// collect collision geometry
	for ( int i = 0 ; i < colliders.size() ; ++i )
	{
		Mesh* mesh = colliders[i];

		ODEObject::GeomType geomtype;
		ODEObject::MassType masstype;
		float mass;
		ODEObject::parseProperties( mesh, propset->get(mesh->name()), &geomtype, &masstype, &mass );

		if ( ODEObject::GEOM_DEFAULT == geomtype )
		{
			// append level collision triangles
			float3x4 bodyworldtm, bodytm, bodyvertextm;
			mesh->getBodyTransform( &bodyworldtm, &bodytm, &bodyvertextm );
			float3x4 tm = bodyworldtm * bodyvertextm;

			for ( int i = 0 ; i < mesh->primitives() ; ++i )
			{
				const int oldverts = m_collisionVertices.size();
				Primitive* prim = mesh->getPrimitive(i);
				Primitive::Lock lk( prim, Primitive::LOCK_READ );

				for ( int k = 0 ; k < prim->vertices() ; ++k )
				{
					float4 v;
					prim->getVertexPositions( k, &v, 1 );
					m_collisionVertices.add( tm.transform( float3(v.x,v.y,v.z) ) );
				}

				for ( int k = 0 ; k < prim->indices() ; k += 3 )
				{
					CollisionTriangle tri;
					
					prim->getIndices( k, tri.indices, 3 );
					tri.indices[0] += oldverts;
					tri.indices[1] += oldverts;
					tri.indices[2] += oldverts;

					m_collisionTriangles.add( tri );
				}
			}
		}
		else
		{
			// dynamic object
			P(GameDynamicObject) obj = new GameDynamicObject( luaState(), this, mesh, geomtype, masstype, mass );
			m_dynObjs[mesh->name()] = obj;
			
			TransformAnimationSet* animset = m_scene->transformAnimations();
			animset->remove( mesh->name() );

			addObject( obj );
		}
	}

	// create level collision object
	dTriMeshDataID data = dGeomTriMeshDataCreate();

	Debug::printf( "Building TriMesh collider of %d vertices and %d triangles\n", m_collisionVertices.size(), m_collisionTriangles.size() );
	dGeomTriMeshDataBuildSingle( data, 
		&m_collisionVertices[0], sizeof(m_collisionVertices[0]), m_collisionVertices.size(),
		&m_collisionTriangles[0], 3*m_collisionTriangles.size(), sizeof(CollisionTriangle) );
	Debug::printf( "TriMesh collider done\n" );

	dGeomID geom = dCreateTriMesh( space(), data, 0, 0, 0 );
	dGeomSetCategoryBits( geom, GameObject::TYPE_LEVEL );
	dGeomSetCollideBits( geom, 0 );
	m_levelObj.create( geom );
	m_levelObj.setTransform( float3x4(1.f) );

	// unlink nodes which are not needed runtime
	for ( int i = 0 ; i < removelist.size() ; ++i )
		removelist[i]->unlink();

	// collect lights
	m_lightSorter.removeLights();
	m_lightSorter.collectLights( m_scene );

	// statistics
	Debug::printf( "Scene \"%s\":", m_scene->name().c_str() );
	Debug::printf( "  Triangles: total=%d, collision=%d, visual=%d\n", polystotal, collisionpolys, visualpolys );
	Debug::printf( "  Lights: %d\n", m_lightSorter.lights() );

	m_scene->printHierarchy();
	validateLevel();
}

void GameLevel::validateLevel()
{
	if ( m_scene == 0 )
		throwError( Exception( Format("Level {0} has no scene object", name()) ) );

	m_lightSorter.removeLights();
	m_lightSorter.collectLights( m_scene );
	if ( 0 == m_lightSorter.lights() )
		throwError( Exception( Format("Level {0} has no lights", name()) ) );

	if ( 0 == m_scene->findNodeByName("PLAYER_START_POINT") )
		throwError( Exception( Format("Level {0} missing player startpoint (dummy object named \"PLAYER_START_POINT\")", name()) ) );

	m_scene->printHierarchy();
}

int GameLevel::checkCollisions( dGeomID o1, dGeomID o2, dContact* contacts, int maxcontacts )
{
	int numc = dCollide( o1, o2, maxcontacts, &contacts[0].geom, sizeof(contacts[0]) );
	if ( numc > 0 ) 
	{
		// set contact non-geometric parameters
		dBodyID b1 = dGeomGetBody( o1 );
		dBodyID b2 = dGeomGetBody( o2 );
		for ( int i = 0 ; i < numc ; ++i ) 
		{
			contacts[i].surface.mode = dContactBounce | dContactSoftCFM;
			contacts[i].surface.mu = dInfinity;
			contacts[i].surface.mu2 = 0.f;
			contacts[i].surface.bounce = 0.1f;
			contacts[i].surface.bounce_vel = 0.1f;
			contacts[i].surface.soft_cfm = 0.0001f;
		}

		// signal collisions to GameObjects
		GameObject* gobj1 = 0;
		GameObject* gobj2 = 0;
		if ( b1 )
			gobj1 = reinterpret_cast<GameObject*>( dGeomGetData(o1) );
		if ( b2 )
			gobj2 = reinterpret_cast<GameObject*>( dGeomGetData(o2) );

		// filter possible contacts through objects
		if ( gobj1 )
			numc = gobj1->collided( gobj2, contacts, numc );
		if ( gobj2 )
			numc = gobj2->collided( gobj1, contacts, numc );
	}

	return numc;
}

void GameLevel::blendLights( const float3& pos, Light* lt )
{
	LightSorter* lightsorter = lightSorter();
	Array<Light*>& lights = lightsorter->getLightsByDistance( pos );

	float ltweight = 0.f;
	float3 ltpos(0,0,0);
	float3 ltcolor(0,0,0);

	for ( int i = 0 ; i < lights.size() ; ++i )
	{
		Light* lt0 = lights[i];
		float3x4 lttm0 = lt0->worldTransform();
		float3 ltpos0 = lttm0.translation();
		float3 worldlight = ltpos0 - pos;
		float dist = worldlight.length();

		if ( dist < lt0->farAttenEnd() )
		{
			float atten = 1.f;

			if ( dist > Float::MIN_VALUE )
			{
				worldlight *= 1.f/dist;

				atten *= lt0->applyDistanceAtten( dist );
				if ( atten > 0.f && lt0->type() == Light::TYPE_SPOT )
				{
					float3 ltdir0 = normalize0(lttm0.getColumn(2));
					atten *= lt0->applyShapeAtten( -dot(worldlight,ltdir0) );
				}
			}

			if ( atten > Float::MIN_VALUE )
			{
				ltweight += atten;
				ltpos += ltpos0 * atten;
				ltcolor += lt0->color() * atten;
			}
		}
	}

	float ltscale = 0.f;
	if ( ltweight > Float::MIN_VALUE )
		ltscale = 1.f/ltweight;
	ltpos *= ltscale;
	ltcolor *= ltscale;
	lt->setPosition( ltpos );
	lt->setColor( ltcolor );
}

hgr::ResourceManager* GameLevel::resourceManager() const		
{
	return m_resourceManager;
}

GameProjectile* GameLevel::addProjectile( const String& /*name*/, 
	const float3x4& tm, GameObject* source )
{
	P(GameProjectile) p = new GameProjectile( luaState(), this );
	p->reset();
	p->setTransform( tm );
	p->setSource( source );
	p->setLinearVel( tm.getColumn(2) * p->speed() );
	p->setAngularVel( float3(0,0,0) );
	p->enable();
	addObject( p );
	return p;
}

bool GameLevel::sampleLightmap( const float3& pos, const float3& delta, float3* color )
{
	// raytrace lightmaps
	Opcode::Model* model = m_lightmapCollisionModel;
	Opcode::CollisionFaces& collisionfacebuffer = *m_collisionFaceBuffer;
	Opcode::RayCollider& raycollider = *m_raycollider;

	raycollider.SetClosestHit( true );
	raycollider.SetCulling( false );
	raycollider.SetDestination( &collisionfacebuffer );
	float len = delta.length();
	raycollider.SetMaxDist( len );
	const char* err = raycollider.ValidateSettings();
	if ( err )
	{
		assert( !err );
		Debug::printf( "ERROR: RayCollider::ValidateSettings failed: %s\n", err );
		return false;
	}
	
	float3 dir = delta * (1.f/len);
	Opcode::Ray ray;
	ray.mOrig.x = pos.x;
	ray.mOrig.y = pos.y;
	ray.mOrig.z = pos.z;
	ray.mDir.x = dir.x;
	ray.mDir.y = dir.y;
	ray.mDir.z = dir.z;
	if ( !raycollider.Collide(ray, *model, 0) )
	{
		Debug::printf( "ERROR: RayCollider::Collide failed\n" );
		return false;
	}

	int collisions = collisionfacebuffer.GetNbFaces();
	const Opcode::CollisionFace* collisionlist = collisionfacebuffer.GetFaces();
	if ( 0 == collisions )
		return false;

	int face = collisionlist[0].mFaceID;
	float b0 = collisionlist[0].mU;
	float b1 = collisionlist[0].mV;

	assert( face >= 0 && face < m_visualTriangles.size() );
	VisualTriangle& tri = m_visualTriangles[face];
	Primitive* prim = tri.prim;
	int indexindex = tri.triangle;

	// DEBUG: draw marker
	/*float3 p = m_visualVertices[tri.indices[0]] + 
		(m_visualVertices[tri.indices[1]]-m_visualVertices[tri.indices[0]])*b0 + 
		(m_visualVertices[tri.indices[2]]-m_visualVertices[tri.indices[0]])*b1;
	lines()->addLine( p, p+float3(0,3,0), float4(0,1,0,1) );*/

	// get uv
	Primitive::Lock primlk( prim, Primitive::LOCK_READ );
	int ind[3];
	prim->getIndices( indexindex, ind, 3 );
	float4 v[3];
	if ( prim->vertexFormat().textureCoordinates() <= 1 )
		return false;
	prim->getVertexTextureCoordinates( ind[0], 1, v+0, 1 );
	prim->getVertexTextureCoordinates( ind[1], 1, v+1, 1 );
	prim->getVertexTextureCoordinates( ind[2], 1, v+2, 1 );
	float2 uv = (v[0] + (v[1]-v[0])*b0 + (v[2]-v[0])*b1).xy();

	// read lightmaps
	const char* lightmapnames[] = {"LIGHTMAP0", "LIGHTMAP1", "LIGHTMAP2"};
	*color = float3(0,0,0);
	for ( int k = 0 ; k < 3 ; ++k )
	{
		Texture* lightmap = static_cast<Texture*>( prim->shader()->getTexture( lightmapnames[k] ) );
		if ( !lightmap )
			return false;

		int w = lightmap->width();
		int h = lightmap->height();
		int x = (int)( uv.x*float(w) );
		int y = (int)( uv.y*float(h) );
		if ( !( x >= 0 && x < w && y >= 0 && y < h ) )
			return false;

		Texture::Lock lightmaplk( lightmap, Texture::LOCK_READ );
		void* bits = 0;
		int pitch = 0;
		lightmap->getData( &bits, &pitch );

		float rgba[4];
		lightmap->format().getPixel( x, y, bits, pitch, SurfaceFormat(), 0, rgba );
		*color += float3( rgba[0], rgba[1], rgba[2] );
	}
	return true;
}

void GameLevel::playAnim( String animname, String levelobjname )
{
	if ( m_scene == 0 )
		throwError( Exception(Format("playAnim requires that the level scene is initalized first by using loadLevel")) );

	// anim filename + source object name
	int ix = animname.indexOf(':');
	if ( ix == -1 )
		throwError( Exception(Format("playAnim requires that the first parameter has format <file name>:<object name>")) );
	String animfilename = animname.substring( 0, ix );
	String animobjname = animname.substring( ix+1 );

	// load animation scene and find object
	if ( m_animLib[animfilename] == 0 )
		m_animLib[animfilename] = new Scene( context(), animfilename, 0, "", "data/shaders", "data/particles" );
	P(Scene) animscene = m_animLib[animfilename];
	P(TransformAnimation) anim = animscene->transformAnimations()->get( animobjname );
	if ( anim == 0 )
		throwError( Exception(Format("playAnim didnt find animation {0} from {1}", animobjname, animfilename)) );

	// find level obj and start anim
	P(GameDynamicObject) levelobj = m_dynObjs[levelobjname];
	if ( levelobj == 0 )
		throwError( Exception(Format("playAnim didnt find dynamic object {0} from level (was object marked as dynamic? with physics=trimesh etc)", levelobjname)) );
	levelobj->startAnim( anim );
}
