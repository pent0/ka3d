#ifndef _LANG_OBJECT_H
#define _LANG_OBJECT_H


#include <lang/pp.h>
#include <lang/Ptr.h>


BEGIN_NAMESPACE(lang) 


/**
 * Object class has functionality for reference counting.
 * If an Object is created in stack then special care
 * must be taken that the object reference count 
 * is not affected anywhere as it will result in crash
 * when the object goes out of scope in the stack.
 * Note that Object reference count modifying is not 
 * thread safe operation.
 * 
 * @ingroup lang
 */
class Object
{
public:
	/** 
	 * Initializes reference count to zero. 
	 */
	Object();

	/** 
	 * Initializes reference count to zero. 
	 */
	Object( const Object& );

	/** 
	 * Ensures that the reference count is zero. 
	 */
	virtual ~Object();

	/** 
	 * Does nothing. 
	*/
	Object&			operator=( const Object& )		{return *this;}

	/** 
	 * Increments reference count by one. 
	 * DO NOT USE THIS METHOD ON OBJECTS
	 * WHICH ARE CREATED IN STACK!
	 */
	void			addReference()					{++m_refs;}

	/** 
	 * Decrements reference count and 
	 * uses operator delete to the object when
	 * the count reaches zero. 
	 * DO NOT USE THIS METHOD ON OBJECTS
	 * WHICH ARE CREATED IN STACK!
	 */
	void			release()						
	{
		if ( --m_refs == 0 ) delete this;
	}

	/**
	 * Returns number of references left.
	 * For DEBUG use only.
	 */
	int				references() const				{return m_refs;}

private:
	int				m_refs;
};


END_NAMESPACE() // lang


#endif // _LANG_OBJECT_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
