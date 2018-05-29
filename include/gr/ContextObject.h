#ifndef _GR_CONTEXTOBJECT_H
#define _GR_CONTEXTOBJECT_H


#include <lang/Object.h>


BEGIN_NAMESPACE(gr) 

	
/**
 * Base class for rendering context dependent objects.
 * @ingroup gr
 */
class ContextObject :
	public NS(lang,Object)
{
public:
	/**
	 * Context object class identifier.
	 * Used by the implementation to avoid depending on RTTI.
	 */
	enum ClassId
	{
		/**
		 * Geometric primitive (list) resource.
		 */
		CLASSID_PRIMITIVE,
		/**
		 * Shader resource.
		 */
		CLASSID_SHADER,
		/**
		 * Texture resource.
		 */
		CLASSID_TEXTURE,
		/**
		 * Cube texture resource.
		 */
		CLASSID_CUBETEXTURE,
		/**
		 * Palette resource.
		 */
		CLASSID_PALETTE,
		CLASSID_COUNT
	};

	/**
	 * Context objects need to be locked before raw data can be accessed.
	 */
	enum LockType
	{
		/** No lock. */
		LOCK_NONE		= 0,
		/** Read-only lock. */
		LOCK_READ		= 1,
		/** Write-only lock. */
		LOCK_WRITE		= 2,
		/** Read-write lock. */
		LOCK_READWRITE	= 3,
	};
	
	/**
	 * Helper class for locking context objects for data access.
	 */
	class Lock
	{
	public:
		/** 
		 * Locks object until end of scope.
		 */
		explicit Lock( ContextObject* obj, LockType lock );
	
		/**
		 * Unlocks object.
		 */
		~Lock();
	
	private:
		P(ContextObject)	m_obj;
	
		Lock();
		Lock( const Lock& );
		Lock& operator=( const Lock& );
	};

	ContextObject( ClassId classid );

	/**
	 * Locks object for specified access. Default behaviour just returns the parameter,
	 * which in effect means that any object property can be accessed any time.
	 * Don't use this method directly, but use Lock handle/wrapper instead,
	 * which is exception-safe.
	 */
	virtual void		lock( LockType lock );

	/**
	 * Releases access to object.
	 * Don't use this method directly, but use Lock handle/wrapper instead,
	 * which is exception-safe.
	 */
	virtual void		unlock();

	/** 
	 * Returns current lock state of the object.
	 * Default state is LOCK_READWRITE (=object can be read
	 * or written any time).
	 */
	virtual LockType	locked() const;

	/**
	 * Returns class type id.
	 * Used by the implementation to avoid depending on RTTI.
	 */
	ClassId				classId() const		{return m_classId;}

private:
	ClassId		m_classId;
};


END_NAMESPACE() // gr


#endif // _GR_CONTEXTOBJECT_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
