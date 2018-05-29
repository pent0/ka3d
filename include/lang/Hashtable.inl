int Hashtable_getLargerInt( int n );


template <class K, class T> HashtableIterator<K,T>::HashtableIterator( HashtablePair<K,T>* data, int cap, HashtablePair<K,T>* item, int index )
{
	assert( item == 0 && index == -1 || index >= 0 && index <= cap ); 

	m_data = data;
	m_last = cap-1; 
	m_item = item;
	m_index = index;

	if ( 0 == item )
		this->operator++();
}

template <class K, class T> HashtableIterator<K,T>&	HashtableIterator<K,T>::operator++()
{
	do 
	{
		if ( m_item )
			m_item = m_item->next;
		if ( !m_item && m_index < m_last )
			m_item = &m_data[ ++m_index ];
	} while ( m_item && !m_item->used );
	return *this;
}

template <class K, class T, class F> HashtableIterator<K,T> Hashtable<K,T,F>::getIterator( const K& key ) const
{
	if ( m_cap > 0 )
	{
		HashtablePair<K,T>* pair = getPair( m_data, m_cap, key );
		if ( pair->used )
		{
			int slot = (m_hashFunc(key) & 0x7FFFFFFF) % m_cap;
			return HashtableIterator<K,T>( m_data, m_cap, pair, slot );
		}
	}
	return end();
}

template <class K, class T, class F> Hashtable<K,T,F>::Hashtable()
{
	defaults();
}

template <class K, class T, class F> Hashtable<K,T,F>::Hashtable( 
	int initialcapacity, float loadfactor, 
	const T& defaultvalue, const F& hashfunc )
{
	assert( loadfactor >= 0.01f && loadfactor <= 0.99f );
	assert( initialcapacity >= 0 );

	defaults();

	if ( initialcapacity > 0 )
	{
		m_cap = Hashtable_getLargerInt( initialcapacity );
		m_data = allocateTable( m_cap );
	}

	m_loadFactor = loadfactor;
	m_entries = 0;
	m_entryLimit = (int)(m_cap * m_loadFactor);
	m_defaultValue = defaultvalue;
	m_hashFunc = hashfunc;
}

template <class K, class T, class F> Hashtable<K,T,F>::Hashtable( const Hashtable<K,T,F>& other )
{
	defaults();
	*this = other;
}

template <class K, class T, class F> Hashtable<K,T,F>::~Hashtable()
{
	destroy();
}

template <class K, class T, class F> Hashtable<K,T,F>& Hashtable<K,T,F>::operator=( const Hashtable<K,T,F>& other )
{
	if ( this != &other )
	{
		destroy();
		if ( other.m_entries > 0 )
		{
			int cap = other.m_cap;
			HashtablePair<K,T>* data = allocateTable( cap );
			other.rehashTo( data, cap );
			m_cap = cap;
			m_data = data;
			m_loadFactor = other.m_loadFactor;
			m_entries = other.m_entries;
			m_entryLimit = other.m_entryLimit;
			m_defaultValue = other.m_defaultValue;
		}
	}
	return *this;
}

template <class K, class T, class F> void Hashtable<K,T,F>::put( const K& key, const T& value )
{
	if ( m_entries+1 >= m_entryLimit )
		grow();
	HashtablePair<K,T>* pair = getPair( m_data, m_cap, key );
	pair->value = value;
	if ( !pair->used )
	{
		pair->used = true;
		++m_entries;
	}
}

template <class K, class T, class F> void Hashtable<K,T,F>::remove( const K& key )
{
	int slot = (m_hashFunc(key) & 0x7FFFFFFF) % m_cap;
	HashtablePair<K,T>* first = &m_data[slot];

	HashtablePair<K,T>* prevPair = 0;
	HashtablePair<K,T>* nextPair = 0;
	for ( HashtablePair<K,T>* pair = first ; pair ; pair = nextPair )
	{
		nextPair = pair->next;

		if ( pair->used && pair->key == key )
		{
			pair->used = false;
			pair->value = T();
			pair->key = K();
			--m_entries;

			if ( pair != first )
			{
				assert( prevPair );
				prevPair->next = pair->next;
				delete pair;
			}
		}

		prevPair = pair;
	}
}

template <class K, class T, class F> void Hashtable<K,T,F>::clear()
{
	for ( int i = 0 ; i < m_cap ; ++i )
	{
		HashtablePair<K,T>* nextPair = 0;
		for ( HashtablePair<K,T>* pair = &m_data[i] ; pair ; pair = nextPair )
		{
			nextPair = pair->next;
			if ( pair->used )
			{
				pair->used = false;
				pair->key = K();
				pair->value = T();
				--m_entries;
			}
		}
	}

	assert( 0 == m_entries );
}

template <class K, class T, class F> T& Hashtable<K,T,F>::operator[]( const K& key )
{
	if ( m_entries+1 >= m_entryLimit )
		grow();
	HashtablePair<K,T>* pair = getPair( m_data, m_cap, key );
	if ( !pair->used )
	{
		pair->used = true;
		++m_entries;
	}
	return pair->value;
}

template <class K, class T, class F> const T& Hashtable<K,T,F>::get( const K& key ) const
{
	if ( m_cap > 0 )
	{
		HashtablePair<K,T>* pair = getPair( m_data, m_cap, key );
		if ( pair->used )
			return pair->value;
	}
	return m_defaultValue;
}

