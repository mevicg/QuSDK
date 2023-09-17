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

#include <quApi.h>
#include <string>
#include <fstream>
#include <cstring>
#include "quLoaderDylib.h"
#include "quLoaderEnvVar.h"
#if defined( __APPLE__ )
#	if !defined( __OBJC__ )
static_assert( false, "This file must be compiled as Objective-C++." );
#	endif
#	import <Cocoa/Cocoa.h>
#endif

namespace qu
{

//QuApi core
static quInitialize_Ptr Initialize = nullptr;
static quRelease_Ptr Release = nullptr;

//Outputs
static quSetupGoogleTraceOutput_Ptr SetupGoogleTraceOutput = nullptr;
static quSetupTCPOutput_Ptr SetupTCPOutput = nullptr;
static quStartOutput_Ptr StartOutput = nullptr;
static quStopOutput_Ptr StopOutput = nullptr;
static quStartAllOutputs_Ptr StartAllOutputs = nullptr;
static quStopAllOutputs_Ptr StopAllOutputs = nullptr;
static quRemoveOutput_Ptr RemoveOutput = nullptr;

//Counters
static quAddCounter_Ptr AddCounter = nullptr;
static quSetCounterValue_Ptr SetCounterValue = nullptr;
static quRemoveCounter_Ptr RemoveCounter = nullptr;

//Activity channels
static quAddActivityChannel_Ptr AddActivityChannel = nullptr;
static quAddActivityChannelForCurrentThread_Ptr AddActivityChannelForCurrentThread = nullptr;
static quGetChannelIDForCurrentThread_Ptr GetChannelIDForCurrentThread = nullptr;
static quAddRecurringActivity_Ptr AddRecurringActivity = nullptr;
static quStartRecurringActivity_Ptr StartRecurringActivity = nullptr;
static quStartActivity_Ptr StartActivity = nullptr;
static quStopActivity_Ptr StopActivity = nullptr;
static quRemoveActivityChannel_Ptr RemoveActivityChannel = nullptr;

//Markers
static quAddMarker_Ptr AddMarker = nullptr;

} //End namespace qu

