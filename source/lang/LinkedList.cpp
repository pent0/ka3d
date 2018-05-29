#include <lang/LinkedList.h>
#include <config.h>


BEGIN_NAMESPACE(lang) 


LinkedListItemBase::~LinkedListItemBase()
{
	assert( !m_previous && !m_next && "Item still linked in its dtor" );
}

LinkedListBase::LinkedListBase() :
	m_first(0)
{
}

LinkedListBase::~LinkedListBase()
{
	assert( !m_first && "Still items in linked list dtor" );
}

void LinkedListBase::insert( LinkedListItemBase* item )
{
	assert( !item->linked() );

	item->m_next = m_first;
	if ( m_first != 0 )
		m_first->m_previous = item;
	m_first = item;
}

void LinkedListBase::remove( LinkedListItemBase* item )
{
	assert( 0 != item );

	if ( m_first == item )
	{
		// remove the first item
		assert( !item->m_previous );
		m_first = item->m_next;
		if ( m_first )
			m_first->m_previous = 0;
	}
	else
	{
		// remove middle / last item
		assert( item->m_previous );
		if ( item->m_previous )
			item->m_previous->m_next = item->m_next;
		if ( item->m_next )
			item->m_next->m_previous = item->m_previous;
	}
	item->m_next = item->m_previous = 0;
}

LinkedListItemBase* LinkedListBase::last() const
{
	LinkedListItemBase* last = 0;
	for ( LinkedListItemBase* it = m_first ; it != 0 ; it = it->m_next )
		last = it;
	return last;
}


END_NAMESPACE() // lang

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
