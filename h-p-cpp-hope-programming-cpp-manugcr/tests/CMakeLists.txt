# Request the minimum version of CMake, in case of lower version throws an error.
# See https://cmake.org/cmake/help/latest/command/cmake_minimum_required.html
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Set flags for coverage
set(CMAKE_CXX_FLAGS_DEBUG "-g --coverage")

# Include headers project
include_directories(${CMAKE_SOURCE_DIR}/../include)

# Collect tests
file(GLOB TESTS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/unit/*.cpp)
file(GLOB SRC_FILES
	${CMAKE_SOURCE_DIR}/lib/libsta/src/alerts.cpp
	${CMAKE_SOURCE_DIR}/lib/libsta/src/supplies.cpp
	${CMAKE_SOURCE_DIR}/lib/libdyn/src/emergency.cpp
)

# Link with gtest header
target_link_directories(${PROJECT_NAME}_server PUBLIC ${CMAKE_SOURCE_DIR}/external/googletest/googletest/src)

# Coverage
message("Coverage enabled")
find_program(LCOV_PATH lcov)
if (NOT LCOV_PATH)
	message(FATAL_ERROR "lcov not found! Aborting...")
endif()

find_program(GCOV_PATH gcov)
if (NOT GCOV_PATH)
	message(FATAL_ERROR "gcov not found! Aborting...")
endif()

add_compile_options(-g -fprofile-arcs -ftest-coverage)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g --test-coverage --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov --coverage")

# Add executable for unit tests
add_executable(test_${PROJECT_NAME} ${TESTS_FILES} ${SRC_FILES})
add_dependencies(${PROJECT_NAME}_server test_${PROJECT_NAME})

# Link with gtest
target_link_libraries(test_${PROJECT_NAME} gtest nlohmann_json::nlohmann_json rocksdb)

# Include gtest headers
target_include_directories(test_${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../lib/libdyn/include
	${CMAKE_CURRENT_SOURCE_DIR}/../lib/libsta/include
)

# Add tests
add_test(NAME test_${PROJECT_NAME} COMMAND test_${PROJECT_NAME})