namespace qul
{

static Dylib library;

static void UnloadQuApi();
static bool LoadQuApi()
{
	//It's possible that the application tries to load the dll multiple times. This might be because it just wants to
	//ensure the library has been loaded before it's using it. If we've already loaded the library we dont have to do it again.
	if( library.IsLoaded() )
		return true;

	/**
	 * For QuApi development it's needed to load the library version that was just compiled. For that purpose we support
	 * setting up environment variables to redirect which library is loaded. These environment variables have to be set
	 * on QuApi development machines. Users of this loader will not have these variables set, and will thus load the QuApi
	 * from the system libraries / %path% environment variable. This will make them use the QuApi library that is provided
	 * by the installed Qumulus application.
	 */
	std::string envVarName;
#if defined( _DEBUG ) || defined( DEBUG )
	envVarName = "QU_API_DEBUG_DLL";
#else
	envVarName = "QU_API_RELEASE_DLL";
#endif

#if defined( _WIN64 )
	//Getting environment variable values might modify the character buffer, so we assign the default library name
	//only after getting the environment variable failed.
	std::string libName( 0xFFFF, char( 0 ) );
	if( !EnvVar::GetValue( envVarName.c_str(), libName.data(), libName.size() ) )
		libName = "QuApi.dll";
#else
	//Xcode doesn't pass through the user's environment variables, so we have to manually
	//parse the .bash_profile file to find the environment variable's value.
	std::string bashProfilePath = std::string( [NSHomeDirectory() UTF8String] ) + "/.bash_profile";
	std::ifstream istream;
	istream.open( bashProfilePath.c_str(), std::ifstream::in );
	std::string bashProfileContents;
	char lineBuffer[ 0xFFFF ];
	std::string libName = "/Applications/Qumulus/Contents/MacOS/libquapi.dylib";
	while( istream.good() )
	{
		memset( lineBuffer, 0, sizeof( lineBuffer ) );
		istream.getline( lineBuffer, sizeof( lineBuffer ) );
		if( const char* pos = strstr( lineBuffer, "#" ); pos == lineBuffer )
			continue; //Skip commented lines

		if( const char* pos = strstr( lineBuffer, envVarName.c_str() ) )
		{
			libName = pos + envVarName.length() + 1;
			break;
		}
	}
	istream.close();
#endif

	/**
	 * If loading the library failed that must mean this QuApi user did not install the QuApi runtime and thus
	 * profiling is not supported. This is fine, it allows keeping intstrumentation enabled even in release builds
	 * without introducing any overhead.
	 */
	if( !library.Load( libName.c_str() ) )
		return false;

	bool gotAllFunctions = true;

	//QuApi core
	gotAllFunctions &= ( qu::Initialize = (quInitialize_Ptr)library.GetFunction( "quInitialize" ) ) != nullptr;
	gotAllFunctions &= ( qu::Release = (quRelease_Ptr)library.GetFunction( "quRelease" ) ) != nullptr;

	//Outputs
	gotAllFunctions &= ( qu::SetupGoogleTraceOutput = (quSetupGoogleTraceOutput_Ptr)library.GetFunction( "quSetupGoogleTraceOutput" ) ) != nullptr;
	gotAllFunctions &= ( qu::SetupTCPOutput = (quSetupTCPOutput_Ptr)library.GetFunction( "quSetupTCPOutput" ) ) != nullptr;
	gotAllFunctions &= ( qu::StartOutput = (quStartOutput_Ptr)library.GetFunction( "quStartOutput" ) ) != nullptr;
	gotAllFunctions &= ( qu::StopOutput = (quStopOutput_Ptr)library.GetFunction( "quStopOutput" ) ) != nullptr;
	gotAllFunctions &= ( qu::StartAllOutputs = (quStartAllOutputs_Ptr)library.GetFunction( "quStartAllOutputs" ) ) != nullptr;
	gotAllFunctions &= ( qu::StopAllOutputs = (quStopAllOutputs_Ptr)library.GetFunction( "quStopAllOutputs" ) ) != nullptr;
	gotAllFunctions &= ( qu::RemoveOutput = (quRemoveOutput_Ptr)library.GetFunction( "quRemoveOutput" ) ) != nullptr;

	//Counters
	gotAllFunctions &= ( qu::AddCounter = (quAddCounter_Ptr)library.GetFunction( "quAddCounter" ) ) != nullptr;
	gotAllFunctions &= ( qu::SetCounterValue = (quSetCounterValue_Ptr)library.GetFunction( "quSetCounterValue" ) ) != nullptr;
	gotAllFunctions &= ( qu::RemoveCounter = (quRemoveCounter_Ptr)library.GetFunction( "quRemoveCounter" ) ) != nullptr;

	//Activity channels
	gotAllFunctions &= ( qu::AddActivityChannel = (quAddActivityChannel_Ptr)library.GetFunction( "quAddActivityChannel" ) ) != nullptr;
	gotAllFunctions &= ( qu::AddActivityChannelForCurrentThread = (quAddActivityChannelForCurrentThread_Ptr)library.GetFunction( "quAddActivityChannelForCurrentThread" ) ) != nullptr;
	gotAllFunctions &= ( qu::GetChannelIDForCurrentThread = (quGetChannelIDForCurrentThread_Ptr)library.GetFunction( "quGetChannelIDForCurrentThread" ) ) != nullptr;
	gotAllFunctions &= ( qu::AddRecurringActivity = (quAddRecurringActivity_Ptr)library.GetFunction( "quAddRecurringActivity" ) ) != nullptr;
	gotAllFunctions &= ( qu::StartRecurringActivity = (quStartRecurringActivity_Ptr)library.GetFunction( "quStartRecurringActivity" ) ) != nullptr;
	gotAllFunctions &= ( qu::StartActivity = (quStartActivity_Ptr)library.GetFunction( "quStartActivity" ) ) != nullptr;
	gotAllFunctions &= ( qu::StopActivity = (quStopActivity_Ptr)library.GetFunction( "quStopActivity" ) ) != nullptr;
	gotAllFunctions &= ( qu::RemoveActivityChannel = (quRemoveActivityChannel_Ptr)library.GetFunction( "quRemoveActivityChannel" ) ) != nullptr;

	//Markers
	gotAllFunctions &= ( qu::AddMarker = (quAddMarker_Ptr)library.GetFunction( "quAddMarker" ) ) != nullptr;

	if( !gotAllFunctions )
	{
		UnloadQuApi();
		return false;
	}
	return true;
}
void UnloadQuApi()
{
	//QuApi core
	qu::Initialize = nullptr;
	qu::Release = nullptr;

	//Outputs
	qu::SetupGoogleTraceOutput = nullptr;
	qu::SetupTCPOutput = nullptr;
	qu::StartOutput = nullptr;
	qu::StopOutput = nullptr;
	qu::StartAllOutputs = nullptr;
	qu::StopAllOutputs = nullptr;
	qu::RemoveOutput = nullptr;

	//Counters
	qu::AddCounter = nullptr;
	qu::SetCounterValue = nullptr;
	qu::RemoveCounter = nullptr;

	//Activity channels
	qu::AddActivityChannel = nullptr;
	qu::AddActivityChannelForCurrentThread = nullptr;
	qu::GetChannelIDForCurrentThread = nullptr;
	qu::AddRecurringActivity = nullptr;
	qu::StartRecurringActivity = nullptr;
	qu::StartActivity = nullptr;
	qu::StopActivity = nullptr;
	qu::RemoveActivityChannel = nullptr;

	//Markers
	qu::AddMarker = nullptr;

	library.Unload();
}

} //End namespace qul

