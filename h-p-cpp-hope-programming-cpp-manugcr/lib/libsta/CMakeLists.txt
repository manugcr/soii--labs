# Request the minimum version of CMake, in case of lower version throws an error.
# See https://cmake.org/cmake/help/latest/command/cmake_minimum_required.html
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Define your project:
# See: https://cmake.org/cmake/help/latest/command/project.html
project(
  "StatiCLib"
  VERSION 1.0.0
  DESCRIPTION "CMake Project Template"
  LANGUAGES CXX
)

# Define the C standard:
# See: https://cmake.org/cmake/help/latest/variable/CMAKE_CXX_STANDARD.html
set(CMAKE_C_STANDARD 20)

# Include the `include` directory, where the source files are located.
# See: https://cmake.org/cmake/help/latest/command/include_directories.html
include_directories(include)

# Add the `src` directory, where the source files are located.
# See: https://cmake.org/cmake/help/latest/command/file.html#glob
file(GLOB_RECURSE SOURCES "src/*.cpp")

#
# Create the executable
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Link the nlohmann_json library
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} PRIVATE rocksdb)
