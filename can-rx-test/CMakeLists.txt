# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD sparkfun_promicro_rp2350 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(can-rx-test C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1


add_executable(can-rx-test can-rx-test.c)

pico_set_program_name(can-rx-test "can-rx-test")
pico_set_program_version(can-rx-test "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(can-rx-test 0)
pico_enable_stdio_usb(can-rx-test 1)

# Add the standard library to the build
add_subdirectory(can2040)
target_link_libraries(can-rx-test
        hardware_adc
        pico_stdlib
        can2040
        )

# Add the standard include files to the build
target_include_directories(can-rx-test PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(can-rx-test)

