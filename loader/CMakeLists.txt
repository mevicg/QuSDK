
if( QU_API_INSTRUMENT )
	set( QU_API_LOADER_SOURCES
		quLoaderDylib.h quLoaderDylib.cpp
		quLoaderEnvVar.h quLoaderEnvVar.cpp
		quLoaderMain.cpp
	)
	add_library( QuApiLoader STATIC ${QU_API_LOADER_SOURCES} )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR}/ FILES ${QU_API_LOADER_SOURCES} )
	set_target_properties( QuApiLoader PROPERTIES FOLDER "libs/qusdk" )

	target_link_libraries( QuApiLoader PUBLIC QuApi )
else()
	#By not building the loader when QuApi is disabled or unsupported we'll compile
	#it's usage as no-ops due to the way QuApi's headers are written.
	add_library( QuApiLoader ALIAS QuApi )
endif()
