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
#if defined( _WIN64 )
#	define DLL_CALL __cdecl
#else
#	define DLL_CALL
#endif

namespace qul
{

class Dylib
{
public:
	typedef void( DLL_CALL* Function )();

	bool Load( const char* libPathAndName );
	void Unload();

	bool IsLoaded() const;
	Function GetFunction( const char* funcName ) const;

private:
	using HandleType = unsigned long long;
	static constexpr HandleType INVALID_HANDLE = 0;

	HandleType handle = INVALID_HANDLE;
};

} //End namespace qul