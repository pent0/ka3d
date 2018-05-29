#ifndef _LANG_LINKEDLIST_H
#define _LANG_LINKEDLIST_H


#include <lang/pp.h>


BEGIN_NAMESPACE(lang) 


class LinkedListItemBase
{
public:
	/**
	 * Returns true if the item is linked to some list.
	 */
	bool	linked() const		{return 0 != m_next || 0 != m_previous;}

protected:
	LinkedListItemBase() : m_next(0), m_previous(0) {}
	virtual ~LinkedListItemBase();

	friend class LinkedListBase;
	LinkedListItemBase* m_next; 
	LinkedListItemBase* m_previous;

private:
	LinkedListItemBase( const LinkedListItemBase& );
	LinkedListItemBase& operator=( const LinkedListItemBase& );
};

class LinkedListBase
{
public:
	LinkedListBase();
	virtual ~LinkedListBase();
	
	void	insert( LinkedListItemBase* item );
	void	remove( LinkedListItemBase* item );

	LinkedListItemBase*		last() const;

protected:
	LinkedListItemBase*		m_first;

private:
	LinkedListBase( const LinkedListBase& );
	LinkedListBase& operator=( const LinkedListBase& );
};

/**
 * Double linked list item.
 * @ingroup lang
 */
template <class T> class LinkedListItem :
	public LinkedListItemBase
{
public:
	/**
	 * Returns the next element in the list if any.
	 */
	T*		next() const			{return static_cast<T*>( m_next );}

	/**
	 * Returns the previous element in the list if any.
	 */
	T*		previous() const		{return static_cast<T*>( m_previous );}
};

/**
 * Double linked list.
 * @ingroup lang
 */
template <class T> class LinkedList :
	public LinkedListBase
{
public:
	/** 
	 * Inserts element to the start of the list.
	 */
	void	insert( T* item )		{NS(LinkedListBase,insert)( item );}

	/** 
	 * Removes element from the list.
	 */
	void	remove( T* item )		{NS(LinkedListBase,remove)( item );}

	/**
	 * Returns the first element in the list.
	 */
	T*		first() const			{return static_cast<T*>( m_first );}
	
	/**
	 * Returns the last element in the list.
	 */
	T*		last() const			{return static_cast<T*>( NS(LinkedListBase,last)() );}
};


END_NAMESPACE() // lang


#endif // _LANG_LINKEDLIST_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
