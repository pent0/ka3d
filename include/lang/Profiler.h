#ifndef _PROFILER_H
#define _PROFILER_H


#include <lang/pp.h>


#ifndef USE_PROFILING
	#include <lang/string.h>
	BEGIN_NAMESPACE(lang) 
		class Profiler
		{ 
		public:
			Profiler(char* /*sampleName*/){};
			~Profiler(){};
			static void start(NS(lang,String) /*fileName*/){};
			static void stop(){};
			static void output(){};
			static void setTestCaseName(const char* /*name*/){};
			static const char* getTestCaseName() { return "Profiler disabled"; }
		};
	END_NAMESPACE()
	#define PRF(x)
#else


#include <time.h>
#include <lang/pp.h>
#include <lang/assert.h>
#include <lang/string.h>

#define MAX_PROFILER_SAMPLES 30
#define	MAX_PROFILER_LINELENGTH 200

#if defined(PLATFORM_NGI)
#	include <runtime.h>
#   include <performancetiming.h>
#elif defined(PLATFORM_WIN32)
#	define NOMINMAX
#	include <windows.h>
#endif

BEGIN_NAMESPACE(io) 
	class FileOutputStream;
END_NAMESPACE()

BEGIN_NAMESPACE(lang) 

class Profiler
{
public:
	Profiler(char* sampleName);
	~Profiler();
	static void start(NS(lang,String) fileName = "");
	static void stop();
	static void output() {sm_outputRequested = true;}
	static void setTestCaseName(const char* name);
	static const char* getTestCaseName() { return sm_testCaseName; }

private:
	static void resetAll();
	static void frameBegin(unsigned long counter);
	static void frameEnd(unsigned long long counter);
	static void printLine( const char* fmt, ... );
	static void printSample_r(int id);
	static void doOutput();

	static inline unsigned long getFrequency()
	{
		assert( sm_instantiated && "Profiler not started.");
		return sm_frequency;
	}

	static inline unsigned long getTime_ns()
	{
		assert( sm_instantiated && "Profiler not started.");
		#if defined(PLATFORM_NGI)
			return sm_performanceTiming->GetHighPrecisionTime();
		#elif defined(PLATFORM_WIN32)
			LARGE_INTEGER freq;
			int err = QueryPerformanceFrequency(&freq);
			assert( err );

			LARGE_INTEGER cur;
			err = QueryPerformanceCounter( &cur );
			assert( err );

			double nsdiv = double(freq.LowPart) / double(1000000000.0);
			double ns = double(cur.LowPart) / nsdiv;
			return (unsigned long)ns;

		#else
			return 1;
		#endif
	}

	static bool sm_instantiated;
	static unsigned long sm_frequency; // Hz
	static char* sm_testCaseName;
	static NS(io,FileOutputStream)*	sm_logFile;
	static char	sm_messageBuffer[(MAX_PROFILER_SAMPLES + 6) * MAX_PROFILER_LINELENGTH];

	int m_sampleIndex;

	static int sm_lastOpenedSample;
	static int sm_openSampleCount;
	
	static unsigned long long	sm_rootFrameBegin;
	static unsigned long long	sm_rootFrameEnd;
	static unsigned long long	sm_periodTime;
	static time_t				sm_frameTimeStamp;
	static unsigned int			sm_validFrameCount;
	static unsigned int			sm_invalidFrameCount;

	static bool					sm_outputRequested;

	static struct ProfilerSample
	{
		ProfilerSample()
		{
		  bIsValid=false; 
		  minPc=maxPc=-1;
		  name = "unused";
		}

		bool bIsValid;    //whether or not this sample is valid to be used
		bool bIsOpen;     //is this sample currently being profiled?
		char* name; //name of the sample
		
		unsigned int frameCallCount; //number of times this sample has been executed
		unsigned int periodCallCount; //number of times this sample has been executed

		unsigned long frameStartTime;  //starting time on the clock
		unsigned long long frameTotalTime;  //total time recorded across all executions of this sample
		unsigned long long frameChildTime;  //total time taken by children of this sample
		
		unsigned long long periodTotalTime;  //total time recorded across all executions of this sample
		unsigned long long periodChildTime;  //total time taken by children of this sample

		// Output formatting helpers
		int parentCount;	// number of parents this sample has (last frame)	
		int parentId;		// ID of parent sample (last frame)

		float minPc;      //minimum percentage of game loop time taken up
		float maxPc;      //maximum percentage of game loop time taken up

	} sm_samples[MAX_PROFILER_SAMPLES];
	
#if defined(PLATFORM_NGI)
	static NS(ngi,IPerformanceTiming)* sm_performanceTiming;
#endif
};

END_NAMESPACE()

#define PRF(x) NS(lang,Profiler) prf_ ## __LINE__ = NS(lang,Profiler)(x);


#endif // USE_PROFILING

#endif // _PROFILER_H