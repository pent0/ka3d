#ifndef _GAMEOBJECTS_GAMEOBJECT_H
#define _GAMEOBJECTS_GAMEOBJECT_H


#include <io/DataInputStream.h>
#include <io/DataOutputStream.h>
#include <hgr/Scene.h>
#include <lua/LuaObject.h>
#include <math/float3x4.h>
#include <ode/ODEObject.h>
#include <ode/odex.h>


class GameLevel;
class GameCamera;

namespace hgr {
	class Light;
	class Node;}


class GameObject :
	public lua::LuaObject
{
public:
	/**
	 * Flags used to avoid optimize collision checks.
	 * For example GameCamera can never collide against GameProjectile, etc.
	 */
	enum Type
	{
		TYPE_NONE				= 0,
		TYPE_LEVEL				= (1<<0),
		TYPE_CAMERA				= (1<<1),
		TYPE_CHARACTER			= (1<<2),
		TYPE_PLAYER				= (1<<3),
		TYPE_PLAYERCHARACTER	= (TYPE_PLAYER|TYPE_CHARACTER),
		TYPE_TRIGGER			= (1<<4),
		TYPE_DYNAMIC			= (1<<5),
		TYPE_WEAPON				= (1<<6),
		TYPE_PROJECTILE			= (1<<7),
		TYPE_MASK				= 0x7FFFFFFF
	};

	GameObject( lua::LuaState* luastate, GameLevel* level );

	~GameObject();

	/** Called before simulation. */
	virtual void			update( float dt );

	/** Called before simulation to verify collected contacts. */
	virtual int				collided( GameObject* obj, dContact* contacts, int numc );

	/** Called before rendering */
	virtual void			getObjectsToRender( GameCamera* camera );

	void					setAngularVel( const NS(math,float3)& vel );

	void					setLinearVel( const NS(math,float3)& vel );

	void					setRotation( const NS(math,float3x3)& rot );

	void					setPosition( const NS(math,float3)& pos );

	void					setTransform( const NS(math,float3x4)& tm );

	void					addImpulse( const NS(math,float3)& imp, float dt );

	void					addImpulseAt( const NS(math,float3)& point,
								const NS(math,float3)& imp, float dt );

	/** Disables rigid body simulations but keeps collisions enabled. */
	void					disableRigidBody();

	/** Disables physics and collisions. */
	void					disable();

	/** Enables physics and collisions. */
	void					enable();

	/** Disables gravity from simulation of this object. */
	void					disableGravity();

	/** Toggle auto-disable features used to optimize simulation. */
	void					setAutoDisable( bool autodisabled );

	/** 
	 * Marks object for removal.
	 * After this destroyed() will return true.
	 */
	void					destroy();

	/**
	 * Sets a 4-vector parameter in all shaders of the object.
	 */
	void					setShaderVector( const char* param, const NS(math,float4)& value );

	void					serialize( io::DataInputStream* in, io::DataOutputStream* out );

	NS(math,float3x3)			rotation() const;

	NS(math,float3)			position() const;

	NS(math,float3x4)			transform() const;

	NS(math,float3)			linearVel() const;

	NS(math,float3)			angularVel() const;

	float					mass() const;

	dBodyID					body() const;

	dGeomID					geom() const;

	bool					isOnGround() const;

	NS(gr,Context)*			context() const;

	hgr::ResourceManager*	resourceManager() const;

	Type					type() const;

	bool					isType( Type x ) const;

	/** Returns true if object has been marked for removal. */
	bool					destroyed() const;

protected:
	friend class GameLevel;

	GameLevel*		m_level;
	P(hgr::Node)	m_root;
	hgr::Mesh*		m_mesh;

	void	setMesh( NS(lang,String) filename );

	void	setSkinnedMesh( NS(lang,String) filename );

	void	setRigidBodyFromRay( Type type, int collides, float mass );

	void	setRigidBodyFromBox( const NS(math,float3)& size, float mass, Type type, int collides );

	void	setRigidBodyFromSphere( float r, float mass, Type type, int collides );

	void	setRigidBodyFromCCylinderY( float r, float height, float mass,
				Type type, int collides );

	void	setRigidBodyFromMesh( hgr::Mesh* mesh, ode::ODEObject::GeomType geomtype, 
				ode::ODEObject::MassType masstype, float mass, 
				Type type, int collides );

	void	setMeshTransformFromRigidBody();

private:
	ode::ODEObject		m_obj;
	bool				m_remove;

	void	setGeom( dGeomID geom, int category, int collides, const dMass* m );

	GameObject( const GameObject& );
	GameObject& operator=( const GameObject& );
};


#include "GameObject.inl"


#endif // _GAMEOBJECTS_GAMEOBJECT_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
