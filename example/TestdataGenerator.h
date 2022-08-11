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

#pragma once
#include <memory>
#include <chrono>
#include <quApi.hpp>
#include "ActivityChannelThread.h"

/**
 * This class implements the live generation of data to be viewed in the Qumulus demo.
 * It has several counters and activity channels which are continuously being populated with
 * data so that in demo mode there is something generated live to inspect.
 */
class TestdataGenerator
{
public:
	TestdataGenerator();
	~TestdataGenerator();

	void Run();

private:
	using Clock = std::chrono::steady_clock;
	using TimePoint = Clock::time_point;

	static TimePoint applicationStartTime;

	void Start();
	void Stop();

	TimePoint GetNextMarkerTime();

	//Counter test data
	qu::ScopedCounter sineWave;
	qu::ScopedCounter saw;
	qu::ScopedCounter saw100;

	//Activity test data
	std::unique_ptr< ActivityChannelThread > lowFrequencyActivityThread;
	std::unique_ptr< ActivityChannelThread > mediumFrequencyActivityThread;
	std::unique_ptr< ActivityChannelThread > highFrequencyActivityThread;

	//Marker test data
	TimePoint timeOfNextMark;
};
