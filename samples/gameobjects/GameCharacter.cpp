#include "GameCharacter.h"
#include "GameCamera.h"
#include "GameLevel.h"
#include <io/all.h>
#include <hgr/Mesh.h>
#include <lang/Math.h>
#include <lang/Float.h>
#include <lang/Debug.h>
#include <lang/Exception.h>
#include <math/float.h>
#include <math/toString.h>
#include <math/InterpolationUtil.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(io)
USING_NAMESPACE(hgr)
USING_NAMESPACE(lua)
USING_NAMESPACE(hgr)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


GameCharacter::GameCharacter( LuaState* luastate, GameLevel* level, const String& scriptfile ) :
	GameObject( luastate, level ),
	m_controlState( 0 ),
	m_dir( 0.f ),
	m_pitch( 0.f ),
	m_turnSpeed( 1.f/1000.f ),
	m_pitchSpeed( 1.f/500.f ),
	m_bipNode( 0 ),
	m_ambIndex( 0 ),
	m_amb( 0, 0, 0 ),
	m_ambTarget( 0, 0, 0 ),
	m_ambSpeed( 0, 0, 0 ),
	m_nextIdleAnim( 1 ),
	m_readyToFire( true )
{
	for ( int i = 0 ; i < int(sizeof(m_amb0)/sizeof(m_amb0[0])) ; ++i )
		m_amb0[i] = float3(0,0,0);

	// setup dynamic light 
	// (properties blended from other lights by character location)
	m_light = new Light;
	m_light->setEnabled( false ); // affects only the meshes this is explicitly set

	// setup character rigid body and collision object
	float radius = 0.5f;
	float height = 1.8f;
	float mass = 100.f;
	setRigidBodyFromCCylinderY( radius, height, mass, TYPE_PLAYERCHARACTER, 
		TYPE_LEVEL+TYPE_CHARACTER+TYPE_TRIGGER+TYPE_DYNAMIC );
	setAutoDisable( false );

	// run scriptable init() function, and register some C++ methods first for scripting usage
	registerMethod( "loadCharacterAnimation", this, &GameCharacter::loadCharacterAnimation );
	registerMethod( "loadUpperBodyAnimation", this, &GameCharacter::loadUpperBodyAnimation );
	registerMethod( "setSkinnedMesh", this, &GameCharacter::setSkinnedMesh );

	FileInputStream in( scriptfile );
	luastate->compile( &in, in.available(), in.toString(), this );
	call( "init" );
}

GameCharacter::~GameCharacter()
{
}

void GameCharacter::serialize( io::DataInputStream* in, io::DataOutputStream* out )
{
	GameObject::serialize( in, out );

	if ( out )
	{
		out->writeFloat( m_dir );
		out->writeFloat( m_pitch );
	}

	if ( in )
	{
		m_dir = in->readFloat();
		m_pitch = in->readFloat();
	}
}

void GameCharacter::setSkinnedMesh( String filename )
{
	GameObject::setSkinnedMesh( filename );
	m_bipNode = m_root->getNodeByName( "Bip01" );
}

void GameCharacter::setTurnSpeed( float x )
{
	m_turnSpeed = x;
}

void GameCharacter::setPitchSpeed( float x )
{
	m_pitchSpeed = x;
}

void GameCharacter::addWeapon( GameWeapon* weapon )
{
	if ( weapon )
		weapon->setOwner( this );

	m_weapons.add( 0, weapon );
}

void GameCharacter::dropWeapon()
{
	if ( m_weapons.size() > 0 )
	{
		GameWeapon* weapon = m_weapons.first();
		weapon->setOwner( 0 );
		m_weapons.remove( 0 );
	}
}

void GameCharacter::update( float dt )
{
	// update animation states
	m_bodyAnim.update( dt );
	m_upperBodyAnim.update( dt );

	// eval set of body animations
	int animlist[4] = {-1,-1,-1,-1};
	float animweights[4] = {0,0,0,0};
	float animtimes[4] = {0,0,0,0};
	int animsets = m_bodyAnim.eval( animlist, animweights, animtimes, 4 );
	TransformAnimationSet* anims[4];
	for ( int i = 0 ; i < animsets ; ++i )
		anims[i] = getBodyAnimation( animlist[i] );

	// calculate character movement from animation of Bip01 bone
	float3 localvel = getLocalVelocityFromBodyAnimation( anims, animtimes, animweights, animsets );

	updateRotation();
	updateVelocity( dt, localvel );
	updateState();
}

