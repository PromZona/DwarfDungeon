cmake_minimum_required(VERSION 3.10)

project(DwarfDungeon)

add_subdirectory(godot-cpp)

add_library(${PROJECT_NAME} SHARED
	"${CMAKE_SOURCE_DIR}/behaviourlib/src/enemy.cpp"
	"${CMAKE_SOURCE_DIR}/behaviourlib/src/enemy.h"
	"${CMAKE_SOURCE_DIR}/behaviourlib/src/BehaviourManager.cpp"
	"${CMAKE_SOURCE_DIR}/behaviourlib/src/BehaviourManager.h"
	"${CMAKE_SOURCE_DIR}/behaviourlib/src/register_types.cpp"
	"${CMAKE_SOURCE_DIR}/behaviourlib/src/register_types.h"
)

# Set the library filename so it matches what I've copied into the .gdextension file
set_target_properties(${PROJECT_NAME} PROPERTIES
                      OUTPUT_NAME   ${PROJECT_NAME}.$<LOWER_CASE:$<PLATFORM_ID>>.template_$<LOWER_CASE:$<CONFIG>>.x86_64
)

# Flags for linking depending library statically for Windows
# https://github.com/godotengine/godot/issues/84160
if (WIN32)
	target_link_options(${PROJECT_NAME} PRIVATE -static -static-libgcc -static-libstdc++)
endif()

target_link_libraries(${PROJECT_NAME} godot-cpp)


install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_SOURCE_DIR}/dwarfdungeon/bin)

