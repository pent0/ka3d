#ifndef LANG_TEMPBUFFER_H
#define LANG_TEMPBUFFER_H


#include <lang/pp.h>


BEGIN_NAMESPACE(lang) 


/*  
 * IMPLEMENTATION HELPER CLASS. This class handles the actual memory allocation,
 * used by the TempBuffer class (to avoid code bloat when the template code gets generated).
 *
 * @see TempBuffer
 */
class ByteTempBuffer
{
public:
	enum Constants
	{
		/** Maximum number of simulatenously allocated temporary buffers (=recursion limit). */
		MAX_TEMP_BUFFERS	= 32,
	};

	/** Allocates temporary buffer of specified size in bytes. */
	explicit ByteTempBuffer( int size );

	/** Create empty temporary buffer. */
	ByteTempBuffer();

	/** Releases temporary buffer. */
	~ByteTempBuffer();

	/** Returns access to the buffer memory. */
	char*			buffer()			{return m_buffer;}

	/** Returns access to the buffer memory. */
	const char*		buffer() const	 	{return m_buffer;}

private:
	char*	m_buffer;
	int		m_size;
	int		m_slot;

	ByteTempBuffer( const ByteTempBuffer& );
	ByteTempBuffer& operator=( const ByteTempBuffer& );
};


/**
 * Temporary buffer, which is released when the object goes out of scope.
 * Can be used to e.g. avoid stack based buffers on platforms with 
 * minimal stack space. 
 *
 * Note that array objects allocated with TempBuffer are NOT constructed or destructed.
 * This means that you should only use TempBuffer objects when
 * constructor/destructor intialization of the memory is not required,
 * or then you need to call constructor/destructor manually in the code.
 * This is due to performance reasons.
 * 
 * Note that creating TempBuffer objects does not cause dynamic memory 
 * allocations, it's allocated from pre-allocated
 * statically sized array stored in NS(lang,lang_Globals).
 * If this array goes out of memory then assertion is failed.
 * (TempBuffer should not be used when needed allocation size is 
 * not known, i.e. it should be used more like a stack array replacement)
 *
 * TempBuffers are freed in stack manner, i.e. TempBuffer memory is not freed
 * until all the TempBuffers allocated after that are freed.
 * You can have maximum of 32 temporary buffers.
 */
template <class T> class TempBuffer
{
public:
	enum Constants
	{
		/** Maximum number of simulatenously allocated temporary buffers (=recursion limit). */
		MAX_TEMP_BUFFERS								= ByteTempBuffer::MAX_TEMP_BUFFERS,
	};

	/** Allocates tempoary buffer of specified size in number of items. */
	explicit TempBuffer( int size )						: m_buf( sizeof(T)*size ), m_size(size) {}

	/** Releases temporary buffer. */
	~TempBuffer()										{}

	/** Returns ith element of the buffer. */
	T&		operator[]( int i )							{return reinterpret_cast<T*>( m_buf.buffer() )[i];}

	/** Returns access to the buffer memory. */
	T*		buffer()									{return reinterpret_cast<T*>( m_buf.buffer() );}

	/** Returns the beginning of the buffer memory. */
	T*		begin()										{return reinterpret_cast<T*>( m_buf.buffer() );}

	/** Returns the end of the buffer memory. */
	T*		end()										{return reinterpret_cast<T*>( m_buf.buffer() )+m_size;}

	/** Returns size (number of items) of the buffer. */
	int		size() const								{return m_size;}

	/** Returns the beginning of the buffer memory. */
	const T*	begin() const							{return reinterpret_cast<const T*>( m_buf.buffer() );}

	/** Returns the end of the buffer memory. */
	const T*	end() const								{return reinterpret_cast<const T*>( m_buf.buffer() )+m_size;}

	/** Returns ith element of the buffer. */
	const T&	operator[]( int i ) const 				{return reinterpret_cast<T*>( m_buf.buffer() )[i];}

private:
	ByteTempBuffer	m_buf;
	int				m_size;

	TempBuffer( const TempBuffer& );
	TempBuffer& operator=( const TempBuffer& );
};


END_NAMESPACE() // lang


#endif // LANG_TEMPBUFFER_H
