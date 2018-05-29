#ifndef _IMG_TEST_H
#define _IMG_TEST_H


BEGIN_NAMESPACE(lang) 
	class String;END_NAMESPACE()


BEGIN_NAMESPACE(img) 


/**
 * Performs img library internal tests.
 * @param datapath Path to data/ directory, where the test images are.
 */
void test( const NS(lang,String)& datapath );


END_NAMESPACE() // img


#endif // _IMG_TEST_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
