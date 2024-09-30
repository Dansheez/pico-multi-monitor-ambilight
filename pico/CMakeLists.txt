# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-multi-monitor-ambilight C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(pico-multi-monitor-ambilight 
    src/main.c
    src/pico_communication.c)

pico_set_program_name(pico-multi-monitor-ambilight "pico-multi-monitor-ambilight")
pico_set_program_version(pico-multi-monitor-ambilight "0.1")

# Generate PIO header
pico_generate_pio_header(pico-multi-monitor-ambilight ${CMAKE_CURRENT_LIST_DIR}/src/blink.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(pico-multi-monitor-ambilight 0)
pico_enable_stdio_usb(pico-multi-monitor-ambilight 1)

# Add the standard library to the build
target_link_libraries(pico-multi-monitor-ambilight
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(pico-multi-monitor-ambilight PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/include
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(pico-multi-monitor-ambilight 
        hardware_dma
        hardware_pio
        
        )

pico_add_extra_outputs(pico-multi-monitor-ambilight)

