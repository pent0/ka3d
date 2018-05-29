#include <hgr/KeyframeSequence.h>
#include <lang/Math.h>
#include <lang/Float.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(math)
USING_NAMESPACE(lang)


BEGIN_NAMESPACE(hgr) 


KeyframeSequence::KeyframeSequence( int keys, VertexFormat::DataFormat format ) :
	m_data( VertexFormat::getDataSize(format,keys) ),
	m_keys( keys ),
	m_format( format ),
	m_scale( 1.f ),
	m_bias( 0, 0, 0, 0 )
{
	assert( VertexFormat::DF_NONE != format );
}

void KeyframeSequence::setKeyframe( int index, const float4& v )
{
	setKeyframe( index, &v.x, sizeof(v), VertexFormat::DF_V4_32, 1.f, float4(0,0,0,0), 1 );
}

void KeyframeSequence::setKeyframe( int index,
	const void* data, int pitch, VertexFormat::DataFormat df, 
	float scale, const float4& bias, int count )
{
	assert( index >= 0 && index < m_keys );
	assert( index+count >= 0 && index+count <= m_keys );

	int dpitch = this->pitch();

	VertexFormat::copyData( &m_data[index*dpitch], dpitch, m_format,
		data, pitch, df, float4(scale,scale,scale,scale), bias, count );
}

void KeyframeSequence::setScale( float s )
{
	m_scale = s;
}

void KeyframeSequence::setBias( const float4 bias )
{
	m_bias = bias;
}

void KeyframeSequence::setKeys( int keys )
{
	m_data.resize( keys*pitch() );
	m_keys = keys;
}

float4 KeyframeSequence::getKeyframe( int index ) const
{
	float4 v(0,0,0,0);
	getKeyframe( index, &v.x, sizeof(v), VertexFormat::DF_V4_32, 1 );
	return v;
}

void KeyframeSequence::getKeyframe( int index,
	void* data, int pitch, VertexFormat::DataFormat df, 
	int count ) const
{
	assert( index >= 0 && index < m_keys );
	assert( index+count >= 0 && index+count <= m_keys );

	int spitch = this->pitch();

	VertexFormat::copyData( 
		data, pitch, df,
		&m_data[index*spitch], spitch, m_format, 
		float4(m_scale,m_scale,m_scale,m_scale), m_bias, count );
}

void KeyframeSequence::getBound( NS(math,float4)* minv, NS(math,float4)* maxv ) const
{
	*minv = float4( Float::MAX_VALUE, Float::MAX_VALUE, Float::MAX_VALUE, Float::MAX_VALUE );
	*maxv = float4( -Float::MAX_VALUE, -Float::MAX_VALUE, -Float::MAX_VALUE, -Float::MAX_VALUE );

	for ( int i = 0 ; i < m_keys ; ++i )
	{
		float4 v = getKeyframe(i);

		for ( int k = 0 ; k < 4 ; ++k )
		{
			(*minv)[k] = Math::min( (*minv)[k], v[k] );
			(*maxv)[k] = Math::max( (*maxv)[k], v[k] );
		}
	}

	if ( minv->x > maxv->x )
		*minv = *maxv = float4(0,0,0,0);
}

int	KeyframeSequence::keys() const
{
	return m_keys;
}

VertexFormat::DataFormat KeyframeSequence::format() const
{
	return m_format;
}

float KeyframeSequence::scale() const
{
	return m_scale;
}

const float4& KeyframeSequence::bias() const
{
	return m_bias;
}

P(KeyframeSequence) KeyframeSequence::getPacked( VertexFormat::DataFormat format, float maxvalue ) const
{
	// quantisize to 16-bit
	float4 min,max;
	getBound( &min, &max );
	float4 bias = (min+max)*.5f;
	float4 dim = (max-min)*.505f; // +1% rounding margin
	float scale = dim.x > dim.y ? (dim.x > dim.z ? dim.x : dim.z) : (dim.y > dim.z ? dim.y : dim.z);
	float iscale = scale > Float::MIN_VALUE ? 1.f/scale : 0.f;
	scale *= 1.f/maxvalue;
	iscale *= maxvalue;

	int keys = this->keys();
	P(KeyframeSequence) seq = new KeyframeSequence( keys, format );
	seq->setScale( scale );
	seq->setBias( bias );

	for ( int i = 0 ; i < keys ; ++i )
	{
		float4 v = getKeyframe(i);
		seq->setKeyframe( i, (v-bias)*iscale );
	}

	return seq;
}

void KeyframeSequence::removeRedundantKeys()
{
	int keys = this->keys();
	if ( keys > 1 )
	{
		int i;
		float4 key0 = getKeyframe(0);
		for ( i = 1 ; i < keys ; ++i )
			if ( getKeyframe(i) != key0 )
				break;

		if ( i == keys )
			setKeys( 1 );
	}
}


END_NAMESPACE() // hgr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
