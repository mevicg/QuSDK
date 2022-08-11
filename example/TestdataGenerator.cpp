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

#include "TestdataGenerator.h"
#include <random>
#include <string>
#include <thread>

static std::random_device rd;
static std::mt19937 gen( rd() );
static std::uniform_real_distribution< float > markerDistribution( 300.0f, 4000.0f );

TestdataGenerator::TimePoint TestdataGenerator::applicationStartTime = TestdataGenerator::Clock::now();

TestdataGenerator::TestdataGenerator() :
    sineWave( "Sine Wave", false ),
    saw( "Sawtooth", false ),
    saw100( "Saw 100", false )
{
}
TestdataGenerator::~TestdataGenerator() = default;

void TestdataGenerator::Run()
{
	Start();

	while( true )
	{
		TimePoint timeNow = Clock::now();
		auto nanosSinceStart = std::chrono::duration_cast< std::chrono::nanoseconds >( timeNow - applicationStartTime ).count();
		//We can use nanosecond precision time snapshots, but we need to be careful not to overflow any data type. So for that reason
		//we use integer division to find seconds and then use floating point math only on the fractional part.
		float secondsSinceStart = 0.0f;
		secondsSinceStart += nanosSinceStart / 1000000000;
		secondsSinceStart += ( nanosSinceStart % 1000000000 ) * 0.000000001f;

		//Test counters
		sineWave.SetValue( sinf( secondsSinceStart * 2.0f * 3.1415f ) );
		saw.SetValue( fmodf( secondsSinceStart, 1.0f ) );
		saw100.SetValue( fmodf( secondsSinceStart * 100.0f, 100.0f ) );

		//Test markers
		if( timeNow > timeOfNextMark )
		{
			quAddMarker( "Test Marker" );
			timeOfNextMark = GetNextMarkerTime();
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
	}

	Stop();
}

void TestdataGenerator::Start()
{
	sineWave.Add();
	saw.Add();
	saw100.Add();
	timeOfNextMark = GetNextMarkerTime();

	lowFrequencyActivityThread.reset( new ActivityChannelThread( "Low Frequency", 10 ) );
	mediumFrequencyActivityThread.reset( new ActivityChannelThread( "Medium Frequency", 5 ) );
	highFrequencyActivityThread.reset( new ActivityChannelThread( "High Frequency", 0 ) );
}
void TestdataGenerator::Stop()
{
	sineWave.Remove();
	saw.Remove();
	saw100.Remove();
	lowFrequencyActivityThread.reset();
	mediumFrequencyActivityThread.reset();
	highFrequencyActivityThread.reset();
}

TestdataGenerator::TimePoint TestdataGenerator::GetNextMarkerTime()
{
	return Clock::now() + std::chrono::milliseconds( (int64_t)markerDistribution( gen ) );
}
