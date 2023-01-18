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

#pragma once
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <quApi.h>

/**
 * Every intance of this thread creates its own activity channel and generates test data at a specific resolution.
 * This is somewhat analogous to how a real application would behave. It could have some ui or resource loading threads
 * that aren't all that active, while also having a render thread that is chewing through a lot of work.
 */
class ActivityChannelThread
{
public:
	ActivityChannelThread( const std::string& threadName, uint32_t loopWaitMS );
	~ActivityChannelThread();

private:
	void ThreadRun();

	std::string threadName;                       //!< The name for this thread, it will be used to give our activity channel the same name.
	uint32_t loopWaitMS;                          //!< Configuration variable used to control the density of activities generated for our activity channel.
	std::vector< quActivityID > activeActivities; //!< We're manually maintaining a stack of active activities, usually you would have functions which have been instrumented.

	std::thread threadHandle;       //!< Handle for our thread, maintained so that we may stop it before destroying this instance.
	std::atomic< bool > shouldStop; //!< Used to signal to the thread loop that it should stop looping.
};