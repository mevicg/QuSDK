/*
 * Copyright (c) 2020-2022 mevicg (https://mevicg.com)
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

#include "ActivityChannelThread.h"
#include <random>
#include <array>
#include <quApi.hpp>

static std::random_device rd;
static std::mt19937 gen( rd() );
static std::uniform_real_distribution< float > dis( 0.0f, 1.0f );

//When at all possible you should predefine recurring activities. These activities take up less
//processing power as well as less network bandwidth and trace storage. You dont have to declare them
//globally. You could use QU_DECLARE_ACTIVITY or QU_DECLARE_SCOPED_ACTIVITY to have a static
//activity id declared in a function. The easiest way to instrument a function is use a
//QU_INSTRUMENT_FUNCTION at the start of the function. This will automatically use the function's signature
//as the activity's name and add the activity to the channel for the thread that the function is called from.
static std::array< quRecurringActivityID, 5 > RECURRING_ACTIVITY_IDS = {
	quAddRecurringActivity( "Stack 0", 0 ),
	quAddRecurringActivity( "Stack 1", 0 ),
	quAddRecurringActivity( "Stack 2", 0 ),
	quAddRecurringActivity( "Stack 3", 0 ),
	quAddRecurringActivity( "Stack 4", 0 )
};

ActivityChannelThread::ActivityChannelThread( const std::string& threadName, uint32_t loopWaitMS ) :
    threadName( threadName ),
    loopWaitMS( loopWaitMS ),
    shouldStop( false )
{
	threadHandle = std::thread( &ActivityChannelThread::ThreadRun, this );
}
ActivityChannelThread::~ActivityChannelThread()
{
	//Explicitly stop before our members that may be used by the Run function are destroyed.
	shouldStop = true;
	threadHandle.join();
}

void ActivityChannelThread::ThreadRun()
{
	std::string channelName = threadName + " Channel";
	//Activity channels do not have to be related to threads, but in our case they are; so we
	//can use this utility function to have a channel be alive on this thread for as long as it runs.
	qu::ScopedActivityChannel activityChannel( channelName.c_str(), true );

	while( !shouldStop.load() )
	{
		if( activeActivities.size() > 0 )
		{
			if( dis( gen ) < 0.9f )
			{
				quStopActivity( activeActivities.back() );
				activeActivities.erase( activeActivities.begin() + activeActivities.size() - 1 );
			}
		}

		if( activeActivities.size() < RECURRING_ACTIVITY_IDS.size() )
		{
			if( dis( gen ) < 0.9f )
				activeActivities.push_back( quStartRecurringActivity( activityChannel.GetID(), RECURRING_ACTIVITY_IDS[ activeActivities.size() ] ) );
		}
		if( loopWaitMS != 0 )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( loopWaitMS ) );
		}
		else
		{
			//Dont yield when we dont have to. Yielding takes too much time when we're trying to pressure the api as much as possible.
			if( dis( gen ) < 0.10f )
				std::this_thread::yield();
		}
	}
}
