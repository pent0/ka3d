//
// Tutorial: Character animation
//
// Movements:
// - Walk forward
// - Jump up (=jump while standing still)
// - Jump forward (=jump while walking)
// - Step left
// - Step right
// - Idle stand
// - Idle look around (triggered after 4 seconds of no user input)
// - Idle stretch
//
// Keys:
// - Left and right arrow turn
// - W walks forward
// - Space jumps
// - A steps left
// - D steps right
// - F4 slowmotion
//
// Tested on:
// - WinXP/DX9/Athlon XP 2700+/ATI Radeon 9600
//
#include "CharacterAnimationApp.h"
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(io)
USING_NAMESPACE(hgr)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)
USING_NAMESPACE(framework)


CharacterAnimationApp::CharacterAnimationApp( OSInterface* os, Context* context ) :
	App( os ),
	m_time( 0 ),
	m_state( STATE_STAND ),
	m_transitionState( TRANSITION_NONE ),
	m_transitionTarget( STATE_STAND ),
	m_transitionTime( 0 ),
	m_transitionStart( 0 ),
	m_transitionLength( 0 ),
	m_idleAnimIndex( 1 )
{
	// setup scene, key light and camera
	m_scene = new Scene;
	P(Light) lt = new Light;
	lt->linkTo( m_scene );
	lt->setPosition( float3(0,3,0) );
	m_camera = new Camera;
	m_camera->linkTo( m_scene );

	// setup character mesh
	m_character = new Scene( context, "data/mesh/zax_mesh.hgr" );
	m_character->linkTo( m_scene );

	// load character animations
	m_anims[STATE_STAND] = 
	loadCharacterAnimation( context, "data/anim/idle_stand.hgr" );

	m_anims[STATE_WALK_FORWARD] = 
	loadCharacterAnimation( context, "data/anim/walk_frwd.hgr" );

	m_anims[STATE_JUMP_UP] = 
	loadCharacterAnimation( context, "data/anim/jump_up.hgr", 
		TransformAnimation::BEHAVIOUR_STOP );

	m_anims[STATE_JUMP_OUT] = 
	loadCharacterAnimation( context, "data/anim/jump_out.hgr", 
		TransformAnimation::BEHAVIOUR_STOP );

	m_anims[STATE_STEP_LEFT] = 
	loadCharacterAnimation( context, "data/anim/sidestep_left.hgr",
		TransformAnimation::BEHAVIOUR_STOP );

	m_anims[STATE_STEP_RIGHT] = 
	loadCharacterAnimation( context, "data/anim/sidestep_right.hgr",
		TransformAnimation::BEHAVIOUR_STOP );

	m_anims[STATE_IDLE1] = 
	loadCharacterAnimation( context, "data/anim/idle_look_around.hgr" );

	m_anims[STATE_IDLE2] = 
	loadCharacterAnimation( context, "data/anim/idle_stretch.hgr" );

	// setup debug line grid
	P(Lines) linegrid = createXZGrid( context, 30.f, 1.f );
	linegrid->linkTo( m_scene );

	// setup rendering pipelines
	m_pipeSetup = new PipeSetup( context );
	m_defaultPipe = new DefaultPipe( m_pipeSetup );
	m_glowPipe = new GlowPipe( m_pipeSetup );
}

P(TransformAnimationSet) CharacterAnimationApp::loadCharacterAnimation( 
	Context* context, const String& filename, 
	TransformAnimation::BehaviourType endbehaviour )
{
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
		animset->rotate( m_character, float3x3(float3(0,1,0), Math::PI) );

	// character animations have been created in inch units
	// so scale them to metric system
	const float INCH_M = 0.02540000508f;
	animset->scale( m_character, INCH_M );

	animset->setEndBehaviour( endbehaviour );
	return animset;
}

void CharacterAnimationApp::update( float dt, Context* context )
{
	animateCharacter( dt );
	animateCamera( dt );
	render( context );
}

void CharacterAnimationApp::render( Context* context )
{
	// render frame
	{
		Context::RenderScene rs( context );
		m_pipeSetup->setup( m_camera );
		m_defaultPipe->render( 0, context, m_scene, m_camera );
		m_glowPipe->render( 0, context, m_scene, m_camera );
	}

	// flip back buffer
	context->present();
}

void CharacterAnimationApp::animateCamera( float dt )
{
	// update camera position based on character transform
	float3x4 targettm = m_character->transform();
	float3 cameradistv( 0.f, 2.5f, 3.f );
	m_camera->setPosition( targettm.transform(cameradistv) );
	m_camera->lookAt( targettm.translation() + float3(0,1.5f,0) );

	// set idle camera position
	if ( isIdle(m_state) && m_state != STATE_STAND )
	{
		m_camera->setPosition( targettm.transform(float3(0,0.2f,-2)) );
		m_camera->lookAt( targettm.transform(float3(0,1.5f,0)) );
	}
}