template <class K, class T, class F> T& Hashtable<K,T,F>::get( const K& key )
{
	if ( m_cap > 0 )
	{
		HashtablePair<K,T>* pair = getPair( m_data, m_cap, key );
		if ( pair->used )
			return pair->value;
	}
	return m_defaultValue;
}

template <class K, class T, class F> bool Hashtable<K,T,F>::containsKey( const K& key ) const
{
	if ( m_entries > 0 )
	{
		int slot = (m_hashFunc(key) & 0x7FFFFFFF) % m_cap;
		HashtablePair<K,T>* pair = &m_data[slot];
		for ( ; pair ; pair = pair->next )
			if ( pair->used && pair->key == key )
				return true;
	}
	return false;
}

template <class K, class T, class F> bool Hashtable<K,T,F>::isEmpty() const
{
	return 0 == m_entries;
}

template <class K, class T, class F> int Hashtable<K,T,F>::size() const
{
	return m_entries;
}

template <class K, class T, class F> const T& Hashtable<K,T,F>::operator[]( const K& key ) const
{
	if ( m_entries+1 >= m_entryLimit )
		const_cast< Hashtable<K,T,F>* >(this)->grow();
	HashtablePair<K,T>* pair = getPair( m_data, m_cap, key );
	if ( !pair->used )
	{
		pair->used = true;
		++m_entries;
	}
	return pair->value;
}

template <class K, class T, class F> void Hashtable<K,T,F>::grow()
{
	int cap = Hashtable_getLargerInt( m_cap );
	HashtablePair<K,T>* data = allocateTable( cap );
	m_collisions = 0;
	
	for ( int i = 0 ; i < m_cap ; ++i )
	{
		HashtablePair<K,T>* nextPair = 0;
		for ( HashtablePair<K,T>* pair = &m_data[i] ; pair ; pair = nextPair )
		{
			nextPair = pair->next;
			if ( pair->used )
			{
				HashtablePair<K,T>* newPair = getPair( data, cap, pair->key );
				newPair->value = pair->value;
				newPair->used = true;
			}
			pair->next = 0;
			if ( m_data+i != pair )
				delete pair;
		}
	}

	deallocateTable( m_data, m_cap );
	m_cap = cap;
	m_data = data;
	m_entryLimit = (int)( m_cap * m_loadFactor );
}

template <class K, class T, class F> void Hashtable<K,T,F>::rehashTo( HashtablePair<K,T>* data, int cap ) const
{
	for ( int i = 0 ; i < m_cap ; ++i )
	{
		HashtablePair<K,T>* nextPair = 0;
		for ( HashtablePair<K,T>* pair = &m_data[i] ; pair ; pair = nextPair )
		{
			nextPair = pair->next;
			if ( pair->used )
			{
				HashtablePair<K,T>* newPair = getPair( data, cap, pair->key );
				newPair->value = pair->value;
				newPair->used = true;
			}
		}
	}
}

template <class K, class T, class F> void Hashtable<K,T,F>::destroy()
{
	if ( m_data )
	{
		deallocateTable( m_data, m_cap );
		defaults();
	}
}

template <class K, class T, class F> void Hashtable<K,T,F>::defaults()
{
	m_cap			= 0;
	m_data			= 0;
	m_loadFactor	= 0.75f;
	m_entries		= 0;
	m_entryLimit	= 0;
	m_defaultValue	= T();
	m_collisions	= 0;
	m_hashFunc		= F();
}

template <class K, class T, class F> HashtablePair<K,T>* Hashtable<K,T,F>::getPair( HashtablePair<K,T>* data, int cap, const K& key ) const
{
	int slot = (m_hashFunc(key) & 0x7FFFFFFF) % cap;
	HashtablePair<K,T>* first = &data[slot];
	HashtablePair<K,T>* unused = 0;
	
	for ( HashtablePair<K,T>* pair = first ; pair ; pair = pair->next )
	{
		if ( !pair->used )
			unused = pair;
		else if ( pair->key == key )
			return pair;
	}

	if ( !unused )
	{
		++m_collisions;
		unused = new HashtablePair<K,T>();
		unused->used = false;
		unused->key = K();
		unused->value = T();
		unused->next = first->next;
		first->next = unused;
	}
	unused->key = key;
	unused->value = m_defaultValue;
	return unused;
}

template <class K, class T, class F> HashtablePair<K,T>* Hashtable<K,T,F>::allocateTable( int cap ) const
{
	return new HashtablePair<K,T>[cap];
}

template <class K, class T, class F> void Hashtable<K,T,F>::deallocateTable( HashtablePair<K,T>* data, int cap ) const
{
	for ( int i = 0 ; i < cap ; ++i )
	{
		HashtablePair<K,T>* nextPair = 0;
		for ( HashtablePair<K,T>* pair = data[i].next ; pair ; pair = nextPair )
		{
			nextPair = pair->next;
			delete pair;
		}
	}
	delete[] data;
}

template <class K, class T, class F> int Hashtable<K,T,F>::collisions() const
{
	return m_collisions;
}

template <class K, class T, class F> void Hashtable<K,T,F>::merge( const Hashtable<K,T,F>& other )
{
	assert( &other != this );

	for ( HashtableIterator<K,T> it = other.begin() ; it != other.end() ; ++it )
		put( it.key(), it.value() );
}

template <class K, class T, class F> HashtableIterator<K,T> Hashtable<K,T,F>::begin() const
{
	return HashtableIterator<K,T>( m_data, m_cap, 0, -1 );
}

template <class K, class T, class F> HashtableIterator<K,T> Hashtable<K,T,F>::end() const
{
	return HashtableIterator<K,T>();
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
