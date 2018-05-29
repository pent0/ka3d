#ifndef _GR_DX_CONTEXTITEM_H
#define _GR_DX_CONTEXTITEM_H


#include <gr/ContextObject.h>
#include <lang/String.h>
#include <lang/SingleLinkedList.h>


BEGIN_NAMESPACE(gr) 


class DX_Context;


/**
 * Common base for keeping track of objects created by DX_Context.
 */
class DX_ContextItem :
	public NS(lang,SingleLinkedListItem)<DX_ContextItem>
{
public:
	explicit DX_ContextItem( DX_Context* context, NS(ContextObject,ClassId) classid );

	DX_ContextItem( const DX_ContextItem& other );

	virtual ~DX_ContextItem();

	/**
	 * Called before reseting device.
	 */
	virtual void	deviceLost() = 0;

	/**
	 * Called after reseting device.
	 */
	virtual void	deviceReset() = 0;

	/**
	 * Returns object string description.
	 */
	virtual NS(lang,String)	toString() const = 0;

	/**
	 * Returns true if resource is specified type.
	 */
	bool	isType( NS(ContextObject,ClassId) classid ) const		{return classid == m_classId;}

protected:
	DX_Context*						m_context;
	const NS(ContextObject,ClassId)	m_classId;

private:
	DX_ContextItem();
	DX_ContextItem& operator=( const DX_ContextItem& );
};


END_NAMESPACE() // gr


#endif // _GR_DX_CONTEXTITEM_H
