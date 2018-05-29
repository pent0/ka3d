#include <lang/TempBuffer.h>
#include <lang/Globals.h>
#include <config.h>


BEGIN_NAMESPACE(lang) 


ByteTempBuffer::ByteTempBuffer() :
	m_buffer( 0 ),
	m_size( 0 ),
	m_slot( 0 )
{
}

ByteTempBuffer::ByteTempBuffer( int size )
{
	lang_Globals& glob = lang_Globals::get();

	assert( glob.tempBufferCount < ByteTempBuffer::MAX_TEMP_BUFFERS );

	size = 	( (size + 15)&~15 ); // align size always to 16 bytes
	m_buffer = glob.tempBufferMem + glob.tempBufferMemUsed;
	glob.tempBufferMemUsed += size;

	m_size = size;
	m_slot = glob.tempBufferCount++;

	glob.tempBuffers[m_slot].m_buffer = m_buffer;
	glob.tempBuffers[m_slot].m_size = m_size;

	assert( glob.tempBufferMemUsed <= glob.tempBufferMemTotal );
}

ByteTempBuffer::~ByteTempBuffer()
{
	if ( 0 != m_buffer )
	{
		lang_Globals& glob = lang_Globals::get();

		assert( glob.tempBufferCount > 0 );

		glob.tempBuffers[m_slot].m_buffer = 0;
		glob.tempBuffers[m_slot].m_size = m_size;

		if ( glob.tempBufferCount-1 == m_slot )
		{
			// top one
			int& count = glob.tempBufferCount;
			while ( count > 0 && 0 == glob.tempBuffers[count-1].m_buffer )
			{
				glob.tempBufferMemUsed -= glob.tempBuffers[count-1].m_size;
				assert( glob.tempBufferMemUsed >= 0 ); // memory corrupted?
				--count;
			}
		}
	}
}


END_NAMESPACE() // lang
