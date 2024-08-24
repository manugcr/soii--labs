# Request the minimum version of CMake, in case of lower version throws an error.
# See https://cmake.org/cmake/help/latest/command/cmake_minimum_required.html
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Define your project:
# See: https://cmake.org/cmake/help/latest/command/project.html
project(
	"lab1"
	VERSION 1.1.0
	DESCRIPTION "Socket and IPC project por Operating Systems II."
	LANGUAGES C
)

# Inclede CTest
# See: https://cmake.org/cmake/help/latest/module/CTest.html
include(CTest)

# Define the C standard:
# See: https://cmake.org/cmake/help/latest/variable/CMAKE_CXX_STANDARD.html
set(CMAKE_C_STANDARD 17)

# for clangd lsp
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include the `include` directory, where the source files are located.
# See: https://cmake.org/cmake/help/latest/command/include_directories.html
include_directories(include)

# To automatic download external deps, we need use FetchContent
# See: https://cmake.org/cmake/help/latest/module/FetchContent.html
include(FetchContent)

# In this example we are going to use Unity to download and test our program
# And cJSON to parse JSON data
set(UNITY_GIT_URL "https://github.com/ThrowTheSwitch/Unity.git")
set(CJSON_GIT_URL "https://github.com/DaveGamble/cJSON.git")

# Enable debug
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
    Unity
    GIT_REPOSITORY ${UNITY_GIT_URL}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/Unity
    UPDATE_COMMAND "" # disable update command
)

FetchContent_GetProperties(Unity)
if(NOT unity_POPULATED)
  FetchContent_Populate(Unity)
  add_subdirectory(${unity_SOURCE_DIR} ${unity_BINARY_DIR})
endif()

FetchContent_Declare(
    cJSON
    GIT_REPOSITORY ${CJSON_GIT_URL}
    GIT_TAG master
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/cJSON
    UPDATE_COMMAND "" # disable update command
)

set(ENABLE_CJSON_TEST Off CACHE BOOL "Disable cJSON tests")
FetchContent_GetProperties(cJSON)
if(NOT cJSON_POPULATED)
  FetchContent_Populate(cJSON)
  add_subdirectory(${cjson_SOURCE_DIR} ${cjson_BINARY_DIR})
endif()

# Add the `src` directory, where the source files are located.
# See: https://cmake.org/cmake/help/latest/command/file.html#glob
file(GLOB_RECURSE CLIENT_SOURCES "src/client.c")
file(GLOB_RECURSE SERVER_SOURCES "src/server.c")

# Create the executable
# See: https://cmake.org/cmake/help/latest/command/add_executable.html
add_executable(${PROJECT_NAME}_client ${CLIENT_SOURCES})
add_executable(${PROJECT_NAME}_server ${SERVER_SOURCES})

# Compile the libraries
# See: https://cmake.org/cmake/help/latest/command/add_subdirectory.html
add_subdirectory(lib/libdyn)
add_subdirectory(lib/libsta)

# Include headers in lib
# See: https://cmake.org/cmake/help/latest/command/target_include_directories.html
target_include_directories(${PROJECT_NAME}_server PUBLIC lib/libdyn/include lib/libsta/include)

# Link the libraries
# See: https://cmake.org/cmake/help/latest/command/target_link_libraries.html
target_link_libraries(${PROJECT_NAME}_server cjson DynamiCLib StatiCLib)

# Add cJSON library
target_link_libraries(${PROJECT_NAME}_server cjson)
target_include_directories(${PROJECT_NAME}_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/cJSON)

# Add subdirectory of tests
if (RUN_TESTS EQUAL 1 OR RUN_COVERAGE EQUAL 1)
	add_subdirectory(tests)
endif()
