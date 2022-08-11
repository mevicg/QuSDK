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

#ifndef _QU_API_H_
#define _QU_API_H_
#pragma once
#include "quConstants.h"

#if defined( QU_API_ENABLED )
#	define QU_INLINE_IF_DISABLED extern
#	define QU_RETURN_IF_DISABLED( value ) ;
#else
#	define QU_INLINE_IF_DISABLED inline
#	define QU_RETURN_IF_DISABLED( value ) { return value; }
#endif

//QuApi core
typedef QU_IMPORT bool( QU_CALL_CONV* quInitialize_Ptr )( quUInt32 version );
QU_INLINE_IF_DISABLED bool QU_CALL_CONV quInitialize( quUInt32 version ) QU_RETURN_IF_DISABLED( false )
typedef QU_IMPORT void( QU_CALL_CONV* quRelease_Ptr )();
QU_INLINE_IF_DISABLED void QU_CALL_CONV quRelease() QU_RETURN_IF_DISABLED( void() );

//Outputs
typedef QU_IMPORT quOutputID( QU_CALL_CONV* quSetupGoogleTraceOutput_Ptr )( const char* outputFile, bool startImmediately );
QU_INLINE_IF_DISABLED quOutputID QU_CALL_CONV quSetupGoogleTraceOutput( const char* outputFile, bool startImmediately ) QU_RETURN_IF_DISABLED( QU_INVALID_OUTPUT_ID )
typedef QU_IMPORT quOutputID( QU_CALL_CONV* quSetupTCPOutput_Ptr )( const char* appName, bool startImmediately );
QU_INLINE_IF_DISABLED quOutputID QU_CALL_CONV quSetupTCPOutput( const char* appName, bool startImmediately ) QU_RETURN_IF_DISABLED( QU_INVALID_OUTPUT_ID )
typedef QU_IMPORT bool( QU_CALL_CONV* quStartOutput_Ptr )( quOutputID outputID );
QU_INLINE_IF_DISABLED bool QU_CALL_CONV quStartOutput( quOutputID outputID ) QU_RETURN_IF_DISABLED( false )
typedef QU_IMPORT bool( QU_CALL_CONV* quStopOutput_Ptr )( quOutputID outputID );
QU_INLINE_IF_DISABLED bool QU_CALL_CONV quStopOutput( quOutputID outputID ) QU_RETURN_IF_DISABLED( false )
typedef QU_IMPORT bool( QU_CALL_CONV* quStartAllOutputs_Ptr )();
QU_INLINE_IF_DISABLED bool QU_CALL_CONV quStartAllOutputs() QU_RETURN_IF_DISABLED( false )
typedef QU_IMPORT bool( QU_CALL_CONV* quStopAllOutputs_Ptr )();
QU_INLINE_IF_DISABLED bool QU_CALL_CONV quStopAllOutputs() QU_RETURN_IF_DISABLED( false )
typedef QU_IMPORT bool( QU_CALL_CONV* quRemoveOutput_Ptr )( quOutputID outputID );
QU_INLINE_IF_DISABLED bool QU_CALL_CONV quRemoveOutput( quOutputID outputID ) QU_RETURN_IF_DISABLED( false )

//Counters
typedef QU_IMPORT quCounterID( QU_CALL_CONV* quAddCounter_Ptr )( const char* counterName, quUInt32 color );
QU_INLINE_IF_DISABLED quCounterID QU_CALL_CONV quAddCounter( const char* counterName, quUInt32 color ) QU_RETURN_IF_DISABLED( QU_INVALID_COUNTER_ID )
typedef QU_IMPORT bool( QU_CALL_CONV* quSetCounterValue_Ptr )( quCounterID counterID, float newCounterValue );
QU_INLINE_IF_DISABLED bool QU_CALL_CONV quSetCounterValue( quCounterID counterID, float newCounterValue ) QU_RETURN_IF_DISABLED( false )
typedef QU_IMPORT bool( QU_CALL_CONV* quRemoveCounter_Ptr )( quCounterID counterID );
QU_INLINE_IF_DISABLED bool QU_CALL_CONV quRemoveCounter( quCounterID counterID ) QU_RETURN_IF_DISABLED( false )