void GameCharacter::updateRotation()
{
	float3x3 rot( float3(0,1,0), m_dir );
	setRotation( rot );
	setAngularVel( float3(0,0,0) );
}

void GameCharacter::updateState()
{
	BodyState nextbodystate = bodyState();
	UpperBodyState nextupperbodystate = upperBodyState();

	// find out next states
	if ( !m_bodyAnim.isTransition() )
		nextbodystate = evalNextBodyState();
	if ( !m_upperBodyAnim.isTransition() )
		nextupperbodystate = evalNextUpperBodyState();

	// resolve dependencies
	if ( nextupperbodystate != UPPERBODY_NONE && isIdle(nextbodystate) )
		nextbodystate = BODY_STAND;
	if ( isJump(nextbodystate) && !isOnGround() )
		nextbodystate = bodyState();

	// fire weapon
	if ( nextupperbodystate == UPPERBODY_SHOOT && hasFireArm() && m_readyToFire )
	{
		weapon()->firePrimary();
		m_readyToFire = false;
	}

	m_bodyAnim.setTransition( nextbodystate, 0.3f, needsTimeReset(nextbodystate) );
	m_upperBodyAnim.setTransition( nextupperbodystate, 0.3f, true );
}

void GameCharacter::applyBodyAnimation()
{
	// eval set of body animations
	int animlist[4] = {-1,-1,-1,-1};
	float animweights[4] = {0,0,0,0};
	float animtimes[4] = {0,0,0,0};
	int animsets = m_bodyAnim.eval( animlist, animweights, animtimes, 4 );
	TransformAnimationSet* anims[4];
	for ( int i = 0 ; i < animsets ; ++i )
		anims[i] = getBodyAnimation( animlist[i] );

	// apply to nodes
	for ( Node* node = m_root ; node != 0 ; node = node->next(m_root) )
		TransformAnimationSet::blend( anims, animtimes, animweights, animsets, node );
}

void GameCharacter::applyUpperBodyAnimation()
{
	// find out blend between none/aim/shoot
	int animlist[4] = {-1,-1,-1,-1};
	float animweights[4] = {0,0,0,0};
	float animtimes[4] = {0,0,0,0};
	int animsets = m_upperBodyAnim.eval( animlist, animweights, animtimes, 4 );
	assert( animsets <= 2 );

	// find out blend between up/front/down
	float pitch = clamp( m_pitch, -1.f, 1.f );
	float u1 = Math::abs(pitch);
	float u0 = 1.f - u1;

	float upperanimweights[4];
	float upperanimtimes[4];
	TransformAnimationSet* anims[4];

	for ( int k = 0 ; k < animsets ; ++k )
	{
		anims[k] = m_upperBodyAnims[ animlist[k] ][VERT_FRONT];
		upperanimweights[k] = animweights[k] * u0;
		upperanimtimes[k] = animtimes[k];
	}
	
	for ( int k = 0 ; k < animsets ; ++k )
	{
		int k1 = animsets+k;
		upperanimweights[k1] = animweights[k] * u1;
		upperanimtimes[k1] = animtimes[k];

		if ( m_pitch < 0 )
			anims[k1] = m_upperBodyAnims[ animlist[k] ][VERT_UP];
		else
			anims[k1] = m_upperBodyAnims[ animlist[k] ][VERT_DOWN];
	}

	// apply upper body animation
	Node* bipspine1 = m_root->getNodeByName( "Bip01 Spine1" );
	for ( Node* node = m_root ; node != 0 ; node = node->next(m_root) )
	{
		if ( node->hasParent(bipspine1) )
			TransformAnimationSet::blend( anims, upperanimtimes, upperanimweights, animsets*2, node );
	}
}

void GameCharacter::updateVelocity( float dt, const float3& localvel )
{
	// calculate change to character world space linear velocity
	// caused by animation
	float3 worldvel = rotation().rotate( localvel );
	float3 oldvel = linearVel();
	float3 imp = (worldvel-oldvel)*mass();
	bool jump = isJump(bodyState()) || isJump(bodyStateTarget());
	if ( !jump ||
		bodyState() == BODY_JUMP_OUT && m_bodyAnim.time() > 2.2f ||
		bodyState() == BODY_JUMP_UP && m_bodyAnim.time() > .5f )
	{
		imp.y = 0.f;
	}
	addImpulse( imp, dt );
}

