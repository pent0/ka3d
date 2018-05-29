inline hgr::Mesh* GameCharacter::mesh() const				
{
	return m_mesh;
}

inline GameCharacter::BodyState GameCharacter::bodyState() const			
{
	return (BodyState)m_bodyAnim.state();
}

inline GameCharacter::UpperBodyState GameCharacter::upperBodyState() const
{
	return (UpperBodyState)m_upperBodyAnim.state();
}

inline GameCharacter::BodyState GameCharacter::bodyStateTarget() const		
{
	return (BodyState)m_bodyAnim.transitionTarget();
}

inline float GameCharacter::bodyStateTime() const		
{
	return m_bodyAnim.time();
}

inline float GameCharacter::upperBodyStateTime() const		
{
	return m_upperBodyAnim.time();
}

inline float GameCharacter::direction() const			
{
	return m_dir;
}

inline float GameCharacter::pitch() const				
{
	return m_pitch;
}

inline bool GameCharacter::hasFireArm() const
{
	return m_weapons.size() > 0 && m_weapons[0] != 0;
}

inline bool GameCharacter::isJump( BodyState bodystate )
{
	return BODY_JUMP_UP == bodystate || BODY_JUMP_OUT == bodystate;
}

inline bool GameCharacter::isStep( BodyState bodystate )
{
	return BODY_STEP_LEFT == bodystate || BODY_STEP_RIGHT == bodystate;
}

inline bool GameCharacter::isIdle( BodyState bodystate )
{
	return bodystate == BODY_IDLE1 || bodystate == BODY_IDLE2;
}

inline bool GameCharacter::isRepeated( BodyState bodystate )
{
	return !isJump(bodystate) && !isStep(bodystate);
}

inline bool GameCharacter::needsTimeReset( BodyState bodystate )
{
	return isIdle(bodystate) || isJump(bodystate) || isStep(bodystate);
}

inline GameWeapon* GameCharacter::weapon() const
{
	assert( m_weapons.size() > 0 );
	return m_weapons.first();
}

inline bool	GameCharacter::isAimingOrShooting() const
{
	UpperBodyState s = upperBodyState();
	return s == UPPERBODY_AIM || s == UPPERBODY_SHOOT;
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
