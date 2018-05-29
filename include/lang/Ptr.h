#ifndef _LANG_PTR_H
#define _LANG_PTR_H


#include <lang/pp.h>


BEGIN_NAMESPACE(lang) 


/**
 * Smart pointer to an object of class T.
 * T must implement addReference() and release().
 * @see Object
 * 
 * @ingroup lang
 */
template <class T> class Ptr
{
public:
	/** Null pointer. */
	Ptr()													{m_o = 0;}

	/** Releases reference to the object. */
	~Ptr()													{if ( m_o ) m_o->release();}

	/** Increments object reference count and stores the pointer to an object. */
	Ptr( const Ptr<T>& other )								{T* o = other.ptr(); if ( o ) o->addReference(); m_o = o;}

	/** Increments object reference count and stores the pointer to an object. */
	Ptr( T* other )											{if ( other ) other->addReference(); m_o = other;}

	/** 
	 * Releases old reference if any, increments other object reference 
	 * count and stores the new reference. 
	 */
	Ptr<T>& operator=( const Ptr<T>& other )				{T* o = other.ptr(); if ( o ) o->addReference(); if ( m_o ) m_o->release(); m_o = o; return *this;}

	/** Return reference to the object. */
	T&		operator*() const								{return *m_o;}

	/** Access member of the object. */
	T*		operator->() const								{return m_o;}

	/** Returns pointer to the object. */
	operator T*() const										{return m_o;}

	/** Returns pointer to the object. */
	T*		ptr() const										{return m_o;}

private:
	T* m_o;
};


END_NAMESPACE() // lang


/** Smart pointer to an object which implements addReference() and release(). */
#define P( MYCLASS ) NS(lang,Ptr)< MYCLASS >


#endif // _LANG_PTR_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
