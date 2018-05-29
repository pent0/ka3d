#ifndef _GAMEOBJECTS_GAMERAY_H
#define _GAMEOBJECTS_GAMERAY_H


#include "GameObject.h"
#include <hgr/Mesh.h>
#include <hgr/Lines.h>
#include <hgr/Scene.h>


/**
 * Helper class for ray collision checking. 
 */
class GameRay :
	public GameObject
{
public:
	GameRay( lua::LuaState* luastate, GameLevel* level, int collides );
	~GameRay();

	void	setRay( const NS(math,float3)& origin, const NS(math,float3)& dir, float maxlen );

	void	setIgnoreObject( GameObject* ignore );

	int		collided( GameObject* obj, dContact* contacts, int numc );

	const NS(math,float3)&		endPoint() const	{return m_endPoint;}

private:
	NS(math,float3)	m_endPoint;
	GameObject*		m_ignore;

	GameRay( const GameRay& );
	GameRay& operator=( const GameRay& );
};


#endif // _GAMEOBJECTS_GAMERAY_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
