#ifndef _GAMEOBJECTS_GAMECHARACTER_H
#define _GAMEOBJECTS_GAMECHARACTER_H


#include "GameObject.h"
#include "GameWeapon.h"
#include "BlendedState.h"
#include <hgr/Scene.h>
#include <hgr/Light.h>
#include <hgr/TransformAnimationSet.h>


namespace gr {
	class Context;}


class GameCharacter :
	public GameObject
{
public:
	enum BodyState
	{
		BODY_STAND,
		BODY_WALK_FORWARD,
		BODY_STEP_LEFT,
		BODY_STEP_RIGHT,
		BODY_JUMP_UP,
		BODY_JUMP_OUT,
		BODY_IDLE1,
		BODY_IDLE2,
		BODY_COUNT
	};

	enum UpperBodyState
	{
		UPPERBODY_NONE,
		UPPERBODY_AIM,
		UPPERBODY_SHOOT,
		UPPERBODY_COUNT
	};

	enum ControlState
	{
		CONTROL_WALK_FORWARD,
		CONTROL_TURN_LEFT,
		CONTROL_TURN_RIGHT,
		CONTROL_TURN_UP,
		CONTROL_TURN_DOWN,
		CONTROL_JUMP,
		CONTROL_STEP_LEFT,
		CONTROL_STEP_RIGHT,
		CONTROL_FIRE_PRIMARY,
		CONTROL_COUNT
	};

	enum VerticalDirection
	{
		VERT_DOWN,
		VERT_FRONT,
		VERT_UP,
		VERT_COUNT
	};

	GameCharacter( lua::LuaState* luastate, GameLevel* level, const NS(lang,String)& scriptname );

	~GameCharacter();

	void				update( float dt );

	int					collided( GameObject* obj, dContact* contacts, int numc );

	void				getObjectsToRender( GameCamera* camera );

	void				setControlState( ControlState state, float value );

	void				resetControlState();

	bool				getControlState( ControlState state ) const;

	void				setSkinnedMesh( NS(lang,String) filename );

	void				setTurnSpeed( float x );

	void				setPitchSpeed( float x );

	void				addWeapon( GameWeapon* weapon );

	void				dropWeapon();

	void				setAnimationDirectory( NS(lang,String) dir );

	void				serialize( io::DataInputStream* in, io::DataOutputStream* out );

	hgr::Mesh*			mesh() const;

	BodyState			bodyState() const;

	BodyState			bodyStateTarget() const;

	UpperBodyState		upperBodyState() const;

	float				bodyStateTime() const;

	float				upperBodyStateTime() const;

	const char*			upperBodyStateString() const;

	const char*			bodyStateString() const;

	float				direction() const;

	float				pitch() const;

	bool				hasFireArm() const;

	GameWeapon*			weapon() const;

	bool				isAimingOrShooting() const;

	NS(math,float3)		ambient() const;

	static const char*	toString( BodyState bodystate );

	static const char*	toString( UpperBodyState bodystate );

	static 	BodyState		toBodyState( const NS(lang,String)& bodystate );

	static UpperBodyState	toUpperBodyState( const NS(lang,String)& bodystate );

private:
	// input controller state
	int								m_controlState;
	float							m_dir;
	float							m_pitch;

	// input controller settings
	float							m_turnSpeed;
	float							m_pitchSpeed;

	// additional character mesh objects
	P(hgr::Light)					m_light;
	hgr::Node*						m_bipNode;

	// environment lighting
	NS(math,float3)					m_amb0[8];
	int								m_ambIndex;
	NS(math,float3)					m_ambTarget;
	NS(math,float3)					m_ambSpeed;
	NS(math,float3)					m_amb;

	// body animations
	NS(lang,String)					m_animDir;
	P(hgr::TransformAnimationSet)	m_bodyAnims[BODY_COUNT];

	// upper body animations
	P(hgr::TransformAnimationSet)	m_upperBodyAnims[UPPERBODY_COUNT][VERT_COUNT];

	// animation state
	BlendedState					m_bodyAnim;
	BlendedState					m_upperBodyAnim;
	int								m_nextIdleAnim;
	
	// weapons
	NS(lang,Array)<P(GameWeapon)>		m_weapons;
	bool							m_readyToFire;

	P(hgr::TransformAnimationSet)	loadCharacterAnimationImpl( const NS(lang,String)& filename, hgr::TransformAnimation::BehaviourType endbehaviour=hgr::TransformAnimation::BEHAVIOUR_REPEAT );
	void							loadCharacterAnimation( NS(lang,String) bodystate, NS(lang,String) filename, NS(lang,String) endbehavior );
	void							loadUpperBodyAnimation( NS(lang,String) bodystate, NS(lang,String) direction, NS(lang,String) filename, NS(lang,String) endbehavior );

	void							updateRotation();
	void							updateState();

	UpperBodyState					evalNextUpperBodyState();
	void							applyUpperBodyAnimation();
	bool							isUpperBodyAnimationEnd( float dt ) const;

	BodyState						selectNextIdle();
	BodyState						evalNextBodyState();
	void							applyBodyAnimation();
	NS(math,float3)					getLocalVelocityFromBodyAnimation( hgr::TransformAnimationSet** anims, const float* animtimes, const float* animweights, int animsets );
	hgr::TransformAnimationSet*		getBodyAnimation( int bodystate ) const;
	bool							isBodyAnimationEnd( float dt ) const;

	void							compensateBodyAnimationMovement();
	void							updateVelocity( float dt, const NS(math,float3)& localvel );

	static bool						isStep( BodyState bodystate );
	static bool						isJump( BodyState bodystate );
	static bool						isIdle( BodyState bodystate );
	static bool						isRepeated( BodyState bodystate );
	static bool						needsTimeReset( BodyState bodystate );

	GameCharacter( const GameCharacter& );
	GameCharacter& operator=( const GameCharacter& );
};


#include "GameCharacter.inl"


#endif // _GAMEOBJECTS_GAMECHARACTER_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
