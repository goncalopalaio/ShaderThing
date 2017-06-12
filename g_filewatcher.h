#define FOLDER_FILEWATCHER "shaders"
int files_changed_filewatcher = 0;
void start_filewatcher();
void stop_filewatcher();

#ifdef __MACH__
	#include <CoreServices/CoreServices.h>
	FSEventStreamRef stream;

	void loop_filewatcher(
	    ConstFSEventStreamRef streamRef,
	    void *clientCallBackInfo,
	    size_t numEvents,
	    void *eventPaths,
	    const FSEventStreamEventFlags eventFlags[],
	    const FSEventStreamEventId eventIds[])
	{
	    int i;
	    char **paths = eventPaths;
	 
	    // printf("Callback called\n");
	    for (i=0; i<numEvents; i++) {
	        int count;
	        /* flags are unsigned long, IDs are uint64_t */
	        printf("Change %llu in %s, flags %u\n", eventIds[i], paths[i], eventFlags[i]);
	   		
	   }

	   files_changed_filewatcher = numEvents > 0;
	}

	void stop_filewatcher() {
		printf("stop_filewatcher: stop\n");
		FSEventStreamStop(stream);
		FSEventStreamInvalidate(stream);
		FSEventStreamRelease(stream);
	}

	/*https://developer.apple.com/library/content/documentation/Darwin/Conceptual/FSEvents_ProgGuide/UsingtheFSEventsFramework/UsingtheFSEventsFramework.html#//apple_ref/doc/uid/TP40005289-CH4-DontLinkElementID_11*/
	void start_filewatcher() {
		printf("start_filewatcher: start\n");
		 /* Define variables and create a CFArray object containing
	       CFString objects containing paths to watch.
	     */
	    CFStringRef mypath = CFSTR(FOLDER_FILEWATCHER);
	    CFArrayRef pathsToWatch = CFArrayCreate(NULL, (const void **)&mypath, 1, NULL);
	    void *callbackInfo = NULL; // could put stream-specific data here.
	    //FSEventStreamRef stream;
	    CFAbsoluteTime latency = 3.0; /* Latency in seconds */
	 
	    /* Create the stream, passing in a callback */
	    stream = FSEventStreamCreate(NULL,
	        &loop_filewatcher,
	        callbackInfo,
	        pathsToWatch,
	        kFSEventStreamEventIdSinceNow, /* Or a previous event ID */
	        latency,
	        kFSEventStreamCreateFlagNone /* Flags explained in reference */
	    );

	    /* Create the stream before calling this. */
	    FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

	    // Start stream
	    bool success = FSEventStreamStart(stream);
	    printf("start_filewatcher: file event stream started? %d\n", success);
	}

#endif
