# Request the minimum version of CMake, in case of lower version throws an error.
# See https://cmake.org/cmake/help/latest/command/cmake_minimum_required.html
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Include headers project
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Collect tests
file(GLOB TESTS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/unit/*.c)
file(GLOB SRC_FILES
	${CMAKE_SOURCE_DIR}/lib/libdyn/src/alerts.c
	${CMAKE_SOURCE_DIR}/lib/libdyn/src/supplies.c
	${CMAKE_SOURCE_DIR}/lib/libsta/src/emergency.c
)

# Link with Unity header
target_link_directories(${PROJECT_NAME}_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/Unity/src)

# Coverage (optional)
if (RUN_COVERAGE)
	message("Coverage enabled")

	find_program(LCOV_PATH lcov)
	if (NOT LCOV_PATH)
		message(FATAL_ERROR "lcov not found! Aborting...")
	endif()

	find_program(GCOV_PATH gcov)
	if (NOT GCOV_PATH)
		message(FATAL_ERROR "gcov not found! Aborting...")
	endif()

	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage --coverage")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov --coverage")
endif()

# Create test executable
add_executable(test_${PROJECT_NAME} ${TESTS_FILES} ${SRC_FILES})
add_dependencies(lab1_server test_${PROJECT_NAME})

# Link with Unity and cJSON
target_link_libraries(test_${PROJECT_NAME} unity cjson)
target_include_directories(test_${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../external/cJSON
	${CMAKE_CURRENT_SOURCE_DIR}/../lib/libdyn/include
	${CMAKE_CURRENT_SOURCE_DIR}/../lib/libsta/include
)

# Add test
# See: https://cmake.org/cmake/help/latest/command/add_test.html
add_test(NAME test_${PROJECT_NAME} COMMAND test_${PROJECT_NAME})
