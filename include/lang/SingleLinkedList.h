#ifndef _LANG_SINGLELINKEDLIST_H
#define _LANG_SINGLELINKEDLIST_H


#include <lang/pp.h>


BEGIN_NAMESPACE(lang) 


class SingleLinkedListItemBase
{
public:
	/**
	 * Returns true if the item is linked to some list.
	 */
	bool	linked() const		{return 0 != m_next || 0 != m_previous;}

protected:
	SingleLinkedListItemBase() : m_next(0), m_previous(0) {}
	virtual ~SingleLinkedListItemBase();

	friend class SingleLinkedListBase;
	SingleLinkedListItemBase* m_next; 
	SingleLinkedListItemBase* m_previous;

private:
	SingleLinkedListItemBase( const SingleLinkedListItemBase& );
	SingleLinkedListItemBase& operator=( const SingleLinkedListItemBase& );
};

class SingleLinkedListBase
{
public:
	SingleLinkedListBase();
	virtual ~SingleLinkedListBase();
	
	void	insert( SingleLinkedListItemBase* item );
	void	remove( SingleLinkedListItemBase* item );

	SingleLinkedListItemBase*		last() const;

protected:
	SingleLinkedListItemBase*		m_first;

private:
	SingleLinkedListBase( const SingleLinkedListBase& );
	SingleLinkedListBase& operator=( const SingleLinkedListBase& );
};

/**
 * Double linked list item.
 * @ingroup lang
 */
template <class T> class SingleLinkedListItem :
	public SingleLinkedListItemBase
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
template <class T> class SingleLinkedList :
	public SingleLinkedListBase
{
public:
	/** 
	 * Inserts element to the start of the list.
	 */
	void	insert( T* item )		{SingleLinkedListBase::insert( item );}

	/** 
	 * Removes element from the list.
	 */
	void	remove( T* item )		{SingleLinkedListBase::remove( item );}

	/**
	 * Returns the first element in the list.
	 */
	T*		first() const			{return static_cast<T*>( m_first );}
	
	/**
	 * Returns the last element in the list.
	 */
	T*		last() const			{return static_cast<T*>( SingleLinkedListBase::last() );}
};


END_NAMESPACE() // lang


#endif // _LANG_SINGLELINKEDLIST_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
