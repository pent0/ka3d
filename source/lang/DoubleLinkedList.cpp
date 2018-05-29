#include <lang/DoubleLinkedList.h>
#include <config.h>
#include <lang/Debug.h>

BEGIN_NAMESPACE(lang) 


DoubleLinkedListItemBase::~DoubleLinkedListItemBase()
{
	assert( !m_previous && !m_next && "Item still linked in its dtor" );
}

DoubleLinkedListBase::DoubleLinkedListBase() :
	m_first(0), m_last(0)
{
}

DoubleLinkedListBase::~DoubleLinkedListBase()
{
	assert( !m_first  && "Still items in linked list dtor" );
}

void DoubleLinkedListBase::insert( DoubleLinkedListItemBase* item )
{
	assert( !item->linked() );

	item->m_next = m_first;
	if ( m_first != 0 )
		m_first->m_previous = item;

	m_first = item;

	if(m_last == 0)
		m_last = m_first;
}

void DoubleLinkedListBase::insertAfter( DoubleLinkedListItemBase* beforeItem, DoubleLinkedListItemBase* item )
{
	assert( !item->linked() );

	if(beforeItem != 0)
	{
		if(beforeItem->m_next != 0)
			beforeItem->m_next->m_previous = item;

		item->m_next = beforeItem->m_next;
		item->m_previous = beforeItem;

		beforeItem->m_next = item;

		if(m_first == 0)
		{
			m_first = item;
			m_last = item;
		}

		if ( m_last == beforeItem )
			m_last = item;

	}
	else
	{
		insert(item);
	}
}

void DoubleLinkedListBase::remove( DoubleLinkedListItemBase* item )
{
	assert( 0 != item );

	if(m_first == item || m_last == item)
	{

		if ( m_first == item )
		{
			// remove the first item
			assert( !item->m_previous );
			m_first = item->m_next;
			if ( m_first )
				m_first->m_previous = 0;
		}
		if ( m_last == item )
		{
			// remove the last item
			assert( !item->m_next );
			m_last = item->m_previous;
			if ( m_last )
				m_last->m_next = 0;
		}
		//Debug::printf("first: [%d], last: [%d]\n", m_first , m_last);
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

DoubleLinkedListItemBase* DoubleLinkedListBase::last() const
{
	/*LinkedListItemBase* last = 0;
	for (DoubleLinkedListItemBase* it = m_first ; it != 0 ; it = it->m_next )
		last = it;*/
	return m_last;
}

END_NAMESPACE() // lang

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
