# Project name
project(ZMQTest)

# Require CMake version >=2.8
cmake_minimum_required(VERSION 2.8)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Appends the cmake/modules path inside the MAKE_MODULE_PATH variable which stores the
# directories of additional CMake modules (ie. MacroOutOfSourceBuild.cmake):
set(CMAKE_MODULE_PATH ${ZMQTest_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

set(CMAKE_MACOSX_RPATH 1)

# Use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# When building, don't use the install RPATH already (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# Add the automatically determined parts of the RPATH which point to directories outside the
# build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# The RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")

# Find and add external packages required for application and test
find_package( Boost 1.41.0
	      REQUIRED
	      COMPONENTS program_options system filesystem unit_test_framework date_time thread regex)
find_package(ZEROMQ 4.1.4 REQUIRED)

# Check if Boost version has placeholders and set definition accordingly
message("\nChecking Boost version placeholder support")
if (NOT DEFINED Boost_VERSION_STRING)
  set(Boost_VERSION_STRING "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")
endif ()
if ((Boost_VERSION_STRING EQUAL 1.60.0) OR (Boost_VERSION_STRING GREATER 1.60.0))
  message(STATUS "Boost version ${Boost_VERSION_STRING} has placeholders")
  ADD_DEFINITIONS(-DBOOST_HAS_PLACEHOLDERS)
else ()
  message(STATUS "Boost version ${Boost_VERSION_STRING} does not have placeholders")
endif ()

# Add the cmake subdirectory so that CMake finders are installed
add_subdirectory(cmake)


include_directories(${Boost_INCLUDE_DIRS} ${ZEROMQ_INCLUDE_DIRS})
add_executable(zmq_test zmq_test.cpp)
target_link_libraries(zmq_test ${Boost_LIBRARIES} ${ZEROMQ_LIBRARIES})
install(TARGETS zmq_test RUNTIME DESTINATION bin)
