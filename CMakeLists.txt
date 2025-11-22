cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Mock-Audio-Output-Generator)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C CXX ASM)

add_subdirectory(utils)

if(CMAKE_CROSSCOMPILING AND NOT TESTS_ENABLED )
    message(STATUS "Build type: Firmware (STM32 Target)")

    enable_language(C CXX ASM)
    
    set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src)
    set(SOURCES
        ${SOURCE_PATH}/MCUSerial.cpp
        ${SOURCE_PATH}/Packetizer.cpp
        ${SOURCE_PATH}/DMAController.cpp
        ${SOURCE_PATH}/SystemTime.cpp
    )

    # Create the firmware executable
    add_executable(${CMAKE_PROJECT_NAME})

    # STM32 Specific definitions
    add_subdirectory(cmake/stm32cubemx)

    # Add sources to executable
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${SOURCES})

    # Remove wrong libob.a library dependency when using cpp files
    list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)
    
    # Link libraries
    target_link_libraries(${CMAKE_PROJECT_NAME} stm32cubemx utils)
    
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND arm-none-eabi-objcopy -O ihex ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.hex 
    )
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND arm-none-eabi-size ${CMAKE_PROJECT_NAME}.hex
    )

else()
    message(STATUS "Build type: Unit Tests (Host Target)")
    add_subdirectory(tests)
endif()
