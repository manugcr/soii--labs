# Request the minimum version of CMake, in case of lower version throws an error.
# See https://cmake.org/cmake/help/latest/command/cmake_minimum_required.html
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Define your project:
# See: https://cmake.org/cmake/help/latest/command/project.html
project(
  "HPCPP"
  VERSION 1.0.0
  DESCRIPTION "High Performance C++"
  LANGUAGES CXX
)

# Include CTest
# See: https://cmake.org/cmake/help/latest/module/CTest.html
include(CTest)
enable_testing()

# Define the C standard:
# See: https://cmake.org/cmake/help/latest/variable/CMAKE_CXX_STANDARD.html
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(WITH_BENCHMARK_TOOLS OFF)
set(WITH_TOOLS OFF)
set(WITH_TESTS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

# Include the `include` directory, where the source files are located.
# See: https://cmake.org/cmake/help/latest/command/include_directories.html
include_directories(include)

# To automatic download external deps, we need use FetchContent
# See: https://cmake.org/cmake/help/latest/module/FetchContent.html
include(FetchContent)

set(GTEST_GIT_URL "https://github.com/google/googletest.git")
set(ROCKSDB_GIT_URL "https://github.com/facebook/rocksdb.git")
set(NLOHMANN_JSON_GIT_URL "https://github.com/nlohmann/json.git")
set(CPP_HTTPLIB_GIT_URL "https://github.com/yhirose/cpp-httplib.git")

FetchContent_Declare(
    googletest
    GIT_REPOSITORY ${GTEST_GIT_URL}
    GIT_TAG release-1.12.0  # Optionally pin to a stable release
)

find_package(RocksDB)
if(ROCKSDB_FOUND)
    set(ROCKSDB_LIB RocksDB::rocksdb)
else()
    message(STATUS "RocksDB not found in the system, fetching from GitHub")
    FetchContent_Declare(
        rocksdb
        GIT_REPOSITORY ${ROCKSDB_GIT_URL}
        GIT_TAG v6.22.1
        CMAKE_ARGS -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DPORTABLE=OFF -DROCKSDB_CMAKE_CXX_FLAGS="-O2" -DWITH_TESTS=OFF
    )
    FetchContent_MakeAvailable(rocksdb)
    set(ROCKSDB_LIB Rocksdb::Rocksdb)
endif()

# FetchContent_Declare(
#     rocksdb
#     GIT_REPOSITORY ${ROCKSDB_GIT_URL}
#     GIT_TAG v9.0.0
# )

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY ${NLOHMANN_JSON_GIT_URL}
    GIT_TAG v3.11.2
)

FetchContent_Declare(
    cpp_httplib
    GIT_REPOSITORY ${CPP_HTTPLIB_GIT_URL}
    GIT_TAG v0.15.3
)

FetchContent_MakeAvailable(googletest)
# FetchContent_MakeAvailable(rocksdb)
FetchContent_MakeAvailable(nlohmann_json)
FetchContent_MakeAvailable(cpp_httplib)
find_package(ZLIB REQUIRED)

# Add the `src` directory, where the source files are located.
# See: https://cmake.org/cmake/help/latest/command/file.html#glob
file(GLOB_RECURSE SERVER_SOURCES "src/server.cpp")
file(GLOB_RECURSE CLIENT_SOURCES "src/client.cpp")

# Create the executable
# See: https://cmake.org/cmake/help/latest/command/add_executable.html
add_executable(${PROJECT_NAME}_server ${SERVER_SOURCES})
add_executable(${PROJECT_NAME}_client ${CLIENT_SOURCES})

# Compile the libraries
# See: https://cmake.org/cmake/help/latest/command/add_subdirectory.html
add_subdirectory(lib/libdyn)
add_subdirectory(lib/libsta)
add_subdirectory(lib/cannyEdgeFilter)

# Set the compile options for debugging
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME}_server PRIVATE -g -O0 -Wall -Wextra -pedantic -Wno-error=maybe-uninitialized)
else()
    target_compile_options(${PROJECT_NAME}_server PRIVATE -O3)
endif()

# Include headers in lib
# See: https://cmake.org/cmake/help/latest/command/target_include_directories.html
target_include_directories(${PROJECT_NAME}_server PUBLIC
    lib/libdyn/include
    lib/libsta/include
    lib/cannyEdgeFilter/include
    ${cpp_httplib_SOURCE_DIR}/include
    ${nlohmann_json_SOURCE_DIR}
    ${rocksdb_SOURCE_DIR}/include
)

# Enable OpenMP
# See: https://cmake.org/cmake/help/latest/module/FindOpenMP.html
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME}_server INTERFACE OpenMP::OpenMP_CXX)
else()
    message(FATAL_ERROR "OpenMP not found")
endif()

# Link the libraries
# See: https://cmake.org/cmake/help/latest/command/target_link_libraries.html
target_link_libraries(${PROJECT_NAME}_server PRIVATE httplib)
target_link_libraries(${PROJECT_NAME}_server PRIVATE rocksdb)
target_link_libraries(${PROJECT_NAME}_server PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME}_server PUBLIC StatiCLib DynamiCLib)
target_link_libraries(${PROJECT_NAME}_server PUBLIC cannyEdge)
target_link_libraries(${PROJECT_NAME}_client PRIVATE ZLIB::ZLIB)

# Add the tests
if (RUN_COVERAGE EQUAL 1)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    add_subdirectory(tests)
endif()
