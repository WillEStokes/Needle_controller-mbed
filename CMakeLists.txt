# mbed-cmake requires at least CMake 3.12
cmake_minimum_required(VERSION 3.12)

# load project upload methods (must be before mbed-cmake is loaded)
# -------------------------------------------------------------
include(cmake/UploadMethodConfig.cmake)

# load compilers and flags
# -------------------------------------------------------------

include(mbed-cmake/mbed-cmake.cmake)

project(mbed-cmake-test-project LANGUAGES NONE)

include_directories(${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/lib)

# recurse to subdirectories
# -------------------------------------------------------------

add_mbed_executable(NeedleController main.cpp)
target_sources(NeedleController PRIVATE ../src/NeedleController.cpp
../lib/ADC18.cpp
../lib/QEI.cpp)

# build report
# -------------------------------------------------------------

mbed_cmake_print_build_report()
mbed_cmake_finalize()