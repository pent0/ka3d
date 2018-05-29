#ifndef _GR_SORTBUFFER_H
#define _GR_SORTBUFFER_H


#include <lang/Array.h>
#include <stdint.h>


BEGIN_NAMESPACE(gr) 


class SortGreater
{
public:
	explicit SortGreater( const float* z ) : m_z(z) {}

	inline bool operator()( int a, int b ) const
	{
		return m_z[a] > m_z[b];
	}

private:
	const float* m_z;
};

class SortLess
{
public:
	explicit SortLess( const float* z ) : m_z(z) {}

	inline bool operator()( int a, int b ) const
	{
		return m_z[a] < m_z[b];
	}

private:
	const float* m_z;
};

/*
 * Temporary buffer used by the gr platform dependent implementations
 * to sort polygons.
 */
class SortBuffer
{
public:
	SortBuffer();
	~SortBuffer();

	void reset( int intbuffersize, int floatbuffersize );

	uint16_t*	intBuffer()			{return m_intBuffer;}
	float*		floatBuffer()		{return m_floatBuffer;}

private:
	NS(lang,Array)<uint8_t>	m_buf;
	uint16_t*				m_intBuffer;
	float*					m_floatBuffer;

	SortBuffer( const SortBuffer& );
	SortBuffer& operator=( const SortBuffer& );
};


END_NAMESPACE() // gr


#endif // _GR_SORTBUFFER_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
