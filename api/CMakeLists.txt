
add_library( QuApi INTERFACE )
target_include_directories( QuApi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Include/ )
target_compile_definitions( QuApi INTERFACE _CRT_SECURE_NO_WARNINGS )

if( QU_API_INSTRUMENT )
	target_compile_definitions( QuApi INTERFACE QU_API_ENABLED )
endif()

#Cmake doesn't support headers for interface libraries. We want to show these headers
#because then it is clear in the IDE that this library is available and which headers
#we can include from it.
#To make it work have to use a workaround with a custom target.
#target_sources( QuApi INTERFACE ${QU_API_SOURCES} )
#source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR}/ FILES ${QU_API_SOURCES} )

set( QU_API_SOURCES
	Include/quConstants.h
	Include/quApi.h
	Include/quApi.hpp
)
add_custom_target( QuApi.h SOURCES ${QU_API_SOURCES} )
set_target_properties( QuApi.h PROPERTIES FOLDER "libs/qusdk" )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR}/Include FILES ${QU_API_SOURCES} )
