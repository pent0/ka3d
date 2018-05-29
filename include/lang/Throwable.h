#ifndef _LANG_THROWABLE_H
#define _LANG_THROWABLE_H


#include <lang/pp.h>
#include <lang/Format.h>


BEGIN_NAMESPACE(lang) 


/**
 * Base class for all exceptions.
 * 
 * @ingroup lang
 */
class Throwable
{
public:
	/** 
	 * Creates throwable object with no error description. 
	 */
	Throwable();

	/** 
	 * Creates throwable object with error description. 
	 * @exception FormatException If message is not valid.
	 */
	Throwable( const Format& msg );

	/** 
	 * Returns the error message format object. 
	 */
	const Format& getMessage() const;

	/*
	 * Used by implementation to show error message dialog box.
	 */
	void showExceptionDialog();

private:
	Format	m_msg;
};


/** 
 * Wrapper for throwing exceptions.
 * Can be made 'work' also on platforms which do not support exceptions.
 */
template <class T> void throwError( T e )
{
#ifdef LANG_EXCEPTIONS
	throw e;
#else
	e.showExceptionDialog();
#endif
}


END_NAMESPACE() // lang


#endif // _LANG_THROWABLE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
