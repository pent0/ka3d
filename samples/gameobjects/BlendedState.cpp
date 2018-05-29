#include "BlendedState.h"
#include <lang/Debug.h>
#include <math/float.h>
#include <assert.h>
#include <config.h>


USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BlendedState::BlendedState() :
	m_time( 0 ),
	m_state( 0 ),
	m_transitionState( BlendedState::TRANSITION_NONE ),
	m_transitionTarget( 0 ),
	m_transitionTime( 0 ),
	m_transitionStart( 0 ),
	m_transitionLength( 0 )
{
}

void BlendedState::update( float dt )
{
	m_time += dt;
	m_transitionTime += dt;

	if ( TRANSITION_BLEND == m_transitionState )
	{
		if ( m_transitionTime >= m_transitionStart+m_transitionLength )
		{
			m_state = m_transitionTarget;
			m_time = m_transitionTime;
			m_transitionState = BlendedState::TRANSITION_NONE;
		}
	}
}

float BlendedState::getWeight( int state ) const
{
	int animlist[4];
	float animweights[4];
	float animtimes[4];
	int animsets = eval( animlist, animweights, animtimes, 4 );
	float w = 0.f;
	for ( int i = 0 ; i < animsets ; ++i )
		if ( animlist[i] == state )
			w += animweights[i];
	return w;
}

int BlendedState::eval( 
	int* animlist,
	float* animweights,
	float* animtimes,
	int maxanims ) const
{
	assert( m_transitionTarget != m_state || m_transitionState == TRANSITION_NONE );

	int animsets = 0;

	switch ( m_transitionState )
	{
	case BlendedState::TRANSITION_NONE:
		animlist[animsets] = m_state;
		animweights[animsets] = 1.f;
		animtimes[animsets] = m_time;
		animsets = 1;
		break;

	case BlendedState::TRANSITION_BLEND:
		assert( m_transitionTarget != m_state );

		animlist[animsets] = m_state;
		animlist[animsets+1] = m_transitionTarget;
		animweights[animsets] = lerp( 1.f, 0.f, (m_transitionTime-m_transitionStart)/m_transitionLength );
		animweights[animsets+1] = 1.f - animweights[animsets];
		animtimes[animsets] = m_time;
		animtimes[animsets+1] = m_transitionTime;
		animsets = 2;
		break;
	}

	assert( animsets < maxanims ); maxanims=maxanims;
	return animsets;
}

void BlendedState::setTransition( int target, float transitiontime, bool needstimereset )
{
	assert( m_transitionTarget != m_state || m_transitionState == TRANSITION_NONE );

	if ( m_transitionState == TRANSITION_NONE )
	{
		m_transitionTarget = target;
		if ( m_transitionTarget != m_state )
		{
			//Debug::printf( "BlendedState %d->%d\n", m_state, target );

			if ( needstimereset )
				m_transitionStart = 0.f;
			else
				m_transitionStart = m_time;

			m_transitionLength = transitiontime;
			m_transitionTime = m_transitionStart;
			m_transitionState = BlendedState::TRANSITION_BLEND;
		}
	}

	assert( m_transitionTarget != m_state || m_transitionState == TRANSITION_NONE );
}

float BlendedState::timeSinceTransition() const 
{
	return m_transitionTime - m_transitionStart;
}
