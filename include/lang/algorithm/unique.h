#ifndef _LANG_UNIQUE_H
#define _LANG_UNIQUE_H


BEGIN_NAMESPACE(lang) 


/** 
 * See NS(std,unique)
 */
template<class T> inline T unique( T first, T last )
{
	for (T firstb; (firstb = first) != last && ++first != last; )
		if (*firstb == *first)
		{
			for (; ++first != last; )
				if (!(*firstb == *first))
					*++firstb = *first;
			return (++firstb);
		}
	return (last);
}


END_NAMESPACE() // lang


#endif // _LANG_UNIQUE_H
