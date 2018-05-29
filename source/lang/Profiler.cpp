#include <lang/Profiler.h>

#ifdef USE_PROFILING

#include <string.h>
#include <stdio.h>
#include <lang/assert.h>
#include <lang/debug.h>
#include <io/FileOutputStream.h>

USING_NAMESPACE(lang)

#ifdef PLATFORM_NGI
#	include <errorcodes.h>
#	include <io/NGI/FileManager_NGI.h>
	ngi::IPerformanceTiming*	Profiler::sm_performanceTiming = 0;
#endif

bool 					Profiler::sm_instantiated = false;
io::FileOutputStream*	Profiler::sm_logFile = 0;
char					Profiler::sm_messageBuffer[];
unsigned long 			Profiler::sm_frequency = 0xFFFFFFFF;

int 					Profiler::sm_lastOpenedSample = -1;
int 					Profiler::sm_openSampleCount = 0;

time_t 					Profiler::sm_frameTimeStamp = 0;
unsigned long long 		Profiler::sm_rootFrameBegin = 0;
unsigned long long 		Profiler::sm_rootFrameEnd = 0;
unsigned long long 		Profiler::sm_periodTime = 0;

unsigned int 			Profiler::sm_validFrameCount = 0;
unsigned int 			Profiler::sm_invalidFrameCount = 0;

bool 					Profiler::sm_outputRequested = false;

Profiler::ProfilerSample Profiler::sm_samples[];
char* Profiler::sm_testCaseName = "No test case specified.";

Profiler::Profiler(char* sampleName)
{
	//The first thing we need to do is restore our previous pieces of sample
	//data from storage. That is, look in the samples[] array to see if there's
	//a valid sample with our name on it
	int i = 0;
	//If we don't find it, we're going to need to create a new sample, and rather
	//than looping through the list a second time we store the first non-valid
	//index that we find this time round
	int storeIndex = -1;
	for(i=0; i<MAX_PROFILER_SAMPLES; ++i)
	{
		if(!sm_samples[i].bIsValid)
		{
			if(storeIndex<0)storeIndex = i;
		}
		else
		{
			if( !strcmp(sm_samples[i].name, sampleName) )
			{
			    //this is the sample we want
			    //check that it's not already open
			    //assert only works in debug builds, but given that you don't use
			    //the profiler in release builds, it doesn't really matter
			    assert(!sm_samples[i].bIsOpen && "Tried to profile a sample which was already being profiled");
			    //first, store its index
			    m_sampleIndex = i;
			    //the parent sample is the last opened sample
			    sm_samples[i].parentId = sm_lastOpenedSample;
			    sm_lastOpenedSample = i;
			    sm_samples[i].parentCount = sm_openSampleCount;
			    ++sm_openSampleCount;
			    sm_samples[i].bIsOpen = true;
			   			    
			    //finally (more or less) store the current time to start the sample
			    sm_samples[i].frameStartTime = getTime_ns();
			    
			    //if this has no parent, it must be the 'main loop' sample, so copy
			    //to the global timer as well
			    if(sm_samples[i].parentId < 0)
			    {
			    	assert(i == 0);
			    	frameBegin(sm_samples[i].frameStartTime);
			    }
			    
				//increment the number of times we've been called
			    ++sm_samples[i].frameCallCount;
			    
				//done
				return;
	 		}
		}
	}
	
	//we've not found it, so it must be a new sample
	//use the storeIndex value to store the new sample
	assert(storeIndex>=0 && "Profiler has run out of sample slots!");
	sm_samples[storeIndex].bIsValid = true;
	sm_samples[storeIndex].name = sampleName;
	m_sampleIndex = storeIndex;

	sm_samples[storeIndex].parentCount = sm_openSampleCount;
	sm_samples[storeIndex].parentId = sm_lastOpenedSample;
	sm_lastOpenedSample = storeIndex;

	sm_openSampleCount++;
	sm_samples[storeIndex].bIsOpen = true;


	//init the statistics for this sample
	sm_samples[storeIndex].frameTotalTime = 0;
	sm_samples[storeIndex].frameChildTime = 0;
	sm_samples[storeIndex].frameStartTime = getTime_ns();

	if(sm_samples[storeIndex].parentId<0)
	{
		assert(storeIndex == 0);
		frameBegin(sm_samples[storeIndex].frameStartTime);

	}
	sm_samples[storeIndex].frameCallCount = 1;

}

void Profiler::frameBegin(unsigned long counter)
{
	assert(sm_lastOpenedSample == 0 && sm_openSampleCount == 1);
	sm_rootFrameBegin = counter;
	sm_frameTimeStamp = time(0);
	
	for(int i=0;i<MAX_PROFILER_SAMPLES; ++i)
	{
			sm_samples[i].frameStartTime = 0;
			sm_samples[i].frameTotalTime = 0;
			sm_samples[i].frameChildTime = 0;
			sm_samples[i].frameCallCount = 0;
	}
	sm_samples[0].frameStartTime = counter;
}

