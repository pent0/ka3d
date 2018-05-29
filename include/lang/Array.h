#ifndef _LANG_ARRAY_H
#define _LANG_ARRAY_H


#include <lang/pp.h>
#include <lang/assert.h>


BEGIN_NAMESPACE(lang) 


/** 
 * Flexible array. Arrays are meant to be used
 * as part of higher abstraction level objects so 
 * they don't have functionality for reference counting.
 *
 * Note that Array has important semantic difference to for
 * example NS(std,vector): Array does NOT call destructors when
 * array is resized to smaller one, but it uses assign operator
 * with default (empty) array element. This behaviour
 * might seem weird, but it is more often useful than not, since
 * for example you don't want memory to be released too easily 
 * if you have for example Array of Arrays. And you shouldn't
 * use too big objects in flexible arrays anyway so in practice
 * this shouldn't harm you in any case.
 *
 * @param T Type of array element. Copy and default ctor should be nothrow.
 * 
 * @ingroup lang
 */
template <class T> class Array
{
public:
	/** 
	 * Creates an empty array. 
	 */
	Array() :
		m_a(0), m_size(0), m_cap(0)
	{
	}

	/** 
	 * Creates an array of specified size. 
	 * @exception OutOfMemoryException
	 */
	explicit Array( int size, const T& defaultvalue=T() ) :
		m_a(0), m_size(0), m_cap(0)
	{
		resize( size, defaultvalue );
	}

	/** 
	 * Copy by value. 
	 */
	Array( const Array<T>& other ) :
		m_a(0), m_size(0), m_cap(0)
	{
		*this = other;
	}

	///
	~Array()
	{
		delete[] m_a;
	}

	/**
	 * Swaps two arrays.
	 */
	void swap( Array<T>& other );

	/** 
	 * Copy by value. 
	 * @exception OutOfMemoryException
	 */
	Array<T>& operator=( const Array<T>& other );

	/** 
	 * Returns ith element from the array. 
	 */
	inline T& operator[]( int index )
	{
		assert( index >= 0 && index < m_size );
		return m_a[index];
	}

	/** 
	 * Adds an element to the end of the array. 
	 * @exception OutOfMemoryException
	 */
	void add( const T& item );

	/** 
	 * Adds an element before specified index. 
	 * @exception OutOfMemoryException
	 */
	void add( int index, const T& item );

	/**
	 * Removes an element at specified index.
	 */
	void remove( int index );

	/**
	 * Removes range of elements at specified index.
	 * @param begin First element to remove.
	 * @param end One beyond the last element to remove.
	 */
	void remove( int begin, int end );

	/** 
	 * Sets number of elements in the array. 
	 * @exception OutOfMemoryException
	 */
	void resize( int size, const T& defaultvalue=T() );

	/** 
	 * Sets number of elements in the array to 0. 
	 */
	void clear()
	{
		resize( 0 );
	}

	/** 
	 * Returns pointer to the beginning of the array (inclusive). 
	 */
	inline T* begin()
	{
		return m_a;
	}

	/** 
	 * Returns pointer to the end of the array (exclusive). 
	 */
	inline T* end()
	{
		return m_a+m_size;
	}

	/** 
	 * Returns first element in the array (inclusive).
	 */
	inline T& first()
	{
		assert( m_size > 0 );
		return *m_a;
	}

	/** 
	 * Returns last element in the array (inclusive).
	 */
	inline T& last()
	{
		assert( m_size > 0 );
		return m_a[m_size-1];
	}
	
	/** 
	 * Returns ith element from the array. 
	 */
	inline const T& operator[]( int index ) const
	{
		assert( index >= 0 && index < m_size );
		return m_a[index];
	}

	/** 
	 * Returns number of elements in the array. 
	 */
	inline int size() const
	{
		return m_size;
	}

	/** 
	 * Returns pointer to the beginning of the array (inclusive).
	 */
	inline const T* begin() const
	{
		return m_a;
	}

	/** 
	 * Returns pointer to the end of the array (exclusive).
	 */
	inline const T* end() const
	{
		return m_a+m_size;
	}

	/** 
	 * Returns first element in the array (inclusive).
	 */
	inline const T& first() const
	{
		assert( m_size > 0 );
		return *m_a;
	}

	/** 
	 * Returns last element in the array (inclusive).
	 */
	inline const T& last() const
	{
		assert( m_size > 0 );
		return m_a[m_size-1];
	}

	/**
	 * Returns index of the item in the array or -1 if not found.
	 */
	int indexOf( const T& item ) const;

	/** 
	 * Returns number of elements allocated in the array.
	 * Note that this does not need to be same as size(), since
	 * if the Array is resized down, memory is by default not freed (to avoid fragmenting memory).
	 */
	inline int allocatedCapacity() const
	{
		return m_cap;
	}

private:
	T*		m_a;
	int		m_size;
	int		m_cap;

	/**
	 * Allocates buffer for n elements. 
	 * @exception OutOfMemoryException
	 */
	void realloc( int size );

	/** 
	 * Copy in normal order.
	 */
	void copy( T* dst, const T* src, int count );

	/** 
	 * Copy in reverse order.
	 */
	void rcopy( T* dst, const T* src, int count );
};


#include <lang/Array.inl>


END_NAMESPACE() // lang


#endif // _LANG_ARRAY_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
