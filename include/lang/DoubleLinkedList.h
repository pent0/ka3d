#ifndef _LANG_DOUBLELINKEDLIST_H
#define _LANG_DOUBLELINKEDLIST_H


#include <lang/pp.h>


BEGIN_NAMESPACE(lang) 


class DoubleLinkedListItemBase
{
public:
	/**
	 * Returns true if the item is linked to some list.
	 */
	bool	linked() const		{return 0 != m_next || 0 != m_previous;}

protected:
	DoubleLinkedListItemBase() : m_next(0), m_previous(0) {}
	virtual ~DoubleLinkedListItemBase();

	friend class DoubleLinkedListBase;
	DoubleLinkedListItemBase* m_next; 
	DoubleLinkedListItemBase* m_previous;

private:
	DoubleLinkedListItemBase( const DoubleLinkedListItemBase& );
	DoubleLinkedListItemBase& operator=( const DoubleLinkedListItemBase& );
};

class DoubleLinkedListBase
{
public:
	DoubleLinkedListBase();
	virtual ~DoubleLinkedListBase();
	
	void	insert( DoubleLinkedListItemBase* item );
	void	insertAfter( DoubleLinkedListItemBase* beforeItem, DoubleLinkedListItemBase* item);
	void	remove( DoubleLinkedListItemBase* item );

	DoubleLinkedListItemBase*		last() const;

protected:
	DoubleLinkedListItemBase*		m_first;
	DoubleLinkedListItemBase*		m_last;

private:
	DoubleLinkedListBase( const DoubleLinkedListBase& );
	DoubleLinkedListBase& operator=( const DoubleLinkedListBase& );
};

/**
 * Double linked list item.
 * @ingroup lang
 */
template <class T> class DoubleLinkedListItem :
	public DoubleLinkedListItemBase
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
template <class T> class DoubleLinkedList :
	public DoubleLinkedListBase
{
public:
	/** 
	 * Inserts element to the start of the list.
	 */
	void	insert( T* item )		{DoubleLinkedListBase::insert( item );}

	/** 
	 * Removes element from the list.
	 */
	void	remove( T* item )		{DoubleLinkedListBase::remove( item );}

	/**
	 * Returns the first element in the list.
	 */
	T*		first() const			{return static_cast<T*>( m_first );}
	
	/**
	 * Returns the last element in the list.
	 */
	T*		last() const			{return static_cast<T*>( DoubleLinkedListBase::last() );}
};


END_NAMESPACE() // lang


#endif // _LANG_DOUBLELINKEDLIST_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
