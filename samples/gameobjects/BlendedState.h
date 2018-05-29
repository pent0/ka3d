#ifndef _GAMEOBJECTS_BLENDEDSTATE_H
#define _GAMEOBJECTS_BLENDEDSTATE_H


/** 
 * Helper class for handling animation etc. transitions needed by game objects.
 * Transitions are identified by integer ids (indices).
 */
class BlendedState
{
public:
	enum TransitionState
	{
		TRANSITION_NONE,
		TRANSITION_BLEND,
	};

	BlendedState();

	void	update( float dt );

	// returns number of simultaneous animations active
	int		eval( int* animlist, float* animweights, float* animtimes, int maxanims ) const;

	// starts transition if new != old
	void	setTransition( int target, float transitiontime, bool needstimereset );

	// time elapsed since last transition started
	float	timeSinceTransition() const;

	// returns weight of specified animation
	float	getWeight( int state ) const;

	// state time
	float	time() const				{return m_time;}

	bool	isTransition() const		{return m_transitionState != TRANSITION_NONE;}

	int		state() const				{return m_state;}

	int		transitionTarget() const	{return m_transitionTarget;}

private:
	float				m_time;
	int					m_state;
	TransitionState		m_transitionState;
	int					m_transitionTarget;
	float				m_transitionTime;
	float				m_transitionStart;
	float				m_transitionLength;
};


#endif // _GAMEOBJECTS_BLENDEDSTATE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
