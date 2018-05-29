#ifndef _HGR_TRANSFORMANIMATION_H
#define _HGR_TRANSFORMANIMATION_H


#include <hgr/KeyframeSequence.h>
#include <math/float4.h>
#include <lang/Object.h>
#include <stdint.h>


BEGIN_NAMESPACE(lang) 
	class String;
	END_NAMESPACE()

BEGIN_NAMESPACE(math) 
	class float3;
	class float3x3;
	class float3x4;
	class quaternion;
	END_NAMESPACE()


BEGIN_NAMESPACE(hgr) 


/**
 * Key frame based affine transformation animation container.
 * @ingroup hgr
 */
class TransformAnimation :
	public NS(lang,Object)
{
public:
	/** Key frame sequence with variable time. */
	class Float3Anim :
		public NS(lang,Object)
	{
	public:
		/** Position/scale plus time in w component. */
		NS(lang,Array< NS(math,float4) >) keys;
	};

	/** 
	 * Animation playback end behaviour.
	 */
	enum BehaviourType
	{
		/** Animation loops to start after reaching end. */
		BEHAVIOUR_REPEAT,
		/** Animation changes direction at the ends. */
		BEHAVIOUR_OSCILLATE,
		/** Animation resets to start frame at the end. */
		BEHAVIOUR_RESET,
		/** Animation stops at the last frame. */
		BEHAVIOUR_STOP,
		/** Number of supported behaviours. */
		BEHAVIOUR_COUNT
	};

	/**
	 * Creates a transform animation with space for specified animation channels.
	 * @param endbehaviour Animation playback end behaviour.
	 * @param poskeys Number of position key frames.
	 * @param rotkeys Number of rotation key frames.
	 * @param sclkeys Number of scale key frames.
	 * @param poskeyrate Sample rate of position keys.
	 * @param rotkeyrate Sample rate of rotation keys.
	 * @param sclkeyrate Sample rate of scale keys.
	 */
	TransformAnimation( BehaviourType endbehaviour,
		int poskeys, int rotkeys, int sclkeys,
		int poskeyrate, int rotkeyrate, int sclkeyrate );

	/**
	 * Creates a transform animation with specified animation channels.
	 * @param endbehaviour Animation playback end behaviour.
	 * @param poskeys Position key frames.
	 * @param rotkeys Rotation key frames.
	 * @param sclkeys Scale key frames.
	 * @param poskeyrate Sample rate of position keys.
	 * @param rotkeyrate Sample rate of rotation keys.
	 * @param sclkeyrate Sample rate of scale keys.
	 */
	TransformAnimation( BehaviourType endbehaviour,
		KeyframeSequence* poskeys, KeyframeSequence* rotkeys, KeyframeSequence* sclkeys,
		int poskeyrate, int rotkeyrate, int sclkeyrate );

	/**
	 * Creates a transform animation with specified animation channels.
	 * @param endbehaviour Animation playback end behaviour.
	 * @param posanim Position key anim.
	 * @param rotkeys Rotation key frames.
	 * @param sclanim Scale key anim.
	 * @param poskeyrate Sample rate of position keys.
	 * @param rotkeyrate Sample rate of rotation keys.
	 * @param sclkeyrate Sample rate of scale keys.
	 * @param endtime Animation length in seconds.
	 */
	TransformAnimation( BehaviourType endbehaviour,
		Float3Anim* posanim, KeyframeSequence* rotkeys, Float3Anim* sclanim,
		int poskeyrate, int rotkeyrate, int sclkeyrate,
		float endtime );

	///
	~TransformAnimation();

	/** 
	 * Evaluates transform at specified time. 
	 */
	void	eval( float time, NS(math,float3)* pos, NS(math,quaternion)* rot, NS(math,float3)* scl );

	/** 
	 * Evaluates transform at specified time. 
	 */
	void	eval( float time, NS(math,float3x4)* tm );

	/**
	 * Sets animation playback end behaviour type.
	 */
	void	setEndBehaviour( BehaviourType endbehaviour );

	/**
	 * Sets position key at specified index.
	 */
	void	setPositionKey( int i, const NS(math,float3)& v );

	/**
	 * Sets rotation key at specified index.
	 */
	void	setRotationKey( int i, const NS(math,quaternion)& v );

	/**
	 * Sets scale key at specified index.
	 */
	void	setScaleKey( int i, const NS(math,float3)& v );

	/**
	 * Removes redundant key frames from the animation
	 * and packs animation keys to 16-bit integer components.
	 * @param quality Procentage to add extra keys. Default is 5% (0.05).
	 */
	void	optimize( float quality = 0.05f );

	/**
	 * Rotates transform animation keys.
	 */
	void	rotate( const NS(math,float3x3)& rot );

	/**
	 * Scales transform animation (by altering scale and position data scaling factors).
	 */
	void	scale( float s );

	/**
	 * Removes redundant keys but doesnt do other optimization.
	 */
	void	removeRedundantKeys();

	/**
	 * Gets position key at specified index.
	 */
	NS(math,float3)		getPositionKey( int i ) const;

	/**
	 * Gets rotation key at specified index.
	 */
	NS(math,quaternion)	getRotationKey( int i ) const;

	/**
	 * Gets scaling key at specified index.
	 */
	NS(math,float3)		getScaleKey( int i ) const;

	/** 
	 * Returns number of position keys. 
	 */
	int		positionKeys() const;

	/** 
	 * Returns number of rotation keys. 
	 */
	int		rotationKeys() const;
	
	/** 
	 * Returns number of scale keys. 
	 */
	int		scaleKeys() const;

	/** 
	 * Returns sample rate of position keys. 
	 */
	int		positionKeyRate() const;

	/** 
	 * Returns sample rate of rotation keys. 
	 */
	int		rotationKeyRate() const;
	
	/** 
	 * Returns sample rate of scale keys. 
	 */
	int		scaleKeyRate() const;

	/**
	 * Returns animation playback end behaviour.
	 */
	BehaviourType	endBehaviour() const;

	/**
	 * Returns linear velocity at specified time in the animation,
	 * calculated from two consequtive position keys.
	 */
	NS(math,float3)	getLinearVelocity( float time ) const;

	/**
	 * Returns distance moved between two position keys in the animation.
	 */
	NS(math,float3)	getPositionDelta( float time0, float time1 ) const;

	/**
	 * Returns position keyframe sequence if any.
	 */
	KeyframeSequence*	positionKeyframeSequence() const	{return m_pos;}

	/**
	 * Returns rotation keyframe sequence if any.
	 */
	KeyframeSequence*	rotationKeyframeSequence() const	{return m_rot;}

	/**
	 * Returns scale keyframe sequence if any.
	 */
	KeyframeSequence*	scaleKeyframeSequence() const		{return m_scl;}

	/**
	 * Returns optimized position animation if any.
	 */ 
	Float3Anim*			positionAnimation() const			{return m_posAnim;}

	/**
	 * Returns optimized scale animation if any.
	 */ 
	Float3Anim*			scaleAnimation() const				{return m_sclAnim;}

	/**
	 * Returns true if the animation has been optimized by calling optimize().
	 */
	bool				isOptimized() const					{return !m_pos && m_posAnim;}

	/**
	 * Returns length of the animation in seconds.
	 */
	float				endTime() const						{return m_endTime;}

	/**
	 * Converts end behavior string to enum.
	 * @param str Either REPEAT, OSCILLATE, RESET or STOP.
	 * @exception GraphicsException
	 */
	static BehaviourType	toBehaviour( const NS(lang,String)& str );

private:
	float				m_endTime;
	P(KeyframeSequence)	m_pos;
	P(KeyframeSequence)	m_rot;
	P(KeyframeSequence)	m_scl;
	uint8_t				m_poskeyrate;
	uint8_t				m_rotkeyrate;
	uint8_t				m_sclkeyrate;
	uint8_t				m_endbehaviour;
	P(Float3Anim)		m_posAnim;
	P(Float3Anim)		m_sclAnim;

	TransformAnimation( const TransformAnimation& );
	TransformAnimation& operator=( const TransformAnimation& );
};


END_NAMESPACE() // hgr


#endif // _HGR_TRANSFORMANIMATION_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
