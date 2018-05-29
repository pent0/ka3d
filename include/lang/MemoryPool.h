#ifndef _LANG_MEMORYPOOL_H
#define _LANG_MEMORYPOOL_H


#include <lang/pp.h>
#include <lang/Array.h>
#include <lang/Object.h>
#include <lang/assert.h>


BEGIN_NAMESPACE(lang) 


/**
 * Handle based memory pool with compact support.
 * Note: Not thread safe.
 * @ingroup lang
 */
class MemoryPool :
	public NS(lang,Object)
{
public:
	/** 
	 * Creates pool with specified initial size and maximum size. 
	 * @param size Initial size.
	 * @param maxsize Maximum pool size. If 0 then the pool can grow without limits.
	 * @param name Name of the pool. Used for debugging.
	 */
	MemoryPool( int size, int maxsize, const char* name );

	///
	~MemoryPool();

	/** 
	 * Allocates a memory block from the pool. 
	 * Returned handle has reference count of 1.
	 * @param bytes Number of bytes to allocate.
	 * @return Handle to the block.
	 * @exception OutOfMemoryException
	 */
	int				allocate( int bytes );

	/** 
	 * Adds reference to memory block handle. 
	 */
	void			ref( int handle )				{(reinterpret_cast<BlockHeader*>(m_blocks[handle])-1)->refs += 1;}

	/** 
	 * Releases reference to memory block handle. 
	 */
	void			unref( int handle )				{if ( ((reinterpret_cast<BlockHeader*>(m_blocks[handle])-1)->refs -= 1) == 0 ) unalloc(handle);}

	/** 
	 * Returns memory associated with handle. 
	 * Does not alter memory block handle reference count.
	 */
	char*			get( int handle ) 				{return m_blocks[handle];}
	
	/** 
	 * Compacts used memory pool.
	 */
	void			compact();

	/** 
	 * Returns number of allocated blocks. 
	 */
	int				blocksAllocated() const;

	/** 
	 * Returns number of allocated bytes (including gaps). 
	 */
	int				bytesAllocated() const			{return m_next;}

	/** 
	 * Returns size of the memory pool. 
	 */
	int				size() const					{return m_mem.size();}

	/** 
	 * Dump all contents of memory pool to debug output. 
	 * Warning: very large overhead. 
	 */
	void			dump() const;

private:
	friend class Handle;

	struct BlockHeader
	{
		int refs;
		int size;
	};

	class HandleSorter
	{
	public:
		HandleSorter( MemoryPool* mp )				: m_mp(mp) {}
		bool operator()( int a, int b ) const		{return m_mp->get(a) < m_mp->get(b);}
			
	private:
		MemoryPool* m_mp;
	};
	
	Array<char>		m_mem;
	Array<char*>	m_blocks;
	Array<int>		m_freed;
	int				m_maxSize;
	int				m_next;
	Array<int>		m_gcbuf;
	char			m_name[32]; // DEBUG
	int				m_maxgctime; // DEBUG

	void			unalloc( int handle );
	void 			resize( int newsize );
	BlockHeader*	getBlockHeader( int handle )	{return reinterpret_cast<BlockHeader*>(m_blocks[handle])-1;}
	
	MemoryPool( const MemoryPool& );
	MemoryPool& operator=( const MemoryPool& );
};


END_NAMESPACE() // lang


#endif // _LANG_MEMORYPOOL_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
