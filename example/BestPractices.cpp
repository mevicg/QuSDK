/*
 * Copyright (c) 2020-2023 mevicg (https://mevicg.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <math.h>

//The hpp is the header that includes the entire api. When you include this you can make use of the c-functions,
//the c++ utilities and the macros. If you're compiling as c++ we suggest including this file instead of the .h file
//which is intended for pure c.
#include <quApi.hpp>

/**
 * Here is the very most basic example on how to instrument your application's threads.
 * When you want a function to show up in the profiler you put QU_INSTRUMENT_FUNCTION inside of it.
 * The instrumentation macro is the most optimal way to instrument your application. It is very low overhead
 * and easy to just paste it in the functions you are interrested in.
 */
void Update()
{
	QU_INSTRUMENT_FUNCTION();
}
void ThreadRun()
{
	//In order for functions to show up in the profiler they must be part of an activity channel.
	//This raii object creates a channel for the current thread and the instrumentation macro
	//automatically adds the function to this channel.
	qu::ScopedActivityChannel channel( "Update thread", true );

	while( true )
		Update(); //Note that the instrumentation is done inside the function we want to see, not here outside of it.
}


/**
 * In addition to functions we are also able to show counters and markers. These
 * can be used so that you can correlate values and events with what you see the functions doing at
 * the same time.
 */
bool DeviceLost()
{
	return false;
}
void RenderThread()
{
	int numFrames = 0;
	float lastTime = 0.0f;

	//Again we have this raii container to inform the profiler of this counter's existance.
	//Note that we are not required to have an activity channel for this to work.
	qu::ScopedCounter counter( "Framerate" );

	while( true )
	{
		float deltaTime = 1.0f / 60.0f;

		numFrames++;
		if( floorf( lastTime + deltaTime ) > lastTime )
		{
			//When we want to update the value shown in the profiler we just set the new value to the counter.
			counter.SetValue( (float)numFrames );
			numFrames = 0;
		}

		if( DeviceLost() )
		{
			//Markers are one-shot events. They are not originating from any channel and can be used
			//to show in the profiler that something special happened.
			quAddMarker( "Device Lost" );
		}
	}
}

/**
 * Sometimes you may want to show special activities in the profiler, not a function name but
 * something more dynamic. This is possible, but you should note that using activities with a dynamic
 * name is slower than if the name is known ahead of time. This is because the name needs to be processed
 * for each activity rather than only once.
 */
#include <string>
void UpdatePlayer( int playerIndex )
{
	std::string message = "Updating player #" + std::to_string( playerIndex );
	qu::ScopedActivity activity( message.c_str() );
	//... do update work.


	//If at all possible, you should prevent activities with dynamic names like above.
	//In this case you may know how many players you have at most, so you could do something like this:
	static quRecurringActivityID ACTIVITY_IDS[] = {
		quAddRecurringActivity( "Updating player #0", 0 ),
		quAddRecurringActivity( "Updating player #1", 0 ),
		quAddRecurringActivity( "Updating player #2", 0 ),
		quAddRecurringActivity( "Updating player #3", 0 ),
	};
	//We have informed the profiler of the existance of these activities, this happens once.
	//After that we can use those IDs freely, bypassing all the repeated processing.
	qu::ScopedActivity activity2( ACTIVITY_IDS[ playerIndex ] );
	//... do update work.
}


/**
 * Finally you may want to instrument code you dont have access to (ie a library). If you cant access the code,
 * you can't put the instrumentation inside those functions. In this case you can only put the instrumentation outside of
 * of the function you want to see. Obviously you have to do this everywhere you use that library, so it should be a last resort.
 */
namespace Lib
{
extern bool Initialize();
extern bool ReadFile( const char* );
extern void* Decode();
} //End namespace Lib
void UseLibrary()
{
	//Predefine the activities again as explained above. This eliminates the overhead for processing the
	//activity names every time we fire them.
	static quRecurringActivityID ACTIVITY_IDS[] = {
		quAddRecurringActivity( "Initializing", 0 ),
		quAddRecurringActivity( "Reading File", 0 ),
		quAddRecurringActivity( "Decoding File", 0 ),
	};

	//We're showing an additional feature of the raii utility here, which is rescoping
	//into a different activity. This finishes the current activity and starts a new one.
	qu::ScopedActivity activity( ACTIVITY_IDS[ 0 ] );
	Lib::Initialize();
	activity.Rescope( ACTIVITY_IDS[ 1 ] );
	Lib::ReadFile( "foo.png" );
	activity.Rescope( ACTIVITY_IDS[ 2 ] );
	Lib::Decode();
	//You can manually end the scope too. This is not needed here though (as it's a raii object
	//so this happens automatically). Being able to manually end an activity prevents you from
	//having to introduce new scopes just because you want your custom instrumentation to stop.
	activity.EndScope();
	//ie it prevents you from having to write code like this:
	//bool initResult = false;
	//{
	//	qu::ScopedActivity activity( ACTIVITY_IDS[ 0 ] );
	//	initResult = Lib::Initialize();
	//}
}


/**
 * That's all!
 * Create ScopedActivityChannels on threads you want to instrument and place QU_INSTRUMENT_FUNCTION macros
 * inside the functions you want to see show up in the profiler. Use counters to visualize values and use markers for events.
 * If you want to use some of the more advanced features like colors and activities not bound to threads
 * you should have a look at the Advanced.cpp file.
 */


/**
 * Assume this is the library code you're unable to edit. You cant place instrumentation macros in these functions.
 * These functions are implented here so that the example can link correctly and there are no missing functions ;)
 */
namespace Lib
{
bool Initialize()
{
	return true;
}
bool ReadFile( const char* )
{
	return true;
}
void* Decode()
{
	return nullptr;
}
} //End namespace Lib