void GameCharacter::compensateBodyAnimationMovement()
{
	// compensate movement extraction from Bip01 node by translating top level nodes
	Node* bipnode = m_bipNode;
	const float biplevel = bipnode->position().y + 0.01f;
	float3 pos = -bipnode->position();
	pos.y += biplevel;
	for ( Node* n = m_root->firstChild() ; n != 0 ; n = m_root->getNextChild(n) )
		n->setPosition( n->position() + pos );
}

float3 GameCharacter::getLocalVelocityFromBodyAnimation( TransformAnimationSet** anims, 
	const float* animtimes, const float* animweights, int animsets )
{
	// calculate character movement from animation of Bip01 bone
	float3 localvel(0,0,0);
	Node* bipnode = m_bipNode;
	bool jump = isJump(bodyState()) || isJump(bodyStateTarget());
	for ( int i = 0 ; i < animsets ; ++i )
	{
		TransformAnimation* a = (*anims[i])[ bipnode->name() ];
		if ( a != 0 )
		{
			float weight = animweights[i];
			if ( jump )
			{
				weight = 1.f;
				if ( !m_bodyAnim.isTransition() )
					weight = 5.f;
			}
			localvel += a->getLinearVelocity( animtimes[i] ) * weight;
		}
	}
	return localvel;
}

GameCharacter::UpperBodyState GameCharacter::evalNextUpperBodyState()
{
	UpperBodyState next = upperBodyState();

	switch ( upperBodyState() )
	{
	case UPPERBODY_NONE:
		if ( getControlState(CONTROL_FIRE_PRIMARY) && hasFireArm() )
			next = UPPERBODY_AIM;
		break;

	case UPPERBODY_AIM:
		if ( !hasFireArm() )
			next = UPPERBODY_NONE;
		else if ( getControlState(CONTROL_FIRE_PRIMARY) )
			next = UPPERBODY_SHOOT;
		else if ( m_upperBodyAnim.timeSinceTransition() > 6.f )
			next = UPPERBODY_NONE;
		break;

	case UPPERBODY_SHOOT:
		if ( isUpperBodyAnimationEnd(0) )
			next = UPPERBODY_AIM;
		break;
	}

	return next;
}

GameCharacter::BodyState GameCharacter::evalNextBodyState()
{
	BodyState next = bodyState();

	if ( BODY_WALK_FORWARD == bodyState() )
	{
		// walk state update
		if ( !getControlState(CONTROL_WALK_FORWARD) )
			next = BODY_STAND;
		if ( getControlState(CONTROL_JUMP) )
			next = BODY_JUMP_OUT;
		if ( getControlState(CONTROL_STEP_LEFT) )
			next = BODY_STEP_LEFT;
		if ( getControlState(CONTROL_STEP_RIGHT) )
			next = BODY_STEP_RIGHT;
	}
	else if ( BODY_STAND == bodyState() || isIdle(bodyState()) )
	{
		// trigger new idle animation if more than x seconds elapsed without action
		if ( m_bodyAnim.timeSinceTransition() > 6.f && 
			upperBodyState() == UPPERBODY_NONE &&
			isBodyAnimationEnd(0) )
		{
			next = selectNextIdle();
		}

		// idle state update
		if ( getControlState(CONTROL_WALK_FORWARD) )
			next = BODY_WALK_FORWARD;
		if ( getControlState(CONTROL_JUMP) )
			next = BODY_JUMP_UP;
		if ( getControlState(CONTROL_STEP_LEFT) )
			next = BODY_STEP_LEFT;
		if ( getControlState(CONTROL_STEP_RIGHT) )
			next = BODY_STEP_RIGHT;
	}
	else if ( isJump(bodyState()) )
	{
		// jump state update
		if ( isBodyAnimationEnd(0.3f) ) //  state re-evaluation needed?
			next = BODY_STAND;
	}
	else if ( isStep(bodyState()) )
	{
		// step state update
		if ( isBodyAnimationEnd(0.3f) ) //  state re-evaluation needed?
			next = BODY_STAND;
	}

	return next;
}

int GameCharacter::collided( GameObject* /*obj*/, dContact* contacts, int numc )
{
	for ( int i = 0 ; i < numc ; ++i )
	{
		contacts[i].surface.mode = dContactBounce | dContactSoftCFM;
		contacts[i].surface.mu = 0.f; // dInfinity
		contacts[i].surface.bounce = 0.1f;
		contacts[i].surface.bounce_vel = 0.01f;
		contacts[i].surface.soft_cfm = 0.000001f;
	}
	return numc;
}

