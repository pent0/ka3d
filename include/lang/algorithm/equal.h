#ifndef _LANG_EQUAL_H
#define _LANG_EQUAL_H


BEGIN_NAMESPACE(lang) 


/** 
 * See NS(std,equal)
 */
template<class T> inline bool equal( T first, T last, T first2 )
{
	while ( first != last )
	{
		if ( *first != *first2 )
			return false;
		++first;
		++first2;
	}
	return true;
}


END_NAMESPACE() // lang


#endif // _LANG_EQUAL_H
