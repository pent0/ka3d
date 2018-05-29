#ifndef _MATH_INTERPOLATIONUTIL_H
#define _MATH_INTERPOLATIONUTIL_H


#include <lang/pp.h>


BEGIN_NAMESPACE(math) 


/**
 * Utilities for interpolating between key values.
 * @ingroup math
 */
class InterpolationUtil
{
public:
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
	};

	/**
	 * Returns animation frame numbers which takes animation end behaviour into account.
	 * @param time Time in seconds.
	 * @param endbehaviour Animation playback end behaviour.
	 * @param keyframes Number of key frames in animation.
	 * @param keyrate Key frame sample rate.
	 * @param frames [out] Four (4) consecutive key frame numbers. Current one is 1. 0 is the previous key and 2 is the next key and 3 is the key after next.
	 * @param phase [out] Fraction [0,1] to interpolate between current frame and next frame.
	 */
	static void	getFrame( float time, BehaviourType endbehaviour, 
					int keyframes, float keyrate, int* frames, float* phase );

	/**
	 * Interpolates vector of values using Catmull-Rom interpolation.
	 * Keys are asssumed to have uniform time spacing.
	 * @param dim Dimensions in each vector.
	 * @param u Phase of interpolation [0,1].
	 * @param key0 Previous key.
	 * @param key1 Current key.
	 * @param key2 Next key.
	 * @param key3 Key after next key.
	 * @param result [out] Interpolated value.
	 */
	static void	interpolateVectorCatmullRom( int dim, float u,
		const float* key0, const float* key1, const float* key2, const float* key3,
		float* result );

	/**
	 * Returns string representing animation start/end behaviour type.
	 */
	static const char*	toString( BehaviourType behaviour );
};


END_NAMESPACE() // math


#endif // _MATH_INTERPOLATIONUTIL_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
