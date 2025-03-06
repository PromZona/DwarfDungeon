cmake_minimum_required(VERSION 3.10)

project(DwarfDungeon)

add_subdirectory(godot-cpp)

set (SOURCES
	"${CMAKE_SOURCE_DIR}/behaviourlib/src/register_types.cpp"
	"${CMAKE_SOURCE_DIR}/behaviourlib/src/SpawnManager.cpp"
	"${CMAKE_SOURCE_DIR}/behaviourlib/src/EntityManager.cpp"
	"${CMAKE_SOURCE_DIR}/behaviourlib/src/enemy.cpp"
	"${CMAKE_SOURCE_DIR}/behaviourlib/src/BehaviourManager.cpp"
	"${CMAKE_SOURCE_DIR}/behaviourlib/src/Game.cpp"
	"${CMAKE_SOURCE_DIR}/behaviourlib/src/MovementManager.cpp"
	"${CMAKE_SOURCE_DIR}/behaviourlib/src/UIManager.cpp"
)


if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
		# Web Emscriten flags are partially from here: https://thatonegamedev.com/cpp/cmake/godot-native-using-cmake/#porting-for-web
		# And of course official docs
		# https://docs.godotengine.org/en/stable/contributing/development/compiling/compiling_for_web.html#building-export-templates
		# https://docs.godotengine.org/en/stable/tutorials/export/exporting_for_web.html
		
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s ASSERTIONS=1 -s SIDE_MODULE=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ASSERTIONS=1 -s SIDE_MODULE=1")
    set(CMAKE_EXECUTABLE_SUFFIX ".wasm")
		set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

    add_executable(${PROJECT_NAME} ${SOURCES})
else()
    add_library(${PROJECT_NAME} SHARED ${SOURCES})
endif()


# Set the library filename so it matches what I've copied into the .gdextension file
set_target_properties(${PROJECT_NAME} PROPERTIES
                      OUTPUT_NAME   ${PROJECT_NAME}.$<LOWER_CASE:$<PLATFORM_ID>>.template_$<LOWER_CASE:$<CONFIG>>.x86_64
)

target_include_directories(${PROJECT_NAME} 
    PRIVATE ${CMAKE_SOURCE_DIR}/behaviourlib/src
)

# Flags for linking depending library statically for Windows
# https://github.com/godotengine/godot/issues/84160
if (WIN32)
	target_link_options(${PROJECT_NAME} PRIVATE -static -static-libgcc -static-libstdc++)
endif()

target_link_libraries(${PROJECT_NAME} godot-cpp)


# Add compiler warning flags
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wnon-virtual-dtor
    -Woverloaded-virtual
    -Wconversion
    -Wsign-conversion
    -Wnull-dereference
    -Wuninitialized
    -Wformat-security
    -Wstrict-aliasing
    -Wredundant-move
    -Wmisleading-indentation
    -Winline
    -Wdeprecated
    -Wthread-safety
    -Watomic-alignment
    -std=c++20
    -O2
    -g
    -Wno-unused-parameter
    -Wno-unused-function
)


install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_SOURCE_DIR}/dwarfdungeon/bin)