void GameCharacter::getObjectsToRender( GameCamera* camera )
{
	// sample lightmap lighting (one corner per frame)
	float3 ambcorner = position() + float3(0,1,0);
	float ambr = 0.3f;
	switch ( m_ambIndex )
	{
	case 0:	ambcorner += float3( ambr, 0.f, ambr ); break;
	case 1:	ambcorner += float3( ambr, 0.f, -ambr ); break;
	case 2:	ambcorner += float3( -ambr, 0.f, -ambr ); break;
	case 3:	ambcorner += float3( -ambr, 0.f, ambr ); break;
	case 4:	ambcorner += float3( ambr, 0.f, 0.f ); break;
	case 5:	ambcorner += float3( 0.f, 0.f, -ambr ); break;
	case 6:	ambcorner += float3( -ambr, 0.f, 0.f ); break;
	case 7:	ambcorner += float3( 0.f, 0.f, ambr ); break;
	}
	m_level->sampleLightmap( ambcorner, float3(0,-5,0), &m_amb0[m_ambIndex&7] );
	m_ambIndex = (m_ambIndex+1) & 7;

	// update node transforms from body animations
	applyBodyAnimation();
	compensateBodyAnimationMovement();
	applyUpperBodyAnimation();

	// set root node transform
	float3 pos = position();
	m_root->setPosition( pos );
	m_root->setRotation( rotation() );

	// setup character lightmap based lighting (unused for now)
	m_amb = (m_amb0[0]+m_amb0[1]+m_amb0[2]+m_amb0[3]+m_amb0[4]+m_amb0[5]+m_amb0[6]+m_amb0[7]) * .125f;

	// setup character direct lighting
	m_level->blendLights( pos, m_light );
	m_light->setColor( m_light->color() );
	m_mesh->removeLights();
	m_mesh->addLight( m_light );
	camera->addObjectToRender( m_root );
	camera->addObjectToRender( m_light );

	// weapon
	if ( hasFireArm() )
	{
		GameWeapon* weapon = this->weapon();

		float3x4 handtm = m_root->getNodeByName( weapon->parentBone() )->worldTransform();
		float aimweight = m_upperBodyAnim.getWeight(UPPERBODY_AIM) + m_upperBodyAnim.getWeight(UPPERBODY_SHOOT);

		weapon->setLight( m_light );
		weapon->setAim( handtm, camera->aimPoint(), aimweight );
	}
}

float3 GameCharacter::ambient() const
{
	return m_amb;
}

void GameCharacter::setControlState( ControlState state, float value )
{
	assert( state < 32 );

	if ( state == CONTROL_FIRE_PRIMARY && value < 0.5f )
		m_readyToFire = true;

	const int flag = (1<<state);
	m_controlState &= ~flag;
	if ( value > 0.5f )
		m_controlState += flag;

	if ( CONTROL_TURN_LEFT == state )
		m_dir -= value * m_turnSpeed;
	else if ( CONTROL_TURN_RIGHT == state )
		m_dir += value * m_turnSpeed;

	if ( CONTROL_TURN_UP == state )
		m_pitch -= value * m_pitchSpeed;
	else if ( CONTROL_TURN_DOWN == state )
		m_pitch += value * m_pitchSpeed;
	m_pitch = clamp( m_pitch, -1.f, 1.f );
}

void GameCharacter::resetControlState()
{
	m_controlState = 0;
}

bool GameCharacter::getControlState( ControlState state ) const
{
	assert( state < 32 );
	const int flag = (1<<state);
	return (m_controlState & flag) != 0;
}

GameCharacter::BodyState GameCharacter::selectNextIdle()
{
	m_nextIdleAnim = (m_nextIdleAnim+1) % (BODY_IDLE2-BODY_IDLE1+1);
	return BodyState( BODY_IDLE1 + m_nextIdleAnim );
}

void GameCharacter::loadCharacterAnimation( NS(lang,String) statename, NS(lang,String) filename, NS(lang,String) endbehavior )
{
	BodyState bodystate = toBodyState( statename );
	TransformAnimation::BehaviourType endbeh = TransformAnimation::toBehaviour( endbehavior );
	m_bodyAnims[bodystate] = loadCharacterAnimationImpl( filename, endbeh );
}

