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

#ifndef _QU_API_HPP_
#define _QU_API_HPP_
#include <string.h> //For strncpy
#include <utility>  //For std::move
#include "quApi.h"

namespace qu
{

class ScopedCounter
{
public:
	ScopedCounter( const char* counterName, bool addImmediately = true ) :
	    ScopedCounter( counterName, 0, addImmediately )
	{
	}
	ScopedCounter( const char* counterName, quUInt32 color, bool addImmediately = true ) :
	    counterID( QU_INVALID_COUNTER_ID ),
	    color( color )
	{
		strncpy( nameBuffer, counterName, QU_MAX_COUNTER_NAME_LENGTH );
		nameBuffer[ QU_MAX_COUNTER_NAME_LENGTH ] = '\0';

		if( addImmediately )
			Add();
	}
	ScopedCounter( ScopedCounter&& movable ) noexcept :
	    ScopedCounter( "", 0, false )
	{
		*this = std::move( movable );
	}
	ScopedCounter& operator=( ScopedCounter&& movable ) noexcept
	{
		Remove();

		std::swap( counterID, movable.counterID );
		memcpy( nameBuffer, movable.nameBuffer, sizeof( nameBuffer ) );
		color = movable.color;
		return *this;
	}
	~ScopedCounter()
	{
		Remove();
	}

	bool Add()
	{
		if( counterID != QU_INVALID_COUNTER_ID )
			return false;

		counterID = quAddCounter( nameBuffer, color );
		return counterID != QU_INVALID_COUNTER_ID;
	}
	void Remove()
	{
		if( counterID == QU_INVALID_COUNTER_ID )
			return;

		quRemoveCounter( counterID );
		counterID = QU_INVALID_COUNTER_ID;
	}

	void SetValue( float newCounterValue ) const
	{
		quSetCounterValue( counterID, newCounterValue );
	}

private:
	ScopedCounter( const ScopedCounter& ) = delete;
	ScopedCounter& operator=( const ScopedCounter& ) = delete;

	quCounterID counterID;
	char nameBuffer[ QU_MAX_COUNTER_NAME_LENGTH + 1 ];
	quUInt32 color;
};
class ScopedActivityChannel
{
public:
	ScopedActivityChannel( const char* channelName, bool forCurrentThread, bool addImmediately = true ) :
	    ScopedActivityChannel( channelName, forCurrentThread, 0, addImmediately )
	{
	}
	ScopedActivityChannel( const char* channelName, bool forCurrentThread, quUInt32 color, bool addImmediately = true ) :
	    activityChannelID( QU_INVALID_ACTIVITY_CHANNEL_ID ),
	    forCurrentThread( forCurrentThread ),
	    color( color )
	{
		strncpy( nameBuffer, channelName, QU_MAX_ACTIVITY_CHANNEL_NAME_LENGTH );
		nameBuffer[ QU_MAX_ACTIVITY_CHANNEL_NAME_LENGTH ] = '\0';

		if( addImmediately )
			Add();
	}
	ScopedActivityChannel( ScopedActivityChannel&& movable ) noexcept :
	    ScopedActivityChannel( "", false, 0, false )
	{
		*this = std::move( movable );
	}
	ScopedActivityChannel& operator=( ScopedActivityChannel&& movable ) noexcept
	{
		Remove();

		std::swap( activityChannelID, movable.activityChannelID );
		memcpy( nameBuffer, movable.nameBuffer, sizeof( nameBuffer ) );
		forCurrentThread = movable.forCurrentThread;
		color = movable.color;
		return *this;
	}
	~ScopedActivityChannel()
	{
		Remove();
	}

	bool Add()
	{
		if( activityChannelID != QU_INVALID_ACTIVITY_CHANNEL_ID )
			return false;

		if( forCurrentThread )
			activityChannelID = quAddActivityChannelForCurrentThread( nameBuffer, color );
		else
			activityChannelID = quAddActivityChannel( nameBuffer, color );

		return activityChannelID != QU_INVALID_ACTIVITY_CHANNEL_ID;
	}
	void Remove()
	{
		if( activityChannelID == QU_INVALID_ACTIVITY_CHANNEL_ID )
			return;

		quRemoveActivityChannel( activityChannelID );
		activityChannelID = QU_INVALID_ACTIVITY_CHANNEL_ID;
	}

	quActivityChannelID GetID() const
	{
		return activityChannelID;
	}

private:
	ScopedActivityChannel( const ScopedActivityChannel& ) = delete;
	ScopedActivityChannel& operator=( const ScopedActivityChannel& ) = delete;

