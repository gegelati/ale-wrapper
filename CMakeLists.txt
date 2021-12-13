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

    # Debug or release
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        MESSAGE("Generate Debug project")
        set(CMAKE_BUILD_TYPE Debug)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug)
        message(${CMAKE_CXX_FLAGS_DEBUG})
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall -O0 --coverage")
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
# *********** Download Roms  ****************
# *******************************************


# download fishing derby
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/roms/fishing_derby")
    MESSAGE("Download Fishing Derby")
    file(DOWNLOAD www.atarimania.com/pgedump.awp?id=13881 "${CMAKE_CURRENT_SOURCE_DIR}/roms/fishingDerb.7z")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar -xf fishingDerb.7z
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/roms/")
    file(RENAME "${CMAKE_CURRENT_SOURCE_DIR}/roms/Fishing Derby (Canal 3 - Intellivision).bin" "${CMAKE_CURRENT_SOURCE_DIR}/roms/fishing_derby")
    file(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/roms/fishingDerb.7z")
endif()

# download frostbite
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/roms/frostbite")
    MESSAGE("Download Frostbite")
    file(DOWNLOAD http://www.atarimania.com/pgedump.awp?id=11601 "${CMAKE_CURRENT_SOURCE_DIR}/roms/frostbite.7z")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar -xf frostbite.7z
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/roms/")
    file(RENAME "${CMAKE_CURRENT_SOURCE_DIR}/roms/Frostbite.bin" "${CMAKE_CURRENT_SOURCE_DIR}/roms/frostbite")
    file(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/roms/frostbite.7z")
endif()

# *******************************************
# ************** Executable  ****************
# *******************************************

# **** ALEGegeglati target ****
file(GLOB_RECURSE
        ALEGelati_files
        ./src/*.cpp
        ./src/*.h
        )

include_directories(${GEGELATI_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${ALEGelati_files})
target_link_libraries(${PROJECT_NAME} ${GEGELATI_LIBRARIES} ale::ale-lib)

target_compile_definitions(${PROJECT_NAME} PRIVATE ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")


# **** Codegen Target *****
list(REMOVE_ITEM
        ALEGelati_files
        ${CMAKE_CURRENT_SOURCE_DIR}/./src/learningExample.cpp)
add_executable(${PROJECT_NAME}Codegen ${ALEGelati_files} ${CMAKE_CURRENT_SOURCE_DIR}/./src/codegen/codegen.cpp)
target_compile_definitions(${PROJECT_NAME}Codegen PRIVATE ROOT_DIR="${CMAKE_SOURCE_DIR}")
target_link_libraries(${PROJECT_NAME}Codegen ${GEGELATI_LIBRARIES} ale::ale-lib)
# **** InferenceTPG Target ****
add_executable(${PROJECT_NAME}InferenceTPG ${ALEGelati_files} ${CMAKE_CURRENT_SOURCE_DIR}/./src/codegen/inferenceTPG.cpp)
target_link_libraries(${PROJECT_NAME}InferenceTPG ${GEGELATI_LIBRARIES} ale::ale-lib)

target_compile_definitions(${PROJECT_NAME}InferenceTPG PRIVATE ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