void GameCharacter::loadUpperBodyAnimation( NS(lang,String) statename, NS(lang,String) direction, NS(lang,String) filename, NS(lang,String) endbehavior )
{
	VerticalDirection dir = VERT_DOWN;
	if ( direction == "DOWN" )
		dir = VERT_DOWN;
	else if ( direction == "FRONT" )
		dir = VERT_FRONT;
	else if ( direction == "UP" )
		dir = VERT_UP;
	else
		throwError( Exception( Format("Invalid upper body direction: {0}", direction) ) );

	UpperBodyState bodystate = toUpperBodyState( statename );
	TransformAnimation::BehaviourType endbeh = TransformAnimation::toBehaviour( endbehavior );
	m_upperBodyAnims[bodystate][dir] = loadCharacterAnimationImpl( filename, endbeh );
}

P(TransformAnimationSet) GameCharacter::loadCharacterAnimationImpl( 
	const String& filename, TransformAnimation::BehaviourType endbehaviour )
{
	assert( m_root != 0 );

	Context* context = this->context();

	// load animation set
	P(Scene) scene = new Scene( context, filename );
	P(TransformAnimationSet) animset = scene->transformAnimations();
	assert( animset != 0 );

	// make sure root bone Bip01 points to forward
	TransformAnimation* anim = animset->get( "Bip01" );
	assert( anim );
	float3x4 tm;
	anim->eval( 0.f, &tm );
	if ( tm.rotation().getColumn(0).z > 0.f )
		animset->rotate( m_root, float3x3(float3(0,1,0), Math::PI) );

	// scale inches to meters
	const float INCH_M = 0.02540000508f;
	animset->scale( m_root, INCH_M );

	animset->setEndBehaviour( endbehaviour );

	Debug::printf( "Animation \"%s\" length is %g seconds\n", filename.c_str(), animset->endTime() );
	return animset;
}

TransformAnimationSet* GameCharacter::getBodyAnimation( int bodystate ) const
{
	assert( bodystate >= 0 && bodystate < BODY_COUNT );
	TransformAnimationSet* a = m_bodyAnims[bodystate];
	if ( !a )
		throwError( Exception( Format( "Character \"{0}\" animation for state {1} has not been set", getString("name"), toString((BodyState)bodystate) ) ) );
	assert( a != 0 );
	return a;
}

bool GameCharacter::isBodyAnimationEnd( float dt ) const
{
	return !m_bodyAnim.isTransition() && 
		dt+m_bodyAnim.time() >= getBodyAnimation(bodyState())->endTime();
}

bool GameCharacter::isUpperBodyAnimationEnd( float dt ) const
{
	TransformAnimationSet* a = m_upperBodyAnims[upperBodyState()][VERT_FRONT];
	return !m_upperBodyAnim.isTransition() && 
		( !a || dt+m_upperBodyAnim.time() >= a->endTime() );
}

const char*	GameCharacter::toString( UpperBodyState state )
{
	static const char* sz[] = 
	{
		"NONE",
		"AIM",
		"SHOOT",
	};
	assert( sizeof(sz)/sizeof(sz[0]) == UPPERBODY_COUNT );
	return sz[state];
}

const char* GameCharacter::toString( BodyState bodystate )
{
	static const char* sz[] = 
	{
		"STAND",
		"WALK_FORWARD",
		"STEP_LEFT",
		"STEP_RIGHT",
		"JUMP_UP",
		"JUMP_OUT",
		"IDLE1",
		"IDLE2",
	};
	assert( sizeof(sz)/sizeof(sz[0]) == BODY_COUNT );
	return sz[bodystate];
}

GameCharacter::BodyState GameCharacter::toBodyState( const String& bodystate )
{
	String str = bodystate.toUpperCase();
	for ( int i = 0 ; i < BODY_COUNT ; ++i )
		if ( str == toString( (BodyState)i ) )
			return (BodyState)i;

	if ( str != "COUNT" )
		throwError( Exception( Format("Invalid character body animation state: {0}", bodystate) ) );
	return BODY_COUNT;
}

GameCharacter::UpperBodyState GameCharacter::toUpperBodyState( const String& bodystate )
{
	String str = bodystate.toUpperCase();
	for ( int i = 0 ; i < UPPERBODY_COUNT ; ++i )
		if ( str == toString( (UpperBodyState)i ) )
			return (UpperBodyState)i;
	if ( str != "COUNT" )
		throwError( Exception( Format("Invalid character upper body animation state: {0}", bodystate) ) );
	return UPPERBODY_COUNT;
}

const char*	GameCharacter::bodyStateString() const
{
	return toString( bodyState() );
}

const char*	GameCharacter::upperBodyStateString() const
{
	return toString( upperBodyState() );
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
