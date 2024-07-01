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

#include "TestdataGenerator.h"
#include <quApi.h>
#include <iostream>

/**
 * This example repository serves two purposes. The main purpose is to demonstrate
 * how you can use the QuApi to instrument your application. The api supports using native
 * c functions, c++ utilities and macros. How you choose to instrument your application
 * is mostly a personal preference. We'll show everything that's available to you.
 * It is okay to mix and match functions with macros and utilities, so you can use whatever is convenient for you.
 * The other purpose for this example is to implement the example application that is shipped
 * with Qumulus so that in demo mode you've got data you can use to evaluate Qumulus' features.
 *
 * 1. The api has some common setup code that needs to be called when you want instrumentation to be enabled.
 * This setup is done inside this main file.
 * 2. After you've looked at how to set up api usage, you should have a look at the BestPractices.cpp file.
 * This file contains an introduction to the api and teaches you everything you need to know to start
 * instrumenting your own application.
 * 3. When you want to go a bit further with your instrumentation and use advanced features like custom colors
 * or custom activity channels you should have a look at the Advanced.cpp file. Here we demonstrate the full
 * potential of the utility classes and some features you wont find in other profilers.
 * 4. Finally the data that's being generated for Qumulus demo purposes is implemented in TestdataGenerator and ActivityChannelThread.
 */

void QU_CALL_CONV LogHook( quLogSeverity severity, const char* logMessage )
{
	if( severity == QU_LOG_SEVERITY_INFO )
		std::cout << logMessage;
	else
		std::cerr << logMessage;
}

int main( int argc, const char* argv[] )
{
	/**
	 * The library needs to be initialized if you want profiling data to become available to the viewer.
	 * If you dont call this or this function fails then all instrumentation is deactivated and wont cause any overhead.
	 * This way you can ship applications with instrumentation enabled. Alternatively you can limit yourself to
	 * using the macros only and compile the calls out completely.
	 */
	if( !quInitialize( QU_VERSION, &LogHook ) )
	{
		std::cerr << "Starting QuApi example failed, QuApi redistributable may not be installed or is of the incorrect version.." << std::endl;
		return -1;
	}
	//In order to get the profiling data to be visible anywhere we need to set up an output. We can choose either
	//between a file output or a tcp output. The tcp output can be used to monitor application performance live as it is running.
	quSetupTCPOutput( "Qumulus Api Example", true );

	std::cout << "Profiling test data is being generated. Please connect using Qumulus to view it." << std::endl;
	TestdataGenerator generator;
	generator.Run();

	//Call this when you're done using the api to clean up any memory and handles to system resources.
	quRelease();
	return 0;
}
