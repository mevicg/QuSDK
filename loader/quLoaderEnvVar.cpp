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

#include "quLoaderEnvVar.h"
#if defined( _WIN64 )
#	include <Windows.h>
#else
//Cant use on windows because it requires a cruntime to be linked and we dont
//want this library to introduce constrains on which runtime the application must use.
#	include <cstdlib>
#	include <string.h>
#endif

namespace qul
{

bool EnvVar::GetValue( const char* varName, char* outValue, size_t valueSize )
{
#if defined( _WIN64 )
	DWORD numCharsStored = GetEnvironmentVariableA( varName, outValue, (DWORD)valueSize );
	if( numCharsStored == 0 || numCharsStored > valueSize )
		return false;
	else
		return true;
#else
	char* str = getenv( varName );
	if( str == nullptr )
		return false;
	size_t strLen = strlen( str );
	if( strLen >= valueSize )
		return false;

	memcpy( outValue, str, strLen + 1 );
	return true;
#endif
}

} //End namespace qul