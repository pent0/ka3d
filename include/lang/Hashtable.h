#ifndef _LANG_HASHTABLE_H
#define _LANG_HASHTABLE_H


#include <lang/pp.h>
#include <lang/Object.h>
#include <lang/assert.h>


BEGIN_NAMESPACE(lang) 

	
/** 
 * Default hash functor used by Hashtable.
 * Uses 'int NS(K,hashCode)()' for hashing
 * if the key is not basic arithmetic type.
 * 
 * @ingroup lang
 */
template <class K> class Hash
{
public:
	int operator()( const K& x ) const
	{
		return x.hashCode();
	}
};

template <> class Hash<char> 
	{public: int operator()( const char& x ) const {return x;}};
template <> class Hash<short> 
	{public: int operator()( const short& x ) const {return x;}};
template <> class Hash<int> 
	{public: int operator()( const int& x ) const {return x;}};
template <> class Hash<float> 
	{public: int operator()( const float& x ) const {return *(int*)&x;}};
template <> class Hash<long> 
	{public: int operator()( const long& x ) const {return (int)x;}};


/*
 * Data structure used by the Hashtable implementation. 
 */
template <class K, class T> class HashtablePair
{
public:
	K					key;
	T					value;
	HashtablePair<K,T>*	next;
	bool				used;

	HashtablePair()
	{	
		defaults();
	}

	void defaults()
	{
		next = 0;
		used = false;
		key = K();
		value = T();
	}
};


/** 
 * Forward iterator of Hashtable elements. 
 * @ingroup lang
 */
template <class K, class T> class HashtableIterator
{
public:
	HashtableIterator()																{m_data=0; m_last=0; m_index=0; m_item=0;}
	HashtableIterator( HashtablePair<K,T>* data, int cap, HashtablePair<K,T>* item, int index );

	/** Sets the iterator to point to the next element. */
	HashtableIterator<K,T>&	operator++();

	/** Returns true if the iterators point to the same item. */
	bool			operator==( const HashtableIterator<K,T>& other ) const			{return m_item == other.m_item;}

	/** Returns true if the iterators point to different items. */
	bool			operator!=( const HashtableIterator<K,T>& other ) const			{return m_item != other.m_item;}

	/** Return item key. */
	const K&		key() const														{assert( m_item ); return m_item->key;}

	/** Return item value. */
	T&				value() const													{assert( m_item ); return m_item->value;}

private:
	HashtablePair<K,T>*		m_data;
	int						m_last;
	int						m_index;
	HashtablePair<K,T>*		m_item;
};

	
/** 
 * Hashtable maps keys to values. 
 * Default hash function uses key hashCode() for hashing.
 * Default compare function uses key operator==.
 * @param K Key type.
 * @param T Data type.
 * @param F Key hash function type.
 * @ingroup lang
 */
template < class K, class T, class F=Hash<K> > class Hashtable :
	public NS(lang,Object)
{
public:
	/** Constructs an empty hash table with load factor of 75/100. */
	Hashtable();

	/** 
	 * Constructs an empty hash table with specified 
	 * initial capacity, load factor, default value and hash function.
	 */
	explicit Hashtable( int initialcapacity, float loadfactor=0.75f, 
		const T& defaultvalue=T(), const F& hashfunc=F() );

	/** Copy by value. */
	Hashtable( const Hashtable<K,T,F>& other );

	///
	~Hashtable();

	/** Copy by value. */
	Hashtable<K,T,F>& operator=( const Hashtable<K,T,F>& other );

	/** Returns the value of specified key. Puts the key to the map if not exist. */
	T&			operator[]( const K& key );

	/** 
	 * Puts the value at specified key to the container. 
	 * If there is already old value with the same key it is overwritten.
	 */
	void		put( const K& key, const T& value );

	/** 
	 * Removes value at specified key. 
	 * Does nothing if the key is not in the table.
	 */
	void		remove( const K& key );

	/** Removes all keys from the container. */
	void		clear();

	/**
	 * Merges two hash tables.
	 * Note that some keys might be overriden if the keys are same
	 * in both sets.
	 */
	void		merge( const Hashtable<K,T,F>& other );

	/** 
	 * Returns the value of specified key. 
	 * Returns default value if the key is not in the table.
	 */
	T&			get( const K& key );

	/** Returns the value of specified key. Puts the key to the map if not exist. */
	const T&	operator[]( const K& key ) const;

	/** 
	 * Returns the value of specified key. 
	 * Returns default value if the key is not in the table.
	 */
	const T&	get( const K& key ) const;

	/** 
	 * Returns iterator to specified key. 
	 * Returns end() the key not in table.
	 */
	HashtableIterator<K,T>	getIterator( const K& key ) const;

	/** Returns number of distinct keys. */
	int			size() const;

	/** Returns true if there is no elements in the table. */
	bool		isEmpty() const;

	/** Returns true if the hash table contains specific key. */
	bool		containsKey( const K& key ) const;

	/** 
	 * Returns number of collisions occured in the hash table.
	 * Can be used for debugging hash functions.
	 */
	int			collisions() const;

	/** 
	 * Returns iterator to the first element. 
	 * Behaviour is undefined if the table is changed during the traversal.
	 */
	HashtableIterator<K,T>	begin() const;

	/** 
	 * Returns iterator to one beyond the last element. 
	 * Behaviour is undefined if the table is changed during the traversal.
	 */
	HashtableIterator<K,T>	end() const;

private:
	int						m_cap;
	HashtablePair<K,T>*		m_data;
	float					m_loadFactor;
	mutable int				m_entries;
	int						m_entryLimit;
	T						m_defaultValue;
	mutable int				m_collisions;
	F						m_hashFunc;

	void				defaults();
	void				destroy();
	void				grow();
	void				rehashTo( HashtablePair<K,T>* data, int cap ) const;
	HashtablePair<K,T>*	allocateTable( int cap ) const;
	void				deallocateTable( HashtablePair<K,T>* data, int cap ) const;
	HashtablePair<K,T>* getPair( HashtablePair<K,T>* data, int cap, const K& key ) const;
};


#include <lang/Hashtable.inl>


END_NAMESPACE() // lang


#endif // _LANG_HASHTABLE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
