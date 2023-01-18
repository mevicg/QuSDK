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
	    ScopedCounter( counterName, 0, 0, 0, addImmediately )
	{
	}
	ScopedCounter( const char* counterName, quUInt8 r, quUInt8 g, quUInt8 b, bool addImmediately = true ) :
	    counterID( QU_INVALID_COUNTER_ID ),
	    color( QU_RGB( r, g, b ) )
	{
		strncpy( nameBuffer, counterName, QU_MAX_COUNTER_NAME_LENGTH );
		nameBuffer[ QU_MAX_COUNTER_NAME_LENGTH ] = '\0';

		if( addImmediately )
			Add();
	}
	ScopedCounter( ScopedCounter&& movable ) noexcept :
	    ScopedCounter( "", 0, 0, 0, false )
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
	    ScopedActivityChannel( channelName, forCurrentThread, 0, 0, 0, addImmediately )
	{
	}
	ScopedActivityChannel( const char* channelName, bool forCurrentThread, quUInt8 r, quUInt8 g, quUInt8 b, bool addImmediately = true ) :
	    activityChannelID( QU_INVALID_ACTIVITY_CHANNEL_ID ),
	    forCurrentThread( forCurrentThread ),
	    color( QU_RGB( r, g, b ) )
	{
		strncpy( nameBuffer, channelName, QU_MAX_ACTIVITY_CHANNEL_NAME_LENGTH );
		nameBuffer[ QU_MAX_ACTIVITY_CHANNEL_NAME_LENGTH ] = '\0';

		if( addImmediately )
			Add();
	}
	ScopedActivityChannel( ScopedActivityChannel&& movable ) noexcept :
	    ScopedActivityChannel( "", false, 0, 0, 0, false )
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
	ScopedActivity( quRecurringActivityID recurringActivityID, quActivityChannelID activityChannelID = quGetChannelIDForCurrentThread() ) :
	    activityChannelID( activityChannelID ),
	    activityID( quStartRecurringActivity( activityChannelID, recurringActivityID ) )
	{
	}
	ScopedActivity( const char* activityName, quActivityChannelID activityChannelID = quGetChannelIDForCurrentThread() ) :
	    ScopedActivity( activityName, 0, 0, 0, activityChannelID )
	{
	}
	ScopedActivity( const char* activityName, quUInt8 r, quUInt8 g, quUInt8 b, quActivityChannelID activityChannelID = quGetChannelIDForCurrentThread() ) :
	    activityChannelID( activityChannelID ),
	    activityID( quStartActivity( activityChannelID, activityName, QU_RGB( r, g, b ) ) )
	{
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

	void Rescope( quRecurringActivityID recurringActivityID )
	{
		if( activityID != QU_INVALID_ACTIVITY_ID )
			quStopActivity( activityID );
		activityID = quStartRecurringActivity( activityChannelID, recurringActivityID );
	}
	void Rescope( const char* newActivityName )
	{
		Rescope( newActivityName, 0, 0, 0 );
	}
	void Rescope( const char* newActivityName, quUInt8 r, quUInt8 g, quUInt8 b )
	{
		if( activityID != QU_INVALID_ACTIVITY_ID )
			quStopActivity( activityID );
		activityID = quStartActivity( activityChannelID, newActivityName, QU_RGB( r, g, b ) );
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
#	define QU_SCOPED_COUNTER( varName, counterName ) qu::ScopedCounter varName( counterName )
#	define QU_SCOPED_COUNTER_NOADD( varName, counterName ) qu::ScopedCounter varName( counterName, false )

#	define QU_SCOPED_ACTIVITY_CHANNEL( varName, channelName, forCurrentThread ) qu::ScopedActivityChannel varName( channelName, forCurrentThread )
#	define QU_SCOPED_ACTIVITY_CHANNEL_NOADD( varName, channelName, forCurrentThread ) qu::ScopedActivityChannel varName( channelName, forCurrentThread, false )

#	define QU_DECLARE_ACTIVITY( IDName, activityName ) static quRecurringActivityID IDName = ::quAddRecurringActivity( activityName, 0 )
#	define QU_DECLARE_ACTIVITY_CLR( IDName, activityName, clr ) static quRecurringActivityID IDName = ::quAddRecurringActivity( activityName, clr )
#	define QU_SCOPED_ACTIVITY( IDName, varName ) qu::ScopedActivity varName( IDName )
#	define QU_SCOPED_ACTIVITY_FOR_CHANNEL( IDName, varName, channelID ) qu::ScopedActivity varName( IDName, channelID )
#	define QU_DECLARE_SCOPED_ACTIVITY( IDName, varName, activityName ) QU_DECLARE_ACTIVITY( IDName, activityName ); QU_SCOPED_ACTIVITY( IDName, varName )
#	define QU_DECLARE_SCOPED_ACTIVITY_CLR( IDName, varName, activityName, clr ) QU_DECLARE_ACTIVITY_CLR( IDName, activityName, clr ); QU_SCOPED_ACTIVITY( IDName, varName )
#	define QU_DECLARE_SCOPED_ACTIVITY_FOR_CHANNEL( IDName, varName, activityName, channelID ) QU_DECLARE_ACTIVITY( IDName, activityName ); QU_SCOPED_ACTIVITY_FOR_CHANNEL( IDName, varName, channelID )
#	define QU_DECLARE_SCOPED_ACTIVITY_CLR_FOR_CHANNEL( IDName, varName, activityName, clr, channelID ) QU_DECLARE_ACTIVITY_CLR( IDName, activityName, clr ); QU_SCOPED_ACTIVITY_FOR_CHANNEL( IDName, varName, channelID )
#	define QU_RESCOPE( varName, newIDName ) varName.Rescope( newIDName )
#	define QU_DECLARE_RESCOPE( IDName, varName, activityName ) QU_DECLARE_ACTIVITY( IDName, activityName ); QU_RESCOPE( varName, IDName )
#	define QU_DECLARE_RESCOPE_CLR( IDName, varName, activityName, clr ) QU_DECLARE_ACTIVITY_CLR( IDName, activityName, clr ); QU_RESCOPE( varName, IDName )
#	define QU_END_SCOPE( varName ) varName.EndScope()

#	define QU_SCOPED_ACTIVITY_NON_RECURRING( varName, activityName ) qu::ScopedActivity varName( activityName )
#	define QU_SCOPED_ACTIVITY_CLR_NON_RECURRING( varName, activityName, clr ) qu::ScopedActivity varName( activityName, QU_R( clr ), QU_G( clr ), QU_B( clr ) )
#else
#	define QU_SCOPED_COUNTER( varName, counterName ) do {} while( false )
#	define QU_SCOPED_COUNTER_NOADD( varName, counterName ) do {} while( false )

#	define QU_SCOPED_ACTIVITY_CHANNEL( varName, channelName, forCurrentThread ) do {} while( false )
#	define QU_SCOPED_ACTIVITY_CHANNEL_NOADD( varName, channelName, forCurrentThread ) do {} while( false )

#	define QU_DECLARE_ACTIVITY( IDName, activityName ) do {} while( false )
#	define QU_DECLARE_ACTIVITY_CLR( IDName, activityName, clr ) do {} while( false )
#	define QU_SCOPED_ACTIVITY( IDName, varName ) do {} while( false )
#	define QU_SCOPED_ACTIVITY_FOR_CHANNEL( IDName, varName, channelID ) do {} while( false )
#	define QU_DECLARE_SCOPED_ACTIVITY( IDName, varName, activityName ) do {} while( false )
#	define QU_DECLARE_SCOPED_ACTIVITY_CLR( IDName, varName, activityName, clr ) do {} while( false )
#	define QU_DECLARE_SCOPED_ACTIVITY_FOR_CHANNEL( IDName, varName, activityName, channelID ) do {} while( false )
#	define QU_DECLARE_SCOPED_ACTIVITY_CLR_FOR_CHANNEL( IDName, varName, activityName, clr, channelID ) do {} while( false )
#	define QU_RESCOPE( varName, newIDName ) do {} while( false )
#	define QU_DECLARE_RESCOPE( IDName, varName, activityName ) do {} while( false )
#	define QU_DECLARE_RESCOPE_CLR( IDName, varName, activityName, clr ) do {} while( false )
#	define QU_END_SCOPE( varName ) do {} while( false )

#	define QU_SCOPED_ACTIVITY_NON_RECURRING( varName, activityName ) do {} while( false )
#	define QU_SCOPED_ACTIVITY_CLR_NON_RECURRING( varName, activityName, clr ) do {} while( false )
#endif

#define QU_CAT(a, b) a ## b
#define QU_CONCAT( a, b ) QU_CAT( a, b )
#define QU_ONESHOT_ACTIVITY( activityName ) QU_SCOPED_ACTIVITY( activityName, QU_CONCAT( qusa, __LINE__ ) )
#define QU_RECURRING_ACTIVITY( activityName ) QU_DECLARE_SCOPED_ACTIVITY( QU_CONCAT( quai, __LINE__ ), QU_CONCAT( qusa, __LINE__ ), activityName )
#if defined( _WIN64 )
#	define QU_INSTRUMENT_FUNCTION() QU_RECURRING_ACTIVITY( __FUNCTION__ )
#else
#	define QU_INSTRUMENT_FUNCTION() QU_RECURRING_ACTIVITY( __PRETTY_FUNCTION__ )
#endif
// clang-format on

} //End namespace qu

#endif
