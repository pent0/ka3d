#ifndef _LANG_QUICKSORT_H
#define _LANG_QUICKSORT_H


BEGIN_NAMESPACE(lang) 


/**
 * Traditional quick sort.
 * Slower than HP/SGI introsort, but generates
 * only a fraction of code so useful on mobile platforms.
 */
/**
 * Traditional quick sort.
 * Slower than HP/SGI introsort, but generates
 * only a fraction of code so useful on mobile platforms.
 */
template <class T> void quicksort( T* begin, T* end )
{
	int count = end - begin;
	if ( count < 1 )
		return;

    int i = 0;
	int j = count - 1;
    T x = begin[count>>1];

    do
    {    
        while ( begin[i] < x )
			i++; 
        while ( x < begin[j] )
			j--;

        if ( i <= j )
        {
            T h = begin[i];
			begin[i] = begin[j];
			begin[j] = h;

            i++;
			j--;
        }
    } while ( i <= j );

    if ( 0 < j )
		quicksort( begin, begin+j+1 );
    if ( i < count-1 )
		quicksort( begin+i, end );
}

/**
 * Traditional quick sort using predicate.
 * Slower than HP/SGI introsort, but generates
 * only a fraction of code so useful on mobile platforms.
 */
template <class T, class C> void quicksort( T* begin, T* end, C comp )
{
	int count = end - begin;
	if ( count < 1 )
		return;

	int i = 0;
	int j = count - 1;
    T x = begin[count>>1];

    do
    {    
        while ( comp(begin[i],x) )
			i++; 
        while ( comp(x,begin[j]) )
			j--;

        if ( i <= j )
        {
            T h = begin[i];
			begin[i] = begin[j];
			begin[j] = h;

            i++;
			j--;
        }
    } while ( i <= j );

    if ( 0 < j )
		quicksort( begin, begin+j+1, comp );
    if ( i < count-1 )
		quicksort( begin+i, end, comp );
}


END_NAMESPACE() // lang


#endif // _LANG_QUICKSORT_H
