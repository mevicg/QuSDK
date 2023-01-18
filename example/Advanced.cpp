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

#include <quApi.hpp>

/**
 * Some activities may be hazardous to your application, or you want to use a custom color for easier identification.
 * In this case you can specify a custom color. There are a few presets like QU_CLR_BLOCKED / QU_CLR_IDLE / others,
 * or you may manually build colors using the QU_RGB macro. Specifying color 0 anywhere will make Qumulus pick a color
 * for you based on the activity's name.
 */
struct Mutex
{
	void Lock()
	{
		static quRecurringActivityID ACTIVITY_ID = quAddRecurringActivity( "Grabbing Lock", QU_CLR_BLOCKED );

		qu::ScopedActivity activity( ACTIVITY_ID );
		//pthread_mutex_lock();
	}
};

/**
 * Some applications may have processes that span multiple threads. For example a resource
 * may be read from disk on an io thread, then decompressed using a task thread and finally be uploaded
 * to the gpu on the render thread. To be able to track usage like this you can manually create
 * activity channels. You should be careful when doing this as channels are still treated as stacks,
 * so you must not end an activity on one thread while an activity started later on a different thread is still busy.
 */
struct IOThread
{
	IOThread( quActivityChannelID uploadChannelID ) :
	    uploadChannelID( uploadChannelID )
	{
	}

	void ReadFile()
	{
		static auto ACTIVITY_ID = quAddRecurringActivity( "Reading File", 0 );
		//Here we specify the channel we want this activity to add to.
		qu::ScopedActivity activity( ACTIVITY_ID, uploadChannelID );
		//Do io work...
	}

	quActivityChannelID uploadChannelID;
};
struct Application
{
	Application() :
	    activityChannel( "Upload process", false ),
	    ioThread( activityChannel.GetID() )
	{
	}
	void Initialize()
	{
		/**
		 * The decompress and render threads would look similar to the io thread. Just pass in the channel id
		 * and provide that id when creating ScopedActivities. We must manually make sure that one thread's
		 * activities have finished before we start a new activity from another thread.
		 */
		ioThread.ReadFile();
		//decodeThread.Decode();
		//renderThread.Upload();
	}

	qu::ScopedActivityChannel activityChannel;
	IOThread ioThread;
};

/**
 * In some cases you may have activities executing on different activity channels (ie different threads with channels created for that thread).
 * It is possible to link activities together. This may be useful when you've got one activity triggering another activity
 * to run in a seperate channel. For example when you're launching tasks this may be used to figure out who launched a task executing
 * on one of your task threads. Flows are attached to the highest level activity in their channel's stack.
 */
quFlowID flowID = QU_INVALID_FLOW_ID;
void IOThread_ReadFile()
{
	QU_INSTRUMENT_FUNCTION();
	flowID = quStartFlow( quGetChannelIDForCurrentThread(), "Detatch for decompression" );
}
void TaskThread_ExecuteTask()
{
	QU_INSTRUMENT_FUNCTION();
	quStopFlow( quGetChannelIDForCurrentThread(), flowID );
}