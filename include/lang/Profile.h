#ifndef _LANG_PROFILE_H
#define _LANG_PROFILE_H


#include <lang/pp.h>


BEGIN_NAMESPACE(lang) 


// profiling is platform dependent
#ifdef PLATFORM_WIN32
	typedef __int64 rdtsc_t;
	__declspec(naked) static __int64 rdtsc()
	{
		__asm
		{
			RDTSC
			ret
		}
	}
#else
	typedef int rdtsc_t;
	static int rdtsc() {return 0;}
#endif


/**
 * Performance profiling is important part of any game development. 
 * Profile is a light-weight and high resolution performance profiling class.
 *
 * @ingroup lang
 *
 * Usage example:
 * \begin{verbatim}
 
   void myFunc() {
     PROFILE(myFunc);
     ...
   }
 
   main() {
     NS(Profile,beginFrame)();
     for ( int i = 0 ; i < 100000 ; ++i )
       myFunc();
     NS(Profile,endFrame)();
   
     for ( int i = 0 ; i < NS(Profile,count)() ; ++i )
       printf( "%s: x%d, %g%%\n", NS(Profile,getName)(i), NS(Profile,getCount)(i), NS(Profile,getPercent)(i) );
   }
 \end{verbatim}
 */
class Profile
{
public:
	enum
	{
		/** Maximum number of profiled blocks. */
		MAX_BLOCKS	= 256,
		/** Maximum name length for a block (keep short to avoid overhead). */
		MAX_NAME	= 16
	};

	/**
	 * Begins profiling a single block.
	 * Do not use this method directly,
	 * but use PROFILE macro instead.
	 * @exception Exception If high resolution timer not available.
	 */
	Profile( int id, const char* sz )	: m_id(id) {if (!sm_names[id][0]) setName(id,sz); m_time=rdtsc();}

	/**
	 * Ends profiling the block.
	 */
	~Profile()							{m_time=rdtsc()-m_time; sm_times[m_id] += m_time; sm_counts[m_id] += 1;}

	/**
	 * Begins a time frame.
	 * Frame is used to report back percentages
	 * which each block took.
	 * All blocks share the same time frame.
	 */
	static void			beginFrame();

	/**
	 * Ends a time frame.
	 * @see beginFrame
	 */
	static void			endFrame();

	/**
	 * Returns name of ith block.
	 */
	static const char*	getName( int i );

	/**
	 * Returns percentage of the frame time which ith block took.
	 */
	static float		getPercent( int i );

	/**
	 * Returns number of times ith block was executed since last reset.
	 */
	static int			getCount( int i );

	/**
	 * Returns number of profiled blocks.
	 */
	static int			blocks();

	/**
	 * Resets all counters to zero.
	 */
	static void			reset();

	/**
	 * Returns unique id.
	 * Do not use this method directly,
	 * but use PROFILE macro instead.
	 */
	static int			id();

private:
	int				m_id;
	rdtsc_t			m_time;

	static int		sm_id;
	static bool		sm_init;
	static rdtsc_t	sm_time;
	static double	sm_scale;
	static rdtsc_t	sm_times[MAX_BLOCKS];
	static int		sm_counts[MAX_BLOCKS];
	static char		sm_names[MAX_BLOCKS][MAX_NAME];

	static void		init();
	static int		indexToId( int i );
	static void		setName( int id, const char* sz );
};


/** 
 * Macro to do the actual profiling for current scope.
 * Pass function name (without quotes) to the macro,
 * for example PROFILE(myFunc).
 */
#define PROFILE(BLOCKNAME) static const int s_id_ ## BLOCKNAME = NS(Profile,id)(); Profile pr_ ## BLOCKNAME( s_id_ ## BLOCKNAME, #BLOCKNAME );


END_NAMESPACE() // lang


#endif // _LANG_PROFILE_H