	quActivityChannelID activityChannelID;
	char nameBuffer[ QU_MAX_ACTIVITY_CHANNEL_NAME_LENGTH + 1 ];
	bool forCurrentThread;
	quUInt32 color;
};
class ScopedActivity
{
public:
	ScopedActivity( const char* activityName, quActivityChannelID activityChannelID = quGetChannelIDForCurrentThread() ) :
	    ScopedActivity( activityName, 0, activityChannelID )
	{
	}
	ScopedActivity( const char* activityName, quUInt32 color, quActivityChannelID activityChannelID = quGetChannelIDForCurrentThread() ) :
	    activityChannelID( activityChannelID )
	{
		if( activityChannelID != QU_INVALID_ACTIVITY_CHANNEL_ID )
			activityID = quStartActivity( activityChannelID, activityName, color );
		else
			activityID = QU_INVALID_ACTIVITY_ID;
	}
	ScopedActivity( quRecurringActivityID recurringActivityID, quActivityChannelID activityChannelID = quGetChannelIDForCurrentThread() ) :
	    activityChannelID( activityChannelID )
	{
		if( activityChannelID != QU_INVALID_ACTIVITY_CHANNEL_ID )
			activityID = quStartRecurringActivity( activityChannelID, recurringActivityID );
		else
			activityID = QU_INVALID_ACTIVITY_ID;
	}
	ScopedActivity( ScopedActivity&& movable ) noexcept :
	    activityChannelID( movable.activityChannelID ),
	    activityID( movable.activityID )
	{
		movable.activityID = QU_INVALID_ACTIVITY_ID;
	}
	ScopedActivity& operator=( ScopedActivity&& movable ) noexcept
	{
		EndScope();

		activityChannelID = movable.activityChannelID;
		std::swap( activityID, movable.activityID );
		return *this;
	}
	~ScopedActivity()
	{
		EndScope();
	}

	void Rescope( const char* newActivityName )
	{
		Rescope( newActivityName, 0 );
	}
	void Rescope( const char* newActivityName, quUInt32 color )
	{
		if( activityID != QU_INVALID_ACTIVITY_ID )
			quStopActivity( activityID );
		activityID = quStartActivity( activityChannelID, newActivityName, color );
	}
	void Rescope( quRecurringActivityID recurringActivityID )
	{
		if( activityID != QU_INVALID_ACTIVITY_ID )
			quStopActivity( activityID );
		activityID = quStartRecurringActivity( activityChannelID, recurringActivityID );
	}
	void EndScope()
	{
		if( activityID != QU_INVALID_ACTIVITY_ID )
		{
			quStopActivity( activityID );
			activityID = QU_INVALID_ACTIVITY_ID;
		}
	}

private:
	ScopedActivity( const ScopedActivity& ) = delete;
	ScopedActivity& operator=( const ScopedActivity& ) = delete;

	quActivityChannelID activityChannelID;
	quActivityID activityID;
};

// clang-format off
#if defined( QU_API_ENABLED )
	//Static init
#	define QU_DECLARE_ACTIVITY( varName, activityName ) static quRecurringActivityID varName = ::quAddRecurringActivity( activityName, 0 )
#	define QU_DECLARE_ACTIVITY_COLOR( varName, activityName, color ) static quRecurringActivityID varName = ::quAddRecurringActivity( activityName, color )

	//Constructors
#	define QU_SCOPED_COUNTER( varName, counterName ) qu::ScopedCounter varName = qu::ScopedCounter( counterName )
#	define QU_SCOPED_COUNTER_COLOR( varName, counterName, color ) qu::ScopedCounter varName = qu::ScopedCounter( counterName, color )

#	define QU_SCOPED_ACTIVITY_CHANNEL( varName, channelName, forCurrentThread ) qu::ScopedActivityChannel varName( channelName, forCurrentThread )
#	define QU_SCOPED_ACTIVITY_CHANNEL_COLOR( varName, channelName, forCurrentThread, color ) qu::ScopedActivityChannel varName( channelName, forCurrentThread, color )

#	define QU_SCOPED_ACTIVITY_ONESHOT( varName, activityName ) qu::ScopedActivity varName( activityName )
#	define QU_SCOPED_ACTIVITY_ONESHOT_COLOR( varName, activityName, color ) qu::ScopedActivity varName( activityName, color )
#	define QU_SCOPED_ACTIVITY_RECURRING( varName, activityName ) QU_DECLARE_ACTIVITY( QU_CONCAT( quaid, __LINE__ ), activityName ); qu::ScopedActivity varName( QU_CONCAT( quaid, __LINE__ ) )
#	define QU_SCOPED_ACTIVITY_RECURRING_COLOR( varName, activityName, color ) QU_DECLARE_ACTIVITY_COLOR( QU_CONCAT( quaid, __LINE__ ), activityName, color ); qu::ScopedActivity varName( QU_CONCAT( quaid, __LINE__ ) )

#	define QU_MARKER( markerName ) ::quAddMarker( markerName )

