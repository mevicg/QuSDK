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

#include "quLoaderDylib.h"
#if defined( _WIN64 )
#	include <Windows.h>
using NativeHandleType = HMODULE;
#else
#	include <cstddef>
#	include <dlfcn.h>
using NativeHandleType = void*;
#endif

namespace qul
{

bool Dylib::Load( const char* libPathAndName )
{
#if defined( _WIN64 )
	handle = (HandleType)LoadLibraryA( libPathAndName );
	return handle != INVALID_HANDLE;
#else
	handle = (HandleType)dlopen( libPathAndName, RTLD_NOW | RTLD_LOCAL );
	return handle != INVALID_HANDLE;
#endif
}
void Dylib::Unload()
{
	if( handle == INVALID_HANDLE )
		return;

#if defined( _WIN64 )
	FreeLibrary( (NativeHandleType)handle );
#else
	dlclose( (NativeHandleType)handle );
#endif
	handle = INVALID_HANDLE;
}

bool Dylib::IsLoaded() const
{
	return handle != INVALID_HANDLE;
}
Dylib::Function Dylib::GetFunction( const char* funcName ) const
{
	if( handle == INVALID_HANDLE )
		return nullptr;

#if defined( _WIN64 )
	return (Function)GetProcAddress( (NativeHandleType)handle, funcName );
#else
	return (Function)dlsym( (NativeHandleType)handle, funcName );
#endif
}

} //End namespace qul