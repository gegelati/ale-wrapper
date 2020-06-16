cmake_minimum_required(VERSION 3.16)

set (PROJECT_NAME ALEGegelati)
project(${PROJECT_NAME})

find_package(ale REQUIRED)

# Disable C and C++ compiler extensions.
# C/CXX_EXTENSIONS are ON by default to allow the compilers to use extended
# variants of the C/CXX language.
# However, this could expose cross-platform bugs in user code or in the headers
# of third-party dependencies and thus it is strongly suggested to turn
# extensions off.
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio.*")

    # Link with pthread
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

    # Set to 1 to activate debug compilation (0 for release)
    set(DEBUG 0)

    if(${DEBUG})
        MESSAGE("Generate Debug project")
        set(CMAKE_BUILD_TYPE Debug)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug)
        message(${CMAKE_CXX_FLAGS_DEBUG})
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg -Wall -O0 --coverage")
        message(${CMAKE_CXX_FLAGS_DEBUG})
    else()
        MESSAGE("Generate Release project")
        set(CMAKE_BUILD_TYPE Release)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
    endif()
    message(${CMAKE_CXX_FLAGS_DEBUG})
    #add libmath during non visual studio builds
    set(CMAKE_EXTRA_LIB m)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Add definitions for testing purposes
if(${TESTING})
    MESSAGE("Testing mode")
    add_definitions(-DNB_GENERATIONS=10)
endif()

# *******************************************
# *********** GEGELATI LIBRARY **************
# *******************************************

if(WIN32)
    set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    # find the gegelatilib-x.y.z folder in the lib directory.
    file(GLOB GEGELATI_ROOT_DIR "${LIBS_DIR}/gegelatilib-[\\.|0-9]*")
    set(ENV{GEGELATI_DIR} ${GEGELATI_ROOT_DIR})
endif()
find_package(GEGELATI)


if (WIN32)
    file(GLOB
            GEGELATI_DLL
            ${GEGELATI_ROOT_DIR}/bin/*.dll
            )

    MESSAGE("Copy GEGELATI D14LLs into ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    if(NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio.*")
        file(COPY ${GEGELATI_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    else()
        file(COPY ${GEGELATI_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
        file(COPY ${GEGELATI_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
    endif()
endif()



# *******************************************
# ************** Executable  ****************
# *******************************************


file(GLOB_RECURSE
        ALEGelati_files
        ./src/*.cpp
        ./src/*.h
        )

include_directories(${GEGELATI_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${ALEGelati_files} src/learningExample.cpp src/replay.cpp)
target_link_libraries(${PROJECT_NAME} ${GEGELATI_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ale::ale-lib)



#################
#
##  Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2019) :
##
##  Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
##  Clément Guy <clement.guy@insa-rennes.fr> (2014)
##  Florian Arrestier <florian.arrestier@insa-rennes.fr> (2017-2019)
##  Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
##  Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2015)
##  Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013 - 2014)
##
##  Spider is a dataflow based runtime used to execute dynamic PiSDF
##  applications. The Preesm tool may be used to design PiSDF applications.
##
##  This software is governed by the CeCILL  license under French law and
##  abiding by the rules of distribution of free software.  You can  use,
##  modify and/ or redistribute the software under the terms of the CeCILL
##  license as circulated by CEA, CNRS and INRIA at the following URL
##  "http://www.cecill.info".
##
##  As a counterpart to the access to the source code and  rights to copy,
##  modify and redistribute granted by the license, users are provided only
##  with a limited warranty  and the software's author,  the holder of the
##  economic rights,  and the successive licensors  have only  limited
##  liability.
##
##  In this respect, the user's attention is drawn to the risks associated
##  with loading,  using,  modifying and/or developing or reproducing the
##  software by the user in light of its specific status of free software,
##  that may mean  that it is complicated to manipulate,  and  that  also
##  therefore means  that it is reserved for developers  and  experienced
##  professionals having in-depth computer knowledge. Users are therefore
##  encouraged to load and test the software's suitability as regards their
##  requirements in conditions enabling the security of their systems and/or
##  data to be ensured and,  more generally, to use and operate it in the
##  same conditions as regards security.
##
##  The fact that you are presently reading this means that you have had
##  knowledge of the CeCILL license and that you accept its terms.
#
## cmake minimum version
#cmake_minimum_required(VERSION 3.12)
#
#project(ALEGegelati
#        LANGUAGES CXX)
#
## Add definition for relative path into project
#add_definitions( -DPROJECT_ROOT_PATH="${CMAKE_CURRENT_SOURCE_DIR}")
#
## Defines the CMAKE_INSTALL_LIBDIR, CMAKE_INSTALL_BINDIR and many other useful macros.
## See https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html
#include(GNUInstallDirs)
#
## Control where libraries and executables are placed during the build.
## With the following settings executables are placed in <the top level of the
## build tree>/bin and libraries/archives in <top level of the build tree>/lib.
## This is particularly useful to run ctests on libraries built on Windows
## machines: tests, which are executables, are placed in the same folders of
## dlls, which are treated as executables as well, so that they can properly
## find the libraries to run. This is a because of missing RPATH on Windows.
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
#
## Compiler specific settings
#if (MSVC)
#    message("Add definition _CRT_SECURE_NO_WARNINGS flag for MSVC compiler.")
#    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
#    install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> CONFIGURATIONS "Debug" "RelWithDebInfo" DESTINATION "${CMAKE_INSTALL_BINDIR}")
#else ()
#    set(ALEGegelati_WARNING_FLAGS "-Wall -Wextra -Wnon-virtual-dtor -Wconversion -Wformat")
#    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg -O0 ${ALEGegelati_WARNING_FLAGS}")
#    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG ${ALEGegelati_WARNING_FLAGS}")
#    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
#        message(STATUS "Using Link Time Optimization (-flto) on GNU platforms.")
#        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
#    endif ()
#    if (${CMAKE_GENERATOR} MATCHES "MinGW Makefiles")
#        # To prevent a redefinition conflict
#        message(STATUS "Add definition _TIMESPEC_DEFINED")
#        add_definitions(-D_TIMESPEC_DEFINED)
#    endif ()
#endif ()
#
## CMake cross compilation to mingw
#if (${CROSS_COMPILE_MINGW})
#    set(CMAKE_SYSTEM_NAME Windows)
#    set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)
#
#    # cross compilers to use for C and C++
#    set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
#    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
#    set(CMAKE_Fortran_COMPILER ${TOOLCHAIN_PREFIX}-gfortran)
#    set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)
#
#    # target environment on the build host system
#    #   set 1st to dir with the cross compiler's C/C++ headers/libs
#    set(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX})
#
#    # modify default behavior of FIND_XXX() commands to
#    # search for headers/libs in the target environment and
#    # search for programs in the build host environment
#    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
#endif ()
#
## Disable C and C++ compiler extensions.
## C/CXX_EXTENSIONS are ON by default to allow the compilers to use extended
## variants of the C/CXX language.
## However, this could expose cross-platform bugs in user code or in the headers
## of third-party dependencies and thus it is strongly suggested to turn
## extensions off.
#set(CMAKE_C_EXTENSIONS OFF)
#set(CMAKE_CXX_EXTENSIONS OFF)
#
## Setting C++ standard to C++17
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
## We used
#find_package(ale REQUIRED)
#
#
## *******************************************
## *********** GEGELATI LIBRARY **************
## *******************************************
#
#if(WIN32)
#    set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
#    # find the gegelatilib-x.y.z folder in the lib directory.
#    file(GLOB GEGELATI_ROOT_DIR "${LIBS_DIR}/gegelatilib-[\\.|0-9]*")
#    set(ENV{GEGELATI_DIR} ${GEGELATI_ROOT_DIR})
#endif()
#find_package(GEGELATI)
#
#
#if (WIN32)
#    file(GLOB
#            GEGELATI_DLL
#            ${GEGELATI_ROOT_DIR}/bin/*.dll
#            )
#
#    MESSAGE("Copy GEGELATI D14LLs into ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
#    if(NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio.*")
#        file(COPY ${GEGELATI_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#    else()
#        file(COPY ${GEGELATI_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
#        file(COPY ${GEGELATI_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
#    endif()
#endif()
#
#### Options
#
## Encourage user to specify a build type (e.g. Release, Debug, etc.), otherwise set it to Release.
#if (NOT CMAKE_CONFIGURATION_TYPES)
#    if (NOT CMAKE_BUILD_TYPE)
#        message(STATUS "Setting build type to 'Release' as none was specified.")
#        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "Release")
#    endif ()
#endif ()
#
## Add the test files
#file(
#        GLOB_RECURSE
#        ${PROJECT_NAME}_SRC
#
#        # Auto generated sources
#        src/*.cpp
#        src/*.h
#)
#
## Set the include directories to use <> instead of ""
#include_directories(
#        ${CMAKE_SOURCE_DIR}/src
#        ${GEGELATI_INCLUDE_DIRS}
#)
#
#add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRC})
#target_link_libraries(${PROJECT_NAME} ale::ale-lib)