//QuApi core
bool QU_CALL_CONV quInitialize( quUInt32 version )
{
	//It's possible for the application to just try to initialize before explicitly loading the dll. To support this case we
	//automatically try to load the library here.
	if( qu::Initialize == nullptr && !qul::LoadQuApi() )
		return false;

	if( qu::Initialize != nullptr )
		return qu::Initialize( version );
	else
		return false;
}
void QU_CALL_CONV quRelease()
{
	if( qu::Release != nullptr )
		qu::Release();
	qul::UnloadQuApi();
}

//Outputs
quOutputID QU_CALL_CONV quSetupGoogleTraceOutput( const char* outputFile, bool startImmediately )
{
	if( qu::SetupGoogleTraceOutput == nullptr )
		return QU_INVALID_OUTPUT_ID;
	else
		return qu::SetupGoogleTraceOutput( outputFile, startImmediately );
}
quOutputID QU_CALL_CONV quSetupTCPOutput( const char* appName, bool startImmediately )
{
	if( qu::SetupTCPOutput == nullptr )
		return QU_INVALID_OUTPUT_ID;
	else
		return qu::SetupTCPOutput( appName, startImmediately );
}
bool QU_CALL_CONV quStartOutput( quOutputID outputID )
{
	if( qu::StartOutput == nullptr )
		return false;
	else
		return qu::StartOutput( outputID );
}
bool QU_CALL_CONV quStopOutput( quOutputID outputID )
{
	if( qu::StopOutput == nullptr )
		return false;
	else
		return qu::StopOutput( outputID );
}
bool QU_CALL_CONV quStartAllOutputs()
{
	if( qu::StartAllOutputs == nullptr )
		return false;
	else
		return qu::StartAllOutputs();
}
bool QU_CALL_CONV quStopAllOutputs()
{
	if( qu::StopAllOutputs == nullptr )
		return false;
	else
		return qu::StopAllOutputs();
}
bool QU_CALL_CONV quRemoveOutput( quOutputID outputID )
{
	if( qu::RemoveOutput == nullptr )
		return false;
	else
		return qu::RemoveOutput( outputID );
}