void Profiler::frameEnd(unsigned long long counter)
{
	time_t endStamp = time(0);
	assert( sm_openSampleCount == 0);
	
	long long frameTime = counter - sm_rootFrameBegin;
		
	if( frameTime > 0 && endStamp == sm_frameTimeStamp ) // skip frames where the performance counter may have overflowed
	{
		// No overflow; add frame statistics to period statistics
		sm_rootFrameEnd = counter;
	
		for(int i=0;i<MAX_PROFILER_SAMPLES; ++i)
		{
			// Only update statistics for samples that were used this frame
			if( sm_samples[i].bIsValid && sm_samples[i].frameCallCount >= 1 )
			{	
				unsigned long long sampleTime = sm_samples[i].frameTotalTime - sm_samples[i].frameChildTime;
				float percentage = ( (float)sampleTime / frameTime ) * 100.0f;

				//assert(percentage <= 100.f && percentage >= 0.f);

				if( (sm_samples[i].minPc == -1) || (percentage < sm_samples[i].minPc) )
					sm_samples[i].minPc = percentage;
				if( (sm_samples[i].maxPc == -1) || (percentage > sm_samples[i].maxPc) )
					sm_samples[i].maxPc = percentage;

				sm_samples[i].periodTotalTime += sm_samples[i].frameTotalTime;
				sm_samples[i].periodChildTime += sm_samples[i].frameChildTime;
				sm_samples[i].periodCallCount += sm_samples[i].frameCallCount;
			}
			
			// Reset statistics for all samples, regardless of wether they were updated this frame
			sm_samples[i].frameTotalTime = 0;
			sm_samples[i].frameChildTime = 0;
			sm_samples[i].frameCallCount = 0;
		}
		
		sm_periodTime += frameTime;
		assert(sm_periodTime > 0); // Should not overflow within our lifetime...
		sm_validFrameCount++;
	
	}
	else
	{
		// Possible overflow; reset frame statistics for all samples
		for(int i=0; i<MAX_PROFILER_SAMPLES; ++i)
		{
			sm_samples[i].frameStartTime = 0;
			sm_samples[i].frameTotalTime = 0;
			sm_samples[i].frameChildTime = 0;
			
			// We want to know the number of calls even during invalid frames 
			// NO! sm_samples[i].frameCallCount = 0;
			sm_samples[i].periodCallCount += sm_samples[i].frameCallCount;
		}
		sm_invalidFrameCount++;
	}

	if( sm_outputRequested )
		doOutput();

}

void Profiler::resetAll()
{
	assert(sm_openSampleCount == 0);
	
	for(int i=0; i<MAX_PROFILER_SAMPLES; ++i)
	{
			sm_samples[i].bIsValid = false;
			sm_samples[i].minPc = -1;
			sm_samples[i].maxPc = -1;
			sm_samples[i].frameCallCount = 0;
			sm_samples[i].frameTotalTime = 0;
			sm_samples[i].frameChildTime = 0;
			sm_samples[i].periodCallCount = 0;
			sm_samples[i].periodTotalTime = 0;
			sm_samples[i].periodChildTime = 0;
	}
	
	sm_lastOpenedSample = -1;
	sm_openSampleCount = 0;
	sm_rootFrameBegin = 0;
	sm_rootFrameEnd = 0;

	sm_frameTimeStamp = 0;
	sm_validFrameCount = 0;
	sm_invalidFrameCount = 0;
	sm_periodTime = 0;
	sm_outputRequested = false;
}

Profiler::~Profiler()
{
	unsigned long endTime = getTime_ns();
	//done timing
	
	sm_samples[m_sampleIndex].bIsOpen = false;
	sm_lastOpenedSample = sm_samples[m_sampleIndex].parentId;
	--sm_openSampleCount;
	
	//calculate the time taken this profile
	unsigned long timeTaken = endTime - sm_samples[m_sampleIndex].frameStartTime;
	sm_samples[m_sampleIndex].frameTotalTime += timeTaken;
	
	if(sm_lastOpenedSample >= 0)
	{
		sm_samples[sm_lastOpenedSample].frameChildTime += timeTaken;
	}
	else
	{
		//no parent, so this is the end of the main loop sample
		frameEnd(endTime);
	}
	
}


void Profiler::setTestCaseName(const char* name)
{
	sm_testCaseName = const_cast<char*>(name);
}

void Profiler::printLine( const char* fmt, ... )
{
	
	// format variable arguments
	char msg[MAX_PROFILER_LINELENGTH+4];

	va_list marker;
	va_start( marker, fmt );
	vsprintf( msg, fmt, marker );
	va_end( marker );
	
	assert( strlen(msg) < MAX_PROFILER_LINELENGTH ); // message too long

	if( sm_logFile )
	{
		// Append to buffer
		strcat(sm_messageBuffer, msg);
	}
	else
	{
		// fix percent characters, pass to Debug::printf()
		char msg2[(MAX_PROFILER_LINELENGTH+4)*2];
		int j = 0;
		for(unsigned int i = 0; i<strlen(msg); ++i, ++j)
		{
			msg2[j]=msg[i];
			if(msg[i] == '%')
			{
				j++;
				msg2[j]='%';
			}
		}
		msg2[j] = 0;
		lang::Debug::printf(msg2);
	}

}