void CharacterAnimationApp::animateCharacter( float dt )
{
	m_time += dt;
	m_transitionTime += dt;

	// select active animations
	TransformAnimationSet* animlist[2] = {0,0};
	float animweights[2] = {0,0};
	float animtimes[2] = {0,0};
	int animsets = 0;
	switch ( m_transitionState )
	{
	case TRANSITION_NONE:
		animlist[animsets] = getAnimation( m_state );
		animweights[animsets] = 1.f;
		animtimes[animsets] = m_time;
		animsets = 1;
		break;

	case TRANSITION_BLEND:
		animlist[animsets+1] = getAnimation( m_transitionTarget );
		animlist[animsets] = getAnimation( m_state );

		animweights[animsets+1] = (m_transitionTime-m_transitionStart)
			/ m_transitionLength;
		animweights[animsets] = 1.f - animweights[animsets+1];

		animtimes[animsets+1] = m_transitionTime;
		animtimes[animsets] = m_time;

		animsets = 2;

		if ( m_transitionTime >= m_transitionStart+m_transitionLength )
		{
			Debug::printf( "Transition done: %s (t=%g)\n", 
				toString(m_transitionTarget), m_transitionTime );

			m_state = m_transitionTarget;
			m_time = m_transitionTime;
			m_transitionState = TRANSITION_NONE;
		}
		break;
	}

	// update node transforms from animations
	for ( Node* node = m_character ; node != 0 ; node = node->next(m_character) )
		TransformAnimationSet::blend( animlist, animtimes, 
			animweights, animsets, node  );

	// calculate character speed from animation of Bip01 bone
	float3 localvelocity(0,0,0);
	Node* bipnode = m_character->getNodeByName( "Bip01" );
	assert( bipnode );
	for ( int i = 0 ; i < animsets ; ++i )
	{
		TransformAnimation* a = (*animlist[i])[ bipnode->name() ];
		if ( a != 0 )
		{
			float weight = animweights[i];
			localvelocity += 
				a->getLinearVelocity( animtimes[i] ) * weight;
		}
	}

	// compensate movement extraction from Bip01 node 
	// by translating top level nodes
	const float biplevel = bipnode->position().y + .01f;
	float3 pos = -bipnode->position();
	pos.y += biplevel;
	for ( Node* n = m_character->firstChild() ; n != 0 ; 
		n = m_character->getNextChild(n) )
	{
		n->setPosition( n->position() + pos );
	}

	// make sure we're on ground if not jumping
	if ( !isJump(m_state) && !isJump(m_transitionTarget) )
	{
		float3 pos = m_character->position();
		pos.y = 0.f;
		m_character->setPosition( pos );
	}

	// update character rotation
	float3x3 rot = m_character->rotation();
	float angle = Math::PI * dt * .5f;
	if ( isKeyDown(KEY_LEFT) )
		rot = rot * float3x3(float3(0,1,0),-angle);
	if ( isKeyDown(KEY_RIGHT) )
		rot = rot * float3x3(float3(0,1,0),angle);

	// orthonormalize rotation so that character doesn't tilt
	float3 xaxis = rot.getColumn(0);
	float3 yaxis = rot.getColumn(1);
	xaxis.y = 0.f;
	yaxis.x = 0.f;
	yaxis.z = 0.f;
	rot.setColumn( 0, xaxis );
	rot.setColumn( 1, yaxis );
	rot = rot.orthonormalize();
	m_character->setRotation( rot );

	// update character position by world space linear velocity
	float3 worldvelocity = rot.rotate( localvelocity );
	worldvelocity.y = 0.f;
	m_character->setPosition( m_character->position() + worldvelocity*dt );

	// update character state
	if ( TRANSITION_NONE == m_transitionState )
	{
		evaluateTransitionTarget();

		// start transition if changed
		if ( m_transitionTarget != m_state )
		{
			Debug::printf( "%s -> %s (t=%g)\n", toString(m_state), 
				toString(m_transitionTarget), m_time );

			m_transitionStart = m_time;
			if ( needsTimeReset(m_transitionTarget) )
				m_transitionStart = 0.f;

			m_transitionLength = 0.30f;
			m_transitionTime = m_transitionStart;
			m_transitionState = TRANSITION_BLEND;
		}
	}
}

void CharacterAnimationApp::evaluateTransitionTarget()
{
	m_transitionTarget = m_state;

	if ( STATE_WALK_FORWARD == m_state )
	{
		// walk state update
		if ( !isKeyDown(KEY_UP) && !isKeyDown(KEY_W) )
			m_transitionTarget = STATE_STAND;
		if ( isKeyDown(KEY_SPACE) )
			m_transitionTarget = STATE_JUMP_OUT;
		if ( isKeyDown(KEY_A) )
			m_transitionTarget = STATE_STEP_LEFT;
		if ( isKeyDown(KEY_D) )
			m_transitionTarget = STATE_STEP_RIGHT;
	}
	else if ( STATE_STAND == m_state || isIdle(m_state) )
	{
		// trigger new idle animation if more than 4 seconds elapsed
		if ( m_transitionTime-m_transitionStart > 4.f && 
			isAnimationEnd(0) )
		{
			m_transitionTarget = selectIdle();
		}

		// idle state update
		if ( isKeyDown(KEY_UP) || isKeyDown(KEY_W) )
			m_transitionTarget = STATE_WALK_FORWARD;
		if ( isKeyDown(KEY_SPACE) )
			m_transitionTarget = STATE_JUMP_UP;
		if ( isKeyDown(KEY_A) )
			m_transitionTarget = STATE_STEP_LEFT;
		if ( isKeyDown(KEY_D) )
			m_transitionTarget = STATE_STEP_RIGHT;
	}
	else if ( isJump(m_state) )
	{
		// jump state update
		if ( isAnimationEnd(0.1f) ) //  state re-evaluation needed?
			m_transitionTarget = STATE_STAND;
	}
	else if ( isStep(m_state) )
	{
		// step state update
		if ( isAnimationEnd(0.3f) ) //  state re-evaluation needed?
			m_transitionTarget = STATE_STAND;
	}
}

TransformAnimationSet* 
	CharacterAnimationApp::getAnimation( State bodystate ) const
{
	assert( m_anims[bodystate] != 0 );
	return m_anims[bodystate];
}

P(Lines) CharacterAnimationApp::createXZGrid( 
	NS(gr,Context)* context, float r, float dr )
{
	P(Lines) lines = new Lines( context );
	
	lines->addLine( float3(-r,0,0), float3(r,0,0), float4(1,0,0,0.5f) );
	lines->addLine( float3(0,0,-r), float3(0,0,r), float4(0,0,1,0.5f) );

	for ( float x = -r ; x < r ; x += dr )
	{
		if ( Math::abs(x) > dr*.5f )
			lines->addLine( float3(x,0,-r), float3(x,0,r), float4(1,1,1,0.25f) );
	}

	for ( float z = -r ; z < r ; z += dr )
	{
		if ( Math::abs(z) > dr*.5f )
			lines->addLine( float3(-r,0,z), float3(r,0,z), float4(1,1,1,0.25f) );
	}

	return lines;
}

void CharacterAnimationApp::keyDown( KeyType key )
{
	if ( KEY_F5 == key )
	{
		m_character->setPosition( float3(0,0,0) );
		m_time = 0.f;
	}

	App::keyDown( key );
}

CharacterAnimationApp::State CharacterAnimationApp::selectIdle()
{
	m_idleAnimIndex = (m_idleAnimIndex+1) % (STATE_IDLE2-STATE_IDLE1+1);
	return State( STATE_IDLE1 + m_idleAnimIndex );
}

bool CharacterAnimationApp::isAnimationEnd( float dt ) const
{
	return m_transitionState == TRANSITION_NONE && dt+m_time >= getAnimation(m_state)->endTime();
}

bool CharacterAnimationApp::isJump( State bodystate )
{
	return STATE_JUMP_UP == bodystate || STATE_JUMP_OUT == bodystate;
}

bool CharacterAnimationApp::isStep( State bodystate )
{
	return STATE_STEP_LEFT == bodystate || STATE_STEP_RIGHT == bodystate;
}

bool CharacterAnimationApp::isIdle( State bodystate )
{
	return bodystate == STATE_IDLE1 || bodystate == STATE_IDLE2;
}

bool CharacterAnimationApp::isRepeated( State bodystate )
{
	return !isJump(bodystate) && !isStep(bodystate);
}

bool CharacterAnimationApp::needsTimeReset( State bodystate )
{
	return isIdle(bodystate) || isJump(bodystate) || isStep(bodystate);
}

const char* CharacterAnimationApp::toString( State bodystate )
{
	static const char* sz[] = {
		"STAND",
		"WALK_FORWARD",
		"STEP_LEFT",
		"STEP_RIGHT",
		"JUMP_UP",
		"JUMP_OUT",
		"IDLE1",
		"IDLE2",
	};
	assert( sizeof(sz)/sizeof(sz[0]) == STATE_COUNT );
	return sz[bodystate];
}

void framework::configure( App::Configuration& config )
{
	config.name = "Tutorial: Character Animation";
}

App* framework::init( OSInterface* os, NS(gr,Context)* context )
{
	return new CharacterAnimationApp( os, context );
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
