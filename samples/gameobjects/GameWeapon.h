#ifndef _GAMEOBJECTS_GAMEWEAPON_H
#define _GAMEOBJECTS_GAMEWEAPON_H


#include "GameObject.h"
#include <hgr/Mesh.h>
#include <hgr/Lines.h>
#include <hgr/Scene.h>


class GameWeapon :
	public GameObject
{
public:
	GameWeapon( lua::LuaState* luastate, GameLevel* level );
	~GameWeapon();

	void		setOwner( GameObject* obj );

	void		setMesh( NS(lang,String) filename );

	void		setParentBone( NS(lang,String) bonename );

	void		setPrimaryEmissionBone( NS(lang,String) nodename );

	void		setSecondaryEmissionBone( NS(lang,String) nodename );

	void		setPrimaryAmmo( NS(lang,String) ammoname );

	void		setSecondaryAmmo( NS(lang,String) ammoname );

	void		update( float dt );

	void		getObjectsToRender( GameCamera* camera );

	void		setLight( hgr::Light* lt );

	/**
	 * Fixes weapon aim direction.
	 * @param handtm Weapon's parent transform in world space.
	 */
	void		setAim( const NS(math,float3x4)& handtm, const NS(math,float3)& target, float aimweight );

	void		firePrimary();

	hgr::Node*				root() const		{return m_root;}

	const NS(lang,String)&		parentBone() const	{return m_parentBone;}

private:
	struct Ammo
	{
		NS(math,float3)	pos;
		NS(math,float3)	vel;
		float			time;
	};

	P(hgr::Lines)		m_lines;
	NS(lang,String)		m_parentBone;
	NS(lang,String)		m_emission1;
	NS(lang,String)		m_emission2;
	NS(lang,String)		m_ammo1;
	NS(lang,String)		m_ammo2;
	NS(math,float3)		m_target;
	NS(lang,Array)<Ammo>	m_shots;
	NS(math,float3x4)		m_bodytm;
	GameObject*			m_owner;
	hgr::Mesh*			m_muzzleflash;
	float				m_muzzleflashVisible;

	GameWeapon( const GameWeapon& );
	GameWeapon& operator=( const GameWeapon& );
};


#endif // _GAMEOBJECTS_GAMEWEAPON_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