//Counters
quCounterID QU_CALL_CONV quAddCounter( const char* counterName, quUInt32 color )
{
	if( qu::AddCounter == nullptr )
		return QU_INVALID_COUNTER_ID;
	else
		return qu::AddCounter( counterName, color );
}
bool QU_CALL_CONV quSetCounterValue( quCounterID counterID, float newCounterValue )
{
	if( qu::SetCounterValue == nullptr )
		return false;
	else
		return qu::SetCounterValue( counterID, newCounterValue );
}
bool QU_CALL_CONV quRemoveCounter( quCounterID counterID )
{
	if( qu::RemoveCounter == nullptr )
		return false;
	else
		return qu::RemoveCounter( counterID );
}

//Activity channels
quActivityChannelID QU_CALL_CONV quAddActivityChannel( const char* channelName, quUInt32 color )
{
	if( qu::AddActivityChannel == nullptr )
		return QU_INVALID_ACTIVITY_CHANNEL_ID;
	else
		return qu::AddActivityChannel( channelName, color );
}
quActivityChannelID QU_CALL_CONV quAddActivityChannelForCurrentThread( const char* channelName, quUInt32 color )
{
	if( qu::AddActivityChannelForCurrentThread == nullptr )
		return QU_INVALID_ACTIVITY_CHANNEL_ID;
	else
		return qu::AddActivityChannelForCurrentThread( channelName, color );
}
quActivityChannelID QU_CALL_CONV quGetChannelIDForCurrentThread()
{
	if( qu::GetChannelIDForCurrentThread == nullptr )
		return QU_INVALID_ACTIVITY_CHANNEL_ID;
	else
		return qu::GetChannelIDForCurrentThread();
}
quRecurringActivityID QU_CALL_CONV quAddRecurringActivity( const char* activityName, quUInt32 color )
{
	//This function is most likely called before QuApi is even loaded as the recurring activity id's are most likely stored in static memory.
	//For this reason we try to load the library once here so that the recurring activity data may be stored right away.
	if( qu::AddRecurringActivity == nullptr )
	{
		static bool loadTriedAndFailed = false;
		//If we've already failed to load the library we wont try again, probably the Qumulus application just isn't installed.
		if( loadTriedAndFailed )
			return QU_INVALID_RECURRING_ACTIVITY_ID;
		if( !qul::LoadQuApi() )
		{
			loadTriedAndFailed = true;
			return QU_INVALID_RECURRING_ACTIVITY_ID;
		}
	}

	if( qu::AddRecurringActivity != nullptr )
		return qu::AddRecurringActivity( activityName, color );
	else
		return QU_INVALID_RECURRING_ACTIVITY_ID;
}
quActivityID QU_CALL_CONV quStartRecurringActivity( quActivityChannelID channelID, quRecurringActivityID activityID )
{
	if( qu::StartRecurringActivity == nullptr )
		return QU_INVALID_ACTIVITY_ID;
	else
		return qu::StartRecurringActivity( channelID, activityID );
}
quActivityID QU_CALL_CONV quStartActivity( quActivityChannelID channelID, const char* activityName, quUInt32 color )
{
	if( qu::StartActivity == nullptr )
		return QU_INVALID_ACTIVITY_ID;
	else
		return qu::StartActivity( channelID, activityName, color );
}
bool QU_CALL_CONV quStopActivity( quActivityID activityID )
{
	if( qu::StopActivity == nullptr )
		return false;
	else
		return qu::StopActivity( activityID );
}
bool QU_CALL_CONV quRemoveActivityChannel( quActivityChannelID channelID )
{
	if( qu::RemoveActivityChannel == nullptr )
		return false;
	else
		return qu::RemoveActivityChannel( channelID );
}

//Markers
void QU_CALL_CONV quAddMarker( const char* markerName )
{
	if( qu::AddMarker != nullptr )
		return qu::AddMarker( markerName );
}
