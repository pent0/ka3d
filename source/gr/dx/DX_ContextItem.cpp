#include <gr/dx/DX_common.h>
#include <gr/dx/DX_ContextItem.h>
#include <gr/dx/DX_Context.h>
#include <config.h>


BEGIN_NAMESPACE(gr) 


DX_ContextItem::DX_ContextItem( DX_Context* context, ContextObject::ClassId classid ) :
	m_context( context ),
	m_classId( classid )
{
	m_context->itemlist.insert( this );
}

DX_ContextItem::DX_ContextItem( const DX_ContextItem& other ) :
	m_context( other.m_context ),
	m_classId( other.m_classId )
{
	m_context->itemlist.insert( this );
}

DX_ContextItem::~DX_ContextItem()
{
	m_context->itemlist.remove( this );
}


END_NAMESPACE() // gr