void Profiler::printSample_r(int id)
{
	//calculate the time spend on the sample itself (excluding children)
	unsigned long long sampleTime = sm_samples[id].periodTotalTime-sm_samples[id].periodChildTime;
	
	// Average percentages
	float percentage_self =	( (float)sampleTime / ( sm_periodTime ) ) * 100.0f;
	float percentage_total =( (float)sm_samples[id].periodTotalTime / ( sm_periodTime ) ) * 100.0f;

	// assert(percentage <= 100.f && percentage >= 0.f);

	char indent[MAX_PROFILER_SAMPLES * 2 + 1];
	*indent = 0;

	for(int j=0; j<sm_samples[id].parentCount; ++j)
	{
		if( j == sm_samples[id].parentCount - 1 )
		{
			strcat(indent, " - ");
		}
		else
		{
			strcat(indent, "  ");
		}
	}
				
	printLine("(%05.2f%% - %05.2f%%) %6.2f%% %6.2f%% %7u %s%s\n", 
					sm_samples[id].minPc,
					sm_samples[id].maxPc,
					percentage_total,
					percentage_self,
					sm_samples[id].periodCallCount,
					indent,
					sm_samples[id].name);
	
	// recurse child nodes
	for(int i=0;i<MAX_PROFILER_SAMPLES; ++i)
	{
		if(sm_samples[i].bIsValid && sm_samples[i].parentId == id)
		{
			printSample_r(i);
		}
	}

}

void Profiler::doOutput()
{
	sm_messageBuffer[0] = 0; // "Clear string"


	printLine("\n-------------PROFILE-------------\n");
	printLine("Test case: %s\n", sm_testCaseName);
	printLine("Test length: %u ms\n", (unsigned long)(sm_periodTime / 1000000) );
	printLine("Timer frequency: %lu Hz\n", getFrequency() );
	printLine("Frames: %u (+%u unprofilable) = %.1f fps\n", sm_validFrameCount, sm_invalidFrameCount, (float)(sm_validFrameCount + sm_invalidFrameCount) / ( (float)sm_periodTime / 1000000000.f) );
	printLine("(  MIN%% -   MAX%%)  TOTAL%%   SELF%%   CALLS SAMPLENAME\n");

	for(int i=0;i<MAX_PROFILER_SAMPLES; ++i)
	{
		if(sm_samples[i].bIsValid && sm_samples[i].parentId == -1)
		{
			printSample_r(i);
			break;
		}
	}
	printLine("---------------------------------\n\n");

	if( sm_logFile ) // Dump to file if specified
	{	
		sm_logFile->write(sm_messageBuffer, strlen(sm_messageBuffer));
		sm_messageBuffer[0] = 0;
	}

	resetAll();
}



void Profiler::start(lang::String fileName)
{
	assert( !sm_instantiated && "Profiler already started.");
#if defined(PLATFORM_NGI)
	ngi::ReturnCode rc = ngi::CRuntime::CreateInstance( sm_performanceTiming );
	assert( ngi::OK == rc );
	sm_frequency = sm_performanceTiming->GetHighPrecisionCounterFrequency();
#elif defined(PLATFORM_WIN32)
	LARGE_INTEGER frq;
	int err = QueryPerformanceFrequency( &frq );
	assert( err && "No performance counter available.");
	assert( frq.HighPart == 0 && "Profiler does not currently support performance counters with frequencies higher than 0xFFFFFFFF Hz.");
	sm_frequency = frq.LowPart;
#else
#warning Profiler not configured for this platform!
#endif

	
	if(fileName != "")
	{
		time_t t = time (0); 					// Get current time in seconds	
		struct tm* timeptr = localtime (&t);	// Fill tm struct
		char* asc_time = asctime (timeptr);		// Convert to string
		char logmsg[100];
		sprintf(logmsg, "Profiler log started on %s\n", asc_time);
		
		#ifdef PLATFORM_NGI
			io::FileManager::deleteFile(fileName);
		#endif
		 
		sm_logFile = new io::FileOutputStream(fileName);
		sm_logFile->write(logmsg, strlen(logmsg) );
	}
	
	sm_instantiated = true;
	resetAll();
}

void Profiler::stop()
{
	if(sm_instantiated)
	{
		if(sm_logFile)
			delete sm_logFile;
		
		sm_instantiated = false;
		sm_openSampleCount = 0;
#ifdef PLATFORM_NGI
		if(sm_performanceTiming)
			sm_performanceTiming->Release();
#endif
	}
}

#endif // USE_PROFILING
