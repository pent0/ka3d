#ifndef _LANG_GREATER_H
#define _LANG_GREATER_H


BEGIN_NAMESPACE(lang) 


template <class T> class greater
{
public:
	bool operator()( const T& left, const T& right ) const
	{
		return (left > right);
	}
};


END_NAMESPACE() // lang


#endif // _LANG_GREATER_H
