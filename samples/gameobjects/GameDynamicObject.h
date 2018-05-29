#ifndef _GAMEOBJECTS_GAMEDYNAMICOBJECT_H
#define _GAMEOBJECTS_GAMEDYNAMICOBJECT_H


#include "GameObject.h"
#include <hgr/Mesh.h>
#include <hgr/Scene.h>
#include <hgr/TransformAnimation.h>


class GameDynamicObject :
	public GameObject
{
public:
	GameDynamicObject( lua::LuaState* luastate, GameLevel* level, hgr::Mesh* mesh, 
		ode::ODEObject::GeomType geomtype, ode::ODEObject::MassType masstype, float mass );

	~GameDynamicObject();

	void		update( float dt );

	void		getObjectsToRender( GameCamera* camera );

	int			collided( GameObject* obj, dContact* contacts, int numc );

	void		startAnim( NS(hgr,TransformAnimation)* anim );

private:
	float								m_time;
	P(NS(hgr,TransformAnimation))		m_anim;

	GameDynamicObject( const GameDynamicObject& );
	GameDynamicObject& operator=( const GameDynamicObject& );
};


#endif // _GAMEOBJECTS_GAMEDYNAMICOBJECT_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
