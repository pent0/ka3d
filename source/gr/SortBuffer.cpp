#include <gr/impl/SortBuffer.h>
#include <config.h>


BEGIN_NAMESPACE(gr) 


SortBuffer::SortBuffer()
{
}

SortBuffer::~SortBuffer()
{
	if ( m_buf.size() > 0 )
	{
		assert( m_buf.size() > 4 );

		#ifdef _DEBUG
		uint8_t* tag = m_buf.end()-4;
		assert( tag[0] == 'o' );
		assert( tag[1] == 'k' );
		assert( tag[2] == '!' );
		assert( tag[3] == '\0' );
		#endif
	}
}

void SortBuffer::reset( int intbuffersize, int floatbuffersize )
{
	int floatbytes = sizeof(float)*floatbuffersize;
	int intbytes = 2*intbuffersize;
	m_buf.resize( floatbytes + intbytes + 4 );
	m_floatBuffer = reinterpret_cast<float*>( m_buf.begin() );
	m_intBuffer = reinterpret_cast<uint16_t*>( m_buf.begin() + floatbytes );
	
	uint8_t* tag = m_buf.begin() + floatbytes + intbytes;
	tag[0] = 'o';
	tag[1] = 'k';
	tag[2] = '!';
	tag[3] = '\0';
}


END_NAMESPACE() // gr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
