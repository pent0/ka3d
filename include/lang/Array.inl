void throw_OutOfMemoryException(); // in OutOfMemoryException.cpp

template <class T> void Array<T>::swap( Array<T>& other )
{
	T*	a = m_a;
	int	size = m_size;
	int	cap = m_cap;

	m_a = other.m_a;
	m_size = other.m_size;
	m_cap = other.m_cap;

	other.m_a = a;
	other.m_size = size;
	other.m_cap = cap;
}

template <class T> Array<T>& Array<T>::operator=( const Array<T>& other )
{
	resize( other.m_size );
	copy( m_a, other.m_a, m_size );
	return *this;
}

template <class T> void Array<T>::add( const T& item )
{
	T itemcopy( item );
	if ( m_size >= m_cap )
		realloc(m_size+1);
	m_a[m_size++] = itemcopy;
}

template <class T> void Array<T>::remove( int index )
{
	assert( index >= 0 && index < size() );
	copy( m_a+index, m_a+index+1, m_size-index-1 );
	m_a[--m_size] = T();
}

template <class T> void Array<T>::remove( int begin, int end )
{
	assert( begin >= 0 && begin <= end );
	assert( end >= 0 && end <= m_size );

	int count = end-begin;
	copy( m_a+begin, m_a+end, m_size-end );
	for ( int i = m_size-count ; i < m_size ; ++i )
		m_a[i] = T();
	m_size -= count;
}

template <class T> void Array<T>::add( int index, const T& item )
{
	assert( index >= 0 && index <= size() );

	T itemcopy( item );
	resize( m_size + 1 );
	rcopy( m_a+(index+1), m_a+index, m_size-(index+1) );
	m_a[index] = itemcopy;
}

template <class T> void Array<T>::resize( int size, const T& defaultvalue )
{
	T itemcopy( defaultvalue );
	if ( size > m_cap )
		realloc( size );

	int i;
	for ( i = m_size ; i < size ; ++i )
		m_a[i] = itemcopy;
	for ( i = size ; i < m_size ; ++i )
		m_a[i] = T();
	m_size = size;
}

template <class T> void Array<T>::realloc( int size )
{
	int cap = m_cap * 2;
	if ( unsigned(cap)*sizeof(T) < 32U )
		cap = (32+sizeof(T)-1)/sizeof(T);
	if ( cap < size )
		cap = size;
	
	T* data = new T[cap];
	if ( !data )
		throw_OutOfMemoryException();

	int count = m_size;
	if ( count > cap )
		count = cap;
	for ( int i = 0 ; i < count ; ++i )
	{
		data[i] = m_a[i];
		m_a[i] = T();
	}

	delete[] m_a;
	m_a = data;
	m_cap = cap;
}

template <class T> void Array<T>::copy( T* dst, const T* src, int count )
{
	for ( int i = 0 ; i < count ; ++i )
		dst[i] = src[i];
}

template <class T> void Array<T>::rcopy( T* dst, const T* src, int count )
{
	for ( int i = count-1 ; i >= 0 ; --i )
		dst[i] = src[i];
}

template <class T> int Array<T>::indexOf( const T& item ) const
{
	const T* begin = m_a;
	const T* end = begin + m_size;
	for ( const T* it = begin ; it != end ; ++it )
		if ( *it == item )
			return it - begin;
	return -1;
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