//Activity channels
typedef QU_IMPORT quActivityChannelID( QU_CALL_CONV* quAddActivityChannel_Ptr )( const char* channelName, quUInt32 color );
QU_INLINE_IF_DISABLED quActivityChannelID QU_CALL_CONV quAddActivityChannel( const char* channelName, quUInt32 color ) QU_RETURN_IF_DISABLED( QU_INVALID_ACTIVITY_CHANNEL_ID )
typedef QU_IMPORT quActivityChannelID( QU_CALL_CONV* quAddActivityChannelForCurrentThread_Ptr )( const char* channelName, quUInt32 color );
QU_INLINE_IF_DISABLED quActivityChannelID QU_CALL_CONV quAddActivityChannelForCurrentThread( const char* channelName, quUInt32 color ) QU_RETURN_IF_DISABLED( QU_INVALID_ACTIVITY_CHANNEL_ID )
typedef QU_IMPORT quActivityChannelID( QU_CALL_CONV* quGetChannelIDForCurrentThread_Ptr )();
QU_INLINE_IF_DISABLED quActivityChannelID QU_CALL_CONV quGetChannelIDForCurrentThread() QU_RETURN_IF_DISABLED( QU_INVALID_ACTIVITY_CHANNEL_ID )
typedef QU_IMPORT quRecurringActivityID( QU_CALL_CONV* quAddRecurringActivity_Ptr )( const char* activityName, quUInt32 color );
QU_INLINE_IF_DISABLED quRecurringActivityID QU_CALL_CONV quAddRecurringActivity( const char* activityName, quUInt32 color ) QU_RETURN_IF_DISABLED( QU_INVALID_RECURRING_ACTIVITY_ID )
typedef QU_IMPORT quActivityID( QU_CALL_CONV* quStartRecurringActivity_Ptr )( quActivityChannelID channelID, quRecurringActivityID activityID );
QU_INLINE_IF_DISABLED quActivityID QU_CALL_CONV quStartRecurringActivity( quActivityChannelID channelID, quRecurringActivityID activityID ) QU_RETURN_IF_DISABLED( QU_INVALID_ACTIVITY_ID )
typedef QU_IMPORT quActivityID( QU_CALL_CONV* quStartActivity_Ptr )( quActivityChannelID channelID, const char* activityName, quUInt32 color );
QU_INLINE_IF_DISABLED quActivityID QU_CALL_CONV quStartActivity( quActivityChannelID channelID, const char* activityName, quUInt32 color ) QU_RETURN_IF_DISABLED( QU_INVALID_ACTIVITY_ID )
typedef QU_IMPORT bool( QU_CALL_CONV* quStopActivity_Ptr )( quActivityID activityID );
QU_INLINE_IF_DISABLED bool QU_CALL_CONV quStopActivity( quActivityID activityID ) QU_RETURN_IF_DISABLED( false )
typedef QU_IMPORT bool( QU_CALL_CONV* quRemoveActivityChannel_Ptr )( quActivityChannelID channelID );
//TODO: Implement flow support
inline quFlowID QU_CALL_CONV quStartFlow( quActivityChannelID sourceChannel, const char* flowName ){ return QU_INVALID_FLOW_ID; }
inline bool QU_CALL_CONV quStopFlow( quActivityChannelID targetChannel, quFlowID flowID ){ return false; }
QU_INLINE_IF_DISABLED bool QU_CALL_CONV quRemoveActivityChannel( quActivityChannelID channelID ) QU_RETURN_IF_DISABLED( false )

//Markers
typedef QU_IMPORT void( QU_CALL_CONV* quAddMarker_Ptr )( const char* markerName );
QU_INLINE_IF_DISABLED void QU_CALL_CONV quAddMarker( const char* markerName ) QU_RETURN_IF_DISABLED( void() )

#endif