	//Mutators
#	define QU_SET_COUNTER_VALUE( varName, newCounterValue ) varName.SetValue( newCounterValue )

#	define QU_RESCOPE_ONESHOT( varName, newActivityName ) varName.Rescope( newActivityName )
#	define QU_RESCOPE_ONESHOT_COLOR( varName, newActivityName, color ) varName.Rescope( newActivityName, color )
#	define QU_RESCOPE_RECURRING( varName, newActivityID ) varName.Rescope( newActivityID )
#	define QU_END_SCOPE( varName ) varName.EndScope()
#else
	//Static init
#	define QU_DECLARE_ACTIVITY( varName, activityName ) static quRecurringActivityID varName = QU_INVALID_RECURRING_ACTIVITY_ID
#	define QU_DECLARE_ACTIVITY_COLOR( varName, activityName, color ) static quRecurringActivityID varName = QU_INVALID_RECURRING_ACTIVITY_ID

	//Constructors
#	define QU_SCOPED_COUNTER( varName, counterName ) do {} while( false )
#	define QU_SCOPED_COUNTER_COLOR( varName, counterName, color ) do {} while( false )

#	define QU_SCOPED_ACTIVITY_CHANNEL( varName, channelName, forCurrentThread ) do {} while( false )
#	define QU_SCOPED_ACTIVITY_CHANNEL_COLOR( varName, channelName, forCurrentThread, color ) do {} while( false )

#	define QU_SCOPED_ACTIVITY_ONESHOT( varName, activityName ) do {} while( false )
#	define QU_SCOPED_ACTIVITY_ONESHOT_COLOR( varName, activityName, color ) do {} while( false )
#	define QU_SCOPED_ACTIVITY_RECURRING( varName, activityID ) do {} while( false )
#	define QU_SCOPED_ACTIVITY_RECURRING_COLOR( varName, activityName, color ) do {} while( false )

#	define QU_MARKER( markerName ) do {} while( false )

	//Mutators
#	define QU_SET_COUNTER_VALUE( varName, newCounterValue ) do {} while( false )

#	define QU_RESCOPE_ONESHOT( varName, newActivityName ) do {} while( false )
#	define QU_RESCOPE_ONESHOT_COLOR( varName, newActivityName, color ) do {} while( false )
#	define QU_RESCOPE_RECURRING( varName, newActivityID ) do {} while( false )
#	define QU_END_SCOPE( varName ) do {} while( false )
#endif

//Automatic varName generation
#define QU_CAT(a, b) a ## b
#define QU_CONCAT( a, b ) QU_CAT( a, b )

#define QU_ONESHOT_ACTIVITY( activityName ) QU_SCOPED_ACTIVITY_ONESHOT( QU_CONCAT( qusaid, __LINE__ ), activityName )
#define QU_ONESHOT_ACTIVITY_COLOR( activityName, color ) QU_SCOPED_ACTIVITY_ONESHOT( QU_CONCAT( qusaid, __LINE__ ), activityName, color )
#define QU_RECURRING_ACTIVITY( activityName ) QU_SCOPED_ACTIVITY_RECURRING( QU_CONCAT( qusaid, __LINE__ ), activityName )
#define QU_RECURRING_ACTIVITY_COLOR( activityName, color ) QU_SCOPED_ACTIVITY_RECURRING_COLOR( QU_CONCAT( qusaid, __LINE__ ), activityName, color )
#define QU_RECURRING_RESCOPE( varName, activityName ) QU_DECLARE_ACTIVITY( QU_CONCAT( quaid, __LINE__ ), activityName ); QU_RESCOPE_RECURRING( varName, QU_CONCAT( quaid, __LINE__ ) )

#if defined( _WIN64 )
#	define QU_INSTRUMENT_FUNCTION() QU_RECURRING_ACTIVITY( __FUNCTION__ )
#else
#	define QU_INSTRUMENT_FUNCTION() QU_RECURRING_ACTIVITY( __PRETTY_FUNCTION__ )
#endif
// clang-format on

} //End namespace qu

#endif
