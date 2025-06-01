set( QU_API_LOADER_SOURCES
	quLoaderDylib.h quLoaderDylib.cpp
	quLoaderEnvVar.h quLoaderEnvVar.cpp
	quLoaderMain.cpp
)
add_library( QuApiLoader STATIC ${QU_API_LOADER_SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR}/ FILES ${QU_API_LOADER_SOURCES} )
set_target_properties( QuApiLoader PROPERTIES FOLDER "libs/qusdk" )

target_link_libraries( QuApiLoader PUBLIC QuApi )

if( QU_API_MACOS )
	target_compile_options( QuApiLoader PRIVATE
		$<$<COMPILE_LANGUAGE:CXX>:-x objective-c++>
		$<$<COMPILE_LANGUAGE:C>:-x objective-c>
	)
	target_link_libraries( QuApiLoader PUBLIC "-framework Foundation" )
endif()

#The loader needs the api to be enabled in order for it to be able to implement it.
target_compile_definitions( QuApiLoader PRIVATE QU_API_ENABLED )