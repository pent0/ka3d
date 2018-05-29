#ifndef _GAMEOBJECTS_GAMELEVEL_H
#define _GAMEOBJECTS_GAMELEVEL_H


#include "GameObject.h"
#include "GameProjectile.h"
#include "GameDynamicObject.h"
#include <hgr/Scene.h>
#include <hgr/Light.h>
#include <hgr/Lines.h>
#include <hgr/LightSorter.h>
#include <lua/LuaObject.h>
#include <ode/ODEWorld.h>
#include <ode/ODEObject.h>
#include <ode/ODECollisionInterface.h>
#include <lang/Array.h>
#include <lang/Hashtable.h>
#include <math/float3.h>


class GameObject;

namespace gr {
	class Context;}

namespace hgr {
	class ResourceManager;
	class Light;}

namespace Opcode {
	class Model;
	class RayCollider;
	class MeshInterface;
	class CollisionFaces;}


class GameLevel :
	public lua::LuaObject,
	public ode::ODECollisionInterface
{
public:
	GameLevel( NS(gr,Context)* context, lua::LuaState* lua, hgr::ResourceManager* res );
	~GameLevel();

	void				update( float dt );

	GameProjectile*		addProjectile( const NS(lang,String)& name, 
							const NS(math,float3x4)& tm, GameObject* source );

	void				addObject( GameObject* obj );

	void				removeObject( GameObject* obj );

	GameObject*			getObject( int i );

	hgr::LightSorter*	lightSorter();

	void				drawSimulationObjects();

	NS(hgr,Scene)*			getSceneToRender( GameCamera* camera, float time, float dt );

	void				removeLines();

	void				blendLights( const NS(math,float3)& pos, hgr::Light* lt );

	bool				sampleLightmap( const NS(math,float3)& pos, const NS(math,float3)& delta, NS(math,float3)* color );

	void				serialize( io::DataInputStream* in, io::DataOutputStream* out );

	int					objects() const;

	NS(lang,String)		name() const;

	dWorldID			world() const;

	dSpaceID			space() const;

	dGeomID				geom() const;

	int					enabledBodies() const;

	NS(math,float3x4)		getStartpoint( const NS(lang,String)& name ) const;

	hgr::Lines*			lines() const					{return m_lines;}
	
	NS(gr,Context)*		context() const					{return m_context;}

	hgr::ResourceManager*	resourceManager() const;

private:
	class CollisionTriangle
	{
	public:
		int					indices[3];
	};

	class VisualTriangle
	{
	public:
		int					indices[3];
		P(NS(gr,Primitive))	prim;
		int					triangle; // index to the indices, i.e. triangle index * 3
	};

	// physics
	ode::ODEWorld						m_world;
	ode::ODEObject						m_levelObj;
	NS(lang,Array)<NS(math,float3)>		m_collisionVertices;
	NS(lang,Array)<CollisionTriangle>	m_collisionTriangles;

	// visuals
	P(NS(gr,Context))					m_context;
	P(hgr::ResourceManager)				m_resourceManager;
	P(NS(hgr,Scene))					m_scene;
	NS(lang,Array)<P(GameObject)>		m_objects;
	hgr::LightSorter					m_lightSorter;
	P(hgr::Lines)						m_lines;

	// lightmap raytracing
	NS(lang,Array)<NS(math,float3)>		m_visualVertices;
	NS(lang,Array)<VisualTriangle>		m_visualTriangles;
	Opcode::CollisionFaces*				m_collisionFaceBuffer;
	Opcode::RayCollider*				m_raycollider;
	Opcode::Model*						m_lightmapCollisionModel;
	Opcode::MeshInterface*				m_lightmapCollisionMeshI;

	// animated objects
	NS(lang,Hashtable)< NS(lang,String), P(GameDynamicObject) >		m_dynObjs;
	NS(lang,Hashtable)< NS(lang,String), P(NS(hgr,Scene)) >			m_animLib;

	void	createDebugXZGridLevel();
	void	setWorldParams();
	void	validateLevel();
	int		checkCollisions( dGeomID o1, dGeomID o2, dContact* contacts, int maxcontacts );

	// scriptable
	void	loadLevel( NS(lang,String) filename );

	/**
	 * Plays animation from different scene.
	 * @param animname Name of animation file and object, e.g. "myscene.hgr:MyObject"
	 * @param name of object in the level scene
	 */
	void	playAnim( NS(lang,String) animname, NS(lang,String) levelobjname );

	GameLevel( const GameLevel& );
	GameLevel& operator=( const GameLevel& );
};


#endif // _GAMEOBJECTS_GAMELEVEL_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
