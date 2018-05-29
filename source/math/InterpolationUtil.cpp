#include <math/InterpolationUtil.h>
#include <xmath.h>
#include <config.h>
#include <lang/assert.h>

BEGIN_NAMESPACE(math) 


void InterpolationUtil::getFrame( float time, BehaviourType endbehaviour,
	int keyframes, float keyrate, int* frames, float* phase )
{
	assert( time >= 0.f );
	assert( keyframes > 0 );
	assert( keyrate >= 0.f );

	float u = -1.f;
	if ( keyframes <= 1 )
	{
		frames[3] = frames[2] = frames[1] = frames[0] = 0;
		u = 0.f;
	}
	else
	{
		const float frame = time * keyrate;

		switch ( endbehaviour )
		{
		case BEHAVIOUR_REPEAT:{
			float framewrapped = fmodf(frame,(float)keyframes);
			u = framewrapped - floorf(framewrapped);
			int framei = (int)framewrapped;
			frames[0] = (framei-1 + keyframes) % keyframes;
			frames[1] = framei;
			frames[2] = (framei+1) % keyframes;
			frames[3] = (framei+2) % keyframes;
			break;}

		case BEHAVIOUR_OSCILLATE:{
			int keyframes2 = keyframes + keyframes;
			float framewrapped = fmodf(frame,(float)keyframes2);
			u = framewrapped - floorf(framewrapped);
			int framei = (int)frame - 1 + keyframes2;
			for ( int i = 0 ; i < 4 ; ++i )
			{
				int f = (framei+i) % keyframes2;
				if ( f >= keyframes )
					frames[i] = keyframes2-1 - f;
				else
					frames[i] = f;
			}
			break;}

		case BEHAVIOUR_RESET:{
			u = frame - floorf(frame);
			int framei = (int)frame;
			if ( framei >= keyframes )
				u = 0.f;

			--framei;
			for ( int i = 0 ; i < 4 ; ++i )
			{
				if ( framei < 0 )
					frames[i] = 0;
				else if ( framei >= keyframes )
					frames[i] = 0;
				else
					frames[i] = framei;
				++framei;
			}
			break;}

		case BEHAVIOUR_STOP:{
			u = frame - floorf(frame);
			int framei = (int)frame;
			if ( framei >= keyframes )
				u = 1.f;

			--framei;
			for ( int i = 0 ; i < 4 ; ++i )
			{
				if ( framei < 0 )
					frames[i] = 0;
				else if ( framei >= keyframes )
					frames[i] = keyframes-1;
				else
					frames[i] = framei;
				++framei;
			}
			break;}
		}
	}

	assert( u >= 0.f && u <= 1.f );
	assert( frames[0] >= 0 && frames[0] < keyframes );
	assert( frames[1] >= 0 && frames[1] < keyframes );
	assert( frames[2] >= 0 && frames[2] < keyframes );

	if ( 0 != phase )
		*phase = u;
}

const char* InterpolationUtil::toString( BehaviourType behaviour )
{
	switch ( behaviour )
	{
	case BEHAVIOUR_REPEAT:		return "REPEAT";
	case BEHAVIOUR_OSCILLATE:	return "OSCILLATE";
	case BEHAVIOUR_RESET:		return "RESET";
	case BEHAVIOUR_STOP:		return "STOP";
	default:					return "INVALID";
	}
}

void InterpolationUtil::interpolateVectorCatmullRom( int dim, float u,
	const float* key0, const float* key1, const float* key2, const float* key3,
	float* result )
{
	assert( dim <= 16 );

	float out[16];
	float in[16];

	// outgoing tangent
	for ( int i = 0 ; i < dim ; ++i )
		out[i] = .5f * (key2[i] - key0[i]);

	// incoming tangent
	for ( int i = 0 ; i < dim ; ++i )
		in[i] = .5f * (key3[i] - key1[i]);
	
	// Hermite interpolation
	float u2 = u * u;
	float u3 = u * u2;
	float tmp = 2.f * u3 - 3.f * u2;
	float h1 = tmp + 1.f;
	float h2 = -tmp;
	float h3 = u3 - 2.f * u2 + u;
	float h4 = u3 - u2;
	for ( int i = 0 ; i < dim ; ++i )
		result[i] = h1 * key1[i] + h2 * key2[i] + h3 * out[i] + h4 * in[i];
}


END_NAMESPACE() // math

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
