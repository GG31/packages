CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(vle.extension.petrinet CXX C)
ENABLE_TESTING()

SET(MODEL_MAJOR 0)
SET(MODEL_MINOR 1)
SET(MODEL_PATCH 0)
SET(MODEL_NAME "${PROJECT_NAME}-${MODEL_MAJOR}.${MODEL_MINOR}.${MODEL_PATCH}")
SET(VLE_NAME "${PROJECT_NAME}-${MODEL_MAJOR}.${MODEL_MINOR}")

##
## Options for compilation of package
##

OPTION(WITH_TEST "will build the test [default: ON]" ON)
OPTION(WITH_DOC "will compile doc and install it [default: OFF]" OFF)
OPTION(WITH_WARNINGS "will compile with g++ warnings [default: ON]" ON)

if (CMAKE_BUILD_TYPE STREQUAL "")
  SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE)
endif ()

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX)
  if (NOT WITH_WARNINGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
  else (NOT WITH_WARNINGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
  endif (NOT WITH_WARNINGS)
  if ("${CMAKE_BUILD_TYPE}" EQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" EQUAL "RelWithDebInfo")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3")
  endif ()
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH})

##
## Modules
##

INCLUDE(CheckIncludeFileCXX)
INCLUDE(CheckIncludeFile)
INCLUDE(CheckLibraryExists)
INCLUDE(CMakeDetermineCCompiler)

##
## Check VLE and package dependencies
##

set(VLE_DEBUG 0)
set(VLE_ABI_VERSION 1.3)
find_package(VLE REQUIRED)
set (VleUtils_DIR "${CMAKE_SOURCE_DIR}/cmake")
find_package(VleUtils REQUIRED)
VleCheckDependencies()

##
## Check GVLE and package dependencies
##

option(WITH_GVLE "use the GVLE [default: on]" ON)

if (WITH_GVLE)
  find_package(GVLE)
  if (GVLE_FOUND)
    set (HAVE_GVLE 1 CACHE INTERNAL "" FORCE)
    if (NOT WIN32)
      pkg_check_modules(GTKSOURCEVIEW gtksourceview-2.0)
    endif (NOT WIN32)
  else ()
    message (WARNING "GVLE not found!")
  endif ()
endif ()

##
## Find boost
##

SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREAD ON)
FIND_PACKAGE(Boost COMPONENTS unit_test_framework date_time)

IF (Boost_UNIT_TEST_FRAMEWORK_FOUND)
  SET(HAVE_UNITTESTFRAMEWORK 1 CACHE INTERNAL "" FORCE)
ENDIF (Boost_UNIT_TEST_FRAMEWORK_FOUND)

##
## Generate the doxygen
##

FIND_PACKAGE(Doxygen)
IF (DOXYGEN AND WITH_DOC)
  SET(DOXYGEN_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
  SET(DOXYGEN_OUTPUT_MODELING_DIR "${PROJECT_BINARY_DIR}/doxygen/modeling")
  SET(DOXYGEN_OUTPUT_SOURCE_DIR "${PROJECT_BINARY_DIR}/doxygen/sources")
  CONFIGURE_FILE("cmake/doxygen-modeling.conf.in"
    "${PROJECT_BINARY_DIR}/doxygen-modeling.conf")
  CONFIGURE_FILE("cmake/doxygen-sources.conf.in"
    "${PROJECT_BINARY_DIR}/doxygen-sources.conf")
  FILE(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/doxygen")
  FILE(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/doxygen/modeling")
  FILE(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/doxygen/sources")

  ADD_CUSTOM_COMMAND(
    OUTPUT "${PROJECT_BINARY_DIR}/doxygen/modeling/index.html"
    DEPENDS "${PROJECT_BINARY_DIR}/doxygen-modeling.conf"
    COMMAND "${DOXYGEN}"
    ARGS "${PROJECT_BINARY_DIR}/doxygen-modeling.conf")

  ADD_CUSTOM_COMMAND(
    OUTPUT "${PROJECT_BINARY_DIR}/doxygen/sources/index.html"
    DEPENDS "${PROJECT_BINARY_DIR}/doxygen-sources.conf"
    COMMAND "${DOXYGEN}"
    ARGS "${PROJECT_BINARY_DIR}/doxygen-sources.conf")

  ADD_CUSTOM_TARGET(doc_modeling ALL DEPENDS
    "${PROJECT_BINARY_DIR}/doxygen-modeling.conf"
    "${PROJECT_BINARY_DIR}/doxygen/modeling/index.html" VERBATIM)
  ADD_CUSTOM_TARGET(doc_sources ALL DEPENDS
    "${PROJECT_BINARY_DIR}/doxygen-sources.conf"
    "${PROJECT_BINARY_DIR}/doxygen/sources/index.html" VERBATIM)

  INSTALL(DIRECTORY "${PROJECT_BINARY_DIR}/doxygen/modeling/html" DESTINATION
    "doc/html/modeling")
  INSTALL(DIRECTORY "${PROJECT_BINARY_DIR}/doxygen/sources/html" DESTINATION
    "doc/html/sources")
ENDIF (DOXYGEN AND WITH_DOC)

##
## Subdirectory
##

#ADD_SUBDIRECTORY(data)
IF(WITH_DOC)
  ADD_SUBDIRECTORY(doc)
ENDIF(WITH_DOC)
#ADD_SUBDIRECTORY(exp)
ADD_SUBDIRECTORY(src)

IF (Boost_UNIT_TEST_FRAMEWORK_FOUND AND WITH_TEST)
  ADD_SUBDIRECTORY(test)
ENDIF (Boost_UNIT_TEST_FRAMEWORK_FOUND AND WITH_TEST)

##
## CPack configuration
##

INSTALL(FILES Authors.txt Description.txt License.txt News.txt Readme.txt
  DESTINATION .)

INCLUDE(CMakeCPack.cmake)
