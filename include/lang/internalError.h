#ifndef _LANG_INTERNALERROR_H
#define _LANG_INTERNALERROR_H


#include <lang/pp.h>


BEGIN_NAMESPACE(lang) 


/** 
 * Throws Exception with 'Internal error at <filename>(<line>): <expr>' message. 
 * @ingroup lang
 */
void internalError( const char* filename, int line, const char* expr );


END_NAMESPACE() // lang


#endif // _LANG_INTERNALERROR_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
