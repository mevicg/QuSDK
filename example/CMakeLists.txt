
set( QU_API_EXAMPLE_SOURCES
	main.cpp
	TestdataGenerator.h TestdataGenerator.cpp
	ActivityChannelThread.h ActivityChannelThread.cpp
)
add_executable( QuApiExample ${QU_API_EXAMPLE_SOURCES} )
#We link to the loader so that we dont have to manually load functions from the
#dll. Note that when the api is disabled the loader is an alias for the api itself
#which compiles the functions as no-ops.
target_link_libraries( QuApiExample PUBLIC QuApiLoader )
set_target_properties( QuApiExample PROPERTIES FOLDER "libs/qusdk" )