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

#ifndef _QU_CONSTANTS_H_
#define _QU_CONSTANTS_H_
#pragma once

#if defined( _WIN64 )
#	define QU_CALL_CONV __cdecl
#elif defined( __APPLE__ )
#	include <cstddef> //For size_t
#	define QU_CALL_CONV
#else
//Any platform can be supported as long as we're not enabled. When we're disabled all our
//functions are implemented as inline functions returning failure inside the header.
#	if defined( QU_API_ENABLED )
#		error "Unsupported Platform"
#	else
//Make it compile with the no-op functions at least.
#		define QU_CALL_CONV
#	endif
#endif

typedef unsigned long long quUInt64;
typedef unsigned int quUInt32;
typedef unsigned short quUInt16;
typedef unsigned char quUInt8;

typedef long long quInt64;
typedef int quInt32;
typedef short quInt16;
typedef char quInt8;

//QuApi core
#define QU_MAKE_VERSION( major, minor, micro ) ( ( major << 24 ) | ( minor << 16 ) | micro )
#define QU_VERSION QU_MAKE_VERSION( 1, 0, 0 )
#define QU_EXTRACT_MAJOR( version ) ( version >> 24 )
#define QU_EXTRACT_MINOR( version ) ( ( version >> 16 ) & 0xFF )
#define QU_EXTRACT_MICRO( version ) ( version & 0xFFFF )

#define QU_RGB( r, g, b ) ( ( ( (quUInt32)r & 0xFF ) << 0 ) | ( ( (quUInt32)g & 0xFF ) << 8 ) | ( ( (quUInt32)b & 0xFF ) << 16 ) )
#define QU_R( rgb ) quUInt8( rgb >> 0 )
#define QU_G( rgb ) quUInt8( rgb >> 8 )
#define QU_B( rgb ) quUInt8( rgb >> 16 )

#define QU_CLR_ERROR QU_RGB( 255, 0, 0 )
#define QU_CLR_WARNING QU_RGB( 255, 255, 0 )
#define QU_CLR_GOOD QU_RGB( 0, 255, 0 )
#define QU_CLR_BLOCKED QU_RGB( 255, 69, 0 )
#define QU_CLR_IDLE QU_RGB( 150, 210, 230 )

//Outputs
#define QU_MAX_PATH_LENGTH 260    //Maximum length of file names including their path passed in when creating outputs.
#define QU_MAX_APP_NAME_LENGTH 64 //Maximum length for an application making it's instrumentation available over the network.
typedef quUInt16 quOutputID;
#define QU_INVALID_OUTPUT_ID ( (quOutputID)-1 )

//Counters
#define QU_MAX_COUNTER_NAME_LENGTH 63 //Maximum length of Counter names not including the nul character.
typedef quUInt16 quCounterID;
#define QU_INVALID_COUNTER_ID ( (quCounterID)-1 )

//Activity channels
#define QU_MAX_ACTIVITY_CHANNEL_NAME_LENGTH 63 //Maximum length of ActivityChannel names not including the nul character.
typedef quUInt16 quActivityChannelID;
#define QU_INVALID_ACTIVITY_CHANNEL_ID ( (quActivityChannelID)-1 )
typedef quUInt32 quRecurringActivityID;
#define QU_INVALID_RECURRING_ACTIVITY_ID ( (quRecurringActivityID)-1 )
#define QU_MAX_RECURRING_ACTIVITY_NAME_LENGTH 63 //Maximum length of Recurring Activity names not including the nul character.
typedef quUInt64 quActivityID;
#define QU_INVALID_ACTIVITY_ID ( (quActivityID)-1 )
#define QU_MAX_ACTIVITY_NAME_LENGTH 63 //Maximum length of Activity names not including the nul character.
typedef quUInt64 quFlowID;
#define QU_INVALID_FLOW_ID ( (quFlowID)-1 )
#define QU_MAX_FLOW_NAME_LENGTH 63

//Markers
#define QU_MAX_MARKER_NAME_LENGTH 63 //Maximum length of Marker names not including the nul character.

#endif